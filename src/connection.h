#pragma once

#include <assert.h>
#include <iostream>
#include <map>
#include <optional>
#include <sodium.h>
#include <string>
#include <vector>

#include "http.h"
#include "keypair.h"
#include "message.h"
#include "transaction.h"
#include "websocket.h"

using namespace solana;
using namespace solana::http;

namespace solana {

enum class Commitment {
  Processed,
  Confirmed,
  Finalized,
};

struct AccountInfo {
  /** `true` if this account's data contains a loaded program */
  bool executable;
  /** Identifier of the program that owns the account */
  PublicKey owner;
  /** Number of lamports assigned to the account */
  uint64_t lamports;
  /** Optional data assigned to the account */
  std::string data;
  /** Optional rent epoch info for account */
  uint64_t rentEpoch;
};

struct Context {
  uint64_t slot;
};

struct KeyedAccountInfo {
  PublicKey accountId;
  AccountInfo accountInfo;
};

struct Logs {
  //TODO
  //err: TransactionError | null;
  std::vector<std::string> logs;
  std::string signature;
};

struct SlotInfo {
  /** Currently processing slot */
  uint64_t slot;
  /** Parent of the current slot */
  uint64_t parent;
  /** The root block of the current slot's fork */
  uint64_t root;
};

struct TokenAmount {
  /** The amount of lamports */
  std::string amount;
  /** The number of decimals for the token amount */
  uint64_t decimals;
  /** The amount of tokens as a floating point number */
  double uiAmount;
  /** The amount of tokens as a string */
  std::string uiAmountString;
};

struct TokenAccount {
  /** The account data */
  struct TokenAccountInfo {
    struct TokenAccountData {
      /* Name of token program (ex: spl-token) */
      PublicKey program;
      struct ParsedTokenAccountData {
        /** The account type (ex: "account") */
        std::string accountType;
        struct TokenAccountBalanceInfo {
          /** The amount of tokens */
          TokenAmount tokenAmount;
          /** The account this token account is delegated to */
          PublicKey delegate;
          /** Amount delegated */
          TokenAmount delegatedAmount;
          /** State of the token account (ex: "initialized") */
          std::string state;
          /** `true` if the account is native */
          bool isNative;
          /** The mint associated with this account */
          PublicKey mint;
          /** The account owner */
          PublicKey owner;
        } info;
        /** The account type */
        std::string type;
      } parsed;
      /** The length of the account data */
      uint64_t space;
    } data;
    /** `true` if this account's data contains a loaded program */
    bool executable;
    /** Number of lamports assigned to the account */
    uint64_t lamports;
    /** Identifier of the program that owns the account */
    PublicKey owner;
    /** Optional rent epoch info for account */
    uint64_t rentEpoch;
  } account;
  /** The account's public key */
  PublicKey pubkey;
};

struct TransactionReponse {
  /* The estimated production time of when the transaction was processed. */
  uint64_t blockTime;
  /* Transaction status metadata object */
  struct Meta {
    /* Error if the transaction failed */
    std::string err;
    /* Fee this transaction was charged, as u64 integer */
    uint64_t fee;
    /* List of inner instructions if inner instruction recording was enabled during this transaction */
    //TODO this should be an array of inner instruction std::string innerInstructions;
    struct LoadedAddresses {
      /* Ordered list of base-58 encoded addresses for writable loaded accounts */
      std::vector<PublicKey> writable;
      /* Ordered list of base-58 encoded addresses for read-only loaded accounts */
      std::vector<PublicKey> readOnly;
    } loadedAddresses;
    /* Array of string log messages if log message recording was enabled during this transaction  */
    std::vector<std::string> logMessages;
    /* Array of u64 account balances from after the transaction was processed */
    std::vector<uint64_t> postBalances;
    /* Array of u64 account balances from before the transaction was processed */
    std::vector<uint64_t> preBalances;
    /* Transaction-level rewards, populated if rewards are requested */
    struct TransactionReward {
      /* The public key of the account that received the reward */
      PublicKey pubkey;
      /* The number of reward lamports credited or debited by the account */
      uint64_t lamports;
      /* The account balance in lamports after the reward was applied */
      uint64_t postBalance;
      /* The type of reward */
      std::string rewardType;
      /* Vote account commission when the reward was credited, only present for voting and staking rewards */
      uint8_t commission;
    };
    std::vector<TransactionReward> rewards;
  } meta;
  /* The slot this transaction was processed in */
  uint64_t slot;
  /* The transaction */
  struct Transaction {
    /* Defines the content of the transaction */
    struct Message {
      /* List of base-58 encoded public keys used by the transaction, including by the instructions and for signatures.
      The first message.header.numRequiredSignatures public keys must sign the transaction. */
      std::vector<PublicKey> accountKeys;
      struct Header {
        /* The number of signatures required to validate this transaction */
        uint8_t numRequiredSignatures;
        /* The number of read-only signed accounts */
        uint8_t numReadonlySignedAccounts;
        /* The number of read-only unsigned accounts */
        uint8_t numReadonlyUnsignedAccounts;
      } header;
      struct Instruction {
        /* Ordered indices into the `message.accountKeys` array indicating which accounts to pass to the program */
        std::vector<uint8_t> accounts;
        /* The program input data encoded as base-58 */
        std::string data;
        /* Index into the `message.accountKeys` array indicating the program account that executes this instruction */
        uint8_t programIdIndex;
      };
      /* List of program instructions that will be executed in sequence and committed in one atomic transaction if all succeed. */
      std::vector<Instruction> instructions;
      /* A base-58 encoded hash of a recent block in the ledger used to prevent transaction duplication and to give transactions lifetimes. */
      std::string recentBlockhash;
    } message;
    /* The transaction signatures */
    std::vector<std::string> signatures;
  } transaction;
};

void from_json(const nlohmann::json& j, TransactionReponse::Meta::TransactionReward& rewards) {
  j.at("commission").get_to(rewards.commission);
  j.at("lamports").get_to(rewards.lamports);
  j.at("postBalance").get_to(rewards.postBalance);
  j.at("pubkey").get_to(rewards.pubkey);
  j.at("rewardType").get_to(rewards.rewardType);
}

void from_json(const nlohmann::json& j, TransactionReponse::Meta::LoadedAddresses& loadedAddresses) {
  j.at("readonly").get_to(loadedAddresses.readOnly);
  j.at("writable").get_to(loadedAddresses.writable);
}

void from_json(const nlohmann::json& j, TransactionReponse::Meta& meta) {
  if (j.at("err").is_null())
    meta.err = "";
  else
    j.at("err").get_to(meta.err);
  j.at("fee").get_to(meta.fee);
  //TODO j.at("innerInstructions").get_to(meta.innerInstructions);
  j.at("loadedAddresses").get_to(meta.loadedAddresses);
  j.at("logMessages").get_to(meta.logMessages);
  j.at("postBalances").get_to(meta.postBalances);
  j.at("preBalances").get_to(meta.preBalances);
  j.at("rewards").get_to(meta.rewards);
}

void from_json(const nlohmann::json& j, TransactionReponse::Transaction::Message::Header& header) {
  j.at("numRequiredSignatures").get_to(header.numRequiredSignatures);
  j.at("numReadonlySignedAccounts").get_to(header.numReadonlySignedAccounts);
  j.at("numReadonlyUnsignedAccounts").get_to(header.numReadonlyUnsignedAccounts);
}

void from_json(const nlohmann::json& j, TransactionReponse::Transaction::Message::Instruction& instruction) {
  j.at("accounts").get_to(instruction.accounts);
  j.at("data").get_to(instruction.data);
  j.at("programIdIndex").get_to(instruction.programIdIndex);
}

void from_json(const nlohmann::json& j, TransactionReponse::Transaction::Message& message) {
  j.at("accountKeys").get_to(message.accountKeys);
  j.at("header").get_to(message.header);
  j.at("instructions").get_to(message.instructions);
  j.at("recentBlockhash").get_to(message.recentBlockhash);
}

void from_json(const nlohmann::json& j, TransactionReponse::Transaction& transaction) {
  j.at("message").get_to(transaction.message);
  j.at("signatures").get_to(transaction.signatures);
}

void from_json(const nlohmann::json& j, TransactionReponse& transactionReponse) {
  j.at("slot").get_to(transactionReponse.slot);
  j.at("transaction").get_to(transactionReponse.transaction);
  j.at("meta").get_to(transactionReponse.meta);
  j.at("blockTime").get_to(transactionReponse.blockTime);
}

std::ostream& operator<<(std::ostream& os, const TransactionReponse& transaction) {
  os << "slot = " << transaction.slot << std::endl << std::endl;
  os << "signatures:" << std::endl;
  for (auto& signature : transaction.transaction.signatures) {
    os << "  " << signature << std::endl;
  }
  os << "accounts:" << std::endl;
  for (auto& accountKey : transaction.transaction.message.accountKeys) {
    os << "  " << accountKey.toBase58() << std::endl;
  }
  os << "recentBlockhash = " << transaction.transaction.message.recentBlockhash << std::endl;
  os << "instructions:" << std::endl;
  for (auto& instruction : transaction.transaction.message.instructions) {
    os << "  programId index = " << instruction.programIdIndex << std::endl;
    for (auto& account : instruction.accounts) {
      os << "    account = " << account << std::endl;
    }
    for (auto& data : instruction.data) {
      os << "    data = " << data << std::endl;
    }
  }
  os << "block time = " << transaction.blockTime << std::endl;
  os << "error = " << transaction.meta.err << std::endl;
  os << "fee = " << transaction.meta.fee << std::endl;
  os << "pre-balances:" << std::endl;
  for (auto& preBalance : transaction.meta.preBalances) {
    os << "  " << preBalance << std::endl;
  }
  os << "post-balances:" << std::endl;
  for (auto& postBalance : transaction.meta.postBalances) {
    os << "  " << postBalance << std::endl;
  }
  //TODO os << "inner instructions = " << transaction.meta.innerInstructions << std::endl;
  os << "logs:" << std::endl;
  for (auto& log : transaction.meta.logMessages) {
    os << "  " << log << std::endl;
  }
  os << "rewards:" << std::endl;
  for (auto& reward : transaction.meta.rewards) {
    os << "  recipient = " << reward.pubkey.toBase58() << std::endl;
    os << "  lamports = " << reward.lamports << std::endl;
    os << "  postBalance = " << reward.postBalance << std::endl;
    os << "  rewardType = " << reward.rewardType << std::endl;
    os << "  commission = " << reward.commission << std::endl;
    os << "----------------" << std::endl;
  }
  os << "loaded addresses:" << std::endl;
  os << "  writable:" << std::endl;
  for (auto& address : transaction.meta.loadedAddresses.writable) {
    os << "    " << address.toBase58() << std::endl;
  }
  os << "  read only:" << std::endl;
  for (auto& address : transaction.meta.loadedAddresses.readOnly) {
    os << "    " << address.toBase58() << std::endl;
  }
  //os << "return data:" << std::endl;
  //os << "  programId = " << transaction.meta.returnData.programId.toBase58() << std::endl;
  //os << "  data = " <<  transaction.meta.returnData.data  << std::endl;
  return os;
}

struct SimulatedTransactionResponse {
  /* Error if the transaction failed */
  std::string err;
  /* Array of log messages the transaction instructions output during execution */
  std::vector<std::string> logs;
  /* Array of accounts with the same length as the accounts.addresses array in the request */
  std::vector<AccountInfo> accounts;
  /* The number of compute budget units consumed during the processing of this transaction */
  uint64_t unitsConsumed;
  /* The return data for the simulated transaction */
  struct ReturnData {
    /* The program that generated the return data */
    PublicKey programId;
    /* The return data itself */
    std::string data;
  } returnData;
};

struct AccountChangeSubscriptionInfo {
  /* The public key of the account to subscribe to */
  PublicKey accountId;
  /* The callback function that will fire on a subscription event for the account */
  std::function<void(Context context, AccountInfo accountInfo)> callback;
};

struct LogsSubscriptionInfo {
  /* The public key of the account to subscribe to for logs */
  PublicKey accountId;
  /* The callback function that will fire on a subscription event for logs */
  std::function<void(Context context, Logs logs)> callback;
};

struct ProgramAccountChangeSubscriptionInfo {
  /* The public key of the program to subscribe to */
  PublicKey programId;
  /* The callback function that will fire on a subscription event for the program */
  std::function<void(Context context, KeyedAccountInfo keyedAccountInfo)> callback;
};

class Connection {
  Commitment _commitment;
  std::string _rpcEndpoint;
  std::string _rpcWsEndpoint;
  //HttpClient _rpcClient;
  //WebSocketClient _rpcWebSocket;

  std::map<int, AccountChangeSubscriptionInfo> _accountChangeSubscriptions;
  std::map<int, LogsSubscriptionInfo> _logsSubscriptions;
  std::map<int, ProgramAccountChangeSubscriptionInfo> _programAccountChangeSubscriptions;
  std::map<int, std::function<void(Context context, SlotInfo slotInfo)>> _slotSubscriptions;

  static std::string makeWebsocketUrl(std::string endpoint) {
    auto url = endpoint;
    url.replace(0, 4, "ws");
    return url;
  }

public:

  Connection(std::string endpoint, Commitment commitment)
    : _commitment(commitment)
    , _rpcEndpoint(endpoint)
    , _rpcWsEndpoint(makeWebsocketUrl(endpoint))
    //, _rpcClient(_rpcEndpoint, 443)
    //, _rpcWebSocket(_rpcWsEndpoint, 443)
  {
    auto sodium_result = sodium_init();
    if (sodium_result == -1) {
      throw std::runtime_error("Failed to initialize libsodium");
    }
  }

  ~Connection() {}

  Connection() = delete;
  Connection(const Connection&) = delete;
  Connection(Connection&&) = delete;
  Connection& operator=(const Connection&) = delete;
  Connection& operator=(Connection&&) = delete;

  /**
   * ================================
   * HTTP METHODS
   * ================================
   */
  /**
   * Returns all information associated with the account of provided Pubkey
   * @param publicKey The public key of the desired account
   */
  AccountInfo getAccountInfo(PublicKey publicKey) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getAccountInfo"},
      {"params", {
        publicKey.toBase58(),
        {
          {"encoding", "base64"},
        },
      }},
    });

    auto value = response["result"]["value"];
    auto accountInfo = AccountInfo {
      value["executable"].get<bool>(),
      PublicKey(value["owner"].get<std::string>()),
      value["lamports"].get<uint64_t>(),
      value["data"][0].get<std::string>(),
      value["rentEpoch"].get<uint64_t>(),
    };

    return accountInfo;
  }

  /**
   * Fetch the balance for the specified public key
   * @param publicKey The public key for the account to query
   */
  uint64_t getBalance(PublicKey publicKey) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getBalance"},
      {"params", {
        publicKey.toBase58(),
      }},
    });

    auto value = response["result"]["value"];
    return value.get<uint64_t>();
  }

  /**
   * Returns information about all the nodes participating in the cluster
   */
  json getClusterNodes() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getClusterNodes"},
    });

    auto result = response["result"];
    return result;
  }

  /**
   * Returns the identity pubkey for the current node
   */
  PublicKey getIdentity() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getIdentity"},
    });

    auto identity = response["result"]["identity"];
    return PublicKey(identity.get<std::string>());
  }

  /**
   * Returns the latest blockhash
   */
  std::string getLatestBlockhash() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getLatestBlockhash"},
    });

    auto blockhash = response["result"]["blockhash"];
    return blockhash;
  }

  /**
   * Returns the leader schedule for an epoch
   */
  json getLeaderSchedule(PublicKey leaderAddress) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getLeaderSchedule"},
      {"params", {
        {
          {"identity", leaderAddress.toBase58()},
        },
      }},
    });

    auto leaderSchedule = response["result"];
    return leaderSchedule;
  }

  /**
   * Returns the account information for a list of Pubkeys.
   * @param publicKeys The public keys of the desired accounts
  */
  std::vector<AccountInfo> getMultipleAccounts(std::vector<PublicKey> publicKeys) {
    std::vector<std::string> base58Keys;
    for (auto publicKey : publicKeys) {
      base58Keys.push_back(publicKey.toBase58());
    }

    auto response = http::post(_rpcEndpoint, {
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

    std::vector<AccountInfo> accountInfos;
    auto accounts = response["result"]["value"];
    for (auto account : accounts) {
      accountInfos.push_back(AccountInfo {
        account["executable"].get<bool>(),
        PublicKey(account["owner"].get<std::string>()),
        account["lamports"].get<uint64_t>(),
        account["data"][0].get<std::string>(),
        account["rentEpoch"].get<uint64_t>(),
      });
    }

    return accountInfos;
  }

  /**
   * Returns all accounts owned by the provided program Pubkey
   * @param programId The public key for the program to query
  */
  std::vector<AccountInfo> getProgramAccounts(PublicKey programId) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getProgramAccounts"},
      {"params", {
        programId.toBase58(),
        {
          {"encoding", "base64"},
        }
      }},
    });

    std::vector<AccountInfo> accountInfos;
    auto accounts = response["result"];
    for (auto account : accounts) {
      accountInfos.push_back(AccountInfo {
        account["executable"].get<bool>(),
        PublicKey(account["owner"].get<std::string>()),
        account["lamports"].get<uint64_t>(),
        account["data"][0].get<std::string>(),
        account["rentEpoch"].get<uint64_t>(),
      });
    }

    return accountInfos;
  }

  /**
   * Returns the slot that has reached the given or default commitment level
  */
  uint64_t getSlot() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getSlot"},
    });

    auto slot = response["result"];
    return slot;
  }

  /**
   * Returns the current slot leader
   */
  PublicKey getSlotLeader() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getSlotLeader"},
    });

    auto slotLeader = response["result"];
    return PublicKey(slotLeader.get<std::string>());
  }

  /**
   * Returns the token balance of an SPL Token account.
   * @param tokenAddress The public key for the token account to query
   */
  TokenAmount getTokenAccountBalance(PublicKey tokenAddress) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getTokenAccountBalance"},
      {"params", {
        tokenAddress.toBase58(),
      }},
    });

    auto tokenBalance = response["result"]["value"];
    return {
      tokenBalance["amount"].get<std::string>(),
      tokenBalance["decimals"].get<uint64_t>(),
      tokenBalance["uiAmount"].get<double>(),
      tokenBalance["uiAmountString"].get<std::string>(),
    };
  }

  /**
   * Returns all SPL Token accounts by token owner.
   * @param ownerAddress The owner of the token accounts
   */
  std::vector<TokenAccount> getTokenAccountsByOwner(PublicKey ownerAddress) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getTokenAccountsByOwner"},
      {"params", {
        ownerAddress.toBase58(),
        {
          {"programId", TOKEN_PROGRAM_ID.toBase58()},
        },
        {
          {"encoding", "jsonParsed"},
        }
      }},
    });

    std::vector<TokenAccount> tokenAccounts;
    auto accounts = response["result"]["value"];
    for (auto account : accounts) {
      auto tokenAccount = account["account"]["data"]["parsed"]["info"];
      tokenAccounts.push_back(TokenAccount {
        {
          {
            PublicKey(account["account"]["data"]["program"].get<std::string>()),
            {
              account["account"]["data"]["parsed"]["accountType"].get<std::string>(),
              {
                {
                  account["data"]["parsed"]["info"]["tokenAmount"]["amount"].get<std::string>(),
                  account["data"]["parsed"]["info"]["tokenAmount"]["decimals"].get<uint64_t>(),
                  account["data"]["parsed"]["info"]["tokenAmount"]["uiAmount"].get<double>(),
                  account["data"]["parsed"]["info"]["tokenAmount"]["uiAmountString"].get<std::string>(),
                },
                PublicKey(account["data"]["parsed"]["info"]["delegate"].get<std::string>()),
                {
                  account["data"]["parsed"]["info"]["delegatedAmount"]["amount"].get<std::string>(),
                  account["data"]["parsed"]["info"]["delegatedAmount"]["decimals"].get<uint64_t>(),
                  account["data"]["parsed"]["info"]["delegatedAmount"]["uiAmount"].get<double>(),
                  account["data"]["parsed"]["info"]["delegatedAmount"]["uiAmountString"].get<std::string>(),
                },
                account["data"]["parsed"]["info"]["state"].get<std::string>(),
                account["data"]["parsed"]["info"]["isNative"].get<bool>(),
                PublicKey(account["data"]["parsed"]["info"]["mint"].get<std::string>()),
                PublicKey(account["data"]["parsed"]["info"]["owner"].get<std::string>()),
              },
              account["account"]["data"]["parsed"]["type"].get<std::string>(),
            },
            account["account"]["data"]["space"].get<uint64_t>(),
          },
          account["account"]["executable"].get<bool>(),
          account["account"]["lamports"].get<uint64_t>(),
          PublicKey(account["account"]["owner"].get<std::string>()),
          account["account"]["rentEpoch"].get<uint64_t>(),
        },
        PublicKey(account["pubkey"].get<std::string>()),
      });
    }

    return tokenAccounts;
  }

  /**
   * Returns all SPL Token accounts or a given mint by token owner.
   * @param ownerAddress The owner of the token accounts
   * @param mintAddress The mint of the token accounts
   */
  std::vector<TokenAccount> getTokenAccountsByOwner(PublicKey ownerAddress, PublicKey tokenMint) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getTokenAccountsByOwner"},
      {"params", {
        ownerAddress.toBase58(),
        {
          {"mint", tokenMint.toBase58()},
        },
        {
          {"encoding", "jsonParsed"},
        }
      }},
    });

    std::vector<TokenAccount> tokenAccounts;
    auto accounts = response["result"]["value"];
    for (auto account : accounts) {
      auto tokenAccount = account["account"]["data"]["parsed"]["info"];
      tokenAccounts.push_back(TokenAccount {
        {
          {
            PublicKey(account["account"]["data"]["program"].get<std::string>()),
            {
              account["account"]["data"]["parsed"]["accountType"].get<std::string>(),
              {
                {
                  account["data"]["parsed"]["info"]["tokenAmount"]["amount"].get<std::string>(),
                  account["data"]["parsed"]["info"]["tokenAmount"]["decimals"].get<uint64_t>(),
                  account["data"]["parsed"]["info"]["tokenAmount"]["uiAmount"].get<double>(),
                  account["data"]["parsed"]["info"]["tokenAmount"]["uiAmountString"].get<std::string>(),
                },
                PublicKey(account["data"]["parsed"]["info"]["delegate"].get<std::string>()),
                {
                  account["data"]["parsed"]["info"]["delegatedAmount"]["amount"].get<std::string>(),
                  account["data"]["parsed"]["info"]["delegatedAmount"]["decimals"].get<uint64_t>(),
                  account["data"]["parsed"]["info"]["delegatedAmount"]["uiAmount"].get<double>(),
                  account["data"]["parsed"]["info"]["delegatedAmount"]["uiAmountString"].get<std::string>(),
                },
                account["data"]["parsed"]["info"]["state"].get<std::string>(),
                account["data"]["parsed"]["info"]["isNative"].get<bool>(),
                PublicKey(account["data"]["parsed"]["info"]["mint"].get<std::string>()),
                PublicKey(account["data"]["parsed"]["info"]["owner"].get<std::string>()),
              },
              account["account"]["data"]["parsed"]["type"].get<std::string>(),
            },
            account["account"]["data"]["space"].get<uint64_t>(),
          },
          account["account"]["executable"].get<bool>(),
          account["account"]["lamports"].get<uint64_t>(),
          PublicKey(account["account"]["owner"].get<std::string>()),
          account["account"]["rentEpoch"].get<uint64_t>(),
        },
        PublicKey(account["pubkey"].get<std::string>()),
      });
    }

    return tokenAccounts;
  }

  /**
   * Returns the total supply of an SPL Token type.
   * @param tokenMintAddress The token mint to query
   */
  TokenAmount getTokenSupply(PublicKey tokenMintAddress) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getTokenSupply"},
      {"params", {
        tokenMintAddress.toBase58(),
      }},
    });

    auto tokenSupply = response["result"]["value"];
    return {
      tokenSupply["amount"].get<std::string>(),
      tokenSupply["decimals"].get<uint64_t>(),
      tokenSupply["uiAmount"].get<double>(),
      tokenSupply["uiAmountString"].get<std::string>(),
    };
  }

  /**
   * Returns transaction details for a confirmed transaction
   * @param transactionSignature The signature of the transaction to query
   */
  TransactionReponse getTransaction(std::string transactionSignature) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getTransaction"},
      {"params", {
        transactionSignature
      }},
    });
    return response["result"];
  }

  /**
   * Returns the current solana versions running on the node
   */
  std::string getVersion() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getVersion"},
    });

    auto result = response["result"];
    return result["solana-core"].get<std::string>();
  }

  /**
   * Submits a signed transaction to the cluster for processing.
   * @param transaction The transaction to submit
   * @param keypair The keypair used to sign the transaction
   */
  // TODO: add an example?
  std::string sendTransaction(Transaction& transaction, std::vector<Keypair> signers) {
    transaction.recentBlockhash = getLatestBlockhash();
    transaction.sign(signers);
    std::vector<uint8_t> wireTransaction = transaction.serialize();
    std::string encodedTransaction = base64::base64_encode(wireTransaction.data(), wireTransaction.size());

    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "sendTransaction"},
      {"params", {
        encodedTransaction,
        {
          {"encoding", "base64"},
        },
      }},
    });

    auto signature = response["result"];
    return signature.get<std::string>();
  }

  /**
   * Simulate sending a transaction
   * @param signedTransaction The signed transaction to simulate
   */
  SimulatedTransactionResponse simulateTransaction(std::string signedTransaction) {
    auto response = http::post(_rpcEndpoint, {
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

    std::vector<std::string> logs;
    for (auto log : response["result"]["logs"]) {
      logs.push_back(log.get<std::string>());
    }

    std::vector<AccountInfo> accounts;
    for (auto account : response["result"]["accounts"]) {
      accounts.push_back(AccountInfo {
        account["executable"].get<bool>(),
        account["data"].get<std::string>(),
        account["lamports"].get<uint64_t>(),
        account["owner"].get<std::string>(),
        account["rentEpoch"].get<uint64_t>(),
      });
    }

    auto result = response["result"]["value"];
    return {
      result["err"].get<std::string>(),
      logs,
      accounts,
      result["unitsConsumed"].get<uint64_t>(),
      {
        PublicKey(result["data"]["programId"].get<std::string>()),
        result["data"]["data"].get<std::string>(),
      }
    };
  }

  /**
   * ================================
   * SUBSCRIPTION WEBSOCKET METHODS
   * ================================
   */
  int onAccountChange(PublicKey accountId, std::function<void(Context context, AccountInfo accountInfo)> callback) {
    //TODO _rpcWebSocket
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "accountSubscribe"},
      {"params", {
        accountId.toBase58(),
        {
          {"encoding", "base64"},
          {"commitment", _commitment},
        },
      }},
    });
    int subscriptionId = response["result"].get<int>();
    _accountChangeSubscriptions[subscriptionId] = { accountId, callback };
    return subscriptionId;
  }

  bool removeAccountChangeListener(int subscriptionId) {
    //TODO _rpcWebSocket
    if (_accountChangeSubscriptions.find(subscriptionId) != _accountChangeSubscriptions.end()) {
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "accountUnsubscribe"},
        {"params", {
          subscriptionId,
        }},
      });
      _accountChangeSubscriptions.erase(subscriptionId);
      return response["result"].get<bool>();
    }
    return false;
  }

  int onLogs(PublicKey accountId, std::function<void(Context context, Logs logs)> callback) {
    //TODO _rpcWebSocket
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "logsSubscribe"},
      {"params", {
        "mentions",
        {
          {"mentions", accountId.toBase58()},
        },
        {
          {"commitment", _commitment},
        },
      }},
    });
    int subscriptionId = response["result"].get<int>();
    _logsSubscriptions[subscriptionId] = { accountId, callback };
    return subscriptionId;
  }

  bool removeOnLogsListener(int subscriptionId) {
    //TODO _rpcWebSocket
    if (_logsSubscriptions.find(subscriptionId) != _logsSubscriptions.end()) {
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "logsUnsubscribe"},
        {"params", {
          subscriptionId,
        }},
      });
      _logsSubscriptions.erase(subscriptionId);
      return response["result"].get<bool>();
    }
    return false;
  }

  int onProgramAccountChange(PublicKey programId, std::function<void(Context context, KeyedAccountInfo keyedAccountInfo)> callback) {
    //TODO _rpcWebSocket
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "programSubscribe"},
      {"params", {
        programId.toBase58(),
        {
          {"encoding", "base64"},
          {"commitment", _commitment},
        },
      }},
    });
    int subscriptionId = response["result"].get<int>();
    _programAccountChangeSubscriptions[subscriptionId] = { programId, callback };
    return subscriptionId;
  }

  bool removeProgramAccountChangeListener(int subscriptionId) {
    //TODO _rpcWebSocket
    if (_programAccountChangeSubscriptions.find(subscriptionId) != _programAccountChangeSubscriptions.end()) {
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "programUnsubscribe"},
        {"params", {
          subscriptionId,
        }},
      });
      _programAccountChangeSubscriptions.erase(subscriptionId);
      return response["result"].get<bool>();
    }
    return false;
  }

  int onSlotChange(std::function<void(Context context, SlotInfo slotInfo)> callback) {
    //TODO _rpcWebSocket
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "slotSubscribe"},
    });
    int subscriptionId = response["result"].get<int>();
    _slotSubscriptions[subscriptionId] = callback;
    return subscriptionId;
  }

  bool removeSlotChangeListener(int subscriptionId) {
    //TODO _rpcWebSocket
    if (_slotSubscriptions.find(subscriptionId) != _slotSubscriptions.end()) {
      auto response = http::post(_rpcEndpoint, {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "slotUnsubscribe"},
        {"params", {
          subscriptionId,
        }},
      });
      _slotSubscriptions.erase(subscriptionId);
      return response["result"].get<bool>();
    }
    return false;
  }
};

}
