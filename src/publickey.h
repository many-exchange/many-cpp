#pragma once

#include <string>

#include "base58.h"

namespace solana {

#define PUBLIC_KEY_LENGTH 32

class PublicKey {

  char bytes[PUBLIC_KEY_LENGTH];

public:

  PublicKey(const std::string& value) {
    size_t length = PUBLIC_KEY_LENGTH;
    b58tobin(bytes, &length, value.c_str(), value.length());
  }

  ~PublicKey() {
  }

  bool operator==(const PublicKey& other) const {
    for (int i = 0; i < PUBLIC_KEY_LENGTH; i++) {
      if (bytes[i] != other.bytes[i]) {
        return false;
      }
    }
    return true;
  }

};

}
