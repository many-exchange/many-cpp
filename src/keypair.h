#pragma once

#include <fstream>
#include <sodium.h>

#include "publickey.h"

namespace solana {

struct Keypair {
  PublicKey publicKey;
  const uint8_t *secretKey;

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
  //static Keypair fromSecretKey(const uint8_t *secretKey, bool skipValidation = false) {
    //return new Keypair(keypair);
  //}

  /*
  static Keypair fromFile(const std::string &path) {
    Keypair result = {};
    std::ifstream fileStream(path);
    std::string fileContent(std::istreambuf_iterator<char>(fileStream), {});
    result.privateKey.data = json::parse(fileContent);
    crypto_sign_ed25519_sk_to_pk(result.publicKey.data.data(),
                                 result.privateKey.data.data());
    return result;
  }
  */

};

}
