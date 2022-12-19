#pragma once

#include <string>

namespace solana {

#define SIZE_PUBKEY 32

class PublicKey {

  uint8_t x[SIZE_PUBKEY];

public:

  PublicKey(const std::string& value) {
    //TODO b58tobin(value);
  }

  bool operator==(const PublicKey& other) const {
    for (int i = 0; i < SIZE_PUBKEY; i++) {
      if (x[i] != other.x[i]) {
        return false;
      }
    }
    return true;
  }

};

}
