#pragma once

#include <array>
#include <string>

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

#include "base58.h"
#include "json.h"

namespace solana {

#define PUBLIC_KEY_LENGTH 32

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
      b58tobin(bytes.data(), &size, value.c_str(), 0);
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
      b58enc(temp, &size, bytes.data(), bytes.size());
      return std::string(temp, size - 1);
    }

  };

  void from_json(const nlohmann::json& j, PublicKey& publicKey) {
    publicKey = PublicKey(j.get<std::string>());
  }

  void to_json(nlohmann::json& j, const PublicKey& publicKey) {
    j = publicKey.toBase58();
  }

} // namespace solana




//#include "base64.h"
#include "cluster.h"
#include "connection.h"
//#include "keypair.h"
//#include "transaction.h"
//#include "websocket.h"
