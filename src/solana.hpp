//  ____ _____ __    _____ _____ _____
// |  __|     |  |  |  _  |   | |  _  |  Solana C++ SDK
// |__  |  |  |  |__|     | | | |     |  version 0.0.1
// |____|_____|_____|__|__|_|___|__|__|  https://github.com/many-exchange/many-exchange-cpp
//
// Copyright (c) 2022-2023 Many Exchange
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "many.hpp"

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

namespace many {

  namespace solana {

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

    enum class Commitment {
      Processed,
      Confirmed,
      Finalized,
    };

    struct ConfirmOptions {
      Commitment commitment = Commitment::Finalized;
      bool preflight_commitment = false;
      bool encoding = "base64";
    };

    struct Blockhash {
      std::string blockhash;
    };

    void from_json(const json& j, Blockhash& blockhash) {
      blockhash.blockhash = j["blockhash"].get<std::string>();
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
      std::array<uint8_t, crypto_sign_SECRETKEYBYTES> secret_key;
      PublicKey public_key;

      Keypair() {
        auto sodium_result = sodium_init();
        if (sodium_result == -1) {
          throw std::runtime_error("Failed to initialize libsodium");
        }
        for (int i = 0; i < crypto_sign_SECRETKEYBYTES; i++) {
          secret_key[i] = 0;
        }
        public_key = PublicKey();
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
       * @param secret_key secret key byte array
       * @param options: skip secret key validation
       */
      Keypair(std::array<char, PUBLIC_KEY_LENGTH> secret_key, bool skipValidation = false) {
        auto sodium_result = sodium_init();
        if (sodium_result == -1) {
          throw std::runtime_error("Failed to initialize libsodium");
        }
        if (!skipValidation && crypto_sign_ed25519_sk_to_pk((unsigned char *)public_key.bytes.data(), (unsigned char *)secret_key.data()) != 0) {
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
            file.read((char *)result.secret_key.data(), size);
            crypto_sign_ed25519_sk_to_pk((unsigned char *)result.public_key.bytes.data(), (unsigned char *)result.secret_key.data());
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
        crypto_sign_seed_keypair((unsigned char *)result.public_key.bytes.data(), (unsigned char *)result.secret_key.data(), seed);
        return result;
      }

      /**
       * Create a new random keypair.
       */
      static Keypair generate() {
        Keypair result = Keypair();
        crypto_sign_keypair((unsigned char *)result.public_key.bytes.data(), (unsigned char *)result.secret_key.data());
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
        if (0 != crypto_sign_detached(sig, &sigSize, message.data(), message.size(), secret_key.data())) {
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
      uint64_t rent_epoch;
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
      account.rent_epoch = j["rentEpoch"].get<uint64_t>();
    }

    struct AccountInfo {
      PublicKey pubkey;
      Account account;
    };

    void from_json(const json& j, AccountInfo& accountInfo) {
      accountInfo.pubkey = j["pubkey"].get<PublicKey>();
      accountInfo.account = j["account"].get<Account>();
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
      uint64_t feature_set;
      /** The shred version the node has been configured to use */
      uint64_t shred_version;
    };

    void from_json(const json& j, ClusterNode& cluster_node) {
      if (!j["featureSet"].is_null()) {
        cluster_node.feature_set = j["featureSet"].get<uint64_t>();
      }
      cluster_node.gossip = j["gossip"].get<std::string>();
      cluster_node.pubkey = j["pubkey"].get<PublicKey>();
      if (!j["rpc"].is_null()) {
        cluster_node.rpc = j["rpc"].get<std::string>();
      }
      cluster_node.shred_version = j["shredVersion"].get<uint64_t>();
      if (!j["tpu"].is_null()) {
        cluster_node.tpu = j["tpu"].get<std::string>();
      }
      if (!j["version"].is_null()) {
        cluster_node.version = j["version"].get<std::string>();
      }
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
      std::vector<std::string> logs;
      std::string signature;
    };

    void from_json(const json& j, Logs& logs) {
      logs.logs = j["logs"].get<std::vector<std::string>>();
      logs.signature = j["signature"].get<std::string>();
    }

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

      std::optional<Context> _context;
      std::optional<T> _result;
      std::optional<ResultError> _error;

      Result() = default;

      Result(T result) : _result(result) {}

      Result(ResultError error) : _error(error) {}

      bool ok() const {
        return _result != std::nullopt;
      }

      T unwrap() {
        if (_error) {
          std::cerr << _error->message << std::endl;
          throw std::runtime_error(_error->message);
        }
        return _result.value();
      }
    };

    template <typename T>
    void from_json(const json& j, Result<T>& r) {
      if (j.contains("result")) {
        if (j["result"].contains("context")) {
          r._context = j["result"]["context"].get<Context>();
        }

        if (j["result"].contains("value")) {
          if (!j["result"]["value"].is_null()) {
            r._result = j["result"]["value"].get<T>();
          }
        } else {
          if (!j["result"].is_null()) {
            r._result = j["result"].get<T>();
          }
        }
      } else if (j.contains("error")) {
        r._error = j["error"].get<ResultError>();
      }
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
              bool is_native;
              /** The mint of the token */
              PublicKey mint;
              /** The Pubkey of the token account owner */
              PublicKey owner;
              /** The amount of tokens */
              TokenBalance token_amount;
              /** The account this token account is delegated to */
              PublicKey delegate;
              /** Amount delegated */
              TokenBalance delegated_amount;
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
        uint64_t rent_epoch;
      } account;
    };

    void from_json(const json& j, TokenAccount::Account::Data::Parsed::Info& parsedAccountInfo) {
      parsedAccountInfo.is_native = j["isNative"].get<bool>();
      parsedAccountInfo.mint = j["mint"].get<PublicKey>();
      parsedAccountInfo.owner = j["owner"].get<PublicKey>();
      parsedAccountInfo.token_amount = j["tokenAmount"].get<TokenBalance>();
      if (j.contains("delegate")) {
        parsedAccountInfo.delegate = j["account"]["data"]["parsed"]["info"]["delegate"].get<PublicKey>();
        parsedAccountInfo.delegated_amount = j["account"]["data"]["parsed"]["info"]["delegatedAmount"].get<TokenBalance>();
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
      account.rent_epoch = j["rentEpoch"].get<uint64_t>();
    }

    void from_json(const json& j, TokenAccount& tokenAccount) {
      tokenAccount.pubkey = j["pubkey"].get<PublicKey>();
      tokenAccount.account = j["account"].get<TokenAccount::Account>();
    }

    struct TransactionMessageHeader {
      /** The number of signatures required to validate this transaction */
      uint8_t num_required_signatures;
      /** The number of read-only signed accounts */
      uint8_t num_readonly_signed_accounts;
      /** The number of read-only unsigned accounts */
      uint8_t num_readonly_unsigned_accounts;
    };

    void from_json(const json& j, TransactionMessageHeader& header) {
      header.num_required_signatures = j["numRequiredSignatures"].get<uint8_t>();
      header.num_readonly_signed_accounts = j["numReadonlySignedAccounts"].get<uint8_t>();
      header.num_readonly_unsigned_accounts = j["numReadonlyUnsignedAccounts"].get<uint8_t>();
    }

    struct CompiledTransaction {
      /** Defines the content of the transaction */
      struct Message {
        /** The message header */
        TransactionMessageHeader header;
        /** List of base-58 encoded Pubkeys used by the transaction, including by the instructions and for signatures.
        The first message.header.numRequiredSignatures Pubkeys must sign the transaction. */
        std::vector<PublicKey> account_keys;
        /** A base-58 encoded hash of a recent block in the ledger used to prevent transaction duplication and to give transactions lifetimes. */
        PublicKey recent_blockhash;
        struct Instruction {
          /** Ordered indices into the `message.accountKeys` array indicating which accounts to pass to the program */
          std::vector<uint8_t> accounts;
          /** The program input data */
          std::vector<uint8_t> data;
          /** Index into the `message.accountKeys` array indicating the program account that executes this instruction */
          uint8_t program_id_index;
        };
        /** List of program instructions that will be executed in sequence and committed in one atomic transaction if all succeed. */
        std::vector<Instruction> instructions;

        void serialize(std::vector<uint8_t>& serialized_message) {
          if (serialized_message.size() > 0) {
            throw new std::runtime_error("Message is already serialized");
          }
          if (header.num_required_signatures == 0) {
            throw new std::runtime_error("Message must have at least one signature");
          }
          serialized_message.push_back(header.num_required_signatures);
          serialized_message.push_back(header.num_readonly_signed_accounts);
          serialized_message.push_back(header.num_readonly_unsigned_accounts);

          auto keyCount = encode_length(account_keys.size());
          serialized_message.insert(serialized_message.end(), keyCount.begin(), keyCount.end());

          for (auto& key : account_keys) {
            serialized_message.insert(serialized_message.end(), key.bytes.begin(), key.bytes.end());
          }

          serialized_message.insert(serialized_message.end(), recent_blockhash.bytes.begin(), recent_blockhash.bytes.end());

          auto instructionCount = encode_length(instructions.size());
          serialized_message.insert(serialized_message.end(), instructionCount.begin(), instructionCount.end());

          for (auto& instruction : instructions) {
            serialized_message.push_back(instruction.program_id_index);

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
        std::vector<uint8_t> signatures_length = encode_length(signatures.size());
        buffer.insert(buffer.end(), signatures_length.begin(), signatures_length.end());
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
          //signatures.insert(signer.public_key, signer.sign(serialized_message));
          signatures.push_back(signer.sign(serialized_message));
        }
      }
    };

    void from_json(const json& j, CompiledTransaction::Message::Instruction& instruction) {
      instruction.accounts = j["accounts"].get<std::vector<uint8_t>>();
      instruction.data = base64::decode(j["data"].get<std::string>());
      instruction.program_id_index = j["programIdIndex"].get<uint8_t>();
    }

    void from_json(const json& j, CompiledTransaction::Message& message) {
      message.account_keys = j["accountKeys"].get<std::vector<PublicKey>>();
      message.header.num_readonly_signed_accounts = j["header"]["numReadonlySignedAccounts"].get<uint8_t>();
      message.header.num_readonly_unsigned_accounts = j["header"]["numReadonlyUnsignedAccounts"].get<uint8_t>();
      message.header.num_required_signatures = j["header"]["numRequiredSignatures"].get<uint8_t>();
      message.instructions = j["instructions"].get<std::vector<CompiledTransaction::Message::Instruction>>();
      message.recent_blockhash = PublicKey(j["recentBlockhash"].get<std::string>());
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
        std::vector<PublicKey> account_keys;
        /** Recent blockhash */
        PublicKey recent_blockhash;
        struct Instruction {
          /** The program id that executes this instruction */
          PublicKey program_id;
          /** Account metadata used to define instructions */
          struct AccountMeta {
            /** An account's Pubkey */
            PublicKey pubkey;
            /** True if an instruction requires a transaction signature matching `pubkey` */
            bool is_signer;
            /** True if the Pubkey can be loaded as a read-write account. */
            bool is_writable;

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

          std::vector<Transaction::Message::Instruction::AccountMeta> account_metas;
          for (auto& instruction : instructions) {
            for (auto& account : instruction.accounts) {
              //TODO there is a bug here, if the same account is used twice with different signer or writable flags.
              if (std::find(account_metas.begin(), account_metas.end(), account) == account_metas.end()) {
                account_metas.push_back(account);
              }
            }
          }

          std::vector<PublicKey> programIds;
          for (auto& instruction : instructions) {
            auto programId = instruction.program_id;
            if (std::find(programIds.begin(), programIds.end(), programId) == programIds.end()) {
              programIds.push_back(programId);
              account_metas.push_back({programId, false, false});
            }
          }

          // Sort. Prioritizing first by signer, then by writable
          std::sort(account_metas.begin(), account_metas.end(), [](const auto& a, const auto& b) {
            if (a.is_signer != b.is_signer) {
              return a.is_signer;
            }
            if (a.is_writable != b.is_writable) {
              return a.is_writable;
            }
            return a.pubkey < b.pubkey;
          });

          // Use implicit fee payer
          auto fee_payer = signers[0].public_key;

          // Move fee payer to the front
          auto fee_payer_index = std::find(account_metas.begin(), account_metas.end(), fee_payer);
          if (fee_payer_index != account_metas.end()) {
            auto payerMeta = *fee_payer_index;
            account_metas.erase(fee_payer_index);
            payerMeta.is_signer = true;
            payerMeta.is_writable = true;
            account_metas.insert(account_metas.begin(), payerMeta);
          } else {
            account_metas.insert(account_metas.begin(), {fee_payer, true, true});
          }

          uint8_t num_required_signatures = 0;
          uint8_t num_readonly_signed_accounts = 0;
          uint8_t num_readonly_unsigned_accounts = 0;

          // Split out signing from non-signing keys and count header values
          std::vector<PublicKey> signed_keys;
          std::vector<PublicKey> unsigned_keys;
          for (auto& account_meta : account_metas) {
            if (account_meta.is_signer) {
              signed_keys.push_back(account_meta.pubkey);
              num_required_signatures += 1;
              if (!account_meta.is_writable) {
                num_readonly_signed_accounts += 1;
              }
            } else {
              unsigned_keys.push_back(account_meta.pubkey);
              if (!account_meta.is_writable) {
                num_readonly_unsigned_accounts += 1;
              }
            }
          }

          auto account_keys = signed_keys;
          account_keys.insert(account_keys.end(), unsigned_keys.begin(), unsigned_keys.end());

          std::vector<CompiledTransaction::Message::Instruction> compiled_instructions;
          for (auto& instruction : instructions) {
            std::vector<uint8_t> accounts_indexes;
            for (auto& account : instruction.accounts) {
              auto index = std::find(account_keys.begin(), account_keys.end(), account.pubkey);
              if (index == account_keys.end()) {
                throw std::runtime_error("Unknown account");
              }
              accounts_indexes.push_back(index - account_keys.begin());
            }
            compiled_instructions.push_back(CompiledTransaction::Message::Instruction {
              .accounts = accounts_indexes,
              .data = instruction.data,
              .program_id_index = (uint8_t)(std::find(account_keys.begin(), account_keys.end(), instruction.program_id) - account_keys.begin()),
            });
          }

          return {
            {
              num_required_signatures,
              num_readonly_signed_accounts,
              num_readonly_unsigned_accounts,
            },
            account_keys,
            recent_blockhash,
            compiled_instructions
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
      accountMeta.is_signer = j["isSigner"].get<bool>();
      accountMeta.is_writable = j["isWritable"].get<bool>();
    }

    struct TransactionResponseReturnData {
      /** The program that generated the return data */
      PublicKey program_d;
      /** The return data itself */
      std::string data;
    };

    void from_json(const json& j, TransactionResponseReturnData& transactionReturnData) {
      transactionReturnData.program_d = j["programId"].get<PublicKey>();
      transactionReturnData.data = j["data"].get<std::string>();
    }

    struct TransactionResponse {
      /** The slot this transaction was processed in */
      uint64_t slot;
      /** The estimated production time of when the transaction was processed. */
      uint64_t block_time;
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
            uint64_t program_id_index;
            /** List of ordered indices into the message.accountKeys array indicating which accounts to pass to the program. */
            std::vector<uint64_t> accounts;
            /** The program input data encoded in a base-58 string. */
            std::string data;
          };
          std::vector<Instruction> instructions;
        };
        std::vector<InnerInstruction> inner_instructions;
        /** Pubkeys for loaded accounts */
        struct LoadedAddresses {
          /** Ordered list of Pubkeys for writable loaded accounts */
          std::vector<PublicKey> writable;
          /** Ordered list of Pubkeys for read-only loaded accounts */
          std::vector<PublicKey> readonly;
        } loaded_addresses;
        /** Array of log messages if log message recording was enabled during this transaction */
        std::vector<std::string> log_messages;
        /** Array of u64 account balances from before the transaction was processed */
        std::vector<uint64_t> pre_balances;
        /** List of token balances from before the transaction was processed */
        std::vector<TokenBalance> pre_token_balances;
        /** Array of u64 account balances from after the transaction was processed */
        std::vector<uint64_t> post_balances;
        /** List of token balances from after the transaction was processed */
        std::vector<TokenBalance> post_token_balances;
        /** Transaction-level rewards, populated if rewards are requested */
        struct TransactionReward {
          /** The Pubkey of the account that received the reward */
          PublicKey pubkey;
          /** The number of reward lamports credited or debited by the account */
          uint64_t lamports;
          /** The account balance in lamports after the reward was applied */
          uint64_t post_balance;
          /** The type of reward */
          std::string reward_type;
          /** Vote account commission when the reward was credited, only present for voting and staking rewards */
          uint8_t commission;
        };
        std::vector<TransactionReward> rewards;
      } meta;
      /** The return data for the transaction */
      TransactionResponseReturnData return_data;
    };

    void from_json(const json& j, TransactionResponse::Meta::InnerInstruction::Instruction& metaInstruction) {
      metaInstruction.program_id_index = j["programIdIndex"].get<uint64_t>();
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
      reward.post_balance = j["postBalance"].get<uint64_t>();
      reward.reward_type = j["rewardType"].get<std::string>();
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
      meta.inner_instructions = j["innerInstructions"].get<std::vector<TransactionResponse::Meta::InnerInstruction>>();
      meta.log_messages = j["logMessages"].get<std::vector<std::string>>();
      meta.loaded_addresses = j["loadedAddresses"].get<TransactionResponse::Meta::LoadedAddresses>();
      meta.post_balances = j["postBalances"].get<std::vector<uint64_t>>();
      meta.post_token_balances = j["postTokenBalances"].get<std::vector<TokenBalance>>();
      meta.pre_balances = j["preBalances"].get<std::vector<uint64_t>>();
      meta.pre_token_balances = j["preTokenBalances"].get<std::vector<TokenBalance>>();
      meta.rewards = j["rewards"].get<std::vector<TransactionResponse::Meta::TransactionReward>>();
    }

    void from_json(const json& j, TransactionResponse& transactionResponse) {
      transactionResponse.slot = j["slot"].get<uint64_t>();
      transactionResponse.block_time = j["blockTime"].get<uint64_t>();
      transactionResponse.transaction = j["transaction"].get<CompiledTransaction>();
      transactionResponse.meta = j["meta"].get<TransactionResponse::Meta>();
      if (j.contains("returnData")) {
        transactionResponse.return_data = j["returnData"].get<TransactionResponseReturnData>();
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
      uint64_t units_consumed;
      /** The return data for the simulated transaction */
      TransactionResponseReturnData return_data;
    };

    void from_json(const json& j, SimulatedTransactionResponse& simulatedTransactoinResponse) {
      simulatedTransactoinResponse.err = j["value"]["err"].get<std::string>();
      simulatedTransactoinResponse.logs = j["value"]["logs"].get<std::vector<std::string>>();
      simulatedTransactoinResponse.accounts = j["value"]["accounts"].get<std::vector<AccountInfo>>();
      simulatedTransactoinResponse.units_consumed = j["value"]["unitsConsumed"].get<uint64_t>();
      simulatedTransactoinResponse.return_data = j["value"]["returnData"].get<TransactionResponseReturnData>();
    }

    class Connection {
      Commitment _commitment;
      std::string _rpc_endpoint;
      std::string _rpc_ws_endpoint;
      websockets::WebSocketClient _rpc_web_socket;

      static std::string make_websocket_url(std::string endpoint) {
        auto url = endpoint;
        url.replace(0, 4, "ws");
        return url;
      }

    public:

      Connection(std::string endpoint, Commitment commitment)
        : _commitment(commitment),
        _rpc_endpoint(endpoint),
        _rpc_ws_endpoint(make_websocket_url(endpoint)),
        _rpc_web_socket(_rpc_ws_endpoint) {
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
       * @param public_key The Pubkey of account to query
       */
      Result<Account> get_account_info(const PublicKey& public_key) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getAccountInfo"},
          {"params", {
            public_key.to_base58(),
            {
              {"encoding", "base64"},
            },
          }},
        });
      }

      /**
       * Returns the balance of the account of provided Pubkey.
       *
       * @param public_key The Pubkey of the account to query
       */
      Result<uint64_t> get_balance(const PublicKey& public_key) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getBalance"},
          {"params", {
            public_key.to_base58(),
          }},
        });
      }

      /**
       * Returns information about all the nodes participating in the cluster.
       */
      Result<std::vector<ClusterNode>> get_cluster_nodes() {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getClusterNodes"},
        });
      }

      /**
       * Returns the identity Pubkey of the current node.
       */
      Result<Identity> get_identity() {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getIdentity"},
        });
      }

      /**
       * Returns the latest blockhash.
       */
      Result<Blockhash> get_latest_blockhash() const {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getLatestBlockhash"},
        });
      }

      /**
       * Returns the schedule for a given leader, for the current epoch.
       *
       * @param leader_address The Pubkey of the leader to query
       */
      Result<LeaderSchedule> get_leader_schedule(const PublicKey& leader_address) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getLeaderSchedule"},
          {"params", {
            {
              {"identity", leader_address.to_base58()},
            },
          }},
        });
      }

      /**
       * Returns the account information for a list of Pubkeys.
       *
       * @param public_keys The Pubkeys of the accounts to query
       */
      Result<std::vector<Account>> get_multiple_accounts(const std::vector<PublicKey>& public_keys) {
        std::vector<std::string> base58Keys;
        for (auto public_key : public_keys) {
          base58Keys.push_back(public_key.to_base58());
        }
        return http::post(_rpc_endpoint, {
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
       * @param program_id The Pubkey of the program to query
       */
      Result<std::vector<AccountInfo>> get_program_accounts(const PublicKey& program_id) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getProgramAccounts"},
          {"params", {
            program_id.to_base58(),
            {
              {"encoding", "base64"},
            },
          }},
        });
      }

      /**
       * Returns the slot that has reached the given or default commitment level.
       */
      Result<uint64_t> get_slot(const Commitment& commitment = Commitment::Finalized) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getSlot"},
        });
      }

      /**
       * Returns the current slot leader.
       */
      Result<PublicKey> get_slot_leader() {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getSlotLeader"},
        });
      }

      /**
       * Returns the token balance of an SPL Token account.
       *
       * @param token_address The Pubkey of the token account to query
       */
      Result<TokenBalance> get_token_account_balance(const PublicKey& token_address) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getTokenAccountBalance"},
          {"params", {
            token_address.to_base58(),
          }},
        });
      }

      /**
       * Returns all SPL Token accounts by token owner.
       *
       * @param owner_address The Pubkey of account owner to query
       */
      Result<std::vector<TokenAccount>> get_token_accounts_by_owner(const PublicKey& owner_address) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getTokenAccountsByOwner"},
          {"params", {
            owner_address.to_base58(),
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
       * @param owner_address The Pubkey of account owner to query
       * @param token_mint The mint of the token to query
       */
      Result<std::vector<TokenAccount>> get_token_accounts_by_owner(const PublicKey& owner_address, const PublicKey& token_mint) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getTokenAccountsByOwner"},
          {"params", {
            owner_address.to_base58(),
            {
              {"mint", token_mint.to_base58()},
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
       * @param token_mint The Pubkey of the token mint to query
       */
      Result<TokenBalance> get_token_supply(const PublicKey& token_mint) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getTokenSupply"},
          {"params", {
            token_mint.to_base58(),
          }},
        });
      }

      /**
       * Returns transaction details for a confirmed transaction.
       *
       * @param transaction_signature The signature of the transaction to query
       */
      Result<TransactionResponse> get_transaction(const std::string& transaction_signature) {
        //TODO commitment

        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getTransaction"},
          {"params", {
            transaction_signature
          }},
        });
      }

      /**
       * Returns the current solana versions running on the node.
       */
      Result<Version> get_version() {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "getVersion"},
        });
      }

      /**
       * Requests an airdrop of lamports to a Pubkey
       *
       * @param recipient_address The Pubkey to airdrop lamports to
       * @param lamports The number of lamports to airdrop
       */
      Result<std::string> request_airdrop(const PublicKey& recipient_address, const uint64_t& lamports = LAMPORTS_PER_SOL) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "requestAirdrop"},
          {"params", {
            recipient_address.to_base58(),
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
        transaction.message.recent_blockhash = get_latest_blockhash().unwrap().blockhash;

        auto compiled_message = transaction.message.compile(signers);
        CompiledTransaction compiled_transaction = {
          .message = compiled_message,
        };

        std::vector<uint8_t> serialized_message;
        compiled_message.serialize(serialized_message);

        compiled_transaction.sign(serialized_message, signers);

        std::vector<uint8_t> serialized_transaction = compiled_transaction.serialize(serialized_message);

        return http::post(_rpc_endpoint, {
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
       * @param signed_transaction The signed transaction to simulate
       */
      Result<SimulatedTransactionResponse> simulate_transaction(const std::string& signed_transaction) {
        return http::post(_rpc_endpoint, {
          {"jsonrpc", "2.0"},
          {"id", 1},
          {"method", "simulateTransaction"},
          {"params", {
            signed_transaction,
            {
              {"encoding", "base64"},
            },
          }},
        });
      }

      //-------- Websocket methods --------------------------------------------------------------------

      bool is_connected() {
        return _rpc_web_socket.is_connected();
      }

      /**
       * Poll the websocket for new messages.
       */
      void poll() {
        if (_rpc_web_socket.is_connected()) {
          _rpc_web_socket.poll();
        }
      }

      /**
       * Add an account change listener.
       *
       * @param account_id The account to listen for changes
       * @param callback The callback function to call when the account changes
       *
       * @return The subscription ID. This can be used to remove the listener with remove_account_change_listener
      */
      int on_account_change(PublicKey account_id, std::function<void(Result<Account>)> callback) {
        return _rpc_web_socket.subscribe("accountSubscribe", {
            account_id.to_base58(),
            {
              {"encoding", "base64"},
              {"commitment", _commitment},
            },
          }, [callback](const json& j) {
            callback(Result<Account>(j));
          }
        );
      }

      /**
       * Remove an account change listener.
       *
       * @param subscription_id The subscription id returned by on_account_change
       *
       * @return True if the listener was removed, false if the subscription_id was not found
      */
      void remove_account_listener(int subscription_id) {
        _rpc_web_socket.unsubscribe(subscription_id, "accountUnsubscribe");
      }

      /**
       * Add a logs listener.
       *
       * @param account_id The account to listen for logs on
       * @param callback The callback function to call when logs are received
       *
       * @return The subscription ID. This can be used to remove the listener with remove_on_logs_listener
      */
      int on_logs(PublicKey account_id, std::function<void(Result<Logs>)> callback) {
        return _rpc_web_socket.subscribe("programSubscribe", {
            "mentions",
            {
              {"mentions", account_id.to_base58()}
            },
            {
              {"commitment", _commitment }
            },
          }, [callback](const json& j) {
            callback(Result<Logs>(j));
          }
        );
      }


      /**
       * Remove a logs listener.
       *
       * @param subscription_id The subscription id returned by on_logs
       *
       * @return true if the listener was removed, false if the subscription_id was not found
      */
      void remove_on_logs_listener(int subscription_id) {
        _rpc_web_socket.unsubscribe(subscription_id, "logsUnsubscribe");
      }

      /**
       * Add a program account change listener.
       *
       * @param program_id The program id to listen for
       * @param callback The callback function to call when a program account changes
       *
       * @return The subscription ID. This can be used to remove the listener with remove_program_account_listener
      */
      int on_program_account_change(PublicKey program_id, std::function<void(Result<Account>)> callback) {
        return _rpc_web_socket.subscribe("programSubscribe", {
            program_id.to_base58(),
            {
              {"encoding", "base64"},
              {"commitment", _commitment},
            },
          }, [callback](const json& j) {
            callback(Result<Account>(j));
          }
        );
      }

      /**
       * Remove a program account change listener.
       *
       * @param subscription_id The subscription id returned by on_program_account_change
       *
       * @return true if the listener was removed, false if the subscription_id was not found
      */
      void remove_program_account_change_listnener(int subscription_id) {
        _rpc_web_socket.unsubscribe(subscription_id, "programUnsubscribe");
      }

      /**
       * Add a slot change listener.
       *
       * @param callback The callback function to call when a slot changes
       *
       * @return The subscription ID. This can be used to remove the listener with remove_slot_change_listener
      */
      int on_slot_change(std::function<void(Result<SlotInfo>)> callback) {
        return _rpc_web_socket.subscribe("slotSubscribe", {
          }, [callback](const json& j) {
            callback(j);
          }
        );
      }

      /**
       * Remove a slot change listener.
       *
       * @param subscription_id The subscription id to remove (returned by on_slot_change)
       *
       * @return true if the listener was removed, false if the subscription_id was not found
      */
      void remove_slot_change_listener(int subscription_id) {
        _rpc_web_socket.unsubscribe(subscription_id, "slotUnsubscribe");
      }

    };

    namespace token {

      /**
       * Returns an Instruction to create an Associated Token Account
       *
       * @param payer Payer of the initialization fees
       * @param associated_token New associated token account
       * @param owner Owner of the new account
       * @param mint Token mint account
       * @param program_id SPL Token program account
       * @param associated_token_program_id SPL Associated Token program account
       */
      Transaction::Message::Instruction create_associated_token_account_instruction(
        const PublicKey& payer,
        const PublicKey& associated_token,
        const PublicKey& owner,
        const PublicKey& mint,
        const PublicKey& program_id = TOKEN_PROGRAM_ID,
        const PublicKey& associated_token_program_id = ASSOCIATED_TOKEN_PROGRAM_ID
      ) {
        json accounts = {
          {
            { "pubkey", payer.to_base58() },
            { "isSigner", true },
            { "isWritable", true },
          },
          {
            { "pubkey", associated_token.to_base58() },
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
            { "pubkey", program_id.to_base58() },
            { "isSigner", false },
            { "isWritable", false },
          },
        };

        return {
          associated_token_program_id,
          accounts,
          {}
        };
      }

      /**
       * Returns a Transaction to create an Associated Token Account
       *
       * @param payer Payer of the initialization fees
       * @param associated_token New associated token account
       * @param owner Owner of the new account
       * @param mint Token mint account
       * @param program_id SPL Token program account
       * @param associated_token_program_id SPL Associated Token program account
       */
      Transaction create_associated_token_account_transaction(
        const PublicKey& payer,
        const PublicKey& associated_token,
        const PublicKey& owner,
        const PublicKey& mint,
        const PublicKey& program_id = TOKEN_PROGRAM_ID,
        const PublicKey& associated_token_program_id = ASSOCIATED_TOKEN_PROGRAM_ID
      ) {
        Transaction tx;
        tx.add(
          create_associated_token_account_instruction(
            payer,
            associated_token,
            owner,
            mint,
            program_id,
            associated_token_program_id
          )
        );

        return tx;
      }

      /**
       * Get the address of the associated token account for a given mint and owner
       *
       * @param mint                     Token mint account
       * @param owner                    Owner of the new account
       * @param allow_owner_off_curve       Allow the owner account to be a PDA (Program Derived Address)
       * @param program_id                SPL Token program account
       * @param associated_token_program_id SPL Associated Token program account
       */
      PublicKey get_associated_token_address(
        const PublicKey& mint,
        const PublicKey& owner,
        const bool& allow_owner_off_curve = false,
        const PublicKey& program_id = TOKEN_PROGRAM_ID,
        const PublicKey& associated_token_program_id = ASSOCIATED_TOKEN_PROGRAM_ID
      ) {
        if (!allow_owner_off_curve && !owner.is_on_curve()) {
          throw std::runtime_error("Token owner is off curve.");
        }

        std::tuple<PublicKey, uint8_t> pda = PublicKey::find_program_address(
          {
            owner.to_buffer(),
            program_id.to_buffer(),
            mint.to_buffer()
          },
          associated_token_program_id
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
      * @param confirm_options Options for confirming the transaction
      * @param program_id SPL Token program account
      * @param associated_token_program_id SPL Associated Token program account
      */
      Result<PublicKey> create_associated_token_account(
        const Connection& connection,
        const Keypair& payer,
        const PublicKey& mint,
        const PublicKey& owner,
        const ConfirmOptions& confirm_options = {},
        const PublicKey& program_id = TOKEN_PROGRAM_ID,
        const PublicKey& associated_token_program_id = ASSOCIATED_TOKEN_PROGRAM_ID
      ) {
        const PublicKey associated_token = get_associated_token_address(mint, owner, false, program_id, associated_token_program_id);

        Transaction transaction = create_associated_token_account_transaction(
          payer.public_key,
          associated_token,
          owner,
          mint,
          program_id,
          associated_token_program_id
        );

        std::string txid = connection.sign_and_send_transaction(transaction, {payer}).unwrap();

        return Result<PublicKey>(associated_token);;
      }

    }

  }

}
