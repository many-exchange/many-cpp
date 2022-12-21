#pragma once

#include <assert.h>
#include <iostream>
#include <string>

#include "http.h"
#include "json.h"
#include "publickey.h"

using namespace solana;
using namespace solana::json;

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
  /*
  _confirmTransactionInitialTimeout?: number;
  */
  std::string _rpcEndpoint;
  std::string _rpcWsEndpoint;
  /*
  _rpcClient: RpcClient;
  _rpcRequest: RpcRequest;
  _rpcBatchRequest: RpcBatchRequest;
  _rpcWebSocket: RpcWebSocketClient;
  _rpcWebSocketConnected: boolean = false;
  _rpcWebSocketHeartbeat: ReturnType<
    typeof setInterval
  > | null = null;
  _rpcWebSocketIdleTimeout: ReturnType<
    typeof setTimeout
  > | null = null;

  _disableBlockhashCaching: boolean = false;
  _pollingBlockhash: boolean = false;
  _blockhashInfo: {
    recentBlockhash: Blockhash | null;
    lastFetch: number;
    simulatedSignatures: Array<string>;
    transactionSignatures: Array<string>;
  } = {
    recentBlockhash: null,
    lastFetch: 0,
    transactionSignatures: [],
    simulatedSignatures: [],
  };

  _accountChangeSubscriptionCounter: number = 0;
  _accountChangeSubscriptions: {
    [id: number]: AccountSubscriptionInfo;
  } = {};

  _programAccountChangeSubscriptionCounter: number = 0;
  _programAccountChangeSubscriptions: {
    [id: number]: ProgramAccountSubscriptionInfo;
  } = {};

  _rootSubscriptionCounter: number = 0;
  _rootSubscriptions: {
    [id: number]: RootSubscriptionInfo;
  } = {};

  _signatureSubscriptionCounter: number = 0;
  _signatureSubscriptions: {
    [id: number]: SignatureSubscriptionInfo;
  } = {};

  _slotSubscriptionCounter: number = 0;
  _slotSubscriptions: {
    [id: number]: SlotSubscriptionInfo;
  } = {};

  _logsSubscriptionCounter: number = 0;
  _logsSubscriptions: {
    [id: number]: LogsSubscriptionInfo;
  } = {};

  _slotUpdateSubscriptionCounter: number = 0;
  _slotUpdateSubscriptions: {
    [id: number]: SlotUpdateSubscriptionInfo;
  } = {};
  */

public:

  Connection(std::string endpoint, Commitment commitment)
    : _commitment(commitment)
  {
    /*
    let url = new URL(endpoint);

    let wsEndpoint;
    let httpHeaders;
    let fetchMiddleware;
    let disableRetryOnRateLimit;
    if (commitmentOrConfig && typeof commitmentOrConfig === 'string') {
      this._commitment = commitmentOrConfig;
    } else if (commitmentOrConfig) {
      this._commitment = commitmentOrConfig.commitment;
      this._confirmTransactionInitialTimeout =
        commitmentOrConfig.confirmTransactionInitialTimeout;
      wsEndpoint = commitmentOrConfig.wsEndpoint;
      httpHeaders = commitmentOrConfig.httpHeaders;
      fetchMiddleware = commitmentOrConfig.fetchMiddleware;
      disableRetryOnRateLimit = commitmentOrConfig.disableRetryOnRateLimit;
    }
    */
    _rpcEndpoint = endpoint;
    /*
    this._rpcWsEndpoint = wsEndpoint || makeWebsocketUrl(endpoint);

    this._rpcClient = createRpcClient(
      url.toString(),
      useHttps,
      httpHeaders,
      fetchMiddleware,
      disableRetryOnRateLimit,
    );
    this._rpcRequest = createRpcRequest(this._rpcClient);
    this._rpcBatchRequest = createRpcBatchRequest(this._rpcClient);

    this._rpcWebSocket = new RpcWebSocketClient(this._rpcWsEndpoint, {
      autoconnect: false,
      max_reconnects: Infinity,
    });
    this._rpcWebSocket.on('open', this._wsOnOpen.bind(this));
    this._rpcWebSocket.on('error', this._wsOnError.bind(this));
    this._rpcWebSocket.on('close', this._wsOnClose.bind(this));
    this._rpcWebSocket.on(
      'accountNotification',
      this._wsOnAccountNotification.bind(this),
    );
    this._rpcWebSocket.on(
      'programNotification',
      this._wsOnProgramAccountNotification.bind(this),
    );
    this._rpcWebSocket.on(
      'slotNotification',
      this._wsOnSlotNotification.bind(this),
    );
    this._rpcWebSocket.on(
      'slotsUpdatesNotification',
      this._wsOnSlotUpdatesNotification.bind(this),
    );
    this._rpcWebSocket.on(
      'signatureNotification',
      this._wsOnSignatureNotification.bind(this),
    );
    this._rpcWebSocket.on(
      'rootNotification',
      this._wsOnRootNotification.bind(this),
    );
    this._rpcWebSocket.on(
      'logsNotification',
      this._wsOnLogsNotification.bind(this),
    );
    */
  }

  ~Connection() {
  }

  AccountInfo* getAccountInfo(const char* publicKey) {

    HttpClient httpClient(_rpcEndpoint, 443);
    httpClient.connect();
    assert(httpClient.is_connected());

    json_object_t request;
    request.add("jsonrpc", "2.0");
    request.add("id", 1);
    request.add("method", "getAccountInfo");
    json_array_t params;
    request.add("params", params);
    params.add(publicKey);
    json_object_t config;
    params.add(config);
    config.add("encoding", "base64");

    int response_length = 0;
    char* response = httpClient.post(request, &response_length);

    httpClient.disconnect();

    json_value_t result;
    solana::json::parse(response, response_length, &result);

    //TODO iterate over the result and return an account.

    /*
    {"jsonrpc":"2.0","result":{"context":{"apiVersion":"1.14.10","slot":168249796},"value":{"data":["AQAAABzjWe1aAS4E+hQrnHUaHF6Hz9CgFhuchf/TG3jN/Nj2Zi8WmEPjEQAGAQEAAAAqnl7btTwEZ5CY/3sSZRcUQ0/AjFYqmjuGEQXmctQicw==","base64"],"executable":false,"lamports":179771985948,"owner":"TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA","rentEpoch":361}},"id":1}
    */

    return nullptr;
  }

  /**
   * Fetch the balance for the specified public key
   */
  void getBalance(PublicKey publicKey) {
    json_object_t request;
    request.add("jsonrpc", "2.0");
    /*
    */

    /*
    */
  }

  void getBlock() {
    /*
    */

    /*
    */
  }

  void getBlockHeight() {
    /*
    */

    /*
    */
  }

  void getBlockProduction() {
    /*
    */

    /*
    */
  }

  void getBlockCommitment() {
    /*
    */

    /*
    */
  }

  void getBlocks() {
    /*
    */

    /*
    */
  }

  void getBlocksWithLimit() {
    /*
    */

    /*
    */
  }

  void getBlockTime() {
    /*
    */

    /*
    */
  }

  void getClusterNodes() {
    /*
    */

    /*
    */
  }

  void getEpochInfo() {
    /*
    */

    /*
    */
  }

  void getEpochSchedule() {
    /*
    */

    /*
    */
  }

  void getFeeForMessage() {
    /*
    */

    /*
    */
  }

  /**
   * Fetch the slot of the lowest confirmed block that has not been purged from the ledger
   */
  void getFirstAvailableBlock() {
    /*
    */

    /*
    */
  }

  void getGenesisHash() {
    /*
    */

    /*
    */
  }

  void getHealth() {
    /*
    */

    /*
    */
  }

  void getHighestSnapshotSlot() {
    /*
    */

    /*
    */
  }

  void getIdentity() {
    /*
    */

    /*
    */
  }

  void getInflationGovernor() {
    /*
    */

    /*
    */
  }

  void getInflationRate() {
    /*
    */

    /*
    */
  }

  void getInflationReward() {
    /*
    */

    /*
    */
  }

  void getLargestAccounts() {
    /*
    */

    /*
    */
  }

  void getLatestBlockhash() {
    /*
    */

    /*
    */
  }

  void getLeaderSchedule() {
    /*
    */

    /*
    */
  }

  void getMaxRetransmitSlot() {
    /*
    */

    /*
    */
  }

  void getMaxShredInsertSlot() {
    /*
    */

    /*
    */
  }

  void getMinimumBalanceForRentExemption() {
    /*
    */

    /*
    */
  }

  void getMultipleAccounts() {
    /*
    */

    /*
    */
  }

  void getProgramAccounts() {
    /*
    */

    /*
    */
  }

  void getRecentPerformanceSamples() {
    /*
    */

    /*
    */
  }

  void getSignaturesForAddress() {
    /*
    */

    /*
    */
  }

  void getSignatureStatuses() {
    /*
    */

    /*
    */
  }

  void getSlot() {
    /*
    */

    /*
    */
  }

  void getSlotLeader() {
    /*
    */

    /*
    */
  }

  void getSlotLeaders() {
    /*
    */

    /*
    */
  }

  void getStakeActivation() {
    /*
    */

    /*
    */
  }

  void getStakeMinimumDelegation() {
    /*
    */

    /*
    */
  }

  /**
   * Fetch information about the current supply
   */
  void getSupply() {
    /*
    */

    /*
    */
  }

  /**
   * Fetch the current balance of a token account
   */
  void getTokenAccountBalance(PublicKey tokenAddress) {
    /*
    */

    /*
    */
  }

  void getTokenAccountsByDelegate() {
    /*
    */

    /*
    */
  }

  /**
   * Fetch all the token accounts owned by the specified account
   */
  void getTokenAccountsByOwner(PublicKey ownerAddress) {
    /*
    */

    /*
    */
  }

  void getTokenLargestAccounts() {
    /*
    */

    /*
    */
  }

  /**
   * Fetch the current supply of a token mint
   */
  void getTokenSupply(PublicKey tokenMintAddress) {
    /*
    */

    /*
    */
  }

  void getTransaction() {
    /*
    */

    /*
    */
  }

  void getTransactionCount() {
    /*
    */

    /*
    */
  }

  void getVersion() {
    /*
    */

    /*
    */
  }

  void getVoteAccounts() {
    /*
    */

    /*
    */
  }

  void isBlockhashValid() {
    /*
    */

    /*
    */
  }

  void minimumLedgerSlot() {
    /*
    */

    /*
    */
  }

  void requestAirdrop() {
    /*
    */

    /*
    */
  }

  void sendTransaction() {
    /*
    */

    /*
    */
  }

  void simulateTransaction() {
    /*
    */

    /*
    */
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

  int signatureSubscribe() {
    return -1;
  }

  int signatureUnsubscribe() {
    return -1;
  }

  int slotSubscribe() {
    return -1;
  }

  int slotUnsubscribe() {
    return -1;
  }

  // Unstable

  int blockSubscribe() {
    return -1;
  }

  int blockUnsubscribe() {
    return -1;
  }

  int slotsUpdatesSubscribe() {
    return -1;
  }

  int slotsUpdatesUnsubscribe() {
    return -1;
  }

  int voteSubscribe() {
    return -1;
  }

  int voteUnsubscribe() {
    return -1;
  }

};

}



/**
 * Send and confirm a raw transaction
 *
 * If `commitment` option is not specified, defaults to 'max' commitment.
 *
 * @param {Connection} connection
 * @param {Buffer} rawTransaction
 * @param {ConfirmOptions} [options]
 * @returns {Promise<TransactionSignature>}
 */
/*
export async function sendAndConfirmRawTransaction(
  connection: Connection,
  rawTransaction: Buffer,
  options?: ConfirmOptions,
): Promise<TransactionSignature> {
  const sendOptions = options && {
    skipPreflight: options.skipPreflight,
    preflightCommitment: options.preflightCommitment || options.commitment,
  };

  const signature = await connection.sendRawTransaction(
    rawTransaction,
    sendOptions,
  );

  const status = (
    await connection.confirmTransaction(
      signature,
      options && options.commitment,
    )
  ).value;

  if (status.err) {
    throw new Error(
      `Raw transaction ${signature} failed (${JSON.stringify(status)})`,
    );
  }

  return signature;
}
*/



/**
 * Sign, send and confirm a transaction.
 *
 * If `commitment` option is not specified, defaults to 'max' commitment.
 *
 * @param {Connection} connection
 * @param {Transaction} transaction
 * @param {Array<Signer>} signers
 * @param {ConfirmOptions} [options]
 * @returns {Promise<TransactionSignature>}
 */
/*
export async function sendAndConfirmTransaction(
  connection: Connection,
  transaction: Transaction,
  signers: Array<Signer>,
  options?: ConfirmOptions,
): Promise<TransactionSignature> {
  const sendOptions = options && {
    skipPreflight: options.skipPreflight,
    preflightCommitment: options.preflightCommitment || options.commitment,
  };

  const signature = await connection.sendTransaction(
    transaction,
    signers,
    sendOptions,
  );

  const status = (
    await connection.confirmTransaction(
      signature,
      options && options.commitment,
    )
  ).value;

  if (status.err) {
    throw new Error(
      `Transaction ${signature} failed (${JSON.stringify(status)})`,
    );
  }

  return signature;
}
*/
