#pragma once

#include "publickey.h"

namespace solana {

class Keypair {

public:

  PublicKey publicKey;
  //PrivateKey privateKey;

  Keypair() {
  }

  ~Keypair() {
  }

  static Keypair generate();

  //static Keypair fromFile(const std::string &path);

};

}
