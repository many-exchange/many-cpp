#pragma once

namespace solana {

class Connection {

public:

  Connection() {
  }

  ~Connection() {
  }

  void getAccountInfo() {
  }

  void getBalance() {
  }

  void getBlock() {
  }

  void getBlockHeight() {
  }

  void getBlockProduction() {
  }

  void getBlockCommitment() {
  }

  void getBlocks() {
  }

  void getBlocksWithLimit() {
  }

  void getBlockTime() {
  }

  void getClusterNodes() {
  }

  void getEpochInfo() {
  }

  void getEpochSchedule() {
  }

  void getFeeForMessage() {
  }

  void getFirstAvailableBlock() {
  }

  void getGenesisHash() {
  }

  void getHealth() {
  }

  void getHighestSnapshotSlot() {
  }

  void getIdentity() {
  }

  void getInflationGovernor() {
  }

  void getInflationRate() {
  }

  void getInflationReward() {
  }

  void getLargestAccounts() {
  }

  void getLatestBlockhash() {
  }

  void getLeaderSchedule() {
  }

  void getMaxRetransmitSlot() {
  }

  void getMaxShredInsertSlot() {
  }

  void getMinimumBalanceForRentExemption() {
  }

  void getMultipleAccounts() {
  }

  void getProgramAccounts() {
  }

  void getRecentPerformanceSamples() {
  }

  void getSignaturesForAddress() {
  }

  void getSignatureStatuses() {
  }

  void getSlot() {
  }

  void getSlotLeader() {
  }

  void getSlotLeaders() {
  }

  void getStakeActivation() {
  }

  void getStakeMinimumDelegation() {
  }

  void getSupply() {
  }

  void getTokenAccountBalance() {
  }

  void getTokenAccountsByDelegate() {
  }

  void getTokenAccountsByOwner() {
  }

  void getTokenLargestAccounts() {
  }

  void getTokenSupply() {
  }

  void getTransaction() {
  }

  void getTransactionCount() {
  }

  void getVersion() {
  }

  void getVoteAccounts() {
  }

  void isBlockhashValid() {
  }

  void minimumLedgerSlot() {
  }

  void requestAirdrop() {
  }

  void sendTransaction() {
  }

  void simulateTransaction() {
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
