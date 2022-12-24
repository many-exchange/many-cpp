#pragma once

#include <assert.h>
#include <iostream>
#include <map>
#include <string>

#include "http.h"
#include "publickey.h"
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

struct TransactionInstruction {
  /* Index into the `message.accountKeys` array indicating the program account that executes this instruction */
  uint64_t programIdIndex;
  /* Ordered indices into the `message.accountKeys` array indicating which accounts to pass to the program */
  std::vector<uint64_t> accounts;
  /* The program input data encoded as base-58 */
  std::vector<uint8_t> data;
};

struct TransactionMessage {
  /* List of base-58 encoded public keys used by the transaction, including by the instructions and for signatures. 
  The first message.header.numRequiredSignatures public keys must sign the transaction. */
  std::vector<PublicKey> accountKeys;
  /* A base-58 encoded hash of a recent block in the ledger used to prevent transaction duplication and to give transactions lifetimes. */
  std::string recentBlockhash;
  /* List of program instructions that will be executed in sequence and committed in one atomic transaction if all succeed. */
  std::vector<TransactionInstruction> instructions;
};

struct Transaction {
  /* The transaction signatures */
  std::vector<std::string> signatures;
  /* Defines the content of the transaction */
  TransactionMessage message;
};

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

struct TransactionReponse {
  /* The slot this transaction was processed in */
  uint64_t slot;
  /* The transaction */
  Transaction transaction;
  /* The estimated production time of when the transaction was processed. */
  uint64_t blockTime;
  /* Transaction status metadata object */
  struct Meta {
    /* Error if the transaction failed */
    std::string err;
    /* Fee this transaction was charged, as u64 integer */
    uint64_t fee;
    /* Array of u64 account balances from before the transaction was processed */
    std::vector<uint64_t> preBalances;
    /* Array of u64 account balances from after the transaction was processed */
    std::vector<uint64_t> postBalances;
    /* List of inner instructions if inner instruction recording was enabled during this transaction */
    std::string innerInstructions;
    /* Array of string log messages if log message recording was enabled during this transaction  */
    std::vector<std::string> logMessages;
    /* Transaction-level rewards, populated if rewards are requested */
    std::vector<TransactionReward> rewards;
    /* Transaction addresses loaded from address lookup tables */
    struct LoadedAddresses {
      /* Ordered list of base-58 encoded addresses for writable loaded accounts */
      std::vector<PublicKey> writable;
      /* Ordered list of base-58 encoded addresses for read-only loaded accounts */
      std::vector<PublicKey> readOnly;
    } loadedAddresses;
    /* The most-recent return data generated by an instruction in the transaction */
    struct ReturnData {
      /* The program that generated the return data */
      PublicKey programId;
      /* The return data itself, as base-64 encoded binary data */
      std::string data;
    } returnData;
    /* The number of compute units consumed by the transaction */
    uint64_t computeUnitsConsumed;
  } meta;
};

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
    auto transaction = response["result"]["transaction"];

    std::vector<std::string> signatures;
    for (auto signature : transaction["signatures"]) {
      signatures.push_back(signature.get<std::string>());
    }

    std::vector<PublicKey> accountKeys;
    for (auto accountKey : transaction["message"]["accountKeys"]) {
      accountKeys.push_back(PublicKey(accountKey.get<std::string>()));
    }

    std::vector<TransactionInstruction> instructions;
    for (auto instruction : transaction["message"]["instructions"]) {
      std::vector<uint64_t> accounts;
      for (auto account : instruction["accounts"]) {
        accounts.push_back(account.get<uint64_t>());
      }
      std::vector<uint8_t> data;
      for (auto byte : instruction["data"]) {
        data.push_back(byte.get<uint8_t>());
      }
      instructions.push_back(TransactionInstruction {
        instruction["programIdIndex"].get<uint64_t>(),
        accounts,
        data
      });
    }

    std::vector<uint64_t> preBalances;
    for (auto preBalance : transaction["meta"]["preBalances"]) {
      preBalances.push_back(preBalance.get<uint64_t>());
    }
    std::vector<uint64_t> postBalances;
    for (auto postBalance : transaction["meta"]["postBalances"]) {
      postBalances.push_back(postBalance.get<uint64_t>());
    }

    std::vector<std::string> logMessages;
    for (auto logMessage : transaction["meta"]["logMessages"]) {
      logMessages.push_back(logMessage.get<std::string>());
    }

    std::vector<TransactionReward> rewards;
    for (auto reward : transaction["meta"]["rewards"]) {
      rewards.push_back(TransactionReward {
        reward["pubkey"].get<std::string>(),
        reward["lamports"].get<uint64_t>(),
        reward["postBalance"].get<uint64_t>(),
        reward["rewardType"].get<std::string>(),
      });
    }

    std::vector<PublicKey> writable;
    for (auto writableAccount : transaction["meta"]["innerInstructions"]["writable"]) {
      writable.push_back(PublicKey(writableAccount.get<std::string>()));
    }
    std::vector<PublicKey> readOnly;
    for (auto readOnlyAccount : transaction["meta"]["innerInstructions"]["readOnly"]) {
      readOnly.push_back(PublicKey(readOnlyAccount.get<std::string>()));
    }

    return {
      transaction["slot"].get<uint64_t>(),
      {
        signatures,
        {
          accountKeys,
          transaction["message"]["blockhash"].get<std::string>(),
          instructions
        },
      },
      transaction["blockTime"].get<uint64_t>(),
      {
        transaction["meta"]["err"].get<std::string>(),
        transaction["meta"]["fee"].get<uint64_t>(),
        preBalances,
        postBalances,
        transaction["meta"]["innerInstructions"].get<std::string>(),
        logMessages,
        rewards,
        {
          writable,
          readOnly
        },
        {
          PublicKey(transaction["meta"]["returnData"]["programId"].get<std::string>()),
          transaction["meta"]["returnData"]["data"].get<std::string>(),
        },
        transaction["meta"]["computeUnitsConsumed"].get<uint64_t>(),
      },
    };
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
   * @param signedTransaction The signed transaction to submit
   */
  std::string sendTransaction(std::string signedTransaction) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "sendTransaction"},
      {"params", {
        signedTransaction,
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
