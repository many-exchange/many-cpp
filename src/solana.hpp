//  ____ _____ __    _____ _____ _____
// |  __|     |  |  |  _  |   | |  _  |  Solana C++ SDK
// |__  |  |  |  |__|     | | | |     |  version 0.0.1
// |____|_____|_____|__|__|_|___|__|__|  https://github.com/many-exchange/solana-sdk-cpp
//
// Copyright (c) 2022-2023 Many Exchange
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <algorithm>
#include <arpa/inet.h>
#include <array>
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

#include "common.hpp"

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
#define ASSOCIATED_TOKEN_PROGRAM_ID PublicKey("ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL")

#define MAX_SEED_LENGTH 32
#define PACKET_DATA_SIZE 1232
#define PRIVATE_KEY_LENGTH 64
#define PUBLIC_KEY_LENGTH 32
#define SIGNATURE_LENGTH 64

namespace solana {

  struct PublicKey {
    /** An array of bytes representing the Pubkey */
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

    bool operator<(const PublicKey& other) const {
      for (int i = 0; i < PUBLIC_KEY_LENGTH; i++) {
        if (bytes[i] < other.bytes[i]) {
          return true;
        }
        else if (bytes[i] > other.bytes[i]) {
          return false;
        }
      }
      return false;
    }

    /**
     * Returns the base-58 representation of the public key
     */
    std::string to_base58() const {
      char temp[45];
      memset(temp, 0, 45);
      size_t size = 45;
      base58::b58enc(temp, &size, bytes.data(), bytes.size());
      return std::string(temp, size - 1);
    }

    /**
     * Returns a buffer representation of the public key
     */
    std::vector<uint8_t> to_buffer() const {
      return std::vector<uint8_t>(bytes.begin(), bytes.end());
    }

    /**
     * Check if this publickey is on the ed25519 curve
     */
    bool is_on_curve() const {
      libsodium::ge25519_p3 point;
      if (libsodium::ge25519_is_canonical(bytes.data()) == 0 || libsodium::ge25519_frombytes(&point, bytes.data()) != 0) {
        return false;
      }
      return true;
    }

    /**
     * Derive a program address from seeds and a program ID.
     *
     * @param seeds Seeds to use to derive the program address
     * @param program_id Program ID to use to derive the program address
    */
    static std::optional<PublicKey> create_program_address(
      const std::vector<std::vector<uint8_t>>& seeds,
      const PublicKey& program_id
    ) {
      std::vector<uint8_t> buffer;
      for (auto seed : seeds) {
        if (seed.size() > MAX_SEED_LENGTH) {
          throw std::runtime_error("Max seed length exceeded");
        }
        buffer.insert(buffer.end(), seed.begin(), seed.end());
      }
      buffer.insert(buffer.end(), program_id.bytes.begin(), program_id.bytes.end());
      std::string ProgramDerivedAddress = "ProgramDerivedAddress";
      buffer.insert(buffer.end(), ProgramDerivedAddress.begin(), ProgramDerivedAddress.end());

      // Create a sha256 hash
      unsigned char hash[crypto_hash_sha256_BYTES];
      memset(hash, 0, crypto_hash_sha256_BYTES);
      static_assert(crypto_hash_sha256_BYTES == PUBLIC_KEY_LENGTH);
      crypto_hash_sha256(hash, buffer.data(), buffer.size());

      PublicKey pubkey = PublicKey((uint8_t*)hash);
      if (pubkey.is_on_curve()) {
        return std::nullopt;
      }
      return pubkey;
    }

    /**
     * Find a valid program address
     *
     * Valid program addresses must fall off the ed25519 curve.  This function
     * iterates a nonce until it finds one that when combined with the seeds
     * results in a valid program address.
     *
     * @param seeds Seed values used to generate the program address
     * @param programId Program ID to generate the address for
     */
    static std::tuple<PublicKey, uint8_t> find_program_address(
      const std::vector<std::vector<uint8_t>>& seeds,
      const PublicKey& programId
    ) {
      uint8_t nonce = 255;
      while (nonce != 0) {
        std::vector<std::vector<uint8_t>> seedsWithNonce(seeds.begin(), seeds.end());
        seedsWithNonce.push_back({nonce});

        std::optional<PublicKey> address = create_program_address(seedsWithNonce, programId);
        if (address.has_value()) {
          return std::make_tuple(address.value(), nonce);
        } else {
          nonce -= 1;
        }
      }
      throw std::runtime_error("Unable to find a viable program address nonce");
    }
  };

  void from_json(const nlohmann::json& j, PublicKey& pubkey) {
    pubkey = PublicKey(j.get<std::string>());
  }

  struct Keypair {
    std::array<uint8_t, crypto_sign_SECRETKEYBYTES> secretKey;
    PublicKey publicKey;

    Keypair() {
      auto sodium_result = sodium_init();
      if (sodium_result == -1) {
        throw std::runtime_error("Failed to initialize libsodium");
      }
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
     * with the {@link Keypair.from_seed} method.
     *
     * @throws error if the provided secret key is invalid and validation is not skipped.
     *
     * @param secretKey secret key byte array
     * @param options: skip secret key validation
     */
    Keypair(std::array<char, PUBLIC_KEY_LENGTH> secretKey, bool skipValidation = false) {
      auto sodium_result = sodium_init();
      if (sodium_result == -1) {
        throw std::runtime_error("Failed to initialize libsodium");
      }
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
    static Keypair from_file(const std::string &path) {
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
    static Keypair from_seed(const uint8_t *seed) {
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
      return std::string((char*)sig, sigSize);
    }
  };

  struct Account {
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

  void from_json(const json& j, Account& account) {
    account.lamports = j["lamports"].get<uint64_t>();
    account.owner = j["owner"].get<PublicKey>();

    if (j["data"].is_string()) {
      account.data = j["data"].get<std::string>();
    } else {
      auto encoding = j["data"][1].get<std::string>();
      ASSERT(encoding == "base64");
      account.data = j["data"][0].get<std::string>();
    }

    account.executable = j["executable"].get<bool>();
    account.rentEpoch = j["rentEpoch"].get<uint64_t>();
  }

  struct AccountInfo {
    PublicKey pubkey;
    Account account;
  };

  void from_json(const json& j, AccountInfo& accountInfo) {
    accountInfo.pubkey = j["pubkey"].get<PublicKey>();
    accountInfo.account = j["account"].get<Account>();
  }

  struct Context {
    uint64_t slot;
  };

  void from_json(const json& j, Context& context) {
    context.slot = j["slot"].get<uint64_t>();
  }

  struct TokenBalance {
    /** The raw balance without decimals, as a string representation */
    uint64_t amount;
    /** The number of base 10 digits to the right of the decimal place */
    uint64_t decimals;

    /**
     * Returns the balance as a number of tokens, with decimals applied.
     */
    double tokens() {
      return (double) amount / (double) pow(10, decimals);
    }
  };

  void from_json(const json& j, TokenBalance& tokenAmount) {
    tokenAmount.amount = stol(j["amount"].get<std::string>());
    tokenAmount.decimals = j["decimals"].get<uint64_t>();
  }

  struct Blockhash {
    std::string blockhash;
  };

  void from_json(const json& j, Blockhash& blockhash) {
    blockhash.blockhash = j["blockhash"].get<std::string>();
  }

  enum class Cluster {
    MainnetBeta,
    Devnet,
    Testnet,
    Localnet
  };

  /**
   * Returns a default cluster API URL for a given cluster.
   *
   * @param cluster The cluster to get the default API URL for
  */
  std::string cluster_api_url(Cluster cluster) {
    switch (cluster) {
      case Cluster::MainnetBeta:
        return "https://api.mainnet-beta.solana.com";
      case Cluster::Devnet:
        return "https://api.devnet.solana.com";
      case Cluster::Testnet:
        return "https://api.testnet.solana.com";
      case Cluster::Localnet:
        return "http://127.0.0.1:8899";
      default:
        throw std::runtime_error("Invalid cluster.");
    }
  }

  struct ClusterNode {
    /** Node public key */
    PublicKey pubkey;
    /** Gossip network address for the node */
    std::string gossip;
    /** TPU network address for the node */
    std::string tpu;
    /** JSON RPC network address for the node, if the JSON RPC service is enabled */
    std::string rpc;
    /** The software version of the node, if the version information is available */
    std::string version;
    /** The unique identifier of the node's feature set */
    uint64_t featureSet;
    /** The shred version the node has been configured to use */
    uint64_t shredVersion;
  };

  void from_json(const json& j, ClusterNode& cluster_node) {
    if (!j["featureSet"].is_null()) {
      cluster_node.featureSet = j["featureSet"].get<uint64_t>();
    }
    cluster_node.gossip = j["gossip"].get<std::string>();
    cluster_node.pubkey = j["pubkey"].get<PublicKey>();
    if (!j["rpc"].is_null()) {
      cluster_node.rpc = j["rpc"].get<std::string>();
    }
    cluster_node.shredVersion = j["shredVersion"].get<uint64_t>();
    if (!j["tpu"].is_null()) {
      cluster_node.tpu = j["tpu"].get<std::string>();
    }
    if (!j["version"].is_null()) {
      cluster_node.version = j["version"].get<std::string>();
    }
  }

  enum class Commitment {
    Processed,
    Confirmed,
    Finalized,
  };

  struct ConfirmOptions {
    Commitment commitment = Commitment::Finalized;
    bool preflightCommitment = false;
    bool encoding = "base64";
  };

  int decode_length(std::vector<uint8_t> bytes) {
    int len = 0;
    int size = 0;
    while (bytes.size() > 0) {
      int elem = bytes.front();
      bytes.erase(bytes.begin());
      len |= (elem & 0x7f) << (size * 7);
      size += 1;
      if ((elem & 0x80) == 0) {
        break;
      }
    }
    return len;
  }

  std::vector<uint8_t> encode_length(int len) {
    std::vector<uint8_t> bytes;
    int rem_len = len;
    for (;;) {
      int elem = rem_len & 0x7f;
      rem_len >>= 7;
      if (rem_len == 0) {
        bytes.push_back(elem);
        break;
      } else {
        elem |= 0x80;
        bytes.push_back(elem);
      }
    }
    ASSERT(bytes.size() <= 2);
    return bytes;
  }

  struct Identity {
    PublicKey identity;
  };

  void from_json(const json& j, Identity& identity) {
    identity.identity = PublicKey(j["identity"].get<std::string>());
  }

  struct LeaderSchedule {
    PublicKey leader;
    std::vector<int> schedule;
  };

  void from_json(const json& j, LeaderSchedule& leader_schedule) {
    auto it = j.begin();
    leader_schedule.leader = PublicKey(it.key());
    leader_schedule.schedule = it.value().get<std::vector<int>>();
  }

  struct Logs {
    //TODO err: TransactionError | null;
    std::vector<std::string> logs;
    std::string signature;
  };

  void from_json(const json& j, Logs& logs) {
    logs.logs = j["logs"].get<std::vector<std::string>>();
    logs.signature = j["signature"].get<std::string>();
  }

  struct SlotInfo {
    /** Currently processing slot */
    uint64_t slot;
    /** Parent of the current slot */
    uint64_t parent;
    /** The root block of the current slot's fork */
    uint64_t root;
  };

  void from_json(const json& j, SlotInfo& slot_info) {
    slot_info.slot = j["slot"].get<uint64_t>();
    slot_info.parent = j["parent"].get<uint64_t>();
    slot_info.root = j["root"].get<uint64_t>();
  }

  struct TokenAccount {
    /** The account's Pubkey */
    PublicKey pubkey;
    /** The account data */
    struct Account {
      /** Number of lamports assigned to the account */
      uint64_t lamports;
      /** The Pubkey of the program that owns the account */
      PublicKey owner;
      struct Data {
        struct Parsed {
          struct Info {
            /** Boolean indicating if the account is native */
            bool isNative;
            /** The mint of the token */
            PublicKey mint;
            /** The Pubkey of the token account owner */
            PublicKey owner;
            /** The amount of tokens */
            TokenBalance tokenAmount;
            /** The account this token account is delegated to */
            PublicKey delegate;
            /** Amount delegated */
            TokenBalance delegatedAmount;
            /** State of the token account (ex: "initialized") */
            std::string state;
          } info;
          /** The account type (ex: "account") */
          std::string type;
        } parsed;
        /* Name of token program (ex: "spl-token") */
        std::string program;
        /** The length of the account data */
        uint64_t space;
      } data;
      /** Boolean indicating if the account contains a program (and is strictly read-only) */
      bool executable;
      /** The epoch at which this account will next owe rent */
      uint64_t rentEpoch;
    } account;
  };

  void from_json(const json& j, TokenAccount::Account::Data::Parsed::Info& parsedAccountInfo) {
    parsedAccountInfo.isNative = j["isNative"].get<bool>();
    parsedAccountInfo.mint = j["mint"].get<PublicKey>();
    parsedAccountInfo.owner = j["owner"].get<PublicKey>();
    parsedAccountInfo.tokenAmount = j["tokenAmount"].get<TokenBalance>();
    if (j.contains("delegate")) {
      parsedAccountInfo.delegate = j["account"]["data"]["parsed"]["info"]["delegate"].get<PublicKey>();
      parsedAccountInfo.delegatedAmount = j["account"]["data"]["parsed"]["info"]["delegatedAmount"].get<TokenBalance>();
    }
    parsedAccountInfo.state = j["state"].get<std::string>();
  }

  void from_json(const json& j, TokenAccount::Account::Data::Parsed& parsedAccountData) {
    parsedAccountData.info = j["info"].get<TokenAccount::Account::Data::Parsed::Info>();
    parsedAccountData.type = j["type"].get<std::string>();
  }

  void from_json(const json& j, TokenAccount::Account::Data& accountData) {
    accountData.program = j["program"].get<std::string>();
    accountData.parsed = j["parsed"].get<TokenAccount::Account::Data::Parsed>();
    accountData.space = j["space"].get<uint64_t>();
  }

  void from_json(const json& j, TokenAccount::Account& account) {
    account.lamports = j["lamports"].get<uint64_t>();
    account.owner = j["owner"].get<PublicKey>();
    account.data.program = j["data"]["program"].get<std::string>();
    account.data.parsed = j["data"]["parsed"].get<TokenAccount::Account::Data::Parsed>();
    account.data.space = j["data"]["space"].get<uint64_t>();
    account.executable = j["executable"].get<bool>();
    account.rentEpoch = j["rentEpoch"].get<uint64_t>();
  }

  void from_json(const json& j, TokenAccount& tokenAccount) {
    tokenAccount.pubkey = j["pubkey"].get<PublicKey>();
    tokenAccount.account = j["account"].get<TokenAccount::Account>();
  }

  struct TransactionMessageHeader {
    /** The number of signatures required to validate this transaction */
    uint8_t numRequiredSignatures;
    /** The number of read-only signed accounts */
    uint8_t numReadonlySignedAccounts;
    /** The number of read-only unsigned accounts */
    uint8_t numReadonlyUnsignedAccounts;
  };

  void from_json(const json& j, TransactionMessageHeader& header) {
    header.numRequiredSignatures = j["numRequiredSignatures"].get<uint8_t>();
    header.numReadonlySignedAccounts = j["numReadonlySignedAccounts"].get<uint8_t>();
    header.numReadonlyUnsignedAccounts = j["numReadonlyUnsignedAccounts"].get<uint8_t>();
  }

  struct CompiledTransaction {
    /** Defines the content of the transaction */
    struct Message {
      /** The message header */
      TransactionMessageHeader header;
      /** List of base-58 encoded Pubkeys used by the transaction, including by the instructions and for signatures.
      The first message.header.numRequiredSignatures Pubkeys must sign the transaction. */
      std::vector<PublicKey> accountKeys;
      /** A base-58 encoded hash of a recent block in the ledger used to prevent transaction duplication and to give transactions lifetimes. */
      PublicKey recentBlockhash;
      struct Instruction {
        /** Ordered indices into the `message.accountKeys` array indicating which accounts to pass to the program */
        std::vector<uint8_t> accounts;
        /** The program input data */
        std::vector<uint8_t> data;
        /** Index into the `message.accountKeys` array indicating the program account that executes this instruction */
        uint8_t programIdIndex;
      };
      /** List of program instructions that will be executed in sequence and committed in one atomic transaction if all succeed. */
      std::vector<Instruction> instructions;

      void serialize(std::vector<uint8_t>& serialized_message) {
        if (serialized_message.size() > 0) {
          throw new std::runtime_error("Message is already serialized");
        }
        if (header.numRequiredSignatures == 0) {
          throw new std::runtime_error("Message must have at least one signature");
        }
        serialized_message.push_back(header.numRequiredSignatures);
        serialized_message.push_back(header.numReadonlySignedAccounts);
        serialized_message.push_back(header.numReadonlyUnsignedAccounts);

        auto keyCount = encode_length(accountKeys.size());
        serialized_message.insert(serialized_message.end(), keyCount.begin(), keyCount.end());

        for (auto& key : accountKeys) {
          serialized_message.insert(serialized_message.end(), key.bytes.begin(), key.bytes.end());
        }

        serialized_message.insert(serialized_message.end(), recentBlockhash.bytes.begin(), recentBlockhash.bytes.end());

        auto instructionCount = encode_length(instructions.size());
        serialized_message.insert(serialized_message.end(), instructionCount.begin(), instructionCount.end());

        for (auto& instruction : instructions) {
          serialized_message.push_back(instruction.programIdIndex);

          auto keyIndicesCount = encode_length(instruction.accounts.size());
          serialized_message.insert(serialized_message.end(), keyIndicesCount.begin(), keyIndicesCount.end());

          for (auto& accountIndex : instruction.accounts) {
            serialized_message.push_back(accountIndex);
          }

          auto dataLength = encode_length(instruction.data.size());
          serialized_message.insert(serialized_message.end(), dataLength.begin(), dataLength.end());

          serialized_message.insert(serialized_message.end(), instruction.data.begin(), instruction.data.end());
        }
      }
    } message;
    /** The transaction signatures */
    std::vector<std::string> signatures;

    /**
     * Serialize the transaction
     */
    std::vector<uint8_t> serialize(std::vector<uint8_t>& serialized_message) {
      std::vector<uint8_t> buffer;
      std::vector<uint8_t> signaturesLength = encode_length(signatures.size());
      buffer.insert(buffer.end(), signaturesLength.begin(), signaturesLength.end());
      for (auto& signature : signatures) {
        ASSERT(signature.size() == 64);
        buffer.insert(buffer.end(), signature.begin(), signature.end());
      }
      if (serialized_message.size() == 0) {
        throw new std::runtime_error("Message is not serialized");
      }
      buffer.insert(buffer.end(), serialized_message.begin(), serialized_message.end());
      return buffer;
    }

    /**
     * Sign the transaction with the provided signers
     */
    void sign(const std::vector<uint8_t>& serialized_message, const std::vector<Keypair>& signers) {
      ASSERT(signatures.size() == 0);
      for (auto& signer : signers) {
        //signatures.insert(signer.publicKey, signer.sign(serialized_message));
        signatures.push_back(signer.sign(serialized_message));
      }
    }
  };

  void from_json(const json& j, CompiledTransaction::Message::Instruction& instruction) {
    instruction.accounts = j["accounts"].get<std::vector<uint8_t>>();
    instruction.data = base64::decode(j["data"].get<std::string>());
    instruction.programIdIndex = j["programIdIndex"].get<uint8_t>();
  }

  void from_json(const json& j, CompiledTransaction::Message& message) {
    message.accountKeys = j["accountKeys"].get<std::vector<PublicKey>>();
    message.header.numReadonlySignedAccounts = j["header"]["numReadonlySignedAccounts"].get<uint8_t>();
    message.header.numReadonlyUnsignedAccounts = j["header"]["numReadonlyUnsignedAccounts"].get<uint8_t>();
    message.header.numRequiredSignatures = j["header"]["numRequiredSignatures"].get<uint8_t>();
    message.instructions = j["instructions"].get<std::vector<CompiledTransaction::Message::Instruction>>();
    message.recentBlockhash = PublicKey(j["recentBlockhash"].get<std::string>());
  }

  void from_json(const json& j, CompiledTransaction& transaction) {
    transaction.message = j["message"].get<CompiledTransaction::Message>();
    transaction.signatures = j["signatures"].get<std::vector<std::string>>();
  }

  struct Transaction {
    /** The transaction signatures */
    std::vector<std::string> signatures;
    /** The transaction message */
    struct Message {
      /** The message header */
      TransactionMessageHeader header;
      /** The account keys used by this transaction */
      std::vector<PublicKey> accountKeys;
      /** Recent blockhash */
      PublicKey recentBlockhash;
      struct Instruction {
        /** The program id that executes this instruction */
        PublicKey programId;
        /** Account metadata used to define instructions */
        struct AccountMeta {
          /** An account's Pubkey */
          PublicKey pubkey;
          /** True if an instruction requires a transaction signature matching `pubkey` */
          bool isSigner;
          /** True if the Pubkey can be loaded as a read-write account. */
          bool isWritable;

          bool operator==(const AccountMeta& other) const {
            return pubkey == other.pubkey;
          }

          bool operator==(const PublicKey& other) const {
            return pubkey == other;
          }
        };
        /** Ordered indices into the transaction keys array indicating which accounts to pass to the program */
        std::vector<AccountMeta> accounts;
        /** Program input data */
        std::vector<uint8_t> data;
      };
      /** The program instructions */
      std::vector<Instruction> instructions;

      CompiledTransaction::Message compile(const std::vector<Keypair>& signers) {

        //TODO check if recentBlockhash is the default pubkey
        // if (recentBlockhash.empty()) {
        //   throw std::runtime_error("recentBlockhash required");
        // }
        if (instructions.empty()) {
          throw std::runtime_error("No instructions provided");
        }
        if (signers.size() == 0) {
          throw std::runtime_error("No signers provided");
        }

        std::vector<Transaction::Message::Instruction::AccountMeta> accountMetas;
        for (auto& instruction : instructions) {
          for (auto& account : instruction.accounts) {
            //TODO there is a bug here, if the same account is used twice with different signer or writable flags.
            if (std::find(accountMetas.begin(), accountMetas.end(), account) == accountMetas.end()) {
              accountMetas.push_back(account);
            }
          }
        }

        std::vector<PublicKey> programIds;
        for (auto& instruction : instructions) {
          auto programId = instruction.programId;
          if (std::find(programIds.begin(), programIds.end(), programId) == programIds.end()) {
            programIds.push_back(programId);
            accountMetas.push_back({programId, false, false});
          }
        }

        // Sort. Prioritizing first by signer, then by writable
        std::sort(accountMetas.begin(), accountMetas.end(), [](const auto& a, const auto& b) {
          if (a.isSigner != b.isSigner) {
            return a.isSigner;
          }
          if (a.isWritable != b.isWritable) {
            return a.isWritable;
          }
          return a.pubkey < b.pubkey;
        });

        // Use implicit fee payer
        auto fee_payer = signers[0].publicKey;

        // Move fee payer to the front
        auto feePayerIndex = std::find(accountMetas.begin(), accountMetas.end(), fee_payer);
        if (feePayerIndex != accountMetas.end()) {
          auto payerMeta = *feePayerIndex;
          accountMetas.erase(feePayerIndex);
          payerMeta.isSigner = true;
          payerMeta.isWritable = true;
          accountMetas.insert(accountMetas.begin(), payerMeta);
        } else {
          accountMetas.insert(accountMetas.begin(), {fee_payer, true, true});
        }

        uint8_t numRequiredSignatures = 0;
        uint8_t numReadonlySignedAccounts = 0;
        uint8_t numReadonlyUnsignedAccounts = 0;

        // Split out signing from non-signing keys and count header values
        std::vector<PublicKey> signedKeys;
        std::vector<PublicKey> unsignedKeys;
        for (auto& accountMeta : accountMetas) {
          if (accountMeta.isSigner) {
            signedKeys.push_back(accountMeta.pubkey);
            numRequiredSignatures += 1;
            if (!accountMeta.isWritable) {
              numReadonlySignedAccounts += 1;
            }
          } else {
            unsignedKeys.push_back(accountMeta.pubkey);
            if (!accountMeta.isWritable) {
              numReadonlyUnsignedAccounts += 1;
            }
          }
        }

        auto accountKeys = signedKeys;
        accountKeys.insert(accountKeys.end(), unsignedKeys.begin(), unsignedKeys.end());

        std::vector<CompiledTransaction::Message::Instruction> compiledInstructions;
        for (auto& instruction : instructions) {
          std::vector<uint8_t> accountsIndexes;
          for (auto& account : instruction.accounts) {
            auto index = std::find(accountKeys.begin(), accountKeys.end(), account.pubkey);
            if (index == accountKeys.end()) {
              throw std::runtime_error("Unknown account");
            }
            accountsIndexes.push_back(index - accountKeys.begin());
          }
          compiledInstructions.push_back(CompiledTransaction::Message::Instruction {
            .accounts = accountsIndexes,
            .data = instruction.data,
            .programIdIndex = (uint8_t)(std::find(accountKeys.begin(), accountKeys.end(), instruction.programId) - accountKeys.begin()),
          });
        }

        return {
          {
            numRequiredSignatures,
            numReadonlySignedAccounts,
            numReadonlyUnsignedAccounts,
          },
          accountKeys,
          recentBlockhash,
          compiledInstructions
        };
      }
    } message;

    /**
     * Add an instruction to the transaction message
     *
     * @param instruction The instruction to add
     */
    void add(Transaction::Message::Instruction instruction) {
      message.instructions.push_back(instruction);
    }
  };

  void from_json(const json& j, Transaction::Message::Instruction::AccountMeta& accountMeta) {
    accountMeta.pubkey = j["pubkey"].get<PublicKey>();
    accountMeta.isSigner = j["isSigner"].get<bool>();
    accountMeta.isWritable = j["isWritable"].get<bool>();
  }

  struct TransactionResponseReturnData {
    /** The program that generated the return data */
    PublicKey programId;
    /** The return data itself */
    std::string data;
  };

  void from_json(const json& j, TransactionResponseReturnData& transactionReturnData) {
    transactionReturnData.programId = j["programId"].get<PublicKey>();
    transactionReturnData.data = j["data"].get<std::string>();
  }

  struct TransactionResponse {
    /** The slot this transaction was processed in */
    uint64_t slot;
    /** The estimated production time of when the transaction was processed. */
    uint64_t blockTime;
    /** The transaction */
    CompiledTransaction transaction;
    /** Transaction status metadata object */
    struct Meta {
      /** Error if the transaction failed */
      std::string err;
      /** Fee this transaction was charged, as u64 integer */
      uint64_t fee;
      /** List of inner instructions if inner instruction recording was enabled during this transaction */
      struct InnerInstruction {
        /** Index of the transaction instruction from which the inner instruction(s) originated */
        uint64_t index;
        /** Ordered list of inner program instructions that were invoked during a single transaction instruction. */
        struct Instruction {
          /** Index into the message.accountKeys array indicating the program account that executes this instruction */
          uint64_t programIdIndex;
          /** List of ordered indices into the message.accountKeys array indicating which accounts to pass to the program. */
          std::vector<uint64_t> accounts;
          /** The program input data encoded in a base-58 string. */
          std::string data;
        };
        std::vector<Instruction> instructions;
      };
      std::vector<InnerInstruction> innerInstructions;
      /** Pubkeys for loaded accounts */
      struct LoadedAddresses {
        /** Ordered list of Pubkeys for writable loaded accounts */
        std::vector<PublicKey> writable;
        /** Ordered list of Pubkeys for read-only loaded accounts */
        std::vector<PublicKey> readonly;
      } loadedAddresses;
      /** Array of log messages if log message recording was enabled during this transaction */
      std::vector<std::string> logMessages;
      /** Array of u64 account balances from before the transaction was processed */
      std::vector<uint64_t> preBalances;
      /** List of token balances from before the transaction was processed */
      std::vector<TokenBalance> preTokenBalances;
      /** Array of u64 account balances from after the transaction was processed */
      std::vector<uint64_t> postBalances;
      /** List of token balances from after the transaction was processed */
      std::vector<TokenBalance> postTokenBalances;
      /** Transaction-level rewards, populated if rewards are requested */
      struct TransactionReward {
        /** The Pubkey of the account that received the reward */
        PublicKey pubkey;
        /** The number of reward lamports credited or debited by the account */
        uint64_t lamports;
        /** The account balance in lamports after the reward was applied */
        uint64_t postBalance;
        /** The type of reward */
        std::string rewardType;
        /** Vote account commission when the reward was credited, only present for voting and staking rewards */
        uint8_t commission;
      };
      std::vector<TransactionReward> rewards;
    } meta;
    /** The return data for the transaction */
    TransactionResponseReturnData returnData;
  };

  void from_json(const json& j, TransactionResponse::Meta::InnerInstruction::Instruction& metaInstruction) {
    metaInstruction.programIdIndex = j["programIdIndex"].get<uint64_t>();
    metaInstruction.accounts = j["accounts"].get<std::vector<uint64_t>>();
    metaInstruction.data = j["data"].get<std::string>();
  }

  void from_json(const json& j, TransactionResponse::Meta::InnerInstruction& metaInnerInstruction) {
    metaInnerInstruction.index = j["index"].get<uint64_t>();
    metaInnerInstruction.instructions = j["instructions"].get<std::vector<TransactionResponse::Meta::InnerInstruction::Instruction>>();
  }

  void from_json(const json& j, TransactionResponse::Meta::TransactionReward& reward) {
    reward.pubkey = j["pubkey"].get<PublicKey>();
    reward.lamports = j["lamports"].get<uint64_t>();
    reward.postBalance = j["postBalance"].get<uint64_t>();
    reward.rewardType = j["rewardType"].get<std::string>();
    if (j.find("commission") != j.end()) {
      reward.commission = j["commission"].get<uint8_t>();
    }
  }

  void from_json(const json& j, TransactionResponse::Meta::LoadedAddresses& loadedAddresses) {
    loadedAddresses.readonly = j["readonly"].get<std::vector<PublicKey>>();
    loadedAddresses.writable = j["writable"].get<std::vector<PublicKey>>();
  }

  void from_json(const json& j, TransactionResponse::Meta& meta) {
    if (!j.at("err").is_null()) {
      meta.err = j["err"].get<std::string>();
    } else {
      meta.err = "";
    }
    meta.fee = j["fee"].get<uint64_t>();
    meta.innerInstructions = j["innerInstructions"].get<std::vector<TransactionResponse::Meta::InnerInstruction>>();
    meta.logMessages = j["logMessages"].get<std::vector<std::string>>();
    meta.loadedAddresses = j["loadedAddresses"].get<TransactionResponse::Meta::LoadedAddresses>();
    meta.postBalances = j["postBalances"].get<std::vector<uint64_t>>();
    meta.postTokenBalances = j["postTokenBalances"].get<std::vector<TokenBalance>>();
    meta.preBalances = j["preBalances"].get<std::vector<uint64_t>>();
    meta.preTokenBalances = j["preTokenBalances"].get<std::vector<TokenBalance>>();
    meta.rewards = j["rewards"].get<std::vector<TransactionResponse::Meta::TransactionReward>>();
  }

  void from_json(const json& j, TransactionResponse& transactionResponse) {
    transactionResponse.slot = j["slot"].get<uint64_t>();
    transactionResponse.blockTime = j["blockTime"].get<uint64_t>();
    transactionResponse.transaction = j["transaction"].get<CompiledTransaction>();
    transactionResponse.meta = j["meta"].get<TransactionResponse::Meta>();
    if (j.contains("returnData")) {
      transactionResponse.returnData = j["returnData"].get<TransactionResponseReturnData>();
    }
  }

  struct SimulatedTransactionResponse {
    /** Error if the transaction failed */
    std::string err;
    /** Array of log messages the transaction instructions output during execution */
    std::vector<std::string> logs;
    /** Array of accounts with the same length as the accounts.addresses array in the request */
    std::vector<AccountInfo> accounts;
    /** The number of compute budget units consumed during the processing of this transaction */
    uint64_t unitsConsumed;
    /** The return data for the simulated transaction */
    TransactionResponseReturnData returnData;
  };

  void from_json(const json& j, SimulatedTransactionResponse& simulatedTransactoinResponse) {
    simulatedTransactoinResponse.err = j["value"]["err"].get<std::string>();
    simulatedTransactoinResponse.logs = j["value"]["logs"].get<std::vector<std::string>>();
    simulatedTransactoinResponse.accounts = j["value"]["accounts"].get<std::vector<AccountInfo>>();
    simulatedTransactoinResponse.unitsConsumed = j["value"]["unitsConsumed"].get<uint64_t>();
    simulatedTransactoinResponse.returnData = j["value"]["returnData"].get<TransactionResponseReturnData>();
  }

  struct Version {
    /** The current solana feature set enabled */
    uint64_t feature_set;
    /** The current solana version */
    std::string version;
  };

  void from_json(const json& j, Version& version) {
    if (j.contains("feature-set")) {
      version.feature_set = j["feature-set"].get<uint64_t>();
    }
    if (j.contains("solana-core")) {
      version.version = j["solana-core"].get<std::string>();
    }
  }

  /** Connection method results / error handling */
  struct ResultError {
    int64_t code;
    std::string message;
  };

  void from_json(const json& j, ResultError& t) {
    t.code = j["code"].get<int64_t>();
    t.message = j["message"].get<std::string>();
  }

  template <typename T>
  class Result {
  public:

    std::optional<Context> context;
    std::optional<T> result;
    std::optional<ResultError> error;

    Result() = default;

    Result(T result) : result(result) {}

    Result(ResultError error) : error(error) {}

    T unwrap() {
      if (error) {
        std::cerr << error->message << std::endl;
        throw std::runtime_error(error->message);
      }
      return result.value();
    }
  };

  template <typename T>
  void from_json(const json& j, Result<T>& r) {
    if (j.contains("result")) {
      if (j["result"].contains("context")) {
        r.context = j["result"]["context"].get<Context>();
      }

      if (j["result"].contains("value")) {
        if (!j["result"]["value"].is_null()) {
          r.result = j["result"]["value"].get<T>();
        }
      } else {
        if (!j["result"].is_null()) {
          r.result = j["result"].get<T>();
        }
      }
    } else if (j.contains("error")) {
      r.error = j["error"].get<ResultError>();
    }
  }

   namespace websockets {

    class WebSocketClient {
      const std::string _interface;
      const std::string _url;

      int _socket;
      bool _use_ssl = false;
      SSL_CTX *_ssl_ctx;
      SSL *_ssl;

      std::array<uint8_t, 17> _nonce;
      bool _handshake_complete = false;

      static const int SEND_BUFFER_SIZE = 65536;
      static const int RECEIVE_BUFFER_SIZE = 8388608;

      char* _send_buffer = nullptr;
      char* _recv_buffer = nullptr;

      static const int MESSAGE_BUFFER_SIZE = 1048576; // 1024 * 1024

      char* _message_buffer;
      int _message_start = 0;
      int _message_end = 0;

      uint8_t _send_mask[4];

      //static const uint8_t OPCODE_CONT   = 0x00;
      static const uint8_t OPCODE_TEXT   = 0x01;
      //static const uint8_t OPCODE_BINARY = 0x02;
      static const uint8_t OPCODE_CLOSE  = 0x08;
      static const uint8_t OPCODE_PING   = 0x09;
      static const uint8_t OPCODE_PONG   = 0x0A;
      static const uint8_t FIN_FLAG      = 0x80;
      static const uint8_t MASK_FLAG     = 0x80;

      int _nextSubscriptionId = 0;
      std::map<int, void*> _subscriptions;
      std::map<int, int> _subscription_map;

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
            if (strncasecmp(&buffer[start], "Connection: ", 12) == 0) {
              if (strncasecmp(&buffer[start], "Connection: Upgrade", 19) != 0 && strncasecmp(&buffer[start], "Connection: upgrade", 19) != 0) {
                return false;
              }
            }
            else if (strncasecmp(&buffer[start], "Upgrade: ", 9) == 0) {
              if (strncasecmp(&buffer[start], "Upgrade: websocket", 18) != 0) {
                return false;
              }
            }
            else if (strncasecmp(&buffer[start], "Sec-WebSocket-Accept: ", 22) == 0) {
              std::string key = base64::encode(_nonce.begin(), 16) + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

              char sha1[20];
              SHA1((const unsigned char *)key.data(), key.size(), (unsigned char *)sha1);
              std::string hash = base64::encode((unsigned char *)sha1, 20);

              accept = (strncasecmp(&buffer[start + 22], hash.c_str(), 20) == 0);
            }

            start = end + 1;
          }
        }

        return accept;
      }

      char* read(int& length) {
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
          length = (_use_ssl) ? SSL_read(_ssl, _recv_buffer, 8192) : ::read(_socket, _recv_buffer, 8192);

          if (length > 0) {
            return _recv_buffer;
          }
          else if (length < 0) {
            std::cerr << "READ FAILED" << std::endl;

            if (_use_ssl) {
              switch (SSL_get_error(_ssl, length)) {
                case SSL_ERROR_WANT_WRITE: std::cerr << "SSL_ERROR_WANT_WRITE" << std::endl; break;
                case SSL_ERROR_WANT_READ: std::cerr << "SSL_ERROR_WANT_READ" << std::endl; break;
                case SSL_ERROR_ZERO_RETURN: std::cerr << "SSL_ERROR_ZERO_RETURN" << std::endl; break;
                case SSL_ERROR_SYSCALL: std::cerr << "SSL_ERROR_SYSCALL" << std::endl; break;
                case SSL_ERROR_SSL: std::cerr << "SSL_ERROR_SSL" << std::endl; break;
              }
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

      bool write(const void* buffer, const int length) {
        if (!is_connected()) {
          return false;
        }

        ASSERT(length > 0);
        int return_code = (_use_ssl) ? SSL_write(_ssl, buffer, length) : ::write(_socket, buffer, length);

        if (_handshake_complete) {
          //TODO: log
        }

        if (return_code <= 0) {
          if (_use_ssl) {
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
          }

          std::cerr << "SEND FAILED" << std::endl;
          disconnect();
          return false;
        }

        return true;
      }

      // void send_close(std::string message) {
      //   send_message(OPCODE_CLOSE, message.c_str(), message.size());
      // }

      // void send_ping(std::string message) {
      //   send_message(OPCODE_PING, message.c_str(), message.size());
      // }

      // void send_pong(std::string message) {
      //   send_message(OPCODE_PONG, message.c_str(), message.size());
      // }

      void send_message(json j) {
        std::string message = j.dump();
        send_message(OPCODE_TEXT, message.c_str(), message.size());
      }

    public:

      WebSocketClient(const std::string url, const std::string interface = "")
        : _url(url),
        _interface(interface),
        _socket(-1),
        _ssl_ctx(nullptr),
        _ssl(nullptr)
      {
        _nonce[16] = 0;

        _send_buffer = (char*)malloc(SEND_BUFFER_SIZE);
        _recv_buffer = (char*)malloc(RECEIVE_BUFFER_SIZE);
        _message_buffer = (char*)malloc(MESSAGE_BUFFER_SIZE);
      }

      ~WebSocketClient() {
        disconnect();

        free(_send_buffer);
        free(_recv_buffer);
        free(_message_buffer);
      }

      WebSocketClient() = delete;
      WebSocketClient(const WebSocketClient&) = delete;
      WebSocketClient(WebSocketClient&&) = delete;
      WebSocketClient& operator=(const WebSocketClient&) = delete;
      WebSocketClient& operator=(WebSocketClient&&) = delete;

      bool is_connected() {
        return _socket != -1;
      }

      bool connect() {
        std::size_t index = _url.find("://");
        ASSERT(index != std::string::npos);
        std::string protocol = _url.substr(0, index);
        _use_ssl = (protocol == "wss");
        int port = _use_ssl ? 443 : 80;
        index += 3; // "://"
        std::size_t end = _url.find("/", index + 1);
        std::string hostname = _url.substr(index, end - index);
        index = hostname.find(":");
        if (index != std::string::npos) {
          port = std::stoi(hostname.substr(index + 1)) + 1;
          hostname = hostname.substr(0, index);
        }

        struct hostent *server;
        server = gethostbyname(hostname.c_str());
        if (server == NULL) {
          std::cerr << "Error: gethostbyname() failed" << std::endl;
          return false;
        }

        ASSERT(server->h_addrtype == AF_INET);

        int i = 0;
        while (server->h_addr_list[i] != NULL) {
          if (connect(hostname, (struct in_addr*)server->h_addr_list[i], port)) {
            return true;
          }
          i++;
        }

        return false;
      }

    private:

      bool connect(const std::string& hostname, struct in_addr *addr, uint16_t port) {
        ASSERT(_socket == -1);

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
        remoteaddr.sin_port = htons(port);
        remoteaddr.sin_addr.s_addr = *(long *)addr;

        std::string address = inet_ntoa(*addr);

        if (::connect(_socket, (sockaddr *)&remoteaddr, (int)sizeof(remoteaddr)) == -1) {
          std::cerr << "CONNECT FAILED" << std::endl;
          std::cerr << "  socket = " << std::to_string(_socket) << std::endl;
          std::cerr << "  address = " << address << std::endl;
          std::cerr << "  port = " << std::to_string(port) << std::endl << std::endl;
          disconnect();
          return false;
        }

        if (_use_ssl) {
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
        }

        // std::cout << "CONNECT" << std::endl;
        // std::cout << "  socket = " << std::to_string(_socket) << std::endl;
        // std::cout << "  address = " << address << std::endl;
        // std::cout << "  port = " << std::to_string(port) << std::endl << std::endl;

        std::generate(_nonce.begin(), _nonce.end(), []() { return (uint8_t)std::rand(); });
        std::string websocket_key = base64::encode(_nonce.begin(), 16);

        int send_length = 0;
        send_length += sprintf(&_send_buffer[send_length], "GET / HTTP/1.1\r\n");
        send_length += sprintf(&_send_buffer[send_length], "Host: %s\r\n", hostname.c_str());
        send_length += sprintf(&_send_buffer[send_length], "Upgrade: websocket\r\n");
        send_length += sprintf(&_send_buffer[send_length], "Connection: Upgrade\r\n");
        send_length += sprintf(&_send_buffer[send_length], "Sec-WebSocket-Key: %s\r\n", websocket_key.c_str());
        send_length += sprintf(&_send_buffer[send_length], "Sec-WebSocket-Version: 13\r\n");
        send_length += sprintf(&_send_buffer[send_length], "\r\n");

        write(_send_buffer, send_length);

        while (is_connected()) {
          int length;
          char *buffer = read(length);

          if (length > 0) {
            if (validate_handshake(buffer, length)) {
              //end_read(0);
              _handshake_complete = true;
              *((uint32_t *)_send_mask) = rand();
              return true;
            }
            else {
              //end_read(0);
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
        if (_use_ssl) {
          if (_ssl != nullptr) {
            SSL_shutdown(_ssl);
            SSL_free(_ssl);
            _ssl = nullptr;
          }
          if (_ssl_ctx != nullptr) {
            SSL_CTX_free(_ssl_ctx);
            _ssl_ctx = nullptr;
          }
        }
        if (_socket != -1) {
          close(_socket);
          _socket = -1;
        }
        return true;
      }

      void poll() {
        if (!is_connected() || !_handshake_complete) {
          return;
        }

        int length = 0;
        char* buffer = read(length);

        if ((_message_end + length) >= MESSAGE_BUFFER_SIZE)
        {
          std::cerr << "Message buffer out of space." << std::endl;
          disconnect();
          _message_start = 0;
          _message_end = 0;
          return;
        }

        memcpy(&_message_buffer[_message_end], buffer, length);
        _message_end += length;

        while ((_message_start + 1) < _message_end)
        {
          uint8_t opcode = _message_buffer[_message_start] & 0x0F;
          bool fin = ((_message_buffer[_message_start] >> 7) & 0x01) != 0;

          bool mask = ((_message_buffer[_message_start + 1] >> 7) & 0x01) != 0;

          if (mask)
          {
            std::cerr << "Mask not expected." << std::endl;
            disconnect();
            _message_start = 0;
            _message_end = 0;
            return;
          }

          size_t payload_size = _message_buffer[_message_start + 1] & ~MASK_FLAG;

          if (payload_size <= 125)
          {
            ASSERT((_message_start + payload_size) < MESSAGE_BUFFER_SIZE);
            if ((_message_start + 2 + payload_size) > _message_end)
            {
              break;
            }
            _message_start += 2;
          }
          else if (payload_size == 126)
          {
            payload_size = ntohs(*(uint16_t*)&_message_buffer[_message_start + 2]);
            ASSERT((_message_start + payload_size) < MESSAGE_BUFFER_SIZE);
            if ((_message_start + 4 + payload_size) > _message_end)
            {
              break;
            }
            _message_start += 4;
          }
          else if (payload_size == 127)
          {
            payload_size = endian::be64toh(*(uint64_t*)&_message_buffer[_message_start + 2]);
            ASSERT((_message_start + payload_size) < MESSAGE_BUFFER_SIZE);
            if ((_message_start + 10 + payload_size) > _message_end)
            {
              break;
            }
            _message_start += 10;
          }

          ASSERT((_message_start + payload_size) <= _message_end);

          switch (opcode)
          {
            case OPCODE_TEXT:
            {
              if (_message_buffer[_message_start] != '{')
              {
                std::cerr << "Expected '{'." << std::endl;
                disconnect();
                _message_start = 0;
                _message_end = 0;
                return;
              }

              if (payload_size > 0) {
                std::string message = std::string(&_message_buffer[_message_start], payload_size);
                json j = json::parse(message);

                if (j.contains("params")) {
                  std::string method = j["method"];
                  int subscription = j["params"]["subscription"];
                  if (_subscription_map.find(subscription) != _subscription_map.end()) {
                    int subscription_id = _subscription_map[subscription];
                    if (_subscriptions.find(subscription_id) != _subscriptions.end()) {
                      if (method == "slotNotification") {
                        SlotInfo slotInfo = j["params"]["result"].get<SlotInfo>();
                        std::function<void(SlotInfo slotInfo)>* callback = (std::function<void(SlotInfo slotInfo)>*)_subscriptions[subscription_id];
                        (*callback)(slotInfo);
                      } else if (method == "accountNotification") {
                        Result<Account> result = j["params"].get<Result<Account>>();
                        std::function<void(Result<Account>)>* callback = (std::function<void(Result<Account>)>*)_subscriptions[subscription_id];
                        (*callback)(result);
                      } else {
                        std::cerr << "Unknown notification: " << method << std::endl;
                      }
                    }
                  }
                } else if (j.contains("id") && j.contains("result")) {
                  int id = j["id"];
                  int result = j["result"];
                  _subscription_map[result] = id;
                }
              }
              break;
            }
            case OPCODE_CLOSE:
            {
              disconnect();
              return;
            }
            case OPCODE_PING:
            {
              //if (_pong_message.size() > 0)
              //{
                //send_pong(_pong_message);
              //}
              break;
            }
            case OPCODE_PONG:
            {
              //LastPongTimestamp = timestamp;
              break;
            }
          }

          _message_start += payload_size;
        }

        if (_message_start == _message_end)
        {
          _message_start = 0;
          _message_end = 0;
        }
        else if (_message_start >= 8192)
        {
          ASSERT(_message_end > _message_start);
          memmove(_message_buffer, &_message_buffer[_message_start], _message_end - _message_start);
          _message_end -= _message_start;
          _message_start = 0;
        }
      }

      int subscribe(std::string method, json params, void* callback) {
        if (!is_connected()) {
          connect();
        }
        ASSERT(is_connected());

        int subscriptionId = ++_nextSubscriptionId;
        if (params.is_null()) {
          send_message({
            {"jsonrpc", "2.0"},
            {"id", subscriptionId},
            {"method", method},
          });
        } else {
          send_message({
            {"jsonrpc", "2.0"},
            {"id", subscriptionId},
            {"method", method},
            {"params", params},
          });
        }

        _subscriptions[subscriptionId] = callback;
        return subscriptionId;
      }

      void unsubscribe(int subscriptionId, std::string method) {
        if (_subscriptions.find(subscriptionId) != _subscriptions.end()) {
          if (is_connected()) {
            send_message({
              {"jsonrpc", "2.0"},
              {"id", subscriptionId},
              {"method", method},
              {"params", {
                subscriptionId,
              }},
            });
          }
          _subscriptions.erase(subscriptionId);
        }
      }

    };

  } // namespace websockets

  class Connection {
    Commitment _commitment;
    std::string _rpcEndpoint;
    std::string _rpcWsEndpoint;
    websockets::WebSocketClient _rpcWebSocket;

    static std::string make_websocket_url(std::string endpoint) {
      auto url = endpoint;
      url.replace(0, 4, "ws");
      return url;
    }

  public:

    Connection(std::string endpoint, Commitment commitment)
      : _commitment(commitment),
      _rpcEndpoint(endpoint),
      _rpcWsEndpoint(make_websocket_url(endpoint)),
      _rpcWebSocket(_rpcWsEndpoint)
    {
      auto sodium_result = sodium_init();
      if (sodium_result == -1) {
        throw std::runtime_error("Failed to initialize libsodium");
      }
    }

    ~Connection() {
    }

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
    Result<Account> get_account_info(const PublicKey& publicKey) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getAccountInfo"},
        {"params", {
          publicKey.to_base58(),
          {
            {"encoding", "base64"},
          },
        }},
      });
    }

    /**
     * Returns the balance of the account of provided Pubkey.
     *
     * @param publicKey The Pubkey of the account to query
     */
    Result<uint64_t> get_balance(const PublicKey& publicKey) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getBalance"},
        {"params", {
          publicKey.to_base58(),
        }},
      });
    }

    /**
     * Returns information about all the nodes participating in the cluster.
     */
    Result<std::vector<ClusterNode>> get_cluster_nodes() {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getClusterNodes"},
      });
    }

    /**
     * Returns the identity Pubkey of the current node.
     */
    Result<Identity> get_identity() {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getIdentity"},
      });
    }

    /**
     * Returns the latest blockhash.
     */
    Result<Blockhash> get_latest_blockhash() const {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getLatestBlockhash"},
      });
    }

    /**
     * Returns the schedule for a given leader, for the current epoch.
     *
     * @param leaderAddress The Pubkey of the leader to query
     */
    Result<LeaderSchedule> get_leader_schedule(const PublicKey& leaderAddress) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getLeaderSchedule"},
        {"params", {
          {
            {"identity", leaderAddress.to_base58()},
          },
        }},
      });
    }

    /**
     * Returns the account information for a list of Pubkeys.
     *
     * @param publicKeys The Pubkeys of the accounts to query
     */
    Result<std::vector<Account>> get_multiple_accounts(const std::vector<PublicKey>& publicKeys) {
      std::vector<std::string> base58Keys;
      for (auto publicKey : publicKeys) {
        base58Keys.push_back(publicKey.to_base58());
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
      });
    }

    /**
     * Returns all accounts owned by the provided program Pubkey.
     *
     * @param programId The Pubkey of the program to query
     */
    Result<std::vector<AccountInfo>> get_program_accounts(const PublicKey& programId) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getProgramAccounts"},
        {"params", {
          programId.to_base58(),
        }},
      });
    }

    /**
     * Returns the slot that has reached the given or default commitment level.
     */
    Result<uint64_t> get_slot(const Commitment& commitment = Commitment::Finalized) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getSlot"},
      });
    }

    /**
     * Returns the current slot leader.
     */
    Result<PublicKey> get_slot_leader() {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getSlotLeader"},
      });
    }

    /**
     * Returns the token balance of an SPL Token account.
     *
     * @param tokenAddress The Pubkey of the token account to query
     */
    Result<TokenBalance> get_token_account_balance(const PublicKey& tokenAddress) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTokenAccountBalance"},
        {"params", {
          tokenAddress.to_base58(),
        }},
      });
    }

    /**
     * Returns all SPL Token accounts by token owner.
     *
     * @param ownerAddress The Pubkey of account owner to query
     */
    Result<std::vector<TokenAccount>> get_token_accounts_by_owner(const PublicKey& ownerAddress) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTokenAccountsByOwner"},
        {"params", {
          ownerAddress.to_base58(),
          {
            {"programId", TOKEN_PROGRAM_ID.to_base58()},
          },
          {
            {"encoding", "jsonParsed"},
          }
        }},
      });
    }

    /**
     * Returns SPL Token accounts for a given mint by token owner.
     *
     * @param ownerAddress The Pubkey of account owner to query
     * @param mintAddress The mint of the token to query
     */
    Result<std::vector<TokenAccount>> get_token_accounts_by_owner(const PublicKey& ownerAddress, const PublicKey& tokenMint) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTokenAccountsByOwner"},
        {"params", {
          ownerAddress.to_base58(),
          {
            {"mint", tokenMint.to_base58()},
          },
          {
            {"encoding", "jsonParsed"},
          }
        }},
      });
    }

    /**
     * Returns the total supply of an SPL Token type.
     *
     * @param tokenMintAddress The Pubkey of the token mint to query
     */
    Result<TokenBalance> get_token_supply(const PublicKey& tokenMintAddress) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTokenSupply"},
        {"params", {
          tokenMintAddress.to_base58(),
        }},
      });
    }

    /**
     * Returns transaction details for a confirmed transaction.
     *
     * @param transactionSignature The signature of the transaction to query
     */
    Result<TransactionResponse> get_transaction(const std::string& transactionSignature) {
      //TODO commitment

      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getTransaction"},
        {"params", {
          transactionSignature
        }},
      });
    }

    /**
     * Returns the current solana versions running on the node.
     */
    Result<Version> get_version() {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getVersion"},
      });
    }

    /**
     * Requests an airdrop of lamports to a Pubkey
     *
     * @param recipientAddress The Pubkey to airdrop lamports to
     * @param lamports The number of lamports to airdrop
     */
    Result<std::string> request_airdrop(const PublicKey& recipientAddress, const uint64_t& lamports = LAMPORTS_PER_SOL) {
      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "requestAirdrop"},
        {"params", {
          recipientAddress.to_base58(),
          lamports,
        }},
      });
    };

    /**
     * Signs and submits a transaction to the cluster for processing.
     *
     * @param transaction The transaction to sign
     * @param signers The keypairs to sign the transaction
     * @param options Options for sending the transaction
     */
    Result<std::string> sign_and_send_transaction(Transaction& transaction, const std::vector<Keypair>& signers) const {
      transaction.message.recentBlockhash = get_latest_blockhash().unwrap().blockhash;

      auto compiled_message = transaction.message.compile(signers);
      CompiledTransaction compiled_transaction = {
        .message = compiled_message,
      };

      std::vector<uint8_t> serialized_message;
      compiled_message.serialize(serialized_message);

      compiled_transaction.sign(serialized_message, signers);

      std::vector<uint8_t> serialized_transaction = compiled_transaction.serialize(serialized_message);

      return http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "sendTransaction"},
        {"params", {
          base64::encode(serialized_transaction),
          {
            {"encoding", "base64"},
          },
        }},
      });
    }

    /**
     * Simulate sending a transaction.
     *
     * @param signedTransaction The signed transaction to simulate
     */
    Result<SimulatedTransactionResponse> simulate_transaction(const std::string& signedTransaction) {
      return http::post(_rpcEndpoint, {
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
    }

    //-------- Websocket methods --------------------------------------------------------------------

    bool is_connected() {
      return _rpcWebSocket.is_connected();
    }

    /**
     * Poll the websocket for new messages.
     */
    void poll() {
      if (_rpcWebSocket.is_connected()) {
        _rpcWebSocket.poll();
      }
    }

    /**
     * Add an account change listener.
     *
     * @param accountId The account to listen for changes
     * @param callback The callback function to call when the account changes
     *
     * @return The subscription id. This can be used to remove the listener with remove_account_change_listener
    */
    int on_account_change(PublicKey accountId, std::function<void(Result<Account>)> callback) {
      return _rpcWebSocket.subscribe("accountSubscribe", {
          accountId.to_base58(),
          {
            {"encoding", "base64"},
            {"commitment", _commitment},
          },
        }, &callback);
    }

    /**
     * Remove an account change listener.
     *
     * @param subscriptionId The subscription id returned by on_account_change
     *
     * @return True if the listener was removed, false if the subscriptionId was not found
    */
    void remove_account_listener(int subscriptionId) {
      _rpcWebSocket.unsubscribe(subscriptionId, "accountUnsubscribe");
    }

    /**
     * Add a logs listener.
     *
     * @param accountId The account to listen for logs on
     * @param callback The callback function to call when logs are received
     *
     * @return The subscription id. This can be used to remove the listener with remove_on_logs_listener
    */
    int on_logs(PublicKey accountId, std::function<void(Context context, Logs logs)> callback) {
      return -1;
      // if (!_rpcWebSocket.is_connected()) {
      //   _rpcWebSocket.connect();
      // }
      // int subscriptionId = ++_nextSubscriptionId;
      // _rpcWebSocket.subscribe({
      //   {"jsonrpc", "2.0"},
      //   {"id", subscriptionId},
      //   {"method", "logsSubscribe"},
      //   {"params", {
      //     "mentions",
      //     {
      //       {"mentions", accountId.to_base58()},
      //     },
      //     {
      //       {"commitment", _commitment},
      //     },
      //   }},
      // });
      // Subscription subscription;
      // subscription.type = SubscriptionType::LOG;
      // subscription.logs = &callback;
      // _rpcWebSocket._subscriptions[subscriptionId] = subscription;
      // return subscriptionId;
    }


    /**
     * Remove a logs listener.
     *
     * @param subscriptionId The subscription id returned by on_logs
     *
     * @return true if the listener was removed, false if the subscriptionId was not found
    */
    void remove_on_logs_listener(int subscriptionId) {
      _rpcWebSocket.unsubscribe(subscriptionId, "logsUnsubscribe");
    }

    /**
     * Add a program account change listener.
     *
     * @param programId The program id to listen for
     * @param callback The callback function to call when a program account changes
     *
     * @return The subscription id. This can be used to remove the listener with remove_program_account_listener
    */
    int on_program_account_change(PublicKey programId, std::function<void(Result<Account>)> callback) {
      return _rpcWebSocket.subscribe("programSubscribe", { programId.to_base58(), {{"encoding", "base64"}, {"commitment", _commitment} } }, &callback);
    }

    /**
     * Remove a program account change listener.
     *
     * @param subscriptionId The subscription id returned by on_program_account_change
     *
     * @return true if the listener was removed, false if the subscriptionId was not found
    */
    void remove_program_account_change_listnener(int subscriptionId) {
      _rpcWebSocket.unsubscribe(subscriptionId, "programUnsubscribe");
    }

    /**
     * Add a slot change listener.
     *
     * @param callback The callback function to call when a slot changes
     *
     * @return The subscription id. This can be used to remove the listener with remove_slot_change_listener
    */
    int on_slot_change(std::function<void(SlotInfo slotInfo)> callback) {
      return _rpcWebSocket.subscribe("slotSubscribe", {}, &callback);
    }

    /**
     * Remove a slot change listener.
     *
     * @param subscriptionId The subscription id to remove (returned by on_slot_change)
     *
     * @return true if the listener was removed, false if the subscriptionId was not found
    */
    void remove_slot_change_listener(int subscriptionId) {
      _rpcWebSocket.unsubscribe(subscriptionId, "slotUnsubscribe");
    }

  };

  namespace token {

    /**
     * Returns an Instruction to create an Associated Token Account
     *
     * @param payer Payer of the initialization fees
     * @param associatedToken New associated token account
     * @param owner Owner of the new account
     * @param mint Token mint account
     * @param programId SPL Token program account
     * @param associatedTokenProgramId SPL Associated Token program account
     */
    Transaction::Message::Instruction create_associated_token_account_instruction(
      const PublicKey& payer,
      const PublicKey& associatedToken,
      const PublicKey& owner,
      const PublicKey& mint,
      const PublicKey& programId = TOKEN_PROGRAM_ID,
      const PublicKey& associatedTokenProgramId = ASSOCIATED_TOKEN_PROGRAM_ID
    ) {
      json accounts = {
        {
          { "pubkey", payer.to_base58() },
          { "isSigner", true },
          { "isWritable", true },
        },
        {
          { "pubkey", associatedToken.to_base58() },
          { "isSigner", false },
          { "isWritable", true },
        },
        {
          { "pubkey", owner.to_base58() },
          { "isSigner", false },
          { "isWritable", false },
        },
        {
          { "pubkey", mint.to_base58() },
          { "isSigner", false },
          { "isWritable", false },
        },
        {
          { "pubkey", SYSTEM_PROGRAM.to_base58() },
          { "isSigner", false },
          { "isWritable", false },
        },
        {
          { "pubkey", programId.to_base58() },
          { "isSigner", false },
          { "isWritable", false },
        },
      };

      return {
        associatedTokenProgramId,
        accounts,
        {}
      };
    }

    /**
     * Returns a Transaction to create an Associated Token Account
     *
     * @param payer Payer of the initialization fees
     * @param associatedToken New associated token account
     * @param owner Owner of the new account
     * @param mint Token mint account
     * @param programId SPL Token program account
     * @param associatedTokenProgramId SPL Associated Token program account
     */
    Transaction create_associated_token_account_transaction(
      const PublicKey& payer,
      const PublicKey& associatedToken,
      const PublicKey& owner,
      const PublicKey& mint,
      const PublicKey& programId = TOKEN_PROGRAM_ID,
      const PublicKey& associatedTokenProgramId = ASSOCIATED_TOKEN_PROGRAM_ID
    ) {
      Transaction tx;
      tx.add(
        create_associated_token_account_instruction(
          payer,
          associatedToken,
          owner,
          mint,
          programId,
          associatedTokenProgramId
        )
      );

      return tx;
    }

    /**
     * Get the address of the associated token account for a given mint and owner
     *
     * @param mint                     Token mint account
     * @param owner                    Owner of the new account
     * @param allowOwnerOffCurve       Allow the owner account to be a PDA (Program Derived Address)
     * @param programId                SPL Token program account
     * @param associatedTokenProgramId SPL Associated Token program account
     */
    PublicKey get_associated_token_address(
      const PublicKey& mint,
      const PublicKey& owner,
      const bool& allowOwnerOffCurve = false,
      const PublicKey& programId = TOKEN_PROGRAM_ID,
      const PublicKey& associatedTokenProgramId = ASSOCIATED_TOKEN_PROGRAM_ID
    ) {
      if (!allowOwnerOffCurve && !owner.is_on_curve()) {
        throw std::runtime_error("Token owner is off curve.");
      }

      std::tuple<PublicKey, uint8_t> pda = PublicKey::find_program_address(
        {
          owner.to_buffer(),
          programId.to_buffer(),
          mint.to_buffer()
        },
        associatedTokenProgramId
      );

      return std::get<0>(pda);
    }

    /**
    * Create and initialize a new associated token account
    *
    * @param connection Connection to use
    * @param payer Payer of the transaction and initialization fees
    * @param mint Mint for the account
    * @param owner Owner of the new account
    * @param confirmOptions Options for confirming the transaction
    * @param programId SPL Token program account
    * @param associatedTokenProgramId SPL Associated Token program account
    */
    Result<PublicKey> create_associated_token_account(
      const Connection& connection,
      const Keypair& payer,
      const PublicKey& mint,
      const PublicKey& owner,
      const ConfirmOptions& confirmOptions = {},
      const PublicKey& programId = TOKEN_PROGRAM_ID,
      const PublicKey& associatedTokenProgramId = ASSOCIATED_TOKEN_PROGRAM_ID
    ) {
      const PublicKey associatedToken = get_associated_token_address(mint, owner, false, programId, associatedTokenProgramId);

      Transaction transaction = create_associated_token_account_transaction(
        payer.publicKey,
        associatedToken,
        owner,
        mint,
        programId,
        associatedTokenProgramId
      );

      Result<std::string> result = connection.sign_and_send_transaction(transaction, {payer});
      if (result.error) {
        return Result<PublicKey>(result.error.value());
      }

      std::cout << result.unwrap() << std::endl;

      return Result<PublicKey>(associatedToken);;
    }

  }

}
