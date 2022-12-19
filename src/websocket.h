#pragma once

#include <iostream>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

namespace solana {

class WebSocketClient {
    int _sockfd;
    SSL_CTX *_ssl_ctx;
    SSL *_ssl;

  std::string _hostname;
  int _port;

public:

  WebSocketClient(const std::string& hostname, int port)
    : _sockfd(-1), _ssl_ctx(nullptr), _ssl(nullptr), _hostname(hostname), _port(port) {
    /*
    // Initialize OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    */
  }

  ~WebSocketClient()
  {
    // Close the SSL connection and the socket
    SSL_shutdown(_ssl);
    SSL_free(_ssl);
    close(_sockfd);
  }

  bool connect() {
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd < 0) {
      std::cerr << "Error creating socket" << std::endl;
      return false;
    }

    struct sockaddr_in serv_addr;
    //memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(_port);
    if (inet_pton(AF_INET, _hostname.c_str(), &serv_addr.sin_addr) <= 0) {
      std::cerr << "Error parsing address" << std::endl;
      return false;
    }

    if (::connect(_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      std::cerr << "Error connecting to host" << std::endl;
      return false;
    }

    // Initialize SSL context
    _ssl_ctx = SSL_CTX_new(TLS_client_method());
    //_ssl_ctx = SSL_CTX_new(SSLv23_client_method());
    if (_ssl_ctx == nullptr) {
      std::cerr << "Error creating SSL context" << std::endl;
      return false;
    }

    // Create SSL object
    _ssl = SSL_new(_ssl_ctx);
    if (_ssl == nullptr) {
      std::cerr << "Error creating SSL object" << std::endl;
      return false;
    }

    // Set the socket to be used with the SSL object
    if (SSL_set_fd(_ssl, _sockfd) != 1) {
      std::cerr << "Error setting SSL socket" << std::endl;
      return false;
    }

    // Perform SSL handshake
    if (SSL_connect(_ssl) != 1) {
      std::cerr << "Error performing SSL handshake" << std::endl;
      return false;
    }

    return true;
  }

  bool send(const void* buffer, int length) {
    if (SSL_write(_ssl, buffer, length) <= 0) {
      std::cerr << "Error sending data" << std::endl;
      return false;
    }
    return true;
  }

  bool receive(void* buffer, int buffer_size) {
    int bytesRead = SSL_read(_ssl, buffer, buffer_size);
    return bytesRead;
  }

};

}
