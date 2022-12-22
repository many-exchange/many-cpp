#pragma once

#include <assert.h>
#include <iostream>
#include <string>

#include "http.h"
#include "publickey.h"
#include "websocket.h"

using namespace solana;
using namespace solana::http;

namespace solana {

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

enum class Commitment {
  Processed,
  Confirmed,
  Finalized,
};

class Connection {
  Commitment _commitment;
  std::string _rpcEndpoint;
  std::string _rpcWsEndpoint;
  //HttpClient _rpcClient;
  WebSocketClient _rpcWebSocket;

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
    , _rpcWebSocket(_rpcWsEndpoint, 443)
  {
  }

  ~Connection() {
  }

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
  void getLeaderSchedule() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getLeaderSchedule"},
    });

    /*
{
  "jsonrpc": "2.0",
  "result": {
    "4Qkev8aNZcqFNSRhQzwyLMFSsi94jHqE8WNVTJzTP99F": [
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
      21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
      39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
      57, 58, 59, 60, 61, 62, 63
    ]
  },
  "id": 1
}    */
  }

  /**
   * Returns the account information for a list of Pubkeys.
  */
  void getMultipleAccounts(std::vector<PublicKey> publicKeys) {
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
          /*
          {"dataSlice", {
            {"offset", 0},
            {"length", 0},
          }},
          */
        },
      }},
    });

    /*
{
  "jsonrpc": "2.0",
  "result": {
    "context": {
      "slot": 1
    },
    "value": [
      {
        "data": ["AAAAAAEAAAACtzNsyJrW0g==", "base64"],
        "executable": false,
        "lamports": 1000000000,
        "owner": "11111111111111111111111111111111",
        "rentEpoch": 2
      },
      {
        "data": ["", "base64"],
        "executable": false,
        "lamports": 5000000000,
        "owner": "11111111111111111111111111111111",
        "rentEpoch": 2
      }
    ]
  },
  "id": 1
}    */
  }

  /**
   * Returns all accounts owned by the provided program Pubkey
  */
  void getProgramAccounts() {
    auto response = http::post(_rpcEndpoint, {
      {"jsonrpc", "2.0"},
      {"id", 1},
      {"method", "getProgramAccounts"},
      {"params", {
        "4Nd1mBQtrMJVYVfKf2PJy9NZUZdTAsp7D4xWLs4gDB4T",
      }},
    });

    /*
{
  "jsonrpc": "2.0",
  "result": [
    {
      "account": {
        "data": "2R9jLfiAQ9bgdcw6h8s44439",
        "executable": false,
        "lamports": 15298080,
        "owner": "4Nd1mBQtrMJVYVfKf2PJy9NZUZdTAsp7D4xWLs4gDB4T",
        "rentEpoch": 28
      },
      "pubkey": "CxELquR1gPP8wHe33gZ4QxqGB3sZ9RSwsJ2KshVewkFY"
    }
  ],
  "id": 1
}    */
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

  int accountSubscribe() {
    return -1;
  }

  int accountUnsubscribe() {
    return -1;
  }

  int logsSubscribe() {
    return -1;
  }

  int logsUnsubscribe() {
    return -1;
  }

  int programSubscribe() {
    return -1;
  }

  int programUnsubscribe() {
    return -1;
  }

  int slotSubscribe() {
    return -1;
  }

  int slotUnsubscribe() {
    return -1;
  }

};

}
