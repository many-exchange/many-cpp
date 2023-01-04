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

    typedef json Block;
      //TODO
      /*
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
      */

    class Contract {
    public:

      Contract() {
      }

    };

    class Signer {
    public:

      Signer(const std::vector<uint8_t> privateKey) {
      }

    };

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
        if (!j["result"].is_null()) {
          r._result = j["result"].get<T>();
        }
      } else if (j.contains("error")) {
        r._error = j["error"].get<ResultError>();
      }
    }

    class JsonRpcProvider {
      std::string _url;
      std::optional<Signer> _signer;

    public:

      JsonRpcProvider(const std::string url): _url(url) { }

      JsonRpcProvider(const std::string url, std::optional<Signer> signer): _url(url), _signer(signer) { }

      /**
       * Returns the current client version.
       */
      Result<std::string> web3_client_version() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "web3_clientVersion"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns Keccak-256 (not the standardized SHA3-256) of the given data.
       */
      Result<std::string> web3_sha3(const std::string data) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "web3_sha3"},
          {"params", {
            data,
          }},
          {"id", id},
        });
      }

      /**
       * Returns the current network id.
       */
      Result<std::string> net_version() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "net_version"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns true if client is actively listening for network connections.
       */
      Result<bool> net_listening() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "net_listening"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns number of peers currently connected to the client.
       */
      Result<std::string> net_peer_count() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "net_peerCount"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns the current ethereum protocol version.
       */
      Result<std::string> eth_protocol_version() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_protocolVersion"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns an object with data about the sync status or false.
       */
      Result<bool> eth_syncing() {
        int id = 1;
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
      Result<std::string> eth_coinbase() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_coinbase"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns true if client is actively mining new blocks.
       */
      Result<bool> eth_mining() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_mining"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns the number of hashes per second that the node is mining with.
       */
      Result<std::string> eth_hashrate() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_hashrate"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns the current price per gas in wei.
       */
      Result<std::string> eth_gas_price() {
        int id = 1;
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
      Result<std::vector<std::string>> eth_accounts() {
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
      Result<std::string> eth_block_number() {
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
      Result<std::string> eth_get_balance(std::string address) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBalance"},
          {"params", {
            address,
            "latest"
          }},
          {"id", id},
        });
      }

      // /**
      //  * Returns the value from a storage position at a given address.
      //  */
      // json eth_get_storage_at() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getStorageAt"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      /**
       * Returns the number of transactions sent from an address.
       */
      Result<std::string> eth_get_transaction_count(std::string address) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionCount"},
          {"params", { address, "latest" }},
          {"id", id},
        });
      }

      /**
       * Returns the number of transactions in a block from a block matching the given block hash.
       */
      Result<std::string> eth_get_block_transaction_count_by_hash(std::string block_hash) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockTransactionCountByHash"},
          {"params", { block_hash }},
          {"id", id},
        });
      }

      /**
       * Returns the number of transactions in a block from a block matching the given block number.
       */
      Result<std::string> eth_get_block_transaction_count_by_number(int block_number) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockTransactionCountByNumber"},
          {"params", { to_hex(block_number) }},
          {"id", id},
        });
      }

      // /**
      //  * Returns the number of uncles in a block from a block matching the given block hash.
      //  */
      // json eth_get_uncle_count_by_block_hash() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getUncleCountByBlockHash"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns the number of uncles in a block from a block matching the given block number.
      //  */
      // json eth_get_uncle_count_by_block_number() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getUncleCountByBlockNumber"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns code at a given address.
      //  */
      // json eth_get_code() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getCode"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      /**
       * The sign method calculates an Ethereum specific signature with: sign(keccak256("\x19Ethereum Signed Message:\n" + len(message) + message))).
       */
      Result<std::string> eth_sign(std::string address, std::string data) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_sign"},
          {"params", { address, data }},
          {"id", id},
        });
      }

      /**
       * Signs a transaction that can be submitted to the network at a later time using with eth_sendRawTransaction.
       */
      Result<std::string> eth_sign_transaction(std::string from, std::string data, std::string to, uint64_t gas, uint64_t gas_price, int value, int nonce) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_signTransaction"},
          {"params", {
            {
              { "from", from },
              { "data", data },
              { "to", to }, //TODO make this optional
              { "gas", to_hex(gas) }, //TODO make this optional
              { "gasPrice", to_hex(gas_price) }, //TODO make this optional
              { "value", to_hex(value) }, //TODO make this optional
              { "nonce", to_hex(nonce) } //TODO make this optional
            }
          }},
          {"id", id},
        });
      }

      /**
       * Creates new message call transaction or a contract creation, if the data field contains code.
       */
      json eth_send_transaction(std::string from, std::string data, std::string to, uint64_t gas, uint64_t gas_price, int value, int nonce) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_sendTransaction"},
          {"params", {
            {
              { "from", from },
              { "data", data },
              { "to", to }, //TODO make this optional
              { "gas", to_hex(gas) }, //TODO make this optional
              { "gasPrice", to_hex(gas_price) }, //TODO make this optional
              { "value", to_hex(value) }, //TODO make this optional
              { "nonce", to_hex(nonce) } //TODO make this optional
            }
          }},
          {"id", id},
        });
      }

      /**
       * Creates new message call transaction or a contract creation for signed transactions.
       */
      json eth_send_raw_transaction(std::string from, std::string data, std::string to, uint64_t gas, uint64_t gas_price, int value, int nonce) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_sendRawTransaction"},
          {"params", {
            {
              { "from", from },
              { "data", data },
              { "to", to }, //TODO make this optional
              { "gas", to_hex(gas) }, //TODO make this optional
              { "gasPrice", to_hex(gas_price) }, //TODO make this optional
              { "value", to_hex(value) }, //TODO make this optional
              { "nonce", to_hex(nonce) } //TODO make this optional
            }
          }},
          {"id", id},
        });
      }

      /**
       * Executes a new message call immediately without creating a transaction on the block chain.
       */
      json eth_call(std::string from, std::string data, std::string to, uint64_t gas, uint64_t gas_price, int value, int nonce) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_call"},
          {"params", {
            {
              { "from", from },
              { "data", data },
              { "to", to }, //TODO make this optional
              { "gas", to_hex(gas) }, //TODO make this optional
              { "gasPrice", to_hex(gas_price) }, //TODO make this optional
              { "value", to_hex(value) }, //TODO make this optional
              { "nonce", to_hex(nonce) } //TODO make this optional
            }
          }},
          {"id", id},
        });
      }

      /**
       * Generates and returns an estimate of how much gas is necessary to allow the transaction to complete.
       */
      json eth_estimate_gas(std::string from, std::string data, std::string to, uint64_t gas, uint64_t gas_price, int value, int nonce) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_estimateGas"},
          {"params", {
            {
              { "from", from },
              { "data", data },
              { "to", to }, //TODO make this optional
              { "gas", to_hex(gas) }, //TODO make this optional
              { "gasPrice", to_hex(gas_price) }, //TODO make this optional
              { "value", to_hex(value) }, //TODO make this optional
              { "nonce", to_hex(nonce) } //TODO make this optional
            }
          }},
          {"id", id},
        });
      }

      /**
       * Returns information about a block by hash.
       */
      Result<Block> eth_get_block_by_hash(std::string hash) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockByHash"},
          {"params", { hash, false }},
          {"id", id},
        });
      }

      /**
       * Returns information about a block by block number.
       */
      Result<Block> eth_get_block_by_number(int block_number) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockByNumber"},
          {"params", { to_hex(block_number), true }},
          {"id", id},
        });
      }

      //TODO
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
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionByHash"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getTransactionByBlockHashAndIndex","params":["0xc6ef2fc5426d6ad6fd9e2a26abeab0aa2411b7ab17f30a99d3cb96aed1d1055b", "0x0"],"id":1}'
      */
      /**
       * Returns information about a transaction by block hash and transaction index position.
       */
      json eth_get_transaction_by_block_hash_and_index() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionByBlockHashAndIndex"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      /*
  // Request
  curl -X POST --data '{"jsonrpc":"2.0","method":"eth_getTransactionByBlockNumberAndIndex","params":["0x29c", "0x0"],"id":1}'
      */
      /**
       * Returns information about a transaction by block number and transaction index position.
       */
      json eth_get_transaction_by_block_number_and_index() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionByBlockNumberAndIndex"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
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
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionReceipt"},
          {"params", json::array()},
          {"id", id},
        });
      }

      // /**
      //  * Returns information about a uncle of a block by hash and uncle index position.
      //  */
      // json eth_get_uncle_by_block_hash_and_index() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getUncleByBlockHashAndIndex"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns information about a uncle of a block by number and uncle index position.
      //  */
      // json eth_get_uncle_by_block_number_and_index() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getUncleByBlockNumberAndIndex"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns a list of available compilers in the client.
      //  */
      // json eth_get_compilers() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getCompilers"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns compiled solidity code.
      //  */
      // json eth_compile_solidity() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_compileSolidity"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns compiled LLL code.
      //  */
      // json eth_compile_LLL() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_compileLLL"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns compiled serpent code.
      //  */
      // json eth_compile_serpent() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_compileSerpent"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Creates a filter object, based on filter options, to notify when the state changes (logs).
      //  * To check if the state has changed, call eth_getFilterChanges.
      //  */
      // json eth_new_filter() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_newFilter"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Creates a filter in the node, to notify when a new block arrives.
      //  * To check if the state has changed, call eth_getFilterChanges.
      //  */
      // json eth_new_block_filter() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_newBlockFilter"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Creates a filter in the node, to notify when new pending transactions arrive.
      //  * To check if the state has changed, call eth_getFilterChanges.
      //  */
      // json eth_new_pending_transaction_filter() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_newPendingTransactionFilter"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Uninstalls a filter with given id.
      //  * Should always be called when watch is no longer needed.
      //  * Additonally Filters timeout when they aren't requested with eth_getFilterChanges for a period of time.
      //  */
      // json eth_uninstall_filter() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_uninstallFilter"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Polling method for a filter, which returns an array of logs which occurred since last poll.
      //  */
      // json eth_get_filter_changes() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getFilterChanges"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns an array of all logs matching filter with given id.
      //  */
      // json eth_get_filter_logs() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getFilterLogs"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns an array of all logs matching a given filter object.
      //  */
      // json eth_get_logs() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getLogs"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Returns the hash of the current block, the seedHash, and the boundary condition to be met ("target").
      //  */
      // json eth_get_work() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_getWork"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Used for submitting a proof-of-work solution.
      //  */
      // json eth_submit_work() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_submitWork"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

      // /**
      //  * Used for submitting mining hashrate.
      //  */
      // json eth_submit_hashrate() {
      //   int id = 1;
      //   return http::post(_url, {
      //     {"jsonrpc", "2.0"},
      //     {"method", "eth_submitHashrate"},
      //     {"params", json::array()},
      //     {"id", id},
      //   });
      // }

    };

  }

}
