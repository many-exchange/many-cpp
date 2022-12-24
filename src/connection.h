#pragma once

#include <assert.h>
#include <iostream>
#include <map>
#include <sodium.h>
#include <string>
#include <vector>

#include "http.h"
#include "keypair.h"
#include "publickey.h"
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

// TODO: comments
struct TransactionReponse {
  uint64_t slot;
  Transaction transaction;
  uint64_t blocktime;
  struct TransactionResponseMeta {
    std::string err;
    uint64_t fee;
    std::vector<uint64_t> preBalances;
    std::vector<uint64_t> postBalances;
    std::vector<std::string> logMessages;
    // TODO: finish this typedef https://docs.solana.com/developing/clients/jsonrpc-api#gettransaction
  } meta;
};

struct SimulatedTransactionResponse {
  /* Error if transaction failed */
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
  PublicKey accountId;
  std::function<void(Context context, AccountInfo accountInfo)> callback;
};

struct LogsSubscriptionInfo {
  PublicKey accountId;
  std::function<void(Context context, Logs logs)> callback;
};

struct ProgramAccountChangeSubscriptionInfo {
  PublicKey programId;
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
   */
  void getTransaction(std::string transactionSignature) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getTransaction"},
      {"params", {
        transactionSignature
      }},
    });

    auto transaction = response["result"]["transaction"];
    // TODO: finish this method once the typedefs are finished

    /*
{
  "jsonrpc": "2.0",
  "result": {
    "meta": {
      "err": null,
      "fee": 5000,
      "innerInstructions": [],
      "postBalances": [499998932500, 26858640, 1, 1, 1],
      "postTokenBalances": [],
      "preBalances": [499998937500, 26858640, 1, 1, 1],
      "preTokenBalances": [],
      "rewards": [],
      "status": {
        "Ok": null
      }
    },
    "slot": 430,
    "transaction": {
      "message": {
        "accountKeys": [
          "3UVYmECPPMZSCqWKfENfuoTv51fTDTWicX9xmBD2euKe",
          "AjozzgE83A3x1sHNUR64hfH7zaEBWeMaFuAN9kQgujrc",
          "SysvarS1otHashes111111111111111111111111111",
          "SysvarC1ock11111111111111111111111111111111",
          "Vote111111111111111111111111111111111111111"
        ],
        "header": {
          "numReadonlySignedAccounts": 0,
          "numReadonlyUnsignedAccounts": 3,
          "numRequiredSignatures": 1
        },
        "instructions": [
          {
            "accounts": [1, 2, 3, 0],
            "data": "37u9WtQpcm6ULa3WRQHmj49EPs4if7o9f1jSRVZpm2dvihR9C8jY4NqEwXUbLwx15HBSNcP1",
            "programIdIndex": 4
          }
        ],
        "recentBlockhash": "mfcyqEXB3DnHXki6KjjmZck6YjmZLvpAByy2fj4nh6B"
      },
      "signatures": [
        "2nBhEBYYvfaAe16UMNqRHre4YNSskvuYgx3M6E4JP1oDYvZEJHvoPzyUidNgNX5r9sTyN1J9UxtbCXy2rqYcuyuv"
      ]
    }
  },
  "blockTime": null,
  "id": 1
}    */
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
  // TODO: finish this return value
  // SimulatedTransactionResponse simulateTransaction(std::string signedTransaction) {
  //   auto response = http::post(_rpcEndpoint, {
  //     {"jsonrpc", "2.0"},
  //     {"id", 1},
  //     {"method", "simulateTransaction"},
  //     {"params", {
  //       signedTransaction,
  //       {
  //         {"encoding", "base64"},
  //       },
  //     }},
  //   });

  //   auto result = response["result"]["value"];
  //   return {
  //     result["err"].get<std::string>(),
  //     result["logs"].get<std::vector<std::string>>(),
  //
  //   };
  // }

  // Subscription Websocket

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
