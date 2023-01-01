// Copyright (c) 2022-2023 Many Exchange
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#define ASSERT(x)                                               \
  do {                                                          \
    if( ! (x) ) {                                               \
      fprintf(stderr, "ERROR: ASSERT(%s) failed\n", #x);        \
      fprintf(stderr, "ERROR: at %s:%d\n", __FILE__, __LINE__); \
      abort();                                                  \
    }                                                           \
  } while( 0 )

namespace common {

  /**
   * Decodes a variable length integer
   * 
   * @param bytes The bytes to decode
  */
  int decode_length(std::vector<uint8_t> bytes) {
    int len = 0;
    int size = 0;
    while (bytes.size() > 0) {
      int elem = bytes.front();
      bytes.erase(bytes.begin());
      len |= (elem & 0x7f) << (size * 7);
      size += 1;
      if ((elem & 0x80) == 0) {
        break;
      }
    }
    return len;
  }

  /**
   * Encodes a length as a variable length integer
   * 
   * @param len The length to encode
   */
  std::vector<uint8_t> encode_length(int len) {
    std::vector<uint8_t> bytes;
    int rem_len = len;
    for (;;) {
      int elem = rem_len & 0x7f;
      rem_len >>= 7;
      if (rem_len == 0) {
        bytes.push_back(elem);
        break;
      } else {
        elem |= 0x80;
        bytes.push_back(elem);
      }
    }
    ASSERT(bytes.size() <= 2);
    return bytes;
  }

  namespace base58 {

    /*
    * Copyright 2012-2014 Luke Dashjr
    *
    * This program is free software; you can redistribute it and/or modify it
    * under the terms of the standard MIT license.
    */

    extern bool (*b58_sha256_impl)(void *, const void *, size_t);

    extern bool b58tobin(void *bin, size_t *binsz, const char *b58, size_t b58sz);
    extern int b58check(const void *bin, size_t binsz, const char *b58, size_t b58sz);

    extern bool b58enc(char *b58, size_t *b58sz, const void *bin, size_t binsz);
    extern bool b58check_enc(char *b58c, size_t *b58c_sz, uint8_t ver, const void *data, size_t datasz);

    bool (*b58_sha256_impl)(void *, const void *, size_t) = NULL;

    static const int8_t b58digits_map[] = {
      -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
      -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
      -1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
      -1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
      22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
      -1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
      47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
    };

    typedef uint64_t b58_maxint_t;
    typedef uint32_t b58_almostmaxint_t;
    #define b58_almostmaxint_bits (sizeof(b58_almostmaxint_t) * 8)
    static const b58_almostmaxint_t b58_almostmaxint_mask = ((((b58_maxint_t)1) << b58_almostmaxint_bits) - 1);

    bool b58tobin(void *bin, size_t *binszp, const char *b58, size_t b58sz) {
      size_t binsz = *binszp;
      const unsigned char *b58u = (const unsigned char *)b58;
      unsigned char *binu = (unsigned char *)bin;
      size_t outisz = (binsz + sizeof(b58_almostmaxint_t) - 1) / sizeof(b58_almostmaxint_t);
      b58_almostmaxint_t outi[outisz];
      b58_maxint_t t;
      b58_almostmaxint_t c;
      size_t i, j;
      uint8_t bytesleft = binsz % sizeof(b58_almostmaxint_t);
      b58_almostmaxint_t zeromask = bytesleft ? (b58_almostmaxint_mask << (bytesleft * 8)) : 0;
      unsigned zerocount = 0;

      if (!b58sz)
        b58sz = strlen(b58);

      for (i = 0; i < outisz; ++i) {
        outi[i] = 0;
      }

      // Leading zeros, just count
      for (i = 0; i < b58sz && b58u[i] == '1'; ++i)
        ++zerocount;

      for ( ; i < b58sz; ++i) {
        if (b58u[i] & 0x80)
          // High-bit set on invalid digit
          return false;
        if (b58digits_map[b58u[i]] == -1)
          // Invalid base58 digit
          return false;
        c = (unsigned)b58digits_map[b58u[i]];
        for (j = outisz; j--; ) {
          t = ((b58_maxint_t)outi[j]) * 58 + c;
          c = t >> b58_almostmaxint_bits;
          outi[j] = t & b58_almostmaxint_mask;
        }
        if (c)
          // Output number too big (carry to the next int32)
          return false;
        if (outi[0] & zeromask)
          // Output number too big (last int32 filled too far)
          return false;
      }

      j = 0;
      if (bytesleft) {
        for (i = bytesleft; i > 0; --i) {
          *(binu++) = (outi[0] >> (8 * (i - 1))) & 0xff;
        }
        ++j;
      }

      for (; j < outisz; ++j) {
        for (i = sizeof(*outi); i > 0; --i) {
          *(binu++) = (outi[j] >> (8 * (i - 1))) & 0xff;
        }
      }

      // Count canonical base58 byte count
      binu = (unsigned char *)bin;
      for (i = 0; i < binsz; ++i) {
        if (binu[i])
          break;
        --*binszp;
      }
      *binszp += zerocount;

      return true;
    }

    static
    bool my_dblsha256(void *hash, const void *data, size_t datasz) {
      uint8_t buf[0x20];
      return b58_sha256_impl(buf, data, datasz) && b58_sha256_impl(hash, buf, sizeof(buf));
    }

    int b58check(const void *bin, size_t binsz, const char *base58str, size_t b58sz) {
      unsigned char buf[32];
      const uint8_t *binc = (const uint8_t *)bin;
      unsigned i;
      if (binsz < 4)
        return -4;
      if (!my_dblsha256(buf, bin, binsz - 4))
        return -2;
      if (memcmp(&binc[binsz - 4], buf, 4))
        return -1;

      // Check number of zeros is correct AFTER verifying checksum (to avoid possibility of accessing base58str beyond the end)
      for (i = 0; binc[i] == '\0' && base58str[i] == '1'; ++i) {}  // Just finding the end of zeros, nothing to do in loop
      if (binc[i] == '\0' || base58str[i] == '1')
        return -3;

      return binc[0];
    }

    static const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

    bool b58enc(char *b58, size_t *b58sz, const void *data, size_t binsz) {
      const uint8_t *bin = (const uint8_t *)data;
      int carry;
      size_t i, j, high, zcount = 0;
      size_t size;

      while (zcount < binsz && !bin[zcount])
        ++zcount;

      size = (binsz - zcount) * 138 / 100 + 1;
      uint8_t buf[size];
      memset(buf, 0, size);

      for (i = zcount, high = size - 1; i < binsz; ++i, high = j) {
        for (carry = bin[i], j = size - 1; (j > high) || carry; --j) {
          carry += 256 * buf[j];
          buf[j] = carry % 58;
          carry /= 58;
          if (!j) {
            // Otherwise j wraps to maxint which is > high
            break;
          }
        }
      }

      for (j = 0; j < size && !buf[j]; ++j);

      if (*b58sz <= zcount + size - j) {
        *b58sz = zcount + size - j + 1;
        return false;
      }

      if (zcount)
        memset(b58, '1', zcount);
      for (i = zcount; j < size; ++i, ++j)
        b58[i] = b58digits_ordered[buf[j]];
      b58[i] = '\0';
      *b58sz = i + 1;

      return true;
    }

    bool b58check_enc(char *b58c, size_t *b58c_sz, uint8_t ver, const void *data, size_t datasz) {
      uint8_t buf[1 + datasz + 0x20];
      uint8_t *hash = &buf[1 + datasz];

      buf[0] = ver;
      memcpy(&buf[1], data, datasz);
      if (!my_dblsha256(hash, buf, datasz + 1)) {
        *b58c_sz = 0;
        return false;
      }

      return b58enc(b58c, b58c_sz, buf, 1 + datasz + 4);
    }

    inline std::string decode(const std::string &b58) {
      size_t decodedSize = b58.size() * 733 / 1000 + 1;
      char decoded[decodedSize];
      const bool ok = b58tobin(decoded, &decodedSize, b58.c_str(), 0);
      if (ok) {
        // remove zero bytes in front of result
        size_t offset = (b58.size() * 733 / 1000 + 1) - decodedSize;
        return std::string(decoded + offset, decodedSize);
      } else
        return std::string();
    }

    template <typename T>
    inline std::string encode(const T &str) {
      size_t b58Size = str.size() * 138 / 100 + 1;
      std::string b58(b58Size, '\0');
      const bool ok = b58enc(b58.data(), &b58Size, str.data(), str.size());
      if (ok) {
        // remove zero byte at end of result
        return std::string(b58.data(), b58Size - 1);
      } else
        return std::string();
    }

  } // namespace base58

  namespace base64 {

    const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    size_t encode(const unsigned char *data, size_t input_length, char *output, size_t output_size) {
      if (output_size < 4 * ((input_length + 2) / 3)) {
        throw std::runtime_error("Output buffer too small");
      }

      for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        output[j++] = base64_chars[(triple >> 3 * 6) & 0x3F];
        output[j++] = base64_chars[(triple >> 2 * 6) & 0x3F];
        output[j++] = base64_chars[(triple >> 1 * 6) & 0x3F];
        output[j++] = base64_chars[(triple >> 0 * 6) & 0x3F];
      }

      for (size_t i = 0; i < (3 - input_length % 3) % 3; i++) {
        output[output_size - 1 - i] = '=';
      }

      return output_size;
    }

    std::string encode(const unsigned char *data, size_t input_length) {
      int output_length = 4 * ((input_length + 2) / 3);
      char output[output_length + 1];
      output[output_length] = '\0';
      encode((unsigned char *)data, input_length, output, output_length);
      return output;
    }

    std::string encode(const std::string& input) {
      int output_length = 4 * ((input.size() + 2) / 3);
      char output[output_length + 1];
      output[output_length] = '\0';
      encode((unsigned char *)input.data(), input.size(), output, output_length);
      return output;
    }

    std::string encode(const std::vector<uint8_t>& input) {
      int output_length = 4 * ((input.size() + 2) / 3);
      char output[output_length + 1];
      output[output_length] = '\0';
      encode((unsigned char *)input.data(), input.size(), output, output_length);
      return output;
    }

    const int base64_decode_chars[] = {
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
      52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
      -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
      15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
      -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
      41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    };

    size_t decode(const char *data, size_t input_length, char *output, size_t output_size) {
      size_t i, j;
      size_t output_length = input_length / 4 * 3;

      if (data[input_length - 1] == '=') output_length--;
      if (data[input_length - 2] == '=') output_length--;

      if (output_size < output_length) {
        throw std::runtime_error("Output buffer too small");
      }

      for (i = 0, j = 0; i < input_length; ) {
        unsigned int a = data[i] == '=' ? 0 & i++ : base64_decode_chars[(int)data[i++]];
        unsigned int b = data[i] == '=' ? 0 & i++ : base64_decode_chars[(int)data[i++]];
        unsigned int c = data[i] == '=' ? 0 & i++ : base64_decode_chars[(int)data[i++]];
        unsigned int d = data[i] == '=' ? 0 & i++ : base64_decode_chars[(int)data[i++]];

        unsigned int triple = (a << 3 * 6) + (b << 2 * 6) + (c << 1 * 6) + (d << 0 * 6);

        if (j < output_length) output[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length) output[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length) output[j++] = (triple >> 0 * 8) & 0xFF;
      }

      return output_length;
    }

    std::string decode(const char *data, size_t input_length) {
      int output_length = input_length / 4 * 3;
      char output[output_length + 1];
      output[output_length] = '\0';
      output_length = decode(data, input_length, output, output_length);
      output[output_length] = '\0';
      return output;
    }

    std::vector<uint8_t> decode(const std::string& input) {
      std::string decoded = decode(input.c_str(), input.size());
      return std::vector<uint8_t>(decoded.begin(), decoded.end());
    }

  } // namespace base64

  namespace endian {

    /**
     * Convert a 16-bit integer from host byte order to big-endian byte order
     *
     * @param x 16-bit integer in host byte order
     */
    inline uint64_t be64toh(uint64_t x) {
      #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        return __builtin_bswap64(x);
      #else
        return x;
      #endif
    }

  } // namespace endian

  namespace http {

    class HttpClient {
      const std::string _interface;
      const std::string _url;

      int _socket;
      bool _use_ssl = false;
      SSL_CTX *_ssl_ctx;
      SSL *_ssl;

      char* _send_buffer = nullptr;
      char* _recv_buffer = nullptr;

      bool write(const char *data, size_t length) {
        if (_use_ssl) {
          int ret = SSL_write(_ssl, data, (int)length);
          if (ret <= 0) {
            std::cerr << "Error: SSL_write() failed" << std::endl;
            disconnect();
            return false;
          }
        } else {
          int ret = ::write(_socket, data, (int)length);
          if (ret <= 0) {
            std::cerr << "Error: write() failed" << std::endl;
            disconnect();
            return false;
          }
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
        if (_use_ssl) {
          int ret = SSL_read(_ssl, data, length);
          if (ret <= 0) {
            std::cerr << "Error: SSL_read() failed" << std::endl;
            disconnect();
            return 0;
          }
          return ret;
        } else {
          int ret = ::read(_socket, data, length);
          if (ret <= 0) {
            std::cerr << "Error: SSL_read() failed" << std::endl;
            disconnect();
            return 0;
          }
          return ret;
        }
      }

    public:

      HttpClient(const std::string url, const std::string interface = "")
        : _url(url),
        _interface(interface),
        _socket(-1),
        _ssl_ctx(nullptr),
        _ssl(nullptr)
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
        return _socket != -1;
      }

      bool connect() {
        std::size_t index = _url.find("://");
        ASSERT(index != std::string::npos);
        std::string protocol = _url.substr(0, index);
        _use_ssl = (protocol == "https");
        int port = _use_ssl ? 443 : 80;
        index += 3; // "://"
        std::size_t end = _url.find("/", index + 1);
        std::string hostname = _url.substr(index, end - index);
        index = hostname.find(":");
        if (index != std::string::npos) {
          port = std::stoi(hostname.substr(index + 1));
          hostname = hostname.substr(0, index);
        }

        struct hostent *server;
        server = gethostbyname(hostname.c_str());
        if (server == NULL) {
          std::cerr << "Error: gethostbyname() failed" << std::endl;
          return false;
        }

        ASSERT(server->h_addrtype == AF_INET);

        int i = 0;
        while (server->h_addr_list[i] != NULL) {
          if (connect(hostname, (struct in_addr*)server->h_addr_list[i], port)) {
            return true;
          }
          i++;
        }

        return false;
      }

    private:

      bool connect(const std::string& hostname, struct in_addr *addr, uint16_t port) {
        ASSERT(_socket == -1);

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
        remoteaddr.sin_port = htons(port);
        remoteaddr.sin_addr.s_addr = *(long *)addr;

        std::string address = inet_ntoa(*addr);

        if (::connect(_socket, (sockaddr *)&remoteaddr, (int)sizeof(remoteaddr)) == -1) {
          std::cerr << "CONNECT FAILED" << std::endl;
          std::cerr << "  socket = " << std::to_string(_socket) << std::endl;
          std::cerr << "  address = " << address << std::endl;
          std::cerr << "  port = " << std::to_string(port) << std::endl << std::endl;
          disconnect();
          return false;
        }

        if (_use_ssl) {
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
        }

        // std::cout << "CONNECT" << std::endl;
        // std::cout << "  socket = " << std::to_string(_socket) << std::endl;
        // std::cout << "  tcp_address = " << tcp_address << std::endl;
        // std::cout << "  tcp_port = " << std::to_string(tcp_port) << std::endl << std::endl;

        return true;
      }

    public:

      bool disconnect() {
        if (_use_ssl) {
          if (_ssl != nullptr) {
            SSL_shutdown(_ssl);
            SSL_free(_ssl);
            _ssl = nullptr;
          }
          if (_ssl_ctx != nullptr) {
            SSL_CTX_free(_ssl_ctx);
            _ssl_ctx = nullptr;
          }
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
     *
     * @param url The url endpoint for the POST request
     * @param request The json request object
     */
    json post(const std::string url, json request) {
      HttpClient client(url);
      client.connect();
      if (!client.is_connected()) {
        throw std::runtime_error("Unable to connect to HttpClient.");
      }

      int response_length = 0;
      char* response = client.post(request, &response_length);
      client.disconnect();

      //std::cout << response << std::endl << std::endl;

      return json::parse(std::string(response, response_length));
    }

  } // namespace http

  namespace libsodium {

    /**
     * The following is based on https://github.com/jedisct1/libsodium
     */

    typedef int32_t fe25519[10];

    typedef struct {
        fe25519 X;
        fe25519 Y;
        fe25519 Z;
        fe25519 T;
    } ge25519_p3;

    /* sqrt(-1) */
    static const fe25519 fe25519_sqrtm1 = {
        -32595792, -7943725,  9377950,  3500415, 12389472, -272473, -25146209, -2005654, 326686, 11406482
    };

    /* 37095705934669439343138083508754565189542113879843219016388785533085940283555 */
    static const fe25519 ed25519_d = {
        -10913610, 13857413, -15372611, 6949391,   114729, -8787816, -6275908, -3247719, -18696448, -12055116
    };

    static inline uint64_t load_3(const unsigned char *in)
    {
        uint64_t result;

        result = (uint64_t) in[0];
        result |= ((uint64_t) in[1]) << 8;
        result |= ((uint64_t) in[2]) << 16;

        return result;
    }

    static inline uint64_t load_4(const unsigned char *in)
    {
        uint64_t result;

        result = (uint64_t) in[0];
        result |= ((uint64_t) in[1]) << 8;
        result |= ((uint64_t) in[2]) << 16;
        result |= ((uint64_t) in[3]) << 24;

        return result;
    }

    int sodium_is_zero(const unsigned char *n, const size_t nlen)
    {
        size_t                 i;
        volatile unsigned char d = 0U;

        for (i = 0U; i < nlen; i++) {
            d |= n[i];
        }
        return 1 & ((d - 1) >> 8);
    }

    static inline void fe25519_1(fe25519 h)
    {
        h[0] = 1;
        h[1] = 0;
        memset(&h[2], 0, 8 * sizeof h[0]);
    }

    static inline void fe25519_add(fe25519 h, const fe25519 f, const fe25519 g)
    {
        int32_t h0 = f[0] + g[0];
        int32_t h1 = f[1] + g[1];
        int32_t h2 = f[2] + g[2];
        int32_t h3 = f[3] + g[3];
        int32_t h4 = f[4] + g[4];
        int32_t h5 = f[5] + g[5];
        int32_t h6 = f[6] + g[6];
        int32_t h7 = f[7] + g[7];
        int32_t h8 = f[8] + g[8];
        int32_t h9 = f[9] + g[9];

        h[0] = h0;
        h[1] = h1;
        h[2] = h2;
        h[3] = h3;
        h[4] = h4;
        h[5] = h5;
        h[6] = h6;
        h[7] = h7;
        h[8] = h8;
        h[9] = h9;
    }

    static void fe25519_cmov(fe25519 f, const fe25519 g, unsigned int b)
    {
        uint32_t mask = (uint32_t) (-(int32_t) b);
        int32_t  f0, f1, f2, f3, f4, f5, f6, f7, f8, f9;
        int32_t  x0, x1, x2, x3, x4, x5, x6, x7, x8, x9;

        f0 = f[0];
        f1 = f[1];
        f2 = f[2];
        f3 = f[3];
        f4 = f[4];
        f5 = f[5];
        f6 = f[6];
        f7 = f[7];
        f8 = f[8];
        f9 = f[9];

        x0 = f0 ^ g[0];
        x1 = f1 ^ g[1];
        x2 = f2 ^ g[2];
        x3 = f3 ^ g[3];
        x4 = f4 ^ g[4];
        x5 = f5 ^ g[5];
        x6 = f6 ^ g[6];
        x7 = f7 ^ g[7];
        x8 = f8 ^ g[8];
        x9 = f9 ^ g[9];

    #ifdef HAVE_INLINE_ASM
        __asm__ __volatile__("" : "+r"(mask));
    #endif

        x0 &= mask;
        x1 &= mask;
        x2 &= mask;
        x3 &= mask;
        x4 &= mask;
        x5 &= mask;
        x6 &= mask;
        x7 &= mask;
        x8 &= mask;
        x9 &= mask;

        f[0] = f0 ^ x0;
        f[1] = f1 ^ x1;
        f[2] = f2 ^ x2;
        f[3] = f3 ^ x3;
        f[4] = f4 ^ x4;
        f[5] = f5 ^ x5;
        f[6] = f6 ^ x6;
        f[7] = f7 ^ x7;
        f[8] = f8 ^ x8;
        f[9] = f9 ^ x9;
    }

    void fe25519_frombytes(fe25519 h, const unsigned char *s)
    {
        int64_t h0 = load_4(s);
        int64_t h1 = load_3(s + 4) << 6;
        int64_t h2 = load_3(s + 7) << 5;
        int64_t h3 = load_3(s + 10) << 3;
        int64_t h4 = load_3(s + 13) << 2;
        int64_t h5 = load_4(s + 16);
        int64_t h6 = load_3(s + 20) << 7;
        int64_t h7 = load_3(s + 23) << 5;
        int64_t h8 = load_3(s + 26) << 4;
        int64_t h9 = (load_3(s + 29) & 8388607) << 2;

        int64_t carry0;
        int64_t carry1;
        int64_t carry2;
        int64_t carry3;
        int64_t carry4;
        int64_t carry5;
        int64_t carry6;
        int64_t carry7;
        int64_t carry8;
        int64_t carry9;

        carry9 = (h9 + (int64_t)(1L << 24)) >> 25;
        h0 += carry9 * 19;
        h9 -= carry9 * ((uint64_t) 1L << 25);
        carry1 = (h1 + (int64_t)(1L << 24)) >> 25;
        h2 += carry1;
        h1 -= carry1 * ((uint64_t) 1L << 25);
        carry3 = (h3 + (int64_t)(1L << 24)) >> 25;
        h4 += carry3;
        h3 -= carry3 * ((uint64_t) 1L << 25);
        carry5 = (h5 + (int64_t)(1L << 24)) >> 25;
        h6 += carry5;
        h5 -= carry5 * ((uint64_t) 1L << 25);
        carry7 = (h7 + (int64_t)(1L << 24)) >> 25;
        h8 += carry7;
        h7 -= carry7 * ((uint64_t) 1L << 25);

        carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
        h1 += carry0;
        h0 -= carry0 * ((uint64_t) 1L << 26);
        carry2 = (h2 + (int64_t)(1L << 25)) >> 26;
        h3 += carry2;
        h2 -= carry2 * ((uint64_t) 1L << 26);
        carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
        h5 += carry4;
        h4 -= carry4 * ((uint64_t) 1L << 26);
        carry6 = (h6 + (int64_t)(1L << 25)) >> 26;
        h7 += carry6;
        h6 -= carry6 * ((uint64_t) 1L << 26);
        carry8 = (h8 + (int64_t)(1L << 25)) >> 26;
        h9 += carry8;
        h8 -= carry8 * ((uint64_t) 1L << 26);

        h[0] = (int32_t) h0;
        h[1] = (int32_t) h1;
        h[2] = (int32_t) h2;
        h[3] = (int32_t) h3;
        h[4] = (int32_t) h4;
        h[5] = (int32_t) h5;
        h[6] = (int32_t) h6;
        h[7] = (int32_t) h7;
        h[8] = (int32_t) h8;
        h[9] = (int32_t) h9;
    }

    static void fe25519_reduce(fe25519 h, const fe25519 f)
    {
        int32_t h0 = f[0];
        int32_t h1 = f[1];
        int32_t h2 = f[2];
        int32_t h3 = f[3];
        int32_t h4 = f[4];
        int32_t h5 = f[5];
        int32_t h6 = f[6];
        int32_t h7 = f[7];
        int32_t h8 = f[8];
        int32_t h9 = f[9];

        int32_t q;
        int32_t carry0, carry1, carry2, carry3, carry4, carry5, carry6, carry7, carry8, carry9;

        q = (19 * h9 + ((uint32_t) 1L << 24)) >> 25;
        q = (h0 + q) >> 26;
        q = (h1 + q) >> 25;
        q = (h2 + q) >> 26;
        q = (h3 + q) >> 25;
        q = (h4 + q) >> 26;
        q = (h5 + q) >> 25;
        q = (h6 + q) >> 26;
        q = (h7 + q) >> 25;
        q = (h8 + q) >> 26;
        q = (h9 + q) >> 25;

        /* Goal: Output h-(2^255-19)q, which is between 0 and 2^255-20. */
        h0 += 19 * q;
        /* Goal: Output h-2^255 q, which is between 0 and 2^255-20. */

        carry0 = h0 >> 26;
        h1 += carry0;
        h0 -= carry0 * ((uint32_t) 1L << 26);
        carry1 = h1 >> 25;
        h2 += carry1;
        h1 -= carry1 * ((uint32_t) 1L << 25);
        carry2 = h2 >> 26;
        h3 += carry2;
        h2 -= carry2 * ((uint32_t) 1L << 26);
        carry3 = h3 >> 25;
        h4 += carry3;
        h3 -= carry3 * ((uint32_t) 1L << 25);
        carry4 = h4 >> 26;
        h5 += carry4;
        h4 -= carry4 * ((uint32_t) 1L << 26);
        carry5 = h5 >> 25;
        h6 += carry5;
        h5 -= carry5 * ((uint32_t) 1L << 25);
        carry6 = h6 >> 26;
        h7 += carry6;
        h6 -= carry6 * ((uint32_t) 1L << 26);
        carry7 = h7 >> 25;
        h8 += carry7;
        h7 -= carry7 * ((uint32_t) 1L << 25);
        carry8 = h8 >> 26;
        h9 += carry8;
        h8 -= carry8 * ((uint32_t) 1L << 26);
        carry9 = h9 >> 25;
        h9 -= carry9 * ((uint32_t) 1L << 25);

        h[0] = h0;
        h[1] = h1;
        h[2] = h2;
        h[3] = h3;
        h[4] = h4;
        h[5] = h5;
        h[6] = h6;
        h[7] = h7;
        h[8] = h8;
        h[9] = h9;
    }

    void fe25519_tobytes(unsigned char *s, const fe25519 h)
    {
        fe25519 t;

        fe25519_reduce(t, h);
        s[0]  = t[0] >> 0;
        s[1]  = t[0] >> 8;
        s[2]  = t[0] >> 16;
        s[3]  = (t[0] >> 24) | (t[1] * ((uint32_t) 1 << 2));
        s[4]  = t[1] >> 6;
        s[5]  = t[1] >> 14;
        s[6]  = (t[1] >> 22) | (t[2] * ((uint32_t) 1 << 3));
        s[7]  = t[2] >> 5;
        s[8]  = t[2] >> 13;
        s[9]  = (t[2] >> 21) | (t[3] * ((uint32_t) 1 << 5));
        s[10] = t[3] >> 3;
        s[11] = t[3] >> 11;
        s[12] = (t[3] >> 19) | (t[4] * ((uint32_t) 1 << 6));
        s[13] = t[4] >> 2;
        s[14] = t[4] >> 10;
        s[15] = t[4] >> 18;
        s[16] = t[5] >> 0;
        s[17] = t[5] >> 8;
        s[18] = t[5] >> 16;
        s[19] = (t[5] >> 24) | (t[6] * ((uint32_t) 1 << 1));
        s[20] = t[6] >> 7;
        s[21] = t[6] >> 15;
        s[22] = (t[6] >> 23) | (t[7] * ((uint32_t) 1 << 3));
        s[23] = t[7] >> 5;
        s[24] = t[7] >> 13;
        s[25] = (t[7] >> 21) | (t[8] * ((uint32_t) 1 << 4));
        s[26] = t[8] >> 4;
        s[27] = t[8] >> 12;
        s[28] = (t[8] >> 20) | (t[9] * ((uint32_t) 1 << 6));
        s[29] = t[9] >> 2;
        s[30] = t[9] >> 10;
        s[31] = t[9] >> 18;
    }

    static inline int fe25519_isnegative(const fe25519 f)
    {
        unsigned char s[32];

        fe25519_tobytes(s, f);

        return s[0] & 1;
    }

    static inline int fe25519_iszero(const fe25519 f)
    {
        unsigned char s[32];

        fe25519_tobytes(s, f);

        return sodium_is_zero(s, 32);
    }

    static void fe25519_mul(fe25519 h, const fe25519 f, const fe25519 g)
    {
        int32_t f0 = f[0];
        int32_t f1 = f[1];
        int32_t f2 = f[2];
        int32_t f3 = f[3];
        int32_t f4 = f[4];
        int32_t f5 = f[5];
        int32_t f6 = f[6];
        int32_t f7 = f[7];
        int32_t f8 = f[8];
        int32_t f9 = f[9];

        int32_t g0 = g[0];
        int32_t g1 = g[1];
        int32_t g2 = g[2];
        int32_t g3 = g[3];
        int32_t g4 = g[4];
        int32_t g5 = g[5];
        int32_t g6 = g[6];
        int32_t g7 = g[7];
        int32_t g8 = g[8];
        int32_t g9 = g[9];

        int32_t g1_19 = 19 * g1; /* 1.959375*2^29 */
        int32_t g2_19 = 19 * g2; /* 1.959375*2^30; still ok */
        int32_t g3_19 = 19 * g3;
        int32_t g4_19 = 19 * g4;
        int32_t g5_19 = 19 * g5;
        int32_t g6_19 = 19 * g6;
        int32_t g7_19 = 19 * g7;
        int32_t g8_19 = 19 * g8;
        int32_t g9_19 = 19 * g9;
        int32_t f1_2  = 2 * f1;
        int32_t f3_2  = 2 * f3;
        int32_t f5_2  = 2 * f5;
        int32_t f7_2  = 2 * f7;
        int32_t f9_2  = 2 * f9;

        int64_t f0g0    = f0 * (int64_t) g0;
        int64_t f0g1    = f0 * (int64_t) g1;
        int64_t f0g2    = f0 * (int64_t) g2;
        int64_t f0g3    = f0 * (int64_t) g3;
        int64_t f0g4    = f0 * (int64_t) g4;
        int64_t f0g5    = f0 * (int64_t) g5;
        int64_t f0g6    = f0 * (int64_t) g6;
        int64_t f0g7    = f0 * (int64_t) g7;
        int64_t f0g8    = f0 * (int64_t) g8;
        int64_t f0g9    = f0 * (int64_t) g9;
        int64_t f1g0    = f1 * (int64_t) g0;
        int64_t f1g1_2  = f1_2 * (int64_t) g1;
        int64_t f1g2    = f1 * (int64_t) g2;
        int64_t f1g3_2  = f1_2 * (int64_t) g3;
        int64_t f1g4    = f1 * (int64_t) g4;
        int64_t f1g5_2  = f1_2 * (int64_t) g5;
        int64_t f1g6    = f1 * (int64_t) g6;
        int64_t f1g7_2  = f1_2 * (int64_t) g7;
        int64_t f1g8    = f1 * (int64_t) g8;
        int64_t f1g9_38 = f1_2 * (int64_t) g9_19;
        int64_t f2g0    = f2 * (int64_t) g0;
        int64_t f2g1    = f2 * (int64_t) g1;
        int64_t f2g2    = f2 * (int64_t) g2;
        int64_t f2g3    = f2 * (int64_t) g3;
        int64_t f2g4    = f2 * (int64_t) g4;
        int64_t f2g5    = f2 * (int64_t) g5;
        int64_t f2g6    = f2 * (int64_t) g6;
        int64_t f2g7    = f2 * (int64_t) g7;
        int64_t f2g8_19 = f2 * (int64_t) g8_19;
        int64_t f2g9_19 = f2 * (int64_t) g9_19;
        int64_t f3g0    = f3 * (int64_t) g0;
        int64_t f3g1_2  = f3_2 * (int64_t) g1;
        int64_t f3g2    = f3 * (int64_t) g2;
        int64_t f3g3_2  = f3_2 * (int64_t) g3;
        int64_t f3g4    = f3 * (int64_t) g4;
        int64_t f3g5_2  = f3_2 * (int64_t) g5;
        int64_t f3g6    = f3 * (int64_t) g6;
        int64_t f3g7_38 = f3_2 * (int64_t) g7_19;
        int64_t f3g8_19 = f3 * (int64_t) g8_19;
        int64_t f3g9_38 = f3_2 * (int64_t) g9_19;
        int64_t f4g0    = f4 * (int64_t) g0;
        int64_t f4g1    = f4 * (int64_t) g1;
        int64_t f4g2    = f4 * (int64_t) g2;
        int64_t f4g3    = f4 * (int64_t) g3;
        int64_t f4g4    = f4 * (int64_t) g4;
        int64_t f4g5    = f4 * (int64_t) g5;
        int64_t f4g6_19 = f4 * (int64_t) g6_19;
        int64_t f4g7_19 = f4 * (int64_t) g7_19;
        int64_t f4g8_19 = f4 * (int64_t) g8_19;
        int64_t f4g9_19 = f4 * (int64_t) g9_19;
        int64_t f5g0    = f5 * (int64_t) g0;
        int64_t f5g1_2  = f5_2 * (int64_t) g1;
        int64_t f5g2    = f5 * (int64_t) g2;
        int64_t f5g3_2  = f5_2 * (int64_t) g3;
        int64_t f5g4    = f5 * (int64_t) g4;
        int64_t f5g5_38 = f5_2 * (int64_t) g5_19;
        int64_t f5g6_19 = f5 * (int64_t) g6_19;
        int64_t f5g7_38 = f5_2 * (int64_t) g7_19;
        int64_t f5g8_19 = f5 * (int64_t) g8_19;
        int64_t f5g9_38 = f5_2 * (int64_t) g9_19;
        int64_t f6g0    = f6 * (int64_t) g0;
        int64_t f6g1    = f6 * (int64_t) g1;
        int64_t f6g2    = f6 * (int64_t) g2;
        int64_t f6g3    = f6 * (int64_t) g3;
        int64_t f6g4_19 = f6 * (int64_t) g4_19;
        int64_t f6g5_19 = f6 * (int64_t) g5_19;
        int64_t f6g6_19 = f6 * (int64_t) g6_19;
        int64_t f6g7_19 = f6 * (int64_t) g7_19;
        int64_t f6g8_19 = f6 * (int64_t) g8_19;
        int64_t f6g9_19 = f6 * (int64_t) g9_19;
        int64_t f7g0    = f7 * (int64_t) g0;
        int64_t f7g1_2  = f7_2 * (int64_t) g1;
        int64_t f7g2    = f7 * (int64_t) g2;
        int64_t f7g3_38 = f7_2 * (int64_t) g3_19;
        int64_t f7g4_19 = f7 * (int64_t) g4_19;
        int64_t f7g5_38 = f7_2 * (int64_t) g5_19;
        int64_t f7g6_19 = f7 * (int64_t) g6_19;
        int64_t f7g7_38 = f7_2 * (int64_t) g7_19;
        int64_t f7g8_19 = f7 * (int64_t) g8_19;
        int64_t f7g9_38 = f7_2 * (int64_t) g9_19;
        int64_t f8g0    = f8 * (int64_t) g0;
        int64_t f8g1    = f8 * (int64_t) g1;
        int64_t f8g2_19 = f8 * (int64_t) g2_19;
        int64_t f8g3_19 = f8 * (int64_t) g3_19;
        int64_t f8g4_19 = f8 * (int64_t) g4_19;
        int64_t f8g5_19 = f8 * (int64_t) g5_19;
        int64_t f8g6_19 = f8 * (int64_t) g6_19;
        int64_t f8g7_19 = f8 * (int64_t) g7_19;
        int64_t f8g8_19 = f8 * (int64_t) g8_19;
        int64_t f8g9_19 = f8 * (int64_t) g9_19;
        int64_t f9g0    = f9 * (int64_t) g0;
        int64_t f9g1_38 = f9_2 * (int64_t) g1_19;
        int64_t f9g2_19 = f9 * (int64_t) g2_19;
        int64_t f9g3_38 = f9_2 * (int64_t) g3_19;
        int64_t f9g4_19 = f9 * (int64_t) g4_19;
        int64_t f9g5_38 = f9_2 * (int64_t) g5_19;
        int64_t f9g6_19 = f9 * (int64_t) g6_19;
        int64_t f9g7_38 = f9_2 * (int64_t) g7_19;
        int64_t f9g8_19 = f9 * (int64_t) g8_19;
        int64_t f9g9_38 = f9_2 * (int64_t) g9_19;

        int64_t h0 = f0g0 + f1g9_38 + f2g8_19 + f3g7_38 + f4g6_19 + f5g5_38 +
                    f6g4_19 + f7g3_38 + f8g2_19 + f9g1_38;
        int64_t h1 = f0g1 + f1g0 + f2g9_19 + f3g8_19 + f4g7_19 + f5g6_19 + f6g5_19 +
                    f7g4_19 + f8g3_19 + f9g2_19;
        int64_t h2 = f0g2 + f1g1_2 + f2g0 + f3g9_38 + f4g8_19 + f5g7_38 + f6g6_19 +
                    f7g5_38 + f8g4_19 + f9g3_38;
        int64_t h3 = f0g3 + f1g2 + f2g1 + f3g0 + f4g9_19 + f5g8_19 + f6g7_19 +
                    f7g6_19 + f8g5_19 + f9g4_19;
        int64_t h4 = f0g4 + f1g3_2 + f2g2 + f3g1_2 + f4g0 + f5g9_38 + f6g8_19 +
                    f7g7_38 + f8g6_19 + f9g5_38;
        int64_t h5 = f0g5 + f1g4 + f2g3 + f3g2 + f4g1 + f5g0 + f6g9_19 + f7g8_19 +
                    f8g7_19 + f9g6_19;
        int64_t h6 = f0g6 + f1g5_2 + f2g4 + f3g3_2 + f4g2 + f5g1_2 + f6g0 +
                    f7g9_38 + f8g8_19 + f9g7_38;
        int64_t h7 = f0g7 + f1g6 + f2g5 + f3g4 + f4g3 + f5g2 + f6g1 + f7g0 +
                    f8g9_19 + f9g8_19;
        int64_t h8 = f0g8 + f1g7_2 + f2g6 + f3g5_2 + f4g4 + f5g3_2 + f6g2 + f7g1_2 +
                    f8g0 + f9g9_38;
        int64_t h9 =
            f0g9 + f1g8 + f2g7 + f3g6 + f4g5 + f5g4 + f6g3 + f7g2 + f8g1 + f9g0;

        int64_t carry0;
        int64_t carry1;
        int64_t carry2;
        int64_t carry3;
        int64_t carry4;
        int64_t carry5;
        int64_t carry6;
        int64_t carry7;
        int64_t carry8;
        int64_t carry9;

        /*
        |h0| <= (1.65*1.65*2^52*(1+19+19+19+19)+1.65*1.65*2^50*(38+38+38+38+38))
        i.e. |h0| <= 1.4*2^60; narrower ranges for h2, h4, h6, h8
        |h1| <= (1.65*1.65*2^51*(1+1+19+19+19+19+19+19+19+19))
        i.e. |h1| <= 1.7*2^59; narrower ranges for h3, h5, h7, h9
        */

        carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
        h1 += carry0;
        h0 -= carry0 * ((uint64_t) 1L << 26);
        carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
        h5 += carry4;
        h4 -= carry4 * ((uint64_t) 1L << 26);
        /* |h0| <= 2^25 */
        /* |h4| <= 2^25 */
        /* |h1| <= 1.71*2^59 */
        /* |h5| <= 1.71*2^59 */

        carry1 = (h1 + (int64_t)(1L << 24)) >> 25;
        h2 += carry1;
        h1 -= carry1 * ((uint64_t) 1L << 25);
        carry5 = (h5 + (int64_t)(1L << 24)) >> 25;
        h6 += carry5;
        h5 -= carry5 * ((uint64_t) 1L << 25);
        /* |h1| <= 2^24; from now on fits into int32 */
        /* |h5| <= 2^24; from now on fits into int32 */
        /* |h2| <= 1.41*2^60 */
        /* |h6| <= 1.41*2^60 */

        carry2 = (h2 + (int64_t)(1L << 25)) >> 26;
        h3 += carry2;
        h2 -= carry2 * ((uint64_t) 1L << 26);
        carry6 = (h6 + (int64_t)(1L << 25)) >> 26;
        h7 += carry6;
        h6 -= carry6 * ((uint64_t) 1L << 26);
        /* |h2| <= 2^25; from now on fits into int32 unchanged */
        /* |h6| <= 2^25; from now on fits into int32 unchanged */
        /* |h3| <= 1.71*2^59 */
        /* |h7| <= 1.71*2^59 */

        carry3 = (h3 + (int64_t)(1L << 24)) >> 25;
        h4 += carry3;
        h3 -= carry3 * ((uint64_t) 1L << 25);
        carry7 = (h7 + (int64_t)(1L << 24)) >> 25;
        h8 += carry7;
        h7 -= carry7 * ((uint64_t) 1L << 25);
        /* |h3| <= 2^24; from now on fits into int32 unchanged */
        /* |h7| <= 2^24; from now on fits into int32 unchanged */
        /* |h4| <= 1.72*2^34 */
        /* |h8| <= 1.41*2^60 */

        carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
        h5 += carry4;
        h4 -= carry4 * ((uint64_t) 1L << 26);
        carry8 = (h8 + (int64_t)(1L << 25)) >> 26;
        h9 += carry8;
        h8 -= carry8 * ((uint64_t) 1L << 26);
        /* |h4| <= 2^25; from now on fits into int32 unchanged */
        /* |h8| <= 2^25; from now on fits into int32 unchanged */
        /* |h5| <= 1.01*2^24 */
        /* |h9| <= 1.71*2^59 */

        carry9 = (h9 + (int64_t)(1L << 24)) >> 25;
        h0 += carry9 * 19;
        h9 -= carry9 * ((uint64_t) 1L << 25);
        /* |h9| <= 2^24; from now on fits into int32 unchanged */
        /* |h0| <= 1.1*2^39 */

        carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
        h1 += carry0;
        h0 -= carry0 * ((uint64_t) 1L << 26);
        /* |h0| <= 2^25; from now on fits into int32 unchanged */
        /* |h1| <= 1.01*2^24 */

        h[0] = (int32_t) h0;
        h[1] = (int32_t) h1;
        h[2] = (int32_t) h2;
        h[3] = (int32_t) h3;
        h[4] = (int32_t) h4;
        h[5] = (int32_t) h5;
        h[6] = (int32_t) h6;
        h[7] = (int32_t) h7;
        h[8] = (int32_t) h8;
        h[9] = (int32_t) h9;
    }

    static inline void fe25519_neg(fe25519 h, const fe25519 f)
    {
        int32_t h0 = -f[0];
        int32_t h1 = -f[1];
        int32_t h2 = -f[2];
        int32_t h3 = -f[3];
        int32_t h4 = -f[4];
        int32_t h5 = -f[5];
        int32_t h6 = -f[6];
        int32_t h7 = -f[7];
        int32_t h8 = -f[8];
        int32_t h9 = -f[9];

        h[0] = h0;
        h[1] = h1;
        h[2] = h2;
        h[3] = h3;
        h[4] = h4;
        h[5] = h5;
        h[6] = h6;
        h[7] = h7;
        h[8] = h8;
        h[9] = h9;
    }

    static void fe25519_sq(fe25519 h, const fe25519 f)
    {
        int32_t f0 = f[0];
        int32_t f1 = f[1];
        int32_t f2 = f[2];
        int32_t f3 = f[3];
        int32_t f4 = f[4];
        int32_t f5 = f[5];
        int32_t f6 = f[6];
        int32_t f7 = f[7];
        int32_t f8 = f[8];
        int32_t f9 = f[9];

        int32_t f0_2  = 2 * f0;
        int32_t f1_2  = 2 * f1;
        int32_t f2_2  = 2 * f2;
        int32_t f3_2  = 2 * f3;
        int32_t f4_2  = 2 * f4;
        int32_t f5_2  = 2 * f5;
        int32_t f6_2  = 2 * f6;
        int32_t f7_2  = 2 * f7;
        int32_t f5_38 = 38 * f5; /* 1.959375*2^30 */
        int32_t f6_19 = 19 * f6; /* 1.959375*2^30 */
        int32_t f7_38 = 38 * f7; /* 1.959375*2^30 */
        int32_t f8_19 = 19 * f8; /* 1.959375*2^30 */
        int32_t f9_38 = 38 * f9; /* 1.959375*2^30 */

        int64_t f0f0    = f0 * (int64_t) f0;
        int64_t f0f1_2  = f0_2 * (int64_t) f1;
        int64_t f0f2_2  = f0_2 * (int64_t) f2;
        int64_t f0f3_2  = f0_2 * (int64_t) f3;
        int64_t f0f4_2  = f0_2 * (int64_t) f4;
        int64_t f0f5_2  = f0_2 * (int64_t) f5;
        int64_t f0f6_2  = f0_2 * (int64_t) f6;
        int64_t f0f7_2  = f0_2 * (int64_t) f7;
        int64_t f0f8_2  = f0_2 * (int64_t) f8;
        int64_t f0f9_2  = f0_2 * (int64_t) f9;
        int64_t f1f1_2  = f1_2 * (int64_t) f1;
        int64_t f1f2_2  = f1_2 * (int64_t) f2;
        int64_t f1f3_4  = f1_2 * (int64_t) f3_2;
        int64_t f1f4_2  = f1_2 * (int64_t) f4;
        int64_t f1f5_4  = f1_2 * (int64_t) f5_2;
        int64_t f1f6_2  = f1_2 * (int64_t) f6;
        int64_t f1f7_4  = f1_2 * (int64_t) f7_2;
        int64_t f1f8_2  = f1_2 * (int64_t) f8;
        int64_t f1f9_76 = f1_2 * (int64_t) f9_38;
        int64_t f2f2    = f2 * (int64_t) f2;
        int64_t f2f3_2  = f2_2 * (int64_t) f3;
        int64_t f2f4_2  = f2_2 * (int64_t) f4;
        int64_t f2f5_2  = f2_2 * (int64_t) f5;
        int64_t f2f6_2  = f2_2 * (int64_t) f6;
        int64_t f2f7_2  = f2_2 * (int64_t) f7;
        int64_t f2f8_38 = f2_2 * (int64_t) f8_19;
        int64_t f2f9_38 = f2 * (int64_t) f9_38;
        int64_t f3f3_2  = f3_2 * (int64_t) f3;
        int64_t f3f4_2  = f3_2 * (int64_t) f4;
        int64_t f3f5_4  = f3_2 * (int64_t) f5_2;
        int64_t f3f6_2  = f3_2 * (int64_t) f6;
        int64_t f3f7_76 = f3_2 * (int64_t) f7_38;
        int64_t f3f8_38 = f3_2 * (int64_t) f8_19;
        int64_t f3f9_76 = f3_2 * (int64_t) f9_38;
        int64_t f4f4    = f4 * (int64_t) f4;
        int64_t f4f5_2  = f4_2 * (int64_t) f5;
        int64_t f4f6_38 = f4_2 * (int64_t) f6_19;
        int64_t f4f7_38 = f4 * (int64_t) f7_38;
        int64_t f4f8_38 = f4_2 * (int64_t) f8_19;
        int64_t f4f9_38 = f4 * (int64_t) f9_38;
        int64_t f5f5_38 = f5 * (int64_t) f5_38;
        int64_t f5f6_38 = f5_2 * (int64_t) f6_19;
        int64_t f5f7_76 = f5_2 * (int64_t) f7_38;
        int64_t f5f8_38 = f5_2 * (int64_t) f8_19;
        int64_t f5f9_76 = f5_2 * (int64_t) f9_38;
        int64_t f6f6_19 = f6 * (int64_t) f6_19;
        int64_t f6f7_38 = f6 * (int64_t) f7_38;
        int64_t f6f8_38 = f6_2 * (int64_t) f8_19;
        int64_t f6f9_38 = f6 * (int64_t) f9_38;
        int64_t f7f7_38 = f7 * (int64_t) f7_38;
        int64_t f7f8_38 = f7_2 * (int64_t) f8_19;
        int64_t f7f9_76 = f7_2 * (int64_t) f9_38;
        int64_t f8f8_19 = f8 * (int64_t) f8_19;
        int64_t f8f9_38 = f8 * (int64_t) f9_38;
        int64_t f9f9_38 = f9 * (int64_t) f9_38;

        int64_t h0 = f0f0 + f1f9_76 + f2f8_38 + f3f7_76 + f4f6_38 + f5f5_38;
        int64_t h1 = f0f1_2 + f2f9_38 + f3f8_38 + f4f7_38 + f5f6_38;
        int64_t h2 = f0f2_2 + f1f1_2 + f3f9_76 + f4f8_38 + f5f7_76 + f6f6_19;
        int64_t h3 = f0f3_2 + f1f2_2 + f4f9_38 + f5f8_38 + f6f7_38;
        int64_t h4 = f0f4_2 + f1f3_4 + f2f2 + f5f9_76 + f6f8_38 + f7f7_38;
        int64_t h5 = f0f5_2 + f1f4_2 + f2f3_2 + f6f9_38 + f7f8_38;
        int64_t h6 = f0f6_2 + f1f5_4 + f2f4_2 + f3f3_2 + f7f9_76 + f8f8_19;
        int64_t h7 = f0f7_2 + f1f6_2 + f2f5_2 + f3f4_2 + f8f9_38;
        int64_t h8 = f0f8_2 + f1f7_4 + f2f6_2 + f3f5_4 + f4f4 + f9f9_38;
        int64_t h9 = f0f9_2 + f1f8_2 + f2f7_2 + f3f6_2 + f4f5_2;

        int64_t carry0;
        int64_t carry1;
        int64_t carry2;
        int64_t carry3;
        int64_t carry4;
        int64_t carry5;
        int64_t carry6;
        int64_t carry7;
        int64_t carry8;
        int64_t carry9;

        carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
        h1 += carry0;
        h0 -= carry0 * ((uint64_t) 1L << 26);
        carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
        h5 += carry4;
        h4 -= carry4 * ((uint64_t) 1L << 26);

        carry1 = (h1 + (int64_t)(1L << 24)) >> 25;
        h2 += carry1;
        h1 -= carry1 * ((uint64_t) 1L << 25);
        carry5 = (h5 + (int64_t)(1L << 24)) >> 25;
        h6 += carry5;
        h5 -= carry5 * ((uint64_t) 1L << 25);

        carry2 = (h2 + (int64_t)(1L << 25)) >> 26;
        h3 += carry2;
        h2 -= carry2 * ((uint64_t) 1L << 26);
        carry6 = (h6 + (int64_t)(1L << 25)) >> 26;
        h7 += carry6;
        h6 -= carry6 * ((uint64_t) 1L << 26);

        carry3 = (h3 + (int64_t)(1L << 24)) >> 25;
        h4 += carry3;
        h3 -= carry3 * ((uint64_t) 1L << 25);
        carry7 = (h7 + (int64_t)(1L << 24)) >> 25;
        h8 += carry7;
        h7 -= carry7 * ((uint64_t) 1L << 25);

        carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
        h5 += carry4;
        h4 -= carry4 * ((uint64_t) 1L << 26);
        carry8 = (h8 + (int64_t)(1L << 25)) >> 26;
        h9 += carry8;
        h8 -= carry8 * ((uint64_t) 1L << 26);

        carry9 = (h9 + (int64_t)(1L << 24)) >> 25;
        h0 += carry9 * 19;
        h9 -= carry9 * ((uint64_t) 1L << 25);

        carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
        h1 += carry0;
        h0 -= carry0 * ((uint64_t) 1L << 26);

        h[0] = (int32_t) h0;
        h[1] = (int32_t) h1;
        h[2] = (int32_t) h2;
        h[3] = (int32_t) h3;
        h[4] = (int32_t) h4;
        h[5] = (int32_t) h5;
        h[6] = (int32_t) h6;
        h[7] = (int32_t) h7;
        h[8] = (int32_t) h8;
        h[9] = (int32_t) h9;
    }

    static void fe25519_sub(fe25519 h, const fe25519 f, const fe25519 g)
    {
        int32_t h0 = f[0] - g[0];
        int32_t h1 = f[1] - g[1];
        int32_t h2 = f[2] - g[2];
        int32_t h3 = f[3] - g[3];
        int32_t h4 = f[4] - g[4];
        int32_t h5 = f[5] - g[5];
        int32_t h6 = f[6] - g[6];
        int32_t h7 = f[7] - g[7];
        int32_t h8 = f[8] - g[8];
        int32_t h9 = f[9] - g[9];

        h[0] = h0;
        h[1] = h1;
        h[2] = h2;
        h[3] = h3;
        h[4] = h4;
        h[5] = h5;
        h[6] = h6;
        h[7] = h7;
        h[8] = h8;
        h[9] = h9;
    }

    static void fe25519_pow22523(fe25519 out, const fe25519 z)
    {
        fe25519 t0, t1, t2;
        int     i;

        fe25519_sq(t0, z);
        fe25519_sq(t1, t0);
        fe25519_sq(t1, t1);
        fe25519_mul(t1, z, t1);
        fe25519_mul(t0, t0, t1);
        fe25519_sq(t0, t0);
        fe25519_mul(t0, t1, t0);
        fe25519_sq(t1, t0);
        for (i = 1; i < 5; ++i) {
            fe25519_sq(t1, t1);
        }
        fe25519_mul(t0, t1, t0);
        fe25519_sq(t1, t0);
        for (i = 1; i < 10; ++i) {
            fe25519_sq(t1, t1);
        }
        fe25519_mul(t1, t1, t0);
        fe25519_sq(t2, t1);
        for (i = 1; i < 20; ++i) {
            fe25519_sq(t2, t2);
        }
        fe25519_mul(t1, t2, t1);
        for (i = 1; i < 11; ++i) {
            fe25519_sq(t1, t1);
        }
        fe25519_mul(t0, t1, t0);
        fe25519_sq(t1, t0);
        for (i = 1; i < 50; ++i) {
            fe25519_sq(t1, t1);
        }
        fe25519_mul(t1, t1, t0);
        fe25519_sq(t2, t1);
        for (i = 1; i < 100; ++i) {
            fe25519_sq(t2, t2);
        }
        fe25519_mul(t1, t2, t1);
        for (i = 1; i < 51; ++i) {
            fe25519_sq(t1, t1);
        }
        fe25519_mul(t0, t1, t0);
        fe25519_sq(t0, t0);
        fe25519_sq(t0, t0);
        fe25519_mul(out, t0, z);
    }

    int ge25519_is_canonical(const unsigned char *s)
    {
        unsigned char c;
        unsigned char d;
        unsigned int  i;

        c = (s[31] & 0x7f) ^ 0x7f;
        for (i = 30; i > 0; i--) {
            c |= s[i] ^ 0xff;
        }
        c = (((unsigned int) c) - 1U) >> 8;
        d = (0xed - 1U - (unsigned int) s[0]) >> 8;

        return 1 - (c & d & 1);
    }

    int ge25519_frombytes(ge25519_p3 *h, const unsigned char *s)
    {
        fe25519 u;
        fe25519 v;
        fe25519 vxx;
        fe25519 m_root_check, p_root_check;
        fe25519 negx;
        fe25519 x_sqrtm1;
        int     has_m_root, has_p_root;

        fe25519_frombytes(h->Y, s);
        fe25519_1(h->Z);
        fe25519_sq(u, h->Y);
        fe25519_mul(v, u, ed25519_d);
        fe25519_sub(u, u, h->Z); /* u = y^2-1 */
        fe25519_add(v, v, h->Z); /* v = dy^2+1 */

        fe25519_mul(h->X, u, v);
        fe25519_pow22523(h->X, h->X);
        fe25519_mul(h->X, u, h->X); /* u((uv)^((q-5)/8)) */

        fe25519_sq(vxx, h->X);
        fe25519_mul(vxx, vxx, v);
        fe25519_sub(m_root_check, vxx, u); /* vx^2-u */
        fe25519_add(p_root_check, vxx, u); /* vx^2+u */
        has_m_root = fe25519_iszero(m_root_check);
        has_p_root = fe25519_iszero(p_root_check);
        fe25519_mul(x_sqrtm1, h->X, fe25519_sqrtm1); /* x*sqrt(-1) */
        fe25519_cmov(h->X, x_sqrtm1, 1 - has_m_root);

        fe25519_neg(negx, h->X);
        fe25519_cmov(h->X, negx, fe25519_isnegative(h->X) ^ (s[31] >> 7));
        fe25519_mul(h->T, h->X, h->Y);

        return (has_m_root | has_p_root) - 1;
    }

  } // namespace libsodium

}
