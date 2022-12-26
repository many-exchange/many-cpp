//  ____ _____ __    _____ _____ _____
// |  __|     |  |  |  _  |   | |  _  |  Solana C++ SDK
// |__  |  |  |  |__|     | | | |     |  version 0.0.1
// |____|_____|_____|__|__|_|___|__|__|  https://github.com/many-exchange/solana-sdk-cpp
//
// base58: 67
// base64: 282
// http: 407
// websockets: 703
// solana: 1145
//   - connection: 1846
//     - http methods: 1887
//     - websocket methods: 2228
//
// Copyright (c) 2022-2023 Many Exchange
//
// This program is free software: you can redistribute it 
// and/or modify it under the terms of the standard MIT license.

#pragma once

#include <algorithm>
#include <arpa/inet.h>
#include <array>
#include <assert.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/ssl.h>
#include <sodium.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

using json = nlohmann::json;

#define LAMPORTS_PER_SOL 1000000000

#define NATIVE_MINT PublicKey("So11111111111111111111111111111111111111112")

#define SYSTEM_PROGRAM PublicKey("11111111111111111111111111111111")
#define SYSVAR_RENT_PUBKEY PublicKey("SysvarRent111111111111111111111111111111111")
#define SYSVAR_CLOCK_PUBKEY PublicKey("SysvarC1ock11111111111111111111111111111111")
#define SYSVAR_REWARDS_PUBKEY PublicKey("SysvarRewards1111111111111111111111111111111")
#define SYSVAR_STAKE_HISTORY_PUBKEY PublicKey("SysvarStakeHistory1111111111111111111111111")
#define SYSVAR_INSTRUCTIONS_PUBKEY PublicKey("Sysvar1nstructions1111111111111111111111111")

#define TOKEN_PROGRAM_ID PublicKey("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA")
#define ASSOCIATED_TOKEN_ID PublicKey("ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL")

#define PUBLIC_KEY_LENGTH 32

#define SIGNATURE_LENGTH 64

namespace solana {

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
    inline std::string b58encode(const T &str) {
      size_t b58Size = str.size() * 138 / 100 + 1;
      std::string b58(b58Size, '\0');
      const bool ok = b58enc(b58.data(), &b58Size, str.data(), str.size());
      if (ok) {
        // remove zero byte at end of result
        return std::string(b58.data(), b58Size - 1);
      } else
        return std::string();
    }

  }

  namespace base64 {

    static const char base64_encoding_table[] = {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
      'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
      'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
      'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
      'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
      'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
      'w', 'x', 'y', 'z', '0', '1', '2', '3',
      '4', '5', '6', '7', '8', '9', '+', '/' };

    static int mod_table[] = {0, 2, 1};

    int encode(const char *input, const int input_length, char *output, const int max_output_length) {
      int output_length = 4 * ((input_length + 2) / 3);
      assert(output_length <= max_output_length);

      for (int i = 0, j = 0; i < input_length;) {
        int octet_a = i < input_length ? input[i++] : 0;
        int octet_b = i < input_length ? input[i++] : 0;
        int octet_c = i < input_length ? input[i++] : 0;

        int triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        output[j++] = base64_encoding_table[(triple >> 3 * 6) & 0x3F];
        output[j++] = base64_encoding_table[(triple >> 2 * 6) & 0x3F];
        output[j++] = base64_encoding_table[(triple >> 1 * 6) & 0x3F];
        output[j++] = base64_encoding_table[(triple >> 0 * 6) & 0x3F];
      }

      for (int i = 0; i < mod_table[input_length % 3]; i++)
          output[output_length - 1 - i] = '=';

      return output_length;
    }

    static const char base64_decoding_table[] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,
      0, 0, 0, -1, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
      14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, 0, 26,
      27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
      45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    int decode(const char *input, const int input_length, char *output, const int max_output_length) {
      assert(input_length % 4 == 0);

      int output_length = input_length / 4 * 3;
      assert(output_length <= max_output_length);
      if (input[input_length - 1] == '=') (output_length)--;
      if (input[input_length - 2] == '=') (output_length)--;

      for (int i = 0, j = 0; i < input_length;) {
        int sextet_a = input[i] == '=' ? 0 & i++ : base64_decoding_table[(int)input[i++]];
        int sextet_b = input[i] == '=' ? 0 & i++ : base64_decoding_table[(int)input[i++]];
        int sextet_c = input[i] == '=' ? 0 & i++ : base64_decoding_table[(int)input[i++]];
        int sextet_d = input[i] == '=' ? 0 & i++ : base64_decoding_table[(int)input[i++]];

        int triple = (sextet_a << 3 * 6)
                      + (sextet_b << 2 * 6)
                      + (sextet_c << 1 * 6)
                      + (sextet_d << 0 * 6);

        if (j < output_length) output[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length) output[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length) output[j++] = (triple >> 0 * 8) & 0xFF;
      }

      return output_length;
    }

    //TODO DELETE
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    //TODO DELETE
    std::string base64_encode(unsigned char const* buf, unsigned int bufLen) {
      std::string ret;
      int i = 0;
      int j = 0;
      unsigned char char_array_3[3];
      unsigned char char_array_4[4];

      while (bufLen--) {
        char_array_3[i++] = *(buf++);
        if (i == 3) {
          char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
          char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
          char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
          char_array_4[3] = char_array_3[2] & 0x3f;

          for(i = 0; (i <4) ; i++)
            ret += base64_chars[char_array_4[i]];
          i = 0;
        }
      }

      if (i) {
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

  }

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
        : _url(url), 
        _port(port), 
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
     * 
     * @param url The url endpoint for the POST request
     * @param request The json request object
     */
    json post(const std::string url, json request) {
      HttpClient client(url, 443);
      client.connect();
      //TODO return error if not connected
      assert(client.is_connected());
      int response_length = 0;
      char* response = client.post(request, &response_length);
      client.disconnect();

      std::cout << response << std::endl;
      //std::cout << "response_length = " << response_length << std::endl;

      return json::parse(std::string(response, response_length));
    }

  }

  namespace websockets {

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

      void send_message(uint8_t opcode, const char* message, size_t message_size) {
        int send_length = 0;

        _send_buffer[send_length] = opcode | FIN_FLAG;
        send_length++;

        //size_t message_size = message.size();

        if (message_size <= 125) {
          _send_buffer[send_length] = message_size | MASK_FLAG;
          send_length++;
        }
        else if (message_size <= 65535) {
          _send_buffer[send_length] = (uint8_t)(126 | MASK_FLAG);
          _send_buffer[send_length + 1] = (message_size >> 8) & 0xFF;
          _send_buffer[send_length + 2] = message_size & 0xFF;
          send_length += 3;
        }
        else {
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
        for (size_t i = 0; i < message_size; ++i) {
            _send_buffer[send_length + i] = message[i] ^ _send_mask[i % 4];
        }

        send_length += message_size;

        write(_send_buffer, send_length);
      }

      bool validate_handshake(char *buffer, int length) {
        bool accept = false;

        int start = 0;
        int end = 0;
        while (end < length) {
          end++;

          if (buffer[end] == '\n') {
            if (strncmp(&buffer[start], "Connection: ", 12) == 0) {
              if (strncmp(&buffer[start], "Connection: Upgrade", 19) != 0 && strncmp(&buffer[start], "Connection: upgrade", 19) != 0) {
                return false;
              }
            }
            else if (strncmp(&buffer[start], "Upgrade: ", 9) == 0) {
              if (strncmp(&buffer[start], "Upgrade: websocket", 18) != 0) {
                return false;
              }
            }
            else if (strncmp(&buffer[start], "Sec-WebSocket-Accept: ", 22) == 0) {
              std::string key = base64::base64_encode(_nonce.begin(), 16) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

              char sha1[20];
              SHA1((const unsigned char *)key.data(), key.size(), (unsigned char *)sha1);
              std::string hash = base64::base64_encode((unsigned char *)sha1, 20);

              accept = (strncmp(&buffer[start + 22], hash.c_str(), 20) == 0);
            }

            start = end + 1;
          }
        }

        return accept;
      }

      bool write(const void* buffer, const int length) {
        if (is_connected()) {
          return false;
        }

        assert(length > 0);
        int return_code = SSL_write(_ssl, buffer, length);

        if (_handshake_complete) {
          //TODO: log
        }

        if (return_code <= 0) {
            int err = SSL_get_error(_ssl, length);
            switch (err) {
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
        : _url(url), 
        _port(port),
        _interface(interface), 
        _socket(-1), 
        _ssl_ctx(nullptr), 
        _ssl(nullptr) 
      {
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

      bool is_connected() {
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
        while (server->h_addr_list[i] != NULL) {
          if (connect(hostname, (struct in_addr*)server->h_addr_list[i], _port, resource)) {
            return true;
          }
          i++;
        }

        return false;
      }

    private:

      bool connect(const std::string& hostname, struct in_addr *addr, uint16_t tcp_port, const std::string& resource) {
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

        std::generate(_nonce.begin(), _nonce.end(), []() { return (uint8_t)std::rand(); });
        std::string websocket_key = base64::base64_encode(_nonce.begin(), 16);

        int send_length = 0;
        send_length += sprintf(&_send_buffer[send_length], "GET %s HTTP/1.1\r\n", resource.c_str());
        send_length += sprintf(&_send_buffer[send_length], "Host: %s:%u\r\n", hostname.c_str(), tcp_port);
        send_length += sprintf(&_send_buffer[send_length], "Upgrade: websocket\r\n");
        send_length += sprintf(&_send_buffer[send_length], "Connection: Upgrade\r\n");
        send_length += sprintf(&_send_buffer[send_length], "Sec-WebSocket-Key: %s\r\n", websocket_key.c_str());
        send_length += sprintf(&_send_buffer[send_length], "Sec-WebSocket-Version: 13\r\n");
        send_length += sprintf(&_send_buffer[send_length], "\r\n");

        write(_send_buffer, send_length);

        while (is_connected()) {
          int length;
          char *buffer = begin_read(length);

          if (length > 0) {
            if (validate_handshake(buffer, length)) {
              end_read(0);
              _handshake_complete = true;
              *((uint32_t *)_send_mask) = rand();
              return true;
            }
            else {
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

      bool can_read() {
        return is_connected() && _handshake_complete;
      }

      char* begin_read(int& length) {
        length = 0;

        if (!is_connected()) {
          return nullptr;
        }

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(_socket, &readfds);
        int rv = select(_socket + 1, &readfds, NULL, NULL, &tv);

        if(rv > 0 && FD_ISSET(_socket, &readfds)) {
          length = SSL_read(_ssl, _receive_buffer, 8192);

          if (length > 0) {
            return _receive_buffer;
          }
          else if (length < 0) {
            std::cerr << "READ FAILED" << std::endl;

            switch (SSL_get_error(_ssl, length)) {
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
        else {
          return nullptr;
        }
      }

      void end_read(uint64_t timestamp) {
        if (_handshake_complete) {
          //TODO: log
        }
      }

      void send_close(std::string message) {
        send_message(OPCODE_CLOSE, message.c_str(), message.size());
      }

      void send_ping(std::string message) {
        send_message(OPCODE_PING, message.c_str(), message.size());
      }

      void send_pong(std::string message) {
        send_message(OPCODE_PONG, message.c_str(), message.size());
      }

      void send_text(std::string message) {
        send_message(OPCODE_TEXT, message.c_str(), message.size());
      }

      void send_text(const char* message, size_t message_size) {
        send_message(OPCODE_TEXT, message, message_size);
      }

    };

  }
  
  //-------- Solana --------------------------------------------------------------------

  enum class Cluster {
    Devnet,
    Testnet,
    MainnetBeta
  };

  /**
   * Returns a default cluster API URL for a given cluster.
   * 
   * @param cluster The cluster to get the default API URL for
  */
  std::string clusterApiUrl(Cluster cluster) {
    switch (cluster) {
      case Cluster::Devnet:
        return "https://api.devnet.solana.com";
      case Cluster::Testnet:
        return "https://api.testnet.solana.com";
      case Cluster::MainnetBeta:
        return "https://api.mainnet-beta.solana.com";
      default:
        throw std::runtime_error("Invalid cluster.");
    }
  }

  enum class Commitment {
    Processed,
    Confirmed,
    Finalized,
  };

  struct Context {
    uint64_t slot;
  };

  int decodeLength(std::vector<uint8_t> bytes) {
    int len = 0;
    int size = 0;
    for (;;) {
      int elem = bytes[0];
      len |= (elem & 0x7f) << (size * 7);
      size += 1;
      if ((elem & 0x80) == 0) {
        break;
      }
    }
    return len;
  }

  std::vector<uint8_t> encodeLength(int len) {
    std::vector<uint8_t> bytes;
    while (len > 0) {
      int elem = len & 0x7f;
      len >>= 7;
      if (len > 0) {
        elem |= 0x80;
      }
      bytes.push_back(elem);
    }
    return bytes;
  }

  const std::vector<uint8_t> DEFAULT_SIGNATURE = {0};

  struct PublicKey {
    /* An array of bytes representing the public key */
    std::array<uint8_t, PUBLIC_KEY_LENGTH> bytes;

    PublicKey() {
      for (int i = 0; i < PUBLIC_KEY_LENGTH; i++) {
        bytes[i] = 0;
      }
    }

    PublicKey(const std::string& value) {
      size_t size = PUBLIC_KEY_LENGTH;
      base58::b58tobin(bytes.data(), &size, value.c_str(), 0);
    }

    PublicKey(const uint8_t* value) {
      for (int i = 0; i < PUBLIC_KEY_LENGTH; i++) {
        bytes[i] = value[i];
      }
    }

    PublicKey(const PublicKey& other) {
      for (int i = 0; i < PUBLIC_KEY_LENGTH; i++) {
        bytes[i] = other.bytes[i];
      }
    }

    bool operator==(const PublicKey& other) const {
      for (int i = 0; i < PUBLIC_KEY_LENGTH; i++) {
        if (bytes[i] != other.bytes[i]) {
          return false;
        }
      }
      return true;
    }

    std::string toBase58() const {
      char temp[45];
      memset(temp, 0, 45);
      size_t size = 45;
      base58::b58enc(temp, &size, bytes.data(), bytes.size());
      return std::string(temp, size - 1);
    }
  };

  void from_json(const json& j, PublicKey& publicKey) {
    publicKey = PublicKey(j.get<std::string>());
  }

  void to_json(json& j, const PublicKey& publicKey) {
    j = publicKey.toBase58();
  }

  struct Keypair {
    std::array<uint8_t, crypto_sign_SECRETKEYBYTES> secretKey;
    PublicKey publicKey;

    Keypair() {
      for (int i = 0; i < crypto_sign_SECRETKEYBYTES; i++) {
        secretKey[i] = 0;
      }
      publicKey = PublicKey();
    }

    /**
     * Create a keypair from a raw secret key byte array.
     *
     * This method should only be used to recreate a keypair from a previously
     * generated secret key. Generating keypairs from a random seed should be done
     * with the {@link Keypair.fromSeed} method.
     *
     * @throws error if the provided secret key is invalid and validation is not skipped.
     *
     * @param secretKey secret key byte array
     * @param options: skip secret key validation
     */
    Keypair(std::array<char, PUBLIC_KEY_LENGTH> secretKey, bool skipValidation = false) {
      if (!skipValidation && crypto_sign_ed25519_sk_to_pk((unsigned char *)publicKey.bytes.data(), (unsigned char *)secretKey.data()) != 0) {
        throw std::runtime_error("invalid secret key");
      }
    }

    ~Keypair() {
    }

    /**
     * Read a keypair from a file.
     *
     * @throws error if the keypair file cannot be read.
     *
     * @param path path to keypair file
     */
    static Keypair fromFile(const std::string &path) {
      Keypair result = Keypair();
      std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
      if (file.is_open()) {
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        if (size == crypto_sign_SECRETKEYBYTES) {
          file.read((char *)result.secretKey.data(), size);
          crypto_sign_ed25519_sk_to_pk((unsigned char *)result.publicKey.bytes.data(), (unsigned char *)result.secretKey.data());
        } else {
          throw std::runtime_error("invalid secret key file");
        }
      } else {
        throw std::runtime_error("could not open secret key file");
      }
      return result;
    }

    /**
     * Create a keypair from a secret key seed.
     *
     * @throws error if the provided seed is invalid.
     *
     * @param seed secret key seed
     */
    static Keypair fromSeed(const uint8_t *seed) {
      Keypair result = Keypair();
      crypto_sign_seed_keypair((unsigned char *)result.publicKey.bytes.data(), (unsigned char *)result.secretKey.data(), seed);
      return result;
    }

    /**
     * Create a new random keypair.
     */
    static Keypair generate() {
      Keypair result = Keypair();
      crypto_sign_keypair((unsigned char *)result.publicKey.bytes.data(), (unsigned char *)result.secretKey.data());
      return result;
    }

    /**
     * Sign a message with the keypair's secret key.
     *
     * @param message message to sign
     */
    std::string sign(const std::vector<uint8_t> message) const {
      uint8_t sig[crypto_sign_BYTES];
      unsigned long long sigSize;
      if (0 != crypto_sign_detached(sig, &sigSize, message.data(), message.size(), secretKey.data())) {
        throw std::runtime_error("could not sign tx with private key");
      }
      return std::string((const char *)sig, sigSize);
    }
  };

  struct AccountInfo {
    /** Number of lamports assigned to this account */
    uint64_t lamports;
    /** Identifier of the program that owns the account */
    PublicKey owner;
    /** Data associated with the account */
    std::string data;
    /** Boolean indicating if the account contains a program (and is strictly read-only) */
    bool executable;
    /** The epoch at which this account will next owe rent */
    uint64_t rentEpoch;
  };

  void from_json(const json& j, AccountInfo& accountInfo) {
    accountInfo.executable = j["executable"].get<bool>();
    accountInfo.owner = j["owner"].get<PublicKey>();
    accountInfo.lamports = j["lamports"].get<uint64_t>();
    accountInfo.data = j["data"].get<std::string>();
    accountInfo.rentEpoch = j["rentEpoch"].get<uint64_t>();
  }

  struct AccountChangeSubscriptionInfo {
    /* The public key of the account to subscribe to */
    PublicKey accountId;
    /* The callback function that will fire on a subscription event for the account */
    std::function<void(Context context, AccountInfo accountInfo)> callback;
  };

  struct KeyedAccountInfo {
    PublicKey accountId;
    AccountInfo accountInfo;
  };

  struct Logs {
    //TODO
    //err: TransactionError | null;
    std::vector<std::string> logs;
    std::string signature;
  };

  struct LogsSubscriptionInfo {
    /* The public key of the account to subscribe to for logs */
    PublicKey accountId;
    /* The callback function that will fire on a subscription event for logs */
    std::function<void(Context context, Logs logs)> callback;
  };

  struct ProgramAccountChangeSubscriptionInfo {
    /* The public key of the program to subscribe to */
    PublicKey programId;
    /* The callback function that will fire on a subscription event for the program */
    std::function<void(Context context, KeyedAccountInfo keyedAccountInfo)> callback;
  };

  struct SimulatedTransactionResponse {
    /* Error if the transaction failed */
    std::string err;
    /* Array of log messages the transaction instructions output during execution */
    std::vector<std::string> logs;
    /* Array of accounts with the same length as the accounts.addresses array in the request */
    std::vector<AccountInfo> accounts;
    /* The number of compute budget units consumed during the processing of this transaction */
    uint64_t unitsConsumed;
    /* The return data for the simulated transaction */
    struct ReturnData {
      /* The program that generated the return data */
      PublicKey programId;
      /* The return data itself */
      std::string data;
    } returnData;
  };

  struct SlotInfo {
    /** Currently processing slot */
    uint64_t slot;
    /** Parent of the current slot */
    uint64_t parent;
    /** The root block of the current slot's fork */
    uint64_t root;
  };

  struct TokenAmount {
    /** The raw balance without decimals, as a string representation */
    std::string amount;
    /** The number of base 10 digits to the right of the decimal place */
    uint64_t decimals;
    /** The balance as a string, using mint-prescribed decimals */
    std::string uiAmountString;
  };

  void from_json(const json& j, TokenAmount& tokenAmount) {
    tokenAmount.amount = j["amount"].get<std::string>();
    tokenAmount.decimals = j["decimals"].get<uint64_t>();
    tokenAmount.uiAmountString = j["uiAmountString"].get<std::string>();
  }

  struct TokenAccount {
    /** The account's Pubkey */
    PublicKey pubkey;
    /** The account data */
    struct TokenAccountInfo {
      /** Number of lamports assigned to the account */
      uint64_t lamports;
      /** The Pubkey of the program that owns the account */
      PublicKey owner;
      struct TokenAccountData {
        /* Name of token program (ex: "spl-token") */
        PublicKey program;
        struct ParsedTokenAccountData {
          /** The account type (ex: "account") */
          std::string accountType;
          struct TokenAccountBalanceInfo {
            /** The amount of tokens */
            TokenAmount tokenAmount;
            /** The account this token account is delegated to */
            PublicKey delegate;
            /** Amount delegated */
            TokenAmount delegatedAmount;
            /** State of the token account (ex: "initialized") */
            std::string state;
            /** Boolean indicating if the account is native */
            bool isNative;
            /** The mint of the token */
            PublicKey mint;
            /** The Pubkey of the token account owner */
            PublicKey owner;
          } info;
          /** The account type (ex: "account") */
          std::string type;
        } parsed;
        /** The length of the account data */
        uint64_t space;
      } data;
      /** Boolean indicating if the account contains a program (and is strictly read-only) */
      bool executable;
      /** The epoch at which this account will next owe rent */
      uint64_t rentEpoch;
    } account;
  };

  void from_json(const json& j, TokenAccount::TokenAccountInfo& tokenAccountInfo) {
    tokenAccountInfo.data.program = j["data"]["program"].get<PublicKey>();
    tokenAccountInfo.data.parsed.accountType = j["data"]["parsed"]["accountType"].get<std::string>();
    tokenAccountInfo.data.parsed.info.tokenAmount.amount = j["data"]["parsed"]["info"]["tokenAmount"]["amount"].get<std::string>();
    tokenAccountInfo.data.parsed.info.tokenAmount.decimals = j["data"]["parsed"]["info"]["tokenAmount"]["decimals"].get<uint64_t>();
    tokenAccountInfo.data.parsed.info.tokenAmount.uiAmountString = j["data"]["parsed"]["info"]["tokenAmount"]["uiAmountString"].get<std::string>();
    tokenAccountInfo.data.parsed.info.delegate = j["data"]["parsed"]["info"]["delegate"].get<PublicKey>();
    tokenAccountInfo.data.parsed.info.delegatedAmount.amount = j["data"]["parsed"]["info"]["delegatedAmount"]["amount"].get<std::string>();
    tokenAccountInfo.data.parsed.info.delegatedAmount.decimals = j["data"]["parsed"]["info"]["delegatedAmount"]["decimals"].get<uint64_t>();
    tokenAccountInfo.data.parsed.info.delegatedAmount.uiAmountString = j["data"]["parsed"]["info"]["delegatedAmount"]["uiAmountString"].get<std::string>();
    tokenAccountInfo.data.parsed.info.state = j["data"]["parsed"]["info"]["state"].get<std::string>();
    tokenAccountInfo.data.parsed.info.isNative = j["data"]["parsed"]["info"]["isNative"].get<bool>();
    tokenAccountInfo.data.parsed.info.mint = j["data"]["parsed"]["info"]["mint"].get<PublicKey>();
    tokenAccountInfo.data.parsed.info.owner = j["data"]["parsed"]["info"]["owner"].get<PublicKey>();
    tokenAccountInfo.data.parsed.type = j["data"]["parsed"]["type"].get<std::string>();
    tokenAccountInfo.data.space = j["data"]["space"].get<uint64_t>();
    tokenAccountInfo.executable = j["executable"].get<bool>();
    tokenAccountInfo.lamports = j["lamports"].get<uint64_t>();
    tokenAccountInfo.owner = j["owner"].get<PublicKey>();
    tokenAccountInfo.rentEpoch = j["rentEpoch"].get<uint64_t>();
  }

  void from_json(const json& j, TokenAccount& tokenAccount) {
    tokenAccount.account = j["account"].get<TokenAccount::TokenAccountInfo>();
    tokenAccount.pubkey = j["pubkey"].get<PublicKey>();
  }

  struct Transaction {
    /* The transaction signatures */
    std::vector<std::string> signatures;
    /* The transaction message */
    struct Message {
      /* The message header */
      struct Header {
        /* The number of signatures required to validate this transaction */
        uint8_t numRequiredSignatures;
        /* The number of read-only signed accounts */
        uint8_t numReadonlySignedAccounts;
        /* The number of read-only unsigned accounts */
        uint8_t numReadonlyUnsignedAccounts;
      } header;
      /* The account keys used by this transaction */
      std::vector<PublicKey> accountKeys;
      /* Recent blockhash */
      std::string recentBlockhash;
      struct Instruction {
        /* The program id that executes this instruction */
        PublicKey programId;
        /**
         * Account metadata used to define instructions
         */
        struct AccountMeta {
          /** An account's public key */
          PublicKey pubkey;
          /** True if an instruction requires a transaction signature matching `pubkey` */
          bool isSigner;
          /** True if the `pubkey` can be loaded as a read-write account. */
          bool isWritable;
          AccountMeta() : pubkey(), isSigner(false), isWritable(false) { }
          AccountMeta(PublicKey pubkey, bool isSigner, bool isWritable) : pubkey(pubkey), isSigner(isSigner), isWritable(isWritable) { }
          AccountMeta(PublicKey pubkey, bool isSigner) : pubkey(pubkey), isSigner(isSigner), isWritable(false) { }
          AccountMeta(PublicKey pubkey) : pubkey(pubkey), isSigner(false), isWritable(false) { }
        };
        /* Ordered indices into the transaction keys array indicating which accounts to pass to the program */
        std::vector<AccountMeta> accounts;
        /* Program input data */
        std::vector<uint8_t> data;
      };
      /* The program instructions */
      std::vector<Instruction> instructions;

      std::vector<uint8_t> serialize() {
        if (recentBlockhash.empty()) {
          throw std::runtime_error("recentBlockhash required");
        }
        if (instructions.empty()) {
          throw std::runtime_error("No instructions provided");
        }

        std::vector<uint8_t> buffer;

        buffer.push_back(header.numRequiredSignatures);
        buffer.push_back(header.numReadonlySignedAccounts);
        buffer.push_back(header.numReadonlyUnsignedAccounts);

        std::vector<uint8_t> accountAddressesLength = encodeLength(accountKeys.size());
        buffer.insert(buffer.end(), accountAddressesLength.begin(), accountAddressesLength.end());

        for (PublicKey accountKey : accountKeys) {
          std::array<uint8_t, PUBLIC_KEY_LENGTH> accountAddress = accountKey.bytes;
          buffer.insert(buffer.end(), accountAddress.begin(), accountAddress.end());
        }

        std::string recentBlockhashBytes = base58::decode(recentBlockhash);
        buffer.insert(buffer.end(), recentBlockhashBytes.begin(), recentBlockhashBytes.end());

        std::vector<uint8_t> instructionsLength = encodeLength(instructions.size());
        buffer.insert(buffer.end(), instructionsLength.begin(), instructionsLength.end());

        for (Instruction instruction : instructions) {
          std::array<uint8_t, PUBLIC_KEY_LENGTH>  programIdBytes = instruction.programId.bytes;
          buffer.insert(buffer.end(), programIdBytes.begin(), programIdBytes.end());

          std::vector<uint8_t> accountsLength = encodeLength(instruction.accounts.size());
          buffer.insert(buffer.end(), accountsLength.begin(), accountsLength.end());

          for (Instruction::AccountMeta accountMeta : instruction.accounts) {
            std::array<uint8_t, PUBLIC_KEY_LENGTH>  accountMetaBytes = accountMeta.pubkey.bytes;
            buffer.insert(buffer.end(), accountMetaBytes.begin(), accountMetaBytes.end());

            uint8_t accountMetaFlags = 0;
            if (accountMeta.isSigner) {
              accountMetaFlags |= 1;
            }
            if (accountMeta.isWritable) {
              accountMetaFlags |= 2;
            }
            buffer.push_back(accountMetaFlags);
          }

          std::vector<uint8_t> dataLength = encodeLength(instruction.data.size());
          buffer.insert(buffer.end(), dataLength.begin(), dataLength.end());

          buffer.insert(buffer.end(), instruction.data.begin(), instruction.data.end());
        }

        return buffer;
      }
    } message;

    std::vector<uint8_t> serialize() {
      std::vector<uint8_t> buffer;
      std::vector<uint8_t> signaturesLength = encodeLength(signatures.size());
      buffer.insert(buffer.end(), signaturesLength.begin(), signaturesLength.end());
      for (auto& signature : signatures) {
        std::string rawSignature = base58::decode(signature);
        buffer.insert(buffer.end(), rawSignature.begin(), rawSignature.end());
      }
      auto serializedMessage = message.serialize();
      buffer.insert(buffer.end(), serializedMessage.begin(), serializedMessage.end());
      return buffer;
    }

    void sign(const std::vector<Keypair> signers) {
      if (signers.empty()) {
        throw std::runtime_error("No signers");
      }
      PublicKey feePayer = signers[0].publicKey;
      std::vector<uint8_t> serializedMessage = message.serialize();
      for (Keypair signer : signers) {
        auto signature = signer.sign(serializedMessage);
        //TODO base58 encode the message
        signatures.push_back(signature);
      }
    }
  };

  struct TransactionReponse {
    /* The slot this transaction was processed in */
    uint64_t slot;
    /* The transaction */
    struct Transaction {
      /* Defines the content of the transaction */
      struct Message {
        /* List of base-58 encoded public keys used by the transaction, including by the instructions and for signatures.
        The first message.header.numRequiredSignatures public keys must sign the transaction. */
        std::vector<PublicKey> accountKeys;
        struct Header {
          /* The number of signatures required to validate this transaction */
          uint8_t numRequiredSignatures;
          /* The number of read-only signed accounts */
          uint8_t numReadonlySignedAccounts;
          /* The number of read-only unsigned accounts */
          uint8_t numReadonlyUnsignedAccounts;
        } header;
        struct Instruction {
          /* Ordered indices into the `message.accountKeys` array indicating which accounts to pass to the program */
          std::vector<uint8_t> accounts;
          /* The program input data encoded as base-58 */
          std::string data;
          /* Index into the `message.accountKeys` array indicating the program account that executes this instruction */
          uint8_t programIdIndex;
        };
        /* List of program instructions that will be executed in sequence and committed in one atomic transaction if all succeed. */
        std::vector<Instruction> instructions;
        /* A base-58 encoded hash of a recent block in the ledger used to prevent transaction duplication and to give transactions lifetimes. */
        std::string recentBlockhash;
      } message;
      /* The transaction signatures */
      std::vector<std::string> signatures;
    } transaction;
    /* The estimated production time of when the transaction was processed. */
    uint64_t blockTime;
    /* Transaction status metadata object */
    struct Meta {
      /* Error if the transaction failed */
      std::string err;
      /* Fee this transaction was charged, as u64 integer */
      uint64_t fee;
      /* List of inner instructions if inner instruction recording was enabled during this transaction */
      //TODO this should be an array of inner instruction std::string innerInstructions;
      struct LoadedAddresses {
        /* Ordered list of base-58 encoded addresses for writable loaded accounts */
        std::vector<PublicKey> writable;
        /* Ordered list of base-58 encoded addresses for read-only loaded accounts */
        std::vector<PublicKey> readOnly;
      } loadedAddresses;
      /* Array of string log messages if log message recording was enabled during this transaction  */
      std::vector<std::string> logMessages;
      /* Array of u64 account balances from after the transaction was processed */
      std::vector<uint64_t> postBalances;
      /* Array of u64 account balances from before the transaction was processed */
      std::vector<uint64_t> preBalances;
      /* Transaction-level rewards, populated if rewards are requested */
      struct TransactionReward {
        /* The public key of the account that received the reward */
        PublicKey pubkey;
        /* The number of reward lamports credited or debited by the account */
        uint64_t lamports;
        /* The account balance in lamports after the reward was applied */
        uint64_t postBalance;
        /* The type of reward */
        std::string rewardType;
        /* Vote account commission when the reward was credited, only present for voting and staking rewards */
        uint8_t commission;
      };
      std::vector<TransactionReward> rewards;
    } meta;
  };

  void from_json(const json& j, TransactionReponse::Meta::TransactionReward& rewards) {
    j.at("commission").get_to(rewards.commission);
    j.at("lamports").get_to(rewards.lamports);
    j.at("postBalance").get_to(rewards.postBalance);
    j.at("pubkey").get_to(rewards.pubkey);
    j.at("rewardType").get_to(rewards.rewardType);
  }

  void from_json(const json& j, TransactionReponse::Meta::LoadedAddresses& loadedAddresses) {
    j.at("readonly").get_to(loadedAddresses.readOnly);
    j.at("writable").get_to(loadedAddresses.writable);
  }

  void from_json(const json& j, TransactionReponse::Meta& meta) {
    if (j.at("err").is_null())
      meta.err = "";
    else
      j.at("err").get_to(meta.err);
    j.at("fee").get_to(meta.fee);
    //TODO j.at("innerInstructions").get_to(meta.innerInstructions);
    j.at("loadedAddresses").get_to(meta.loadedAddresses);
    j.at("logMessages").get_to(meta.logMessages);
    j.at("postBalances").get_to(meta.postBalances);
    j.at("preBalances").get_to(meta.preBalances);
    j.at("rewards").get_to(meta.rewards);
  }

  void from_json(const json& j, TransactionReponse::Transaction::Message::Header& header) {
    j.at("numRequiredSignatures").get_to(header.numRequiredSignatures);
    j.at("numReadonlySignedAccounts").get_to(header.numReadonlySignedAccounts);
    j.at("numReadonlyUnsignedAccounts").get_to(header.numReadonlyUnsignedAccounts);
  }

  void from_json(const json& j, TransactionReponse::Transaction::Message::Instruction& instruction) {
    j.at("accounts").get_to(instruction.accounts);
    j.at("data").get_to(instruction.data);
    j.at("programIdIndex").get_to(instruction.programIdIndex);
  }

  void from_json(const json& j, TransactionReponse::Transaction::Message& message) {
    j.at("accountKeys").get_to(message.accountKeys);
    j.at("header").get_to(message.header);
    j.at("instructions").get_to(message.instructions);
    j.at("recentBlockhash").get_to(message.recentBlockhash);
  }

  void from_json(const json& j, TransactionReponse::Transaction& transaction) {
    j.at("message").get_to(transaction.message);
    j.at("signatures").get_to(transaction.signatures);
  }

  void from_json(const json& j, TransactionReponse& transactionReponse) {
    j.at("slot").get_to(transactionReponse.slot);
    j.at("transaction").get_to(transactionReponse.transaction);
    j.at("meta").get_to(transactionReponse.meta);
    j.at("blockTime").get_to(transactionReponse.blockTime);
  }

  std::ostream& operator<<(std::ostream& os, const TransactionReponse& transaction) {
    os << "slot = " << transaction.slot << std::endl << std::endl;
    os << "signatures:" << std::endl;
    for (auto& signature : transaction.transaction.signatures) {
      os << "  " << signature << std::endl;
    }
    os << "accounts:" << std::endl;
    for (auto& accountKey : transaction.transaction.message.accountKeys) {
      os << "  " << accountKey.toBase58() << std::endl;
    }
    os << "recentBlockhash = " << transaction.transaction.message.recentBlockhash << std::endl;
    os << "instructions:" << std::endl;
    for (auto& instruction : transaction.transaction.message.instructions) {
      os << "  programId index = " << instruction.programIdIndex << std::endl;
      for (auto& account : instruction.accounts) {
        os << "    account = " << account << std::endl;
      }
      for (auto& data : instruction.data) {
        os << "    data = " << data << std::endl;
      }
    }
    os << "block time = " << transaction.blockTime << std::endl;
    os << "error = " << transaction.meta.err << std::endl;
    os << "fee = " << transaction.meta.fee << std::endl;
    os << "pre-balances:" << std::endl;
    for (auto& preBalance : transaction.meta.preBalances) {
      os << "  " << preBalance << std::endl;
    }
    os << "post-balances:" << std::endl;
    for (auto& postBalance : transaction.meta.postBalances) {
      os << "  " << postBalance << std::endl;
    }
    //TODO os << "inner instructions = " << transaction.meta.innerInstructions << std::endl;
    os << "logs:" << std::endl;
    for (auto& log : transaction.meta.logMessages) {
      os << "  " << log << std::endl;
    }
    os << "rewards:" << std::endl;
    for (auto& reward : transaction.meta.rewards) {
      os << "  recipient = " << reward.pubkey.toBase58() << std::endl;
      os << "  lamports = " << reward.lamports << std::endl;
      os << "  postBalance = " << reward.postBalance << std::endl;
      os << "  rewardType = " << reward.rewardType << std::endl;
      os << "  commission = " << reward.commission << std::endl;
      os << "----------------" << std::endl;
    }
    os << "loaded addresses:" << std::endl;
    os << "  writable:" << std::endl;
    for (auto& address : transaction.meta.loadedAddresses.writable) {
      os << "    " << address.toBase58() << std::endl;
    }
    os << "  read only:" << std::endl;
    for (auto& address : transaction.meta.loadedAddresses.readOnly) {
      os << "    " << address.toBase58() << std::endl;
    }
    os << "return data:" << std::endl;
    //TODO os << "  programId = " << transaction.meta.returnData.programId.toBase58() << std::endl;
    //TODO os << "  data = " <<  transaction.meta.returnData.data  << std::endl;
    return os;
  }

  class Connection {
    Commitment _commitment;
    std::string _rpcEndpoint;
    std::string _rpcWsEndpoint;
    // http::HttpClient _rpcClient;
    // websockets::WebSocketClient _rpcWebSocket;

    std::map<int, AccountChangeSubscriptionInfo> _accountChangeSubscriptions;
    std::map<int, LogsSubscriptionInfo> _logsSubscriptions;
    std::map<int, ProgramAccountChangeSubscriptionInfo> _programAccountChangeSubscriptions;
    std::map<int, std::function<void(Context context, SlotInfo slotInfo)>> _slotSubscriptions;

    static std::string makeWebsocketUrl(std::string endpoint) {
      auto url = endpoint;
      url.replace(0, 4, "ws");
      return url;
    }

  public:

    Connection(std::string endpoint, Commitment commitment)
      : _commitment(commitment),
      _rpcEndpoint(endpoint),
      _rpcWsEndpoint(makeWebsocketUrl(endpoint))
      // _rpcClient(_rpcEndpoint, 443),
      // _rpcWebSocket(_rpcWsEndpoint, 443)
    {
      auto sodium_result = sodium_init();
      if (sodium_result == -1) {
        throw std::runtime_error("Failed to initialize libsodium");
      }
    }

    ~Connection() {}

    Connection() = delete;
    Connection(const Connection&) = delete;
    Connection(Connection&&) = delete;
    Connection& operator=(const Connection&) = delete;
    Connection& operator=(Connection&&) = delete;

    //-------- Http methods --------------------------------------------------------------------

    /**
     * Returns all information associated with the account of provided Pubkey.
     * 
     * @param publicKey The Pubkey of account to query
     */
    // TODO null
    AccountInfo getAccountInfo(PublicKey publicKey) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getAccountInfo"},
        {"params", {
          publicKey.toBase58(),
          {
            {"encoding", "base64"},
          },
        }},
      })["result"]["value"];
    }

    /**
     * Returns all SPL Token accounts by token owner.
     * 
     * @param ownerAddress The Pubkey of account owner to query
     */
    std::vector<TokenAccount> getAllTokenAccountsByOwner(PublicKey ownerAddress) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTokenAccountsByOwner"},
        {"params", {
          ownerAddress.toBase58(),
          {
            {"programId", TOKEN_PROGRAM_ID.toBase58()},
          },
          {
            {"encoding", "jsonParsed"},
          }
        }},
      })["result"]["value"];
    }

    /**
     * Returns the balance of the account of provided Pubkey.
     * 
     * @param publicKey The Pubkey of the account to query
     */
    uint64_t getBalance(PublicKey publicKey) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getBalance"},
        {"params", {
          publicKey.toBase58(),
        }},
      })["result"]["value"];
    }

    /**
     * Returns information about all the nodes participating in the cluster.
     */
    json getClusterNodes() {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getClusterNodes"},
      })["result"];
    }

    /**
     * Returns the identity Pubkey of the current node.
     */
    PublicKey getIdentity() {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getIdentity"},
      })["result"]["identity"];
    }

    /**
     * Returns the latest blockhash.
     */
    std::string getLatestBlockhash() {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getLatestBlockhash"},
      })["result"]["blockhash"];
    }

    /**
     * Returns the schedule for a given leader, for the current epoch.
     * 
     * @param leaderAddress The Pubkey of the leader to query
     */
    json getLeaderSchedule(PublicKey leaderAddress) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getLeaderSchedule"},
        {"params", {
          {
            {"identity", leaderAddress.toBase58()},
          },
        }},
      })["result"];
    }

    /**
     * Returns the account information for a list of Pubkeys.
     * 
     * @param publicKeys The Pubkeys of the accounts to query
    */
   // TODO null
    std::vector<AccountInfo> getMultipleAccounts(std::vector<PublicKey> publicKeys) {
      std::vector<std::string> base58Keys;
      for (auto publicKey : publicKeys) {
        base58Keys.push_back(publicKey.toBase58());
      }
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getMultipleAccounts"},
        {"params", {
          base58Keys,
          {
            {"encoding", "base64"},
          },
        }},
      })["result"]["value"];
    }

    /**
     * Returns all accounts owned by the provided program Pubkey.
     * 
     * @param programId The Pubkey of the program to query
    */
   // TODO null
    std::vector<AccountInfo> getProgramAccounts(PublicKey programId) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getProgramAccounts"},
        {"params", {
          programId.toBase58(),
        }},
      })["result"];
    }

    /**
     * Returns the slot that has reached the given or default commitment level.
    */
    uint64_t getSlot(Commitment commitment = Commitment::Finalized) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getSlot"},
      })["result"];
    }

    /**
     * Returns the current slot leader.
     */
    PublicKey getSlotLeader() {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getSlotLeader"},
      })["result"];
    }

    /**
     * Returns the token balance of an SPL Token account.
     * 
     * @param tokenAddress The Pubkey of the token account to query
     */
    TokenAmount getTokenAccountBalance(PublicKey tokenAddress) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTokenAccountBalance"},
        {"params", {
          tokenAddress.toBase58(),
        }},
      })["result"]["value"];
    }

    /**
     * Returns SPL Token accounts for a given mint by token owner.
     * 
     * @param ownerAddress The Pubkey of account owner to query
     * @param mintAddress The mint of the token to query
     */
    std::vector<TokenAccount> getTokenAccountsByOwner(PublicKey ownerAddress, PublicKey tokenMint) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTokenAccountsByOwner"},
        {"params", {
          ownerAddress.toBase58(),
          {
            {"mint", tokenMint.toBase58()},
          },
          {
            {"encoding", "jsonParsed"},
          }
        }},
      })["result"]["value"];
    }

    /**
     * Returns the total supply of an SPL Token type.
     * 
     * @param tokenMintAddress The Pubkey of the token mint to query
     */
    TokenAmount getTokenSupply(PublicKey tokenMintAddress) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTokenSupply"},
        {"params", {
          tokenMintAddress.toBase58(),
        }},
      })["result"]["value"];
    }

    /**
     * Returns transaction details for a confirmed transaction.
     * 
     * @param transactionSignature The signature of the transaction to query
     */
    // TODO null
    TransactionReponse getTransaction(std::string transactionSignature) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTransaction"},
        {"params", {
          transactionSignature
        }},
      })["result"];
    }

    /**
     * Returns the current solana versions running on the node.
     */
    std::string getVersion() {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getVersion"},
      })["result"]["solana-core"];
    }

    /**
     * Submits a signed transaction to the cluster for processing.
     * 
     * @param signedTransaction The signed transaction to submit
     */
    std::string sendTransaction(std::string signedTransaction) {
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "sendTransaction"},
        {"params", {
          signedTransaction,
          {
            {"encoding", "base64"},
          },
        }},
      });

      auto hash = response["result"];
      return hash.get<std::string>();
    }

    /**
     * Returns a signed transaction.
     * 
     * @param transaction The transaction to sign
     * @param signers The keypairs to sign the transaction
     */
    std::string signTransaction(Transaction& transaction, std::vector<Keypair> signers) {
      transaction.message.recentBlockhash = getLatestBlockhash();
      transaction.sign(signers);
      std::vector<uint8_t> wireTransaction = transaction.serialize();
      std::string signedTransaction = base64::base64_encode(wireTransaction.data(), wireTransaction.size());

      return signedTransaction;
    }

    /**
     * Simulate sending a transaction.
     * 
     * @param signedTransaction The signed transaction to simulate
     */
    SimulatedTransactionResponse simulateTransaction(std::string signedTransaction) {
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "simulateTransaction"},
        {"params", {
          signedTransaction,
          {
            {"encoding", "base64"},
          },
        }},
      });

      std::vector<std::string> logs;
      for (auto log : response["result"]["logs"]) {
        logs.push_back(log.get<std::string>());
      }

      std::vector<AccountInfo> accounts;
      for (auto account : response["result"]["accounts"]) {
        accounts.push_back(AccountInfo {
          account["lamports"].get<uint64_t>(),
          account["owner"].get<std::string>(),
          account["data"].get<std::string>(),
          account["executable"].get<bool>(),
          account["rentEpoch"].get<uint64_t>(),
        });
      }

      auto result = response["result"]["value"];
      return {
        result["err"].get<std::string>(),
        logs,
        accounts,
        result["unitsConsumed"].get<uint64_t>(),
        {
          PublicKey(result["data"]["programId"].get<std::string>()),
          result["data"]["data"].get<std::string>(),
        }
      };
    }

    //-------- Websocket methods --------------------------------------------------------------------

    int onAccountChange(PublicKey accountId, std::function<void(Context context, AccountInfo accountInfo)> callback) {
      //TODO _rpcWebSocket
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "accountSubscribe"},
        {"params", {
          accountId.toBase58(),
          {
            {"encoding", "base64"},
            {"commitment", _commitment},
          },
        }},
      });
      int subscriptionId = response["result"].get<int>();
      _accountChangeSubscriptions[subscriptionId] = { accountId, callback };
      return subscriptionId;
    }

    bool removeAccountChangeListener(int subscriptionId) {
      //TODO _rpcWebSocket
      if (_accountChangeSubscriptions.find(subscriptionId) != _accountChangeSubscriptions.end()) {
        auto response = http::post(_rpcEndpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "accountUnsubscribe"},
          {"params", {
            subscriptionId,
          }},
        });
        _accountChangeSubscriptions.erase(subscriptionId);
        return response["result"].get<bool>();
      }
      return false;
    }

    int onLogs(PublicKey accountId, std::function<void(Context context, Logs logs)> callback) {
      //TODO _rpcWebSocket
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "logsSubscribe"},
        {"params", {
          "mentions",
          {
            {"mentions", accountId.toBase58()},
          },
          {
            {"commitment", _commitment},
          },
        }},
      });
      int subscriptionId = response["result"].get<int>();
      _logsSubscriptions[subscriptionId] = { accountId, callback };
      return subscriptionId;
    }

    bool removeOnLogsListener(int subscriptionId) {
      //TODO _rpcWebSocket
      if (_logsSubscriptions.find(subscriptionId) != _logsSubscriptions.end()) {
        auto response = http::post(_rpcEndpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "logsUnsubscribe"},
          {"params", {
            subscriptionId,
          }},
        });
        _logsSubscriptions.erase(subscriptionId);
        return response["result"].get<bool>();
      }
      return false;
    }

    int onProgramAccountChange(PublicKey programId, std::function<void(Context context, KeyedAccountInfo keyedAccountInfo)> callback) {
      //TODO _rpcWebSocket
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "programSubscribe"},
        {"params", {
          programId.toBase58(),
          {
            {"encoding", "base64"},
            {"commitment", _commitment},
          },
        }},
      });
      int subscriptionId = response["result"].get<int>();
      _programAccountChangeSubscriptions[subscriptionId] = { programId, callback };
      return subscriptionId;
    }

    bool removeProgramAccountChangeListener(int subscriptionId) {
      //TODO _rpcWebSocket
      if (_programAccountChangeSubscriptions.find(subscriptionId) != _programAccountChangeSubscriptions.end()) {
        auto response = http::post(_rpcEndpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "programUnsubscribe"},
          {"params", {
            subscriptionId,
          }},
        });
        _programAccountChangeSubscriptions.erase(subscriptionId);
        return response["result"].get<bool>();
      }
      return false;
    }

    int onSlotChange(std::function<void(Context context, SlotInfo slotInfo)> callback) {
      //TODO _rpcWebSocket
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "slotSubscribe"},
      });
      int subscriptionId = response["result"].get<int>();
      _slotSubscriptions[subscriptionId] = callback;
      return subscriptionId;
    }

    bool removeSlotChangeListener(int subscriptionId) {
      //TODO _rpcWebSocket
      if (_slotSubscriptions.find(subscriptionId) != _slotSubscriptions.end()) {
        auto response = http::post(_rpcEndpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "slotUnsubscribe"},
          {"params", {
            subscriptionId,
          }},
        });
        _slotSubscriptions.erase(subscriptionId);
        return response["result"].get<bool>();
      }
      return false;
    }
  };

}
