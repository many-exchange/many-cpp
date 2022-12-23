#pragma once

#include <array>
#include <string>

#include "base58.h"

namespace solana {

#define PUBLIC_KEY_LENGTH 32

class PublicKey {
  /* An array of bytes representing the public key */
  std::array<char, PUBLIC_KEY_LENGTH> bytes;

public:
  PublicKey() {
    for (int i = 0; i < PUBLIC_KEY_LENGTH; i++) {
      bytes[i] = 0;
    }
  }

  PublicKey(const char* value) {
    size_t size = PUBLIC_KEY_LENGTH;
    b58tobin(bytes.data(), &size, value, 0);
  }

  PublicKey(const std::string& value) {
    size_t size = PUBLIC_KEY_LENGTH;
    b58tobin(bytes.data(), &size, value.c_str(), 0);
  }

  PublicKey(const PublicKey& other) {
    for (int i = 0; i < PUBLIC_KEY_LENGTH; i++) {
      bytes[i] = other.bytes[i];
    }
  }

  ~PublicKey() {}

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
    b58enc(temp, &size, bytes.data(), bytes.size());

    return std::string(temp, size - 1);
  }
};

}
