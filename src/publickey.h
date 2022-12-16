#pragma once

#include <string>

namespace solana {

#define SIZE_PUBKEY 32

class PublicKey {

  uint8_t x[SIZE_PUBKEY];

public:

    PublicKey(const std::string& value) {
        //const decoded = bs58.decode(value);
        //if (decoded.length != 32) {
            //throw new Error(`Invalid public key input`);
        //}
        //this._bn = new BN(decoded);
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
