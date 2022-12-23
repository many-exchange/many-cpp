#pragma once

#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "json.h"

using json = nlohmann::json;

namespace solana {
namespace http {

class HttpClient {
  const std::string _interface;
  const std::string _url;
  const int _port;

  int _socket;
  SSL_CTX *_ssl_ctx;
  SSL *_ssl;

  char* _send_buffer = nullptr;
  char* _recv_buffer = nullptr;

  bool write(const char *data, size_t length) {
    int ret = SSL_write(_ssl, data, (int)length);
    if (ret <= 0) {
      std::cerr << "Error: SSL_write() failed" << std::endl;
      disconnect();
      return false;
    }
    return true;
  }

  bool can_read() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(_socket, &readfds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    int ret = select(_socket + 1, &readfds, NULL, NULL, &timeout);
    if (ret == -1) {
      std::cerr << "Error: select() failed" << std::endl;
      disconnect();
      return false;
    }
    return ret > 0;
  }

  int read(char *data, int length) {
    int ret = SSL_read(_ssl, data, length);
    if (ret <= 0) {
      std::cerr << "Error: SSL_read() failed" << std::endl;
      disconnect();
      return 0;
    }
    return ret;
  }

public:
  HttpClient(const std::string url, int port, const std::string interface = "")
    : _url(url), _port(port), _interface(interface), _socket(-1), _ssl_ctx(nullptr), _ssl(nullptr)
  {
    _send_buffer = (char*)malloc(65536);
    _recv_buffer = (char*)malloc(8388608);
  }

  ~HttpClient() {
    disconnect();

    free(_send_buffer);
    free(_recv_buffer);
  }

  HttpClient() = delete;
  HttpClient(const HttpClient&) = delete;
  HttpClient(HttpClient&&) = delete;
  HttpClient& operator=(const HttpClient&) = delete;
  HttpClient& operator=(HttpClient&&) = delete;

  bool is_connected() {
    return _socket != -1 && _ssl != nullptr;
  }

  bool connect() {
    std::size_t index = _url.find("://");
    assert(index != std::string::npos);
    std::string protocol = _url.substr(0, index);
    assert(protocol == "https");
    index += 3; // "://"
    std::string hostname = _url.substr(index);

    struct hostent *server;
    server = gethostbyname(hostname.c_str());
    if (server == NULL) {
      std::cerr << "Error: gethostbyname() failed" << std::endl;
      return false;
    }

    assert(server->h_addrtype == AF_INET);

    int i = 0;
    while (server->h_addr_list[i] != NULL) {
      if (connect(hostname, (struct in_addr*)server->h_addr_list[i], _port)) {
        return true;
      }
      i++;
    }

    return false;
  }

private:
  bool connect(const std::string& hostname, struct in_addr *addr, uint16_t tcp_port) {
    assert(_socket == -1);

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) {
      std::cerr << "Error: socket() failed" << std::endl;
      disconnect();
      return false;
    }

    struct sockaddr_in localaddr;
    memset(&localaddr, 0, sizeof(localaddr));
    localaddr.sin_family = AF_INET;

    if (_interface.size() > 0) {
      localaddr.sin_addr.s_addr = inet_addr(_interface.c_str());
      bind(_socket, (sockaddr *)&localaddr, sizeof(localaddr));
    }

    struct sockaddr_in remoteaddr;
    bzero(&remoteaddr, sizeof(remoteaddr));
    remoteaddr.sin_family = AF_INET;
    remoteaddr.sin_port = htons(_port);
    remoteaddr.sin_addr.s_addr = *(long *)addr;

    std::string tcp_address = inet_ntoa(*addr);

    if (::connect(_socket, (sockaddr *)&remoteaddr, (int)sizeof(remoteaddr)) == -1) {
      std::cerr << "CONNECT FAILED" << std::endl;
      std::cerr << "  socket = " << std::to_string(_socket) << std::endl;
      std::cerr << "  tcp_address = " << tcp_address << std::endl;
      std::cerr << "  tcp_port = " << std::to_string(tcp_port) << std::endl << std::endl;
      disconnect();
      return false;
    }

    const SSL_METHOD *method = TLS_client_method();

    _ssl_ctx = SSL_CTX_new(method);
    if (_ssl_ctx == NULL) {
      std::cerr << "Error: SSL_CTX_new() failed" << std::endl;
      disconnect();
      return false;
    }

    _ssl = SSL_new(_ssl_ctx);
    if (_ssl == NULL) {
      std::cerr << "Error: SSL_new() failed" << std::endl;
      int err;
      while ((err = ERR_get_error()) != 0) {
        char *str = ERR_error_string(err, 0);
        if (str != nullptr) {
          std::cerr << str << std::endl;
        }
      }
      disconnect();
      return false;
    }

    if (SSL_set_fd(_ssl, _socket) == 0) {
      std::cerr << "Error: SSL_set_fd() failed" << std::endl;
      disconnect();
      return false;
    }

    SSL_set_tlsext_host_name(_ssl, hostname.c_str());

    while (true) {
      int ret = SSL_connect(_ssl);

      if (ret == 1) {
        break;
      }
      else if (SSL_get_error(_ssl, ret) == SSL_ERROR_WANT_READ) {
        // Not enough data because we are using non-blocking IO.
      }
      else {
        std::cerr << "Error: SSL_connect() failed" << std::endl;
        disconnect();
        return false;
      }
    }

    std::cout << "CONNECT" << std::endl;
    std::cout << "  socket = " << std::to_string(_socket) << std::endl;
    std::cout << "  tcp_address = " << tcp_address << std::endl;
    std::cout << "  tcp_port = " << std::to_string(tcp_port) << std::endl << std::endl;

    return true;
  }

public:
  bool disconnect() {
    if (_ssl != nullptr) {
      SSL_shutdown(_ssl);
      SSL_free(_ssl);
      _ssl = nullptr;
    }
    if (_ssl_ctx != nullptr) {
      SSL_CTX_free(_ssl_ctx);
      _ssl_ctx = nullptr;
    }
    if (_socket != -1) {
      close(_socket);
      _socket = -1;
    }
    return true;
  }

  char* post(json& request, int* recv_length) {
    std::string json_string = request.dump();

    int send_length = 0;
    send_length += sprintf(&_send_buffer[send_length], "POST / HTTP/1.1\r\n");
    send_length += sprintf(&_send_buffer[send_length], "Content-Type: application/json\r\n");
    send_length += sprintf(&_send_buffer[send_length], "Content-Length: %d\r\n", (int)json_string.size());
    send_length += sprintf(&_send_buffer[send_length], "\r\n");
    memcpy(&_send_buffer[send_length], json_string.c_str(), json_string.size());
    send_length += json_string.size();

    write(_send_buffer, send_length);
    std::cout << _send_buffer << std::endl << std::endl;

    *recv_length = read(&_recv_buffer[*recv_length], 8192);

    int content_length = 0;
    int header_length = 0;


    char* saveptr;
    char* line = strtok_r(_recv_buffer, "\r\n", &saveptr);
    while (line != nullptr) {
      if (line[0] == '{') {
        header_length = (int)(line - _recv_buffer);
        break;
      }
      char* colon = strchr(line, ':');
      if (colon != nullptr) {
        *colon = '\0';
        char* value = colon + 1;
        while (*value == ' ') {
          value++;
        }
        std::string key = std::string(line);
        if (key == "content-length" || key == "Content-Length") {
          std::string val = std::string(value);
          content_length = std::stoi(val);
        }
      }
      line = strtok_r(nullptr, "\r\n", &saveptr);
    }

    while (*recv_length < header_length + content_length) {
      *recv_length += read(&_recv_buffer[*recv_length], 65536);
    }
    *recv_length -= header_length;
    return &_recv_buffer[header_length];
  }

};

/**
 * Takes a url and a json request object and returns a json response
 */
json post(const std::string url, json request) {
  HttpClient client(url, 443);
  client.connect();
  //TODO return error if not connected
  assert(client.is_connected());
  int response_length = 0;
  char* response = client.post(request, &response_length);
  client.disconnect();

  //std::cout << response << std::endl;
  //std::cout << "response_length = " << response_length << std::endl;

  return json::parse(std::string(response, response_length));
}

}
}
