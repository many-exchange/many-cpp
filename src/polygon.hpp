// Copyright (c) 2023 Many Exchange
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include "many.hpp"

namespace many {

  namespace polygon {

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

    public:

      JsonRpcProvider(const std::string url): _url(url) { }

      /**
       * Returns the currently configured chain id, a value used in replay-protected transaction signing as introduced by EIP-155.
       */
      Result<std::string> eth_chain_Id() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_chainId"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns information about the sync status of the node
       */
      Result<json> eth_syncing() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_syncing"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns block information by number.
       */
      Result<json> eth_get_block_by_number() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockByNumber"},
          {"params", {
            "latest", true
          }},
          {"id", id},
        });
      }

      /**
       * Returns block information by hash.
       */
      Result<json> eth_get_block_by_hash() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockByHash"},
          {"params", { "0xdc0818cf78f21a8e70579cb46a43643f78291264dda342ae31049421c82d21ae", false }},
          {"id", id},
        });
      }

      /**
       * Returns the number of the most recent block.
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
       * Returns the current price of gas in wei. If minimum gas price is enforced by setting the --price-limit flag, this endpoint will return the value defined by this flag as minimum gas price.
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
       * Returns the balance of the account of the given address.
       */
      Result<std::string> eth_get_balance(std::string address) {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBalance"},
          {"params", {
            address, "latest"
          }},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_sendRawTransaction","params":["0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675"],"id":1}'
      /**
       * Creates new message call transaction or a contract creation for signed transactions.
       */
      Result<json> eth_send_raw_transaction() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_sendRawTransaction"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_getTransactionByHash","params":["0x88df016429689c079f3b2f6ad39fa052532c56795b733da78a91ebe6a713944b"],"id":1}'
      /**
       * Returns the information about a transaction requested by transaction hash.
       */
      Result<json> eth_get_transaction_by_hash() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionByHash"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_getTransactionReceipt","params":["0xb903239f8543d04b5dc1ba6579132b143087c68db1b2168786408fcbce568238"],"id":1}'
      /**
       * Returns the receipt of a transaction by transaction hash.
       *
       * Note That the receipt is not available for pending transactions.
       */
      Result<json> eth_get_transaction_receipt() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionReceipt"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_getTransactionCount","params":["0x407d73d8a49eeb85d32cf465507dd71d507100c1","latest"],"id":1}'
      /**
       * Returns the number of transactions sent from an address.
       */
      Result<json> eth_get_transaction_count() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getTransactionCount"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_getBlockTransactionCountByNumber","params":["latest"],"id":1}'
      /**
       * Returns the number of transactions in a block matching the given block number.
       */
      Result<json> eth_get_block_transaction_count_by_number() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getBlockTransactionCountByNumber"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_getLogs","params":[{"topics": ["0x000000000000000000000000a94f5374fce5edbc8e2a8697c15331677e6ebf0b"]}],"id":1}'
      /**
       * Returns an array of all logs matching a given filter object.
       */
      Result<json> eth_get_logs() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getLogs"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_getCode","params":["0xa94f5374fce5edbc8e2a8697c15331677e6ebf0b", "0x2"],"id":1}'
      /**
       * Returns code at a given address.
       */
      Result<json> eth_get_code() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getCode"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_call","params":[{see above}],"id":1}'
      /**
       * Executes a new message call immediately without creating a transaction on the blockchain.
       */
      Result<json> eth_call() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_call"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_getStorageAt","params":["0x295a70b2de5e3953354a6a8344e616ed314d7251", "0x0", "latest"],"id":1}'
      /**
       * Returns the value from a storage position at a given address.
       */
      Result<json> eth_get_storage_at() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getStorageAt"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_estimateGas","params":[{see above}],"id":1}'
      /**
       * Generates and returns an estimate of how much gas is necessary to allow the transaction to complete. The transaction will not be added to the blockchain. Note that the estimate may be significantly more than the amount of gas actually used by the transaction, for a variety of reasons including EVM mechanics and node performance.
       */
      Result<json> eth_estimate_gas() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_estimateGas"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_newFilter","params":[{"topics":["0x12341234"]}],"id":1}'
      /**
       * Creates a filter object, based on filter options. To get all matching logs for specific filter, call eth_getFilterLogs. To check if the state has changed, call eth_getFilterChanges.
       */
      Result<json> eth_new_filter() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_newFilter"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_newBlockFilter","params":[],"id":1}'
      /**
       * Creates a filter in the node, to notify when a new block arrives. To check if the state has changed, call eth_getFilterChanges.
       */
      Result<json> eth_new_block_filter() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_newBlockFilter"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_getFilterLogs","params":["0x16"],"id":1}'
      /**
       * Returns an array of all logs matching filter with given id.
       */
      Result<json> eth_get_filter_logs() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getFilterLogs"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_getFilterChanges","params":["0x16"],"id":1}'
      /**
       * Polling method for a filter, which returns an array of logs that occurred since the last poll.
       */
      Result<json> eth_get_filter_changes() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_getFilterChanges"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_uninstallFilter","params":["0xb"],"id":1}'
      /**
       * Uninstalls a filter with a given id. Should always be called when a watch is no longer needed. Additionally, filters timeout when they aren’t requested with eth_getFilterChanges for some time.
       */
      Result<json> eth_uninstall_filter() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_uninstallFilter"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"eth_unsubscribe","params":["0x9cef478923ff08bf67fde6c64013158d"],"id":1}'
      /**
       * Subscriptions are cancelled with a regular RPC call with eth_unsubscribe as a method and the subscription id as the first parameter. It returns a bool indicating if the subscription was cancelled successfully.
       */
      Result<json> eth_unsubscribe() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "eth_unsubscribe"},
          {"params", json::array()},
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
       * Returns true if a client is actively listening for network connections.
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
      Result<json> net_peer_count() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "net_peerCount"},
          {"params", json::array()},
          {"id", id},
        });
      }

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

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"web3_sha3","params":["0x68656c6c6f20776f726c64"],"id":1}'
      /**
       * Returns Keccak-256 (not the standardized SHA3-256) of the given data.
       */
      Result<json> web3_sha3() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "web3_sha3"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns a list with the exact details of all the transactions currently pending for inclusion in the next block(s), as well as the ones that are being scheduled for future execution only.
       */
      Result<json> txpool_content() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "txpool_content"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns a list with a textual summary of all the transactions currently pending for inclusion in the next block(s), as well as the ones that are being scheduled for future execution only. This is a method specifically tailored to developers to quickly see the transactions in the pool and find any potential issues.
       */
      Result<json> txpool_inspect() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "txpool_inspect"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Returns the number of transactions currently pending for inclusion in the next block(s), as well as the ones that are being scheduled for future execution only.
       */
      Result<json> txpool_status() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "txpool_status"},
          {"params", json::array()},
          {"id", id},
        });
      }

      /**
       * Executes all transactions in the block specified by number with a tracer and returns the tracing result.
       */
      Result<json> debug_trace_block_by_number() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "debug_traceBlockByNumber"},
          {"params", { "latest" }},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"debug_traceBlockByHash","params":["0xdc0818cf78f21a8e70579cb46a43643f78291264dda342ae31049421c82d21ae"],"id":1}'
      /**
       * Executes all transactions in the block specified by block hash with a tracer and returns the tracing result.
       */
      Result<json> debug_trace_block_by_hash() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "debug_traceBlockByHash"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"debug_traceBlock","params":["0xf9...."],"id":1}'
      /**
       * Executes all transactions in the block given from the first argument with a tracer and returns the tracing result.
       */
      Result<json> debug_trace_block() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "debug_traceBlock"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"debug_traceTransaction","params":["0xdc0818cf78f21a8e70579cb46a43643f78291264dda342ae31049421c82d21ae"],"id":1}'
      /**
       * Executes the transaction specified by transaction hash with a tracer and returns the tracing result.
       */
      Result<json> debug_trace_transaction() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "debug_traceTransaction"},
          {"params", json::array()},
          {"id", id},
        });
      }

      //TODO
      // curl  https://rpc.poa.psdk.io:8545 -X POST -H "Content-Type: application/json" --data '{"jsonrpc":"2.0","method":"debug_traceCall","params":[{"to": "0x1234", "data": "0x1234"}, "latest", {}],"id":1}'
      /**
       * Executes a new message call with a tracer and returns the tracing result.
       */
      Result<json> debug_trace_call() {
        int id = 1;
        return http::post(_url, {
          {"jsonrpc", "2.0"},
          {"method", "debug_traceCall"},
          {"params", json::array()},
          {"id", id},
        });
      }

    };

  }

}
