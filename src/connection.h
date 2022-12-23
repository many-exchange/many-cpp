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
  WebSocketClient _rpcWebSocket;

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
    , _rpcWebSocket(_rpcWsEndpoint, 443) {}

  ~Connection() {}

  /**
   * Returns all information associated with the account of provided Pubkey
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
  json getLeaderSchedule() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getLeaderSchedule"},
    });

    auto leaderSchedule = response["result"];
    return leaderSchedule;
  }

  /**
   * Returns the account information for a list of Pubkeys.
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
  void getSlot() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getSlot"},
    });

    /*
{ "jsonrpc": "2.0", "result": 1234, "id": 1 }
    */
  }

  /**
   * Returns the current slot leader
   */
  void getSlotLeader() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getSlotLeader"},
    });

    /*
{
  "jsonrpc": "2.0",
  "result": "ENvAW7JScgYq6o4zKZwewtkzzJgDzuJAFxYasvmEQdpS",
  "id": 1
}    */
  }

  /**
   * Returns the token balance of an SPL Token account.
   */
  void getTokenAccountBalance(PublicKey tokenAddress) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getTokenAccountBalance"},
      {"params", {
        tokenAddress.toBase58(),
      }},
    });

    /*
{
  "jsonrpc": "2.0",
  "result": {
    "context": {
      "slot": 1114
    },
    "value": {
      "amount": "9864",
      "decimals": 2,
      "uiAmount": 98.64,
      "uiAmountString": "98.64"
    },
    "id": 1
  }
}    */
  }

  /**
   * Returns all SPL Token accounts by token owner.
   */
  void getTokenAccountsByOwner(PublicKey ownerAddress) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getTokenAccountsByOwner"},
      {"params", {
        ownerAddress.toBase58(),
        {
          {"mint", "3wyAj7Rt1TWVPZVteFJPLa26JmLvdb1CAKEFZm3NY75E"},
        },
      }},
    });

    /*
curl http://localhost:8899 -X POST -H "Content-Type: application/json" -d '
  {
    "jsonrpc": "2.0",
    "id": 1,
    "method": "getTokenAccountsByOwner",
    "params": [
      "4Qkev8aNZcqFNSRhQzwyLMFSsi94jHqE8WNVTJzTP99F",
      {
        "mint": "3wyAj7Rt1TWVPZVteFJPLa26JmLvdb1CAKEFZm3NY75E"
      },
      {
        "encoding": "jsonParsed"
      }
    ]
  }
'    */

    /*
{
  "jsonrpc": "2.0",
  "result": {
    "context": {
      "slot": 1114
    },
    "value": [
      {
        "account": {
          "data": {
            "program": "spl-token",
            "parsed": {
              "accountType": "account",
              "info": {
                "tokenAmount": {
                  "amount": "1",
                  "decimals": 1,
                  "uiAmount": 0.1,
                  "uiAmountString": "0.1"
                },
                "delegate": "4Nd1mBQtrMJVYVfKf2PJy9NZUZdTAsp7D4xWLs4gDB4T",
                "delegatedAmount": {
                  "amount": "1",
                  "decimals": 1,
                  "uiAmount": 0.1,
                  "uiAmountString": "0.1"
                },
                "state": "initialized",
                "isNative": false,
                "mint": "3wyAj7Rt1TWVPZVteFJPLa26JmLvdb1CAKEFZm3NY75E",
                "owner": "4Qkev8aNZcqFNSRhQzwyLMFSsi94jHqE8WNVTJzTP99F"
              },
              "type": "account"
            },
            "space": 165
          },
          "executable": false,
          "lamports": 1726080,
          "owner": "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA",
          "rentEpoch": 4
        },
        "pubkey": "C2gJg6tKpQs41PRS1nC8aw3ZKNZK3HQQZGVrDFDup5nx"
      }
    ]
  },
  "id": 1
}    */
  }

  /**
   * Returns the total supply of an SPL Token type.
   */
  void getTokenSupply(PublicKey tokenMintAddress) {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getTokenSupply"},
      {"params", {
        tokenMintAddress.toBase58(),
      }},
    });

    /*
{
  "jsonrpc": "2.0",
  "result": {
    "context": {
      "slot": 1114
    },
    "value": {
      "amount": "100000",
      "decimals": 2,
      "uiAmount": 1000,
      "uiAmountString": "1000"
    }
  },
  "id": 1
}
    */
  }

  /**
   * Returns transaction details for a confirmed transaction
   */
  void getTransaction() {
    /*
curl http://localhost:8899 -X POST -H "Content-Type: application/json" -d '
  {
    "jsonrpc": "2.0",
    "id": 1,
    "method": "getTransaction",
    "params": [
      "2nBhEBYYvfaAe16UMNqRHre4YNSskvuYgx3M6E4JP1oDYvZEJHvoPzyUidNgNX5r9sTyN1J9UxtbCXy2rqYcuyuv",
      "json"
    ]
  }
'    */

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
   */
  void sendTransaction(std::string signedTransaction) {
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

    /*
{
  "jsonrpc": "2.0",
  "result": "2id3YC2jK9G5Wo2phDx4gJVAew8DcY5NAojnVuao8rkxwPYPe8cSwE5GzhEgJA2y8fVjDEo6iR6ykBvDxrTQrtpb",
  "id": 1
}    */
  }

  /**
   * Simulate sending a transaction
   */
  void simulateTransaction(std::string signedTransaction) {
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

    /*
{
  "jsonrpc": "2.0",
  "result": {
    "context": {
      "slot": 218
    },
    "value": {
      "err": null,
      "accounts": null,
      "logs": [
        "Program 83astBRguLMdt2h5U1Tpdq5tjFoJ6noeGwaY3mDLVcri invoke [1]",
        "Program 83astBRguLMdt2h5U1Tpdq5tjFoJ6noeGwaY3mDLVcri consumed 2366 of 1400000 compute units",
        "Program return: 83astBRguLMdt2h5U1Tpdq5tjFoJ6noeGwaY3mDLVcri KgAAAAAAAAA=",
        "Program 83astBRguLMdt2h5U1Tpdq5tjFoJ6noeGwaY3mDLVcri success"
      ],
      "returnData": {
        "data": [
          "Kg==",
          "base64"
        ],
        "programId": "83astBRguLMdt2h5U1Tpdq5tjFoJ6noeGwaY3mDLVcri"
      },
      "unitsConsumed": 2366
    }
  },
  "id": 1
}    */
  }

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
