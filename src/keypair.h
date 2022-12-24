#pragma once

#include <fstream>
#include <sodium.h>

#include "publickey.h"

namespace solana {

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

}
