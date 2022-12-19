#pragma once

#include <vector>

#include "publickey.h"

namespace solana {

class Transaction {

public:

  /**
   * The transaction fee payer
   */
  PublicKey feePayer;

  /**
   * The instructions to atomically execute
   */
  std::vector<TransactionInstruction> instructions;

  Transaction() {
  }

  ~Transaction() {
  }

};

}
