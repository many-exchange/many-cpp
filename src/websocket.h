#pragma once

#include <algorithm>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace solana {

class WebSocketClient {
  const std::string _interface;
  const std::string _url;
  const int _port;

  int _socket;
  SSL_CTX *_ssl_ctx;
  SSL *_ssl;

  std::array<uint8_t, 17> _nonce;
  bool _handshake_complete = false;

  static const int RECEIVE_BUFFER_SIZE = 65536;

  char _receive_buffer[RECEIVE_BUFFER_SIZE];

  static const int SEND_BUFFER_SIZE = 8192;

  char _send_buffer[SEND_BUFFER_SIZE];
  uint8_t _send_mask[4];

  //static const uint8_t OPCODE_CONT   = 0x00;
  static const uint8_t OPCODE_TEXT   = 0x01;
  //static const uint8_t OPCODE_BINARY = 0x02;
  static const uint8_t OPCODE_CLOSE  = 0x08;
  static const uint8_t OPCODE_PING   = 0x09;
  static const uint8_t OPCODE_PONG   = 0x0A;
  static const uint8_t FIN_FLAG      = 0x80;
  static const uint8_t MASK_FLAG     = 0x80;

  //TODO move this into the base64 header
  const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  std::string base64_encode(unsigned char const* buf, unsigned int bufLen)
  {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (bufLen--)
    {
      char_array_3[i++] = *(buf++);
      if (i == 3)
      {
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for(i = 0; (i <4) ; i++)
          ret += base64_chars[char_array_4[i]];
        i = 0;
      }
    }

    if (i)
    {
      for(j = i; j < 3; j++)
        char_array_3[j] = '\0';

      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (j = 0; (j < i + 1); j++)
        ret += base64_chars[char_array_4[j]];

      while((i++ < 3))
        ret += '=';
    }

    return ret;
  }

  void send_message(uint8_t opcode, const char* message, size_t message_size)
  {
    int send_length = 0;

    _send_buffer[send_length] = opcode | FIN_FLAG;
    send_length++;

    //size_t message_size = message.size();

    if (message_size <= 125)
    {
      _send_buffer[send_length] = message_size | MASK_FLAG;
      send_length++;
    }
    else if (message_size <= 65535)
    {
      _send_buffer[send_length] = (uint8_t)(126 | MASK_FLAG);
      _send_buffer[send_length + 1] = (message_size >> 8) & 0xFF;
      _send_buffer[send_length + 2] = message_size & 0xFF;
      send_length += 3;
    }
    else
    {
      std::cerr << "Message too big." << std::endl;
      disconnect();
      return;
    }

    _send_buffer[send_length] = _send_mask[0];
    _send_buffer[send_length + 1] = _send_mask[1];
    _send_buffer[send_length + 2] = _send_mask[2];
    _send_buffer[send_length + 3] = _send_mask[3];
    send_length += 4;

    //const char* msg = message.c_str();
    for (size_t i = 0; i < message_size; ++i)
    {
        _send_buffer[send_length + i] = message[i] ^ _send_mask[i % 4];
    }

    send_length += message_size;

    write(_send_buffer, send_length);
  }

  bool validate_handshake(char *buffer, int length)
  {
    bool accept = false;

    int start = 0;
    int end = 0;
    while (end < length)
    {
      end++;

      if (buffer[end] == '\n')
      {
        if (strncmp(&buffer[start], "Connection: ", 12) == 0)
        {
          if (strncmp(&buffer[start], "Connection: Upgrade", 19) != 0 && strncmp(&buffer[start], "Connection: upgrade", 19) != 0)
          {
            return false;
          }
        }
        else if (strncmp(&buffer[start], "Upgrade: ", 9) == 0)
        {
          if (strncmp(&buffer[start], "Upgrade: websocket", 18) != 0)
          {
            return false;
          }
        }
        else if (strncmp(&buffer[start], "Sec-WebSocket-Accept: ", 22) == 0)
        {
          std::string key = base64_encode(_nonce.begin(), 16) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

          char sha1[20];
          SHA1((const unsigned char *)key.data(), key.size(), (unsigned char *)sha1);
          std::string hash = base64_encode((unsigned char *)sha1, 20);

          accept = (strncmp(&buffer[start + 22], hash.c_str(), 20) == 0);
        }

        start = end + 1;
      }
    }

    return accept;
  }

  bool write(const void* buffer, const int length)
  {
    if (is_connected())
    {
      return false;
    }

    assert(length > 0);
    int return_code = SSL_write(_ssl, buffer, length);

    if (_handshake_complete)
    {
      //TODO: log
    }

    if (return_code <= 0)
    {
        int err = SSL_get_error(_ssl, length);
        switch (err)
        {
            case SSL_ERROR_WANT_WRITE:
            {
                std::cerr << "SSL_ERROR_WANT_WRITE" << std::endl;
                break;
            }
            case SSL_ERROR_WANT_READ:
            {
                std::cerr << "SSL_ERROR_WANT_READ" << std::endl;
                break;
            }
            case SSL_ERROR_ZERO_RETURN:
            {
                std::cerr << "SSL_ERROR_ZERO_RETURN" << std::endl;
                break;
            }
            case SSL_ERROR_SYSCALL:
            {
                std::cerr << "SSL_ERROR_SYSCALL" << std::endl;
                break;
            }
            case SSL_ERROR_SSL:
            {
                std::cerr << "SSL_ERROR_SSL" << std::endl;
                break;
            }
        }
        std::cerr << "SEND FAILED" << std::endl;
        disconnect();
        return false;
    }

    return true;
  }

public:

  WebSocketClient(const std::string url, int port, const std::string interface = "")
    : _url(url), _port(port), _interface(interface), _socket(-1), _ssl_ctx(nullptr), _ssl(nullptr) {
    _nonce[16] = 0;
  }

  ~WebSocketClient() {
    disconnect();
  }

  WebSocketClient() = delete;
  WebSocketClient(const WebSocketClient&) = delete;
  WebSocketClient(WebSocketClient&&) = delete;
  WebSocketClient& operator=(const WebSocketClient&) = delete;
  WebSocketClient& operator=(WebSocketClient&&) = delete;

  bool is_connected()
  {
    return _socket != -1 && _ssl != nullptr;
  }

  bool connect() {
    std::size_t index = _url.find("://");
    assert(index != std::string::npos);
    std::string protocol = _url.substr(0, index);
    assert(protocol == "wss");
    index += 3;
    std::size_t end = _url.find("/", index + 1);
    std::string hostname = _url.substr(index, end - index);
    std::string resource = _url.substr(end);

    struct hostent *server;
    server = gethostbyname(hostname.c_str());
    if (server == NULL) {
      std::cerr << "Error: gethostbyname() failed" << std::endl;
      return false;
    }

    assert(server->h_addrtype == AF_INET);

    int i = 0;
    while (server->h_addr_list[i] != NULL)
    {
      if (connect(hostname, (struct in_addr*)server->h_addr_list[i], _port, resource))
      {
        return true;
      }
      i++;
    }

    return false;
  }

private:

  bool connect(const std::string& hostname, struct in_addr *addr, uint16_t tcp_port, const std::string& resource)
  {
    assert(_socket == -1);

    _handshake_complete = false;

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

    if (::connect(_socket, (sockaddr *)&remoteaddr, (int)sizeof(remoteaddr)) == -1)
    {
      std::cerr << "CONNECT FAILED" << std::endl;
      std::cerr << "  socket = " << std::to_string(_socket) << std::endl;
      std::cerr << "  tcp_address = " << tcp_address << std::endl;
      std::cerr << "  tcp_port = " << std::to_string(tcp_port) << std::endl;
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
      while ((err = ERR_get_error()) != 0)
      {
        char *str = ERR_error_string(err, 0);
        if (str != nullptr)
        {
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

    while (true)
    {
      int ret = SSL_connect(_ssl);

      if (ret == 1)
      {
        break;
      }
      else if (SSL_get_error(_ssl, ret) == SSL_ERROR_WANT_READ)
      {
        // Not enough data because we are using non-blocking IO.
      }
      else
      {
        std::cerr << "Error: SSL_connect() failed" << std::endl;
        disconnect();
        return false;
      }
    }

    std::cout << "CONNECT" << std::endl;
    std::cout << "  socket = " << std::to_string(_socket) << std::endl;
    std::cout << "  tcp_address = " << tcp_address << std::endl;
    std::cout << "  tcp_port = " << std::to_string(tcp_port) << std::endl;

    std::generate(_nonce.begin(), _nonce.end(), []() { return (uint8_t)std::rand(); });
    std::string websocket_key = base64_encode(_nonce.begin(), 16);

    int send_length = 0;
    send_length += sprintf(&_send_buffer[send_length], "GET %s HTTP/1.1\r\n", resource.c_str());
    send_length += sprintf(&_send_buffer[send_length], "Host: %s:%u\r\n", hostname.c_str(), tcp_port);
    send_length += sprintf(&_send_buffer[send_length], "Upgrade: websocket\r\n");
    send_length += sprintf(&_send_buffer[send_length], "Connection: Upgrade\r\n");
    send_length += sprintf(&_send_buffer[send_length], "Sec-WebSocket-Key: %s\r\n", websocket_key.c_str());
    send_length += sprintf(&_send_buffer[send_length], "Sec-WebSocket-Version: 13\r\n");
    send_length += sprintf(&_send_buffer[send_length], "\r\n");

    write(_send_buffer, send_length);

    while (is_connected())
    {
      int length;
      char *buffer = begin_read(length);

      if (length > 0)
      {
        if (validate_handshake(buffer, length))
        {
          end_read(0);
          _handshake_complete = true;
          *((uint32_t *)_send_mask) = rand();
          return true;
        }
        else
        {
          end_read(0);
          std::cerr << "HANDSHAKE FAILED" << std::endl;
          disconnect();
          return false;
        }

        break;
      }
    }

    return false;
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

  bool can_read()
  {
    return is_connected() && _handshake_complete;
  }

  char* begin_read(int& length)
  {
    length = 0;

    if (!is_connected())
    {
      return nullptr;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(_socket, &readfds);
    int rv = select(_socket + 1, &readfds, NULL, NULL, &tv);

    if(rv > 0 && FD_ISSET(_socket, &readfds))
    {
      length = SSL_read(_ssl, _receive_buffer, 8192);

      if (length > 0)
      {
        return _receive_buffer;
      }
      else if (length < 0)
      {
        std::cerr << "READ FAILED" << std::endl;

        switch (SSL_get_error(_ssl, length))
        {
          case SSL_ERROR_WANT_WRITE: std::cerr << "SSL_ERROR_WANT_WRITE" << std::endl; break;
          case SSL_ERROR_WANT_READ: std::cerr << "SSL_ERROR_WANT_READ" << std::endl; break;
          case SSL_ERROR_ZERO_RETURN: std::cerr << "SSL_ERROR_ZERO_RETURN" << std::endl; break;
          case SSL_ERROR_SYSCALL: std::cerr << "SSL_ERROR_SYSCALL" << std::endl; break;
          case SSL_ERROR_SSL: std::cerr << "SSL_ERROR_SSL" << std::endl; break;
        }

        disconnect();

        return nullptr;
      }

      return nullptr;
    }
    else
    {
      return nullptr;
    }
  }

  void end_read(uint64_t timestamp)
  {
    if (_handshake_complete)
    {
      //TODO: log
    }
  }

  void send_close(std::string message)
  {
    send_message(OPCODE_CLOSE, message.c_str(), message.size());
  }

  void send_ping(std::string message)
  {
    send_message(OPCODE_PING, message.c_str(), message.size());
  }

  void send_pong(std::string message)
  {
    send_message(OPCODE_PONG, message.c_str(), message.size());
  }

  void send_text(std::string message)
  {
    send_message(OPCODE_TEXT, message.c_str(), message.size());
  }

  void send_text(const char* message, size_t message_size)
  {
    send_message(OPCODE_TEXT, message, message_size);
  }

};

}
