//  ____ _____ _____ ____ _____ ____ __ __ __ _ __
// |  __|_   _|  |  |  __|  _  |  __|  |  |       |  Ethereum C++ SDK
// |  __| | | |     |  __|    \|  __|  |  |  | |  |  version 0.0.1
// |____| |_| |__|__|____|__|__|____|_____|__|_|__|  https://github.com/many-exchange/many-exchange-cpp
//
// Copyright (c) 2022-2023 Many Exchange
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "many.hpp"

using namespace many;

#define WEI_PER_ETH 1000000000000000000

namespace many {

  namespace ethereum {

    class Provider {
      std::string _url;

    public:

      Provider(const std::string url): _url(url) { }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"web3_clientVersion","params":[],"id":67}'
  // Result
  {
    "id":67,
    "jsonrpc":"2.0",
    "result": "Mist/v0.9.3/darwin/go1.4.1"
  }
      */
      /**
       * Returns the current client version.
       */
      json web3_client_version() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "web3_clientVersion"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"web3_sha3","params":["0x68656c6c6f20776f726c64"],"id":64}'
  // Result
  {
    "id":64,
    "jsonrpc": "2.0",
    "result": "0x47173285a8d7341e5e972fc677286384f802f8ef42a5ec5f03bbfa254cb01fad"
  }
      */
      /**
       * Returns Keccak-256 (not the standardized SHA3-256) of the given data.
       */
      json web3_sha3(const std::string sha3) {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "web3_sha3"},
          {"params", {
            sha3,
          }},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"net_version","params":[],"id":67}'
  // Result
  {
    "id":67,
    "jsonrpc": "2.0",
    "result": "3"
  }
      */
      /**
       * Returns the current network id.
       */
      json net_version() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "net_version"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"net_listening","params":[],"id":67}'
  // Result
  {
    "id":67,
    "jsonrpc":"2.0",
    "result":true
  }
      */
      /**
       * Returns true if client is actively listening for network connections.
       */
      json net_listening() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "net_listening"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"net_peerCount","params":[],"id":74}'
  // Result
  {
    "id":74,
    "jsonrpc": "2.0",
    "result": "0x2" // 2
  }
      */
      /**
       * Returns number of peers currently connected to the client.
       */
      json net_peer_count() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "net_peerCount"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_protocolVersion","params":[],"id":67}'
  // Result
  {
    "id":67,
    "jsonrpc": "2.0",
    "result": "54"
  }
      */
      /**
       * Returns the current ethereum protocol version.
       */
      json eth_protocol_version() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_protocolVersion"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_syncing","params":[],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": {
      startingBlock: '0x384',
      currentBlock: '0x386',
      highestBlock: '0x454'
    }
  }
  // Or when not syncing
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": false
  }
      */
      /**
       * Returns an object with data about the sync status or false.
       */
      json eth_syncing() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_syncing"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_coinbase","params":[],"id":64}'
  // Result
  {
    "id":64,
    "jsonrpc": "2.0",
    "result": "0x407d73d8a49eeb85d32cf465507dd71d507100c1"
  }
      */
      /**
       * Returns the client coinbase address.
       */
      json eth_coinbase() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_coinbase"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_mining","params":[],"id":71}'
  //
  {
    "id":71,
    "jsonrpc": "2.0",
    "result": true
  }
      */
      /**
       * Returns true if client is actively mining new blocks.
       */
      json eth_mining() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_mining"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_hashrate","params":[],"id":71}'
  // Result
  {
    "id":71,
    "jsonrpc": "2.0",
    "result": "0x38a"
  }
      */
      /**
       * Returns the number of hashes per second that the node is mining with.
       */
      json eth_hashrate() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_hashrate"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_gasPrice","params":[],"id":73}'
  // Result
  {
    "id":73,
    "jsonrpc": "2.0",
    "result": "0x1dfd14000" // 8049999872 Wei
  }
      */
      /**
       * Returns the current price per gas in wei.
       */
      json eth_gas_price() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_gasPrice"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns a list of addresses owned by client.
       */
      json eth_accounts() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_accounts"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns the number of most recent block.
       */
      json eth_block_number() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_blockNumber"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns the balance of the account of given address.
       */
      json eth_get_balance() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBalance"},
          {"params", {
            "0x407d73d8a49eeb85d32cf465507dd71d507100c1",
            "latest"
          }},
          {"id", id},
        });
      }

      /*
  curl -X POST --data '{"jsonrpc":"2.0", "method": "eth_getStorageAt", "params": ["0x295a70b2de5e3953354a6a8344e616ed314d7251", "0x0", "latest"], "id": 1}' localhost:8545
  {"jsonrpc":"2.0","id":1,"result":"0x00000000000000000000000000000000000000000000000000000000000004d2"}
      */
      /**
       * Returns the value from a storage position at a given address.
       */
      json eth_get_storage_at() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getStorageAt"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getTransactionCount","params":["0x407d73d8a49eeb85d32cf465507dd71d507100c1","latest"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0x1" // 1
  }
      */
      /**
       * Returns the number of transactions sent from an address.
       */
      json eth_get_transaction_count() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionCount"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getBlockTransactionCountByHash","params":["0xb903239f8543d04b5dc1ba6579132b143087c68db1b2168786408fcbce568238"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0xb" // 11
  }
      */
      /**
       * Returns the number of transactions in a block from a block matching the given block hash.
       */
      json eth_get_block_transaction_count_by_hash() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockTransactionCountByHash"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getBlockTransactionCountByNumber","params":["0xe8"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0xa" // 10
  }
      */
      /**
       * Returns the number of transactions in a block from a block matching the given block number.
       */
      json eth_get_block_transaction_count_by_number() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockTransactionCountByNumber"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getUncleCountByBlockHash","params":["0xb903239f8543d04b5dc1ba6579132b143087c68db1b2168786408fcbce568238"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0x1" // 1
  }
      */
      /**
       * Returns the number of uncles in a block from a block matching the given block hash.
       */
      json eth_get_uncle_count_by_block_hash() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getUncleCountByBlockHash"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getUncleCountByBlockNumber","params":["0xe8"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0x1" // 1
  }
      */
      /**
       * Returns the number of uncles in a block from a block matching the given block number.
       */
      json eth_get_uncle_count_by_block_number() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getUncleCountByBlockNumber"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getCode","params":["0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b", "0x2"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0x600160008035811a818181146012578301005b601b6001356025565b8060005260206000f25b600060078202905091905056"
  }
      */
      /**
       * Returns code at a given address.
       */
      json eth_get_code() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getCode"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_sign","params":["0x9b2055d370f73ec7d8a03e965129118dc8f5bf83", "0xdeadbeaf"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0xa3f20717a250c2b0b729b7e5becbff67fdaef7e0699da4de7ca5895b02a170a12d887fd3b17bfdce3481f10bea41f45ba9f709d39ce8325427b57afcfc994cee1b"
  }
      */
      /**
       * The sign method calculates an Ethereum specific signature with: sign(keccak256("\x19Ethereum Signed Message:\n" + len(message) + message))).
       */
      json eth_sign() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_sign"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"id": 1,"jsonrpc": "2.0","method": "eth_signTransaction","params": [{"data":"0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675","from": "0xb60e8dd61c5d32be8058bb8eb970870f07233155","gas": "0x76c0","gasPrice": "0x9184e72a000","to": "0xd46e8dd67c5d32be8058bb8eb970870f07244567","value": "0x9184e72a"}]}'
  // Result
  {
      "id": 1,
      "jsonrpc": "2.0",
      "result": "0xa3f20717a250c2b0b729b7e5becbff67fdaef7e0699da4de7ca5895b02a170a12d887fd3b17bfdce3481f10bea41f45ba9f709d39ce8325427b57afcfc994cee1b"
  }
      */
      /**
       * Signs a transaction that can be submitted to the network at a later time using with eth_sendRawTransaction.
       */
      json eth_sign_transaction() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_signTransaction"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_sendTransaction","params":[{see above}],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0xe670ec64341771606e55d6b4ca35a1a6b75ee3d5145a99d05921026d1527331"
  }
      */
      /**
       * Creates new message call transaction or a contract creation, if the data field contains code.
       */
      json eth_send_transaction() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_sendTransaction"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_sendRawTransaction","params":[{see above}],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0xe670ec64341771606e55d6b4ca35a1a6b75ee3d5145a99d05921026d1527331"
  }
      */
      /**
       * Creates new message call transaction or a contract creation for signed transactions.
       */
      json eth_send_raw_transaction() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_sendRawTransaction"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_call","params":[{see above}],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0x"
  }
      */
      /**
       * Executes a new message call immediately without creating a transaction on the block chain.
       */
      json eth_call() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_call"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_estimateGas","params":[{see above}],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0x5208" // 21000
  }
      */
      /**
       * Generates and returns an estimate of how much gas is necessary to allow the transaction to complete.
       */
      json eth_estimate_gas() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_estimateGas"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getBlockByHash","params":["0xdc0818cf78f21a8e70579cb46a43643f78291264dda342ae31049421c82d21ae", false],"id":1}'
  // Result
  {
  {
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
      "difficulty": "0x4ea3f27bc",
      "extraData": "0x476574682f4c5649562f76312e302e302f6c696e75782f676f312e342e32",
      "gasLimit": "0x1388",
      "gasUsed": "0x0",
      "hash": "0xdc0818cf78f21a8e70579cb46a43643f78291264dda342ae31049421c82d21ae",
      "logsBloom": "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
      "miner": "0xbb7b8287f3f0a933474a79eae42cbca977791171",
      "mixHash": "0x4fffe9ae21f1c9e15207b1f472d5bbdd68c9595d461666602f2be20daf5e7843",
      "nonce": "0x689056015818adbe",
      "number": "0x1b4",
      "parentHash": "0xe99e022112df268087ea7eafaf4790497fd21dbeeb6bd7a1721df161a6657a54",
      "receiptsRoot": "0x56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421",
      "sha3Uncles": "0x1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347",
      "size": "0x220",
      "stateRoot": "0xddc8b0234c2e0cad087c8b389aa7ef01f7d79b2570bccb77ce48648aa61c904d",
      "timestamp": "0x55ba467c",
      "totalDifficulty": "0x78ed983323d",
      "transactions": [
      ],
      "transactionsRoot": "0x56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421",
      "uncles": [
      ]
  }
  }
      */
      /**
       * Returns information about a block by hash.
       */
      json eth_get_block_by_hash() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockByHash"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getBlockByNumber","params":["0x1b4", true],"id":1}'
      */
      /**
       * Returns information about a block by block number.
       */
      json eth_get_block_by_number() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockByNumber"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getTransactionByHash","params":["0x88df016429689c079f3b2f6ad39fa052532c56795b733da78a91ebe6a713944b"],"id":1}'
  // Result
  {
    "jsonrpc":"2.0",
    "id":1,
    "result":{
      "blockHash":"0x1d59ff54b1eb26b013ce3cb5fc9dab3705b415a67127a003c3e61eb445bb8df2",
      "blockNumber":"0x5daf3b", // 6139707
      "from":"0xa7d9ddbe1f17865597fbd27ec712455208b6b76d",
      "gas":"0xc350", // 50000
      "gasPrice":"0x4a817c800", // 20000000000
      "hash":"0x88df016429689c079f3b2f6ad39fa052532c56795b733da78a91ebe6a713944b",
      "input":"0x68656c6c6f21",
      "nonce":"0x15", // 21
      "to":"0xf02c1c8e6114b1dbe8937a39260b5b0a374432bb",
      "transactionIndex":"0x41", // 65
      "value":"0xf3dbb76162000", // 4290000000000000
      "v":"0x25", // 37
      "r":"0x1b5e176d927f8e9ab405058b2d2457392da3e20f328b16ddabcebc33eaac5fea",
      "s":"0x4ba69724e8f69de52f0125ad8b3c5c2cef33019bac3249e2c0a2192766d1721c"
    }
  }
      */
      /**
       * Returns the information about a transaction requested by transaction hash.
       */
      json eth_get_transaction_by_hash() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionByHash"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getTransactionByBlockHashAndIndex","params":["0xc6ef2fc5426d6ad6fd9e2a26abeab0aa2411b7ab17f30a99d3cb96aed1d1055b", "0x0"],"id":1}'
      */
      /**
       * Returns information about a transaction by block hash and transaction index position.
       */
      json eth_get_transaction_by_block_hash_and_index() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionByBlockHashAndIndex"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getTransactionByBlockNumberAndIndex","params":["0x29c", "0x0"],"id":1}'
      */
      /**
       * Returns information about a transaction by block number and transaction index position.
       */
      json eth_get_transaction_by_block_number_and_index() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionByBlockNumberAndIndex"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getTransactionReceipt","params":["0x85d995eba9763907fdf35cd2034144dd9d53ce32cbec21349d4b12823c6860c5"],"id":1}'
  // Result
  {
    "jsonrpc": "2.0",
    "id": 1,
    "result": {
      "blockHash":
        "0xa957d47df264a31badc3ae823e10ac1d444b098d9b73d204c40426e57f47e8c3",
      "blockNumber": "0xeff35f",
      "contractAddress": null, // string of the address if it was created
      "cumulativeGasUsed": "0xa12515",
      "effectiveGasPrice": "0x5a9c688d4",
      "from": "0x6221a9c005f6e47eb398fd867784cacfdcfff4e7",
      "gasUsed": "0xb4c8",
      "logs": [{
        // logs as returned by getFilterLogs, etc.
      }],
      "logsBloom": "0x00...0", // 256 byte bloom filter
      "status": "0x1",
      "to": "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2",
      "transactionHash":
        "0x85d995eba9763907fdf35cd2034144dd9d53ce32cbec21349d4b12823c6860c5",
      "transactionIndex": "0x66",
      "type": "0x2"
    }
  }
      */
      /**
       * Returns the receipt of a transaction by transaction hash.
       */
      json eth_get_transaction_receipt() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionReceipt"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getUncleByBlockHashAndIndex","params":["0xc6ef2fc5426d6ad6fd9e2a26abeab0aa2411b7ab17f30a99d3cb96aed1d1055b", "0x0"],"id":1}'
      */
      /**
       * Returns information about a uncle of a block by hash and uncle index position.
       */
      json eth_get_uncle_by_block_hash_and_index() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getUncleByBlockHashAndIndex"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getUncleByBlockNumberAndIndex","params":["0x29c", "0x0"],"id":1}'
      */
      /**
       * Returns information about a uncle of a block by number and uncle index position.
       */
      json eth_get_uncle_by_block_number_and_index() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getUncleByBlockNumberAndIndex"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getCompilers","params":[],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": ["solidity", "lll", "serpent"]
  }
      */
      /**
       * Returns a list of available compilers in the client.
       */
      json eth_get_compilers() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getCompilers"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_compileSolidity","params":["contract test { function multiply(uint a) returns(uint d) {   return a * 7;   } }"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": {
        "code": "0x605880600c6000396000f3006000357c010000000000000000000000000000000000000000000000000000000090048063c6888fa114602e57005b603d6004803590602001506047565b8060005260206000f35b60006007820290506053565b91905056",
        "info": {
          "source": "contract test {\n   function multiply(uint a) constant returns(uint d) {\n       return a * 7;\n   }\n}\n",
          "language": "Solidity",
          "languageVersion": "0",
          "compilerVersion": "0.9.19",
          "abiDefinition": [
            {
              "constant": true,
              "inputs": [
                {
                  "name": "a",
                  "type": "uint256"
                }
              ],
              "name": "multiply",
              "outputs": [
                {
                  "name": "d",
                  "type": "uint256"
                }
              ],
              "type": "function"
            }
          ],
          "userDoc": {
            "methods": {}
          },
          "developerDoc": {
            "methods": {}
          }
        }
  }
      */
      /**
       * Returns compiled solidity code.
       */
      json eth_compile_solidity() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_compileSolidity"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_compileLLL","params":["(returnlll (suicide (caller)))"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0x603880600c6000396000f3006001600060e060020a600035048063c6888fa114601857005b6021600435602b565b8060005260206000f35b600081600702905091905056" // the compiled source code
  }
      */
      /**
       * Returns compiled LLL code.
       */
      json eth_compile_LLL() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_compileLLL"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_compileSerpent","params":["/some serpent/"],"id":1}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0x603880600c6000396000f3006001600060e060020a600035048063c6888fa114601857005b6021600435602b565b8060005260206000f35b600081600702905091905056" // the compiled source code
  }
      */
      /**
       * Returns compiled serpent code.
       */
      json eth_compile_serpent() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_compileSerpent"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_newFilter","params":[{"topics":["0x12341234"]}],"id":73}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": "0x1" // 1
  }
      */
      /**
       * Creates a filter object, based on filter options, to notify when the state changes (logs).
       * To check if the state has changed, call eth_getFilterChanges.
       */
      json eth_new_filter() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_newFilter"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_newBlockFilter","params":[],"id":73}'
  // Result
  {
    "id":1,
    "jsonrpc":  "2.0",
    "result": "0x1" // 1
  }
      */
      /**
       * Creates a filter in the node, to notify when a new block arrives.
       * To check if the state has changed, call eth_getFilterChanges.
       */
      json eth_new_block_filter() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_newBlockFilter"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_newPendingTransactionFilter","params":[],"id":73}'
  // Result
  {
    "id":1,
    "jsonrpc":  "2.0",
    "result": "0x1" // 1
  }
      */
      /**
       * Creates a filter in the node, to notify when new pending transactions arrive.
       * To check if the state has changed, call eth_getFilterChanges.
       */
      json eth_new_pending_transaction_filter() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_newPendingTransactionFilter"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_uninstallFilter","params":["0xb"],"id":73}'
  // Result
  {
    "id":1,
    "jsonrpc": "2.0",
    "result": true
  }
      */
      /**
       * Uninstalls a filter with given id.
       * Should always be called when watch is no longer needed.
       * Additonally Filters timeout when they aren't requested with eth_getFilterChanges for a period of time.
       */
      json eth_uninstall_filter() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_uninstallFilter"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getFilterChanges","params":["0x16"],"id":73}'
  // Result
  {
    "id":1,
    "jsonrpc":"2.0",
    "result": [{
      "logIndex": "0x1", // 1
      "blockNumber":"0x1b4", // 436
      "blockHash": "0x8216c5785ac562ff41e2dcfdf5785ac562ff41e2dcfdf829c5a142f1fccd7d",
      "transactionHash":  "0xdf829c5a142f1fccd7d8216c5785ac562ff41e2dcfdf5785ac562ff41e2dcf",
      "transactionIndex": "0x0", // 0
      "address": "0x16c5785ac562ff41e2dcfdf829c5a142f1fccd7d",
      "data":"0x0000000000000000000000000000000000000000000000000000000000000000",
      "topics": ["0x59ebeb90bc63057b6515673c3ecf9438e5058bca0f92585014eced636878c9a5"]
      },{
        ...
      }]
  }
      */
      /**
       * Polling method for a filter, which returns an array of logs which occurred since last poll.
       */
      json eth_get_filter_changes() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getFilterChanges"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getFilterLogs","params":["0x16"],"id":74}'
      */
      /**
       * Returns an array of all logs matching filter with given id.
       */
      json eth_get_filter_logs() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getFilterLogs"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getLogs","params":[{"topics":["0x000000000000000000000000a94f5374fce5edbc8e2a8697c15331677e6ebf0b"]}],"id":74}'
      */
      /**
       * Returns an array of all logs matching a given filter object.
       */
      json eth_get_logs() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getLogs"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getWork","params":[],"id":73}'
  // Result
  {
    "id":1,
    "jsonrpc":"2.0",
    "result": [
        "0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef",
        "0x5EED00000000000000000000000000005EED0000000000000000000000000000",
        "0xd1ff1c01710000000000000000000000d1ff1c01710000000000000000000000"
      ]
  }
      */
      /**
       * Returns the hash of the current block, the seedHash, and the boundary condition to be met ("target").
       */
      json eth_get_work() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getWork"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0", "method":"eth_submitWork", "params":["0x0000000000000001", "0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef", "0xD1GE5700000000000000000000000000D1GE5700000000000000000000000000"],"id":73}'
  // Result
  {
    "id":73,
    "jsonrpc":"2.0",
    "result": true
  }
      */
      /**
       * Used for submitting a proof-of-work solution.
       */
      json eth_submit_work() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_submitWork"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0", "method":"eth_submitHashrate", "params":["0x0000000000000000000000000000000000000000000000000000000000500000", "0x59daa26581d0acd1fce254fb7e85952f4c09d0915afd33d3886cd914bc7d283c"],"id":73}'
  // Result
  {
    "id":73,
    "jsonrpc":"2.0",
    "result": true
  }
      */
      /**
       * Used for submitting mining hashrate.
       */
      json eth_submit_hashrate() {
        int id = 0;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_submitHashrate"},
          {"params", json::array()},
          {"id", id},
        });
      }

    };

    class Signer {
    public:

      Provider& provider;

      Signer(const std::vector<uint8_t> privateKey, Provider& provider): provider(provider) {
      }

    };

    class Contract {
    public:

      Contract() {
      }

    };

  }

}
