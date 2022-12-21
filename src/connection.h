#pragma once

#include <string>

#include "publickey.h"

namespace solana {

enum Commitment {
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
    const useHttps = url.protocol === 'https:';

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

    this._rpcEndpoint = endpoint;
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

  void getAccountInfo() {
    /*
    */
  }

  /**
   * Fetch the balance for the specified public key
   */
  void getBalance(PublicKey publicKey) {
    /*
    const args = this._buildArgs([publicKey.toBase58()], commitment);
    const unsafeRes = await this._rpcRequest('getBalance', args);
    const res = create(unsafeRes, jsonRpcResultAndContext(number()));
    if ('error' in res) {
      throw new Error(
        'failed to get balance for ' +
          publicKey.toBase58() +
          ': ' +
          res.error.message,
      );
    }
    return res.result;
    */
  }

  void getBlock() {
    /*
    */
  }

  void getBlockHeight() {
    /*
    */
  }

  void getBlockProduction() {
    /*
    */
  }

  void getBlockCommitment() {
    /*
    */
  }

  void getBlocks() {
    /*
    */
  }

  void getBlocksWithLimit() {
    /*
    */
  }

  void getBlockTime() {
    /*
    */
  }

  void getClusterNodes() {
    /*
    */
  }

  void getEpochInfo() {
    /*
    */
  }

  void getEpochSchedule() {
    /*
    */
  }

  void getFeeForMessage() {
    /*
    */
  }

  /**
   * Fetch the slot of the lowest confirmed block that has not been purged from the ledger
   */
  void getFirstAvailableBlock() {
    /*
    const unsafeRes = await this._rpcRequest('getFirstAvailableBlock', []);
    const res = create(unsafeRes, SlotRpcResult);
    if ('error' in res) {
      throw new Error(
        'failed to get first available block: ' + res.error.message,
      );
    }
    return res.result;
    */
  }

  void getGenesisHash() {
    /*
    */
  }

  void getHealth() {
    /*
    */
  }

  void getHighestSnapshotSlot() {
    /*
    */
  }

  void getIdentity() {
    /*
    */
  }

  void getInflationGovernor() {
    /*
    */
  }

  void getInflationRate() {
    /*
    */
  }

  void getInflationReward() {
    /*
    */
  }

  void getLargestAccounts() {
    /*
    */
  }

  void getLatestBlockhash() {
    /*
    */
  }

  void getLeaderSchedule() {
    /*
    */
  }

  void getMaxRetransmitSlot() {
    /*
    */
  }

  void getMaxShredInsertSlot() {
    /*
    */
  }

  void getMinimumBalanceForRentExemption() {
    /*
    */
  }

  void getMultipleAccounts() {
    /*
    */
  }

  void getProgramAccounts() {
    /*
    */
  }

  void getRecentPerformanceSamples() {
    /*
    */
  }

  void getSignaturesForAddress() {
    /*
    */
  }

  void getSignatureStatuses() {
    /*
    */
  }

  void getSlot() {
    /*
    */
  }

  void getSlotLeader() {
    /*
    */
  }

  void getSlotLeaders() {
    /*
    */
  }

  void getStakeActivation() {
    /*
    */
  }

  void getStakeMinimumDelegation() {
    /*
    */
  }

  /**
   * Fetch information about the current supply
   */
  void getSupply() {
    /*
    let configArg: GetSupplyConfig = {};
    if (typeof config === 'string') {
      configArg = {commitment: config};
    } else if (config) {
      configArg = {
        ...config,
        commitment: (config && config.commitment) || this.commitment,
      };
    } else {
      configArg = {
        commitment: this.commitment,
      };
    }

    const unsafeRes = await this._rpcRequest('getSupply', [configArg]);
    const res = create(unsafeRes, GetSupplyRpcResult);
    if ('error' in res) {
      throw new Error('failed to get supply: ' + res.error.message);
    }
    return res.result;
    */
  }

  /**
   * Fetch the current balance of a token account
   */
  void getTokenAccountBalance(PublicKey tokenAddress) {
    /*
    const args = this._buildArgs([tokenAddress.toBase58()], commitment);
    const unsafeRes = await this._rpcRequest('getTokenAccountBalance', args);
    const res = create(unsafeRes, jsonRpcResultAndContext(TokenAmountResult));
    if ('error' in res) {
      throw new Error(
        'failed to get token account balance: ' + res.error.message,
      );
    }
    return res.result;
    */
  }

  void getTokenAccountsByDelegate() {
    /*
    */
  }

  /**
   * Fetch all the token accounts owned by the specified account
   */
  void getTokenAccountsByOwner(PublicKey ownerAddress) {
    /*
    let _args: any[] = [ownerAddress.toBase58()];
    if ('mint' in filter) {
      _args.push({mint: filter.mint.toBase58()});
    } else {
      _args.push({programId: filter.programId.toBase58()});
    }

    const args = this._buildArgs(_args, commitment, 'base64');
    const unsafeRes = await this._rpcRequest('getTokenAccountsByOwner', args);
    const res = create(unsafeRes, GetTokenAccountsByOwner);
    if ('error' in res) {
      throw new Error(
        'failed to get token accounts owned by account ' +
          ownerAddress.toBase58() +
          ': ' +
          res.error.message,
      );
    }
    return res.result;
    */
  }

  void getTokenLargestAccounts() {
    /*
    */
  }

  /**
   * Fetch the current supply of a token mint
   */
  void getTokenSupply(PublicKey tokenMintAddress) {
    /*
    const args = this._buildArgs([tokenMintAddress.toBase58()], commitment);
    const unsafeRes = await this._rpcRequest('getTokenSupply', args);
    const res = create(unsafeRes, jsonRpcResultAndContext(TokenAmountResult));
    if ('error' in res) {
      throw new Error('failed to get token supply: ' + res.error.message);
    }
    return res.result;
    */
  }

  void getTransaction() {
    /*
    */
  }

  void getTransactionCount() {
    /*
    */
  }

  void getVersion() {
    /*
    */
  }

  void getVoteAccounts() {
    /*
    */
  }

  void isBlockhashValid() {
    /*
    */
  }

  void minimumLedgerSlot() {
    /*
    */
  }

  void requestAirdrop() {
    /*
    */
  }

  void sendTransaction() {
    /*
    */
  }

  void simulateTransaction() {
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



/*
export function makeWebsocketUrl(endpoint: string) {
  let url = new URL(endpoint);
  const useHttps = url.protocol === 'https:';

  url.protocol = useHttps ? 'wss:' : 'ws:';
  url.host = '';

  // Only shift the port by +1 as a convention for ws(s) only if given endpoint
  // is explictly specifying the endpoint port (HTTP-based RPC), assuming
  // we're directly trying to connect to solana-validator's ws listening port.
  // When the endpoint omits the port, we're connecting to the protocol
  // default ports: http(80) or https(443) and it's assumed we're behind a reverse
  // proxy which manages WebSocket upgrade and backend port redirection.
  if (url.port !== '') {
    url.port = String(Number(url.port) + 1);
  }
  return url.toString();
}
*/



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
