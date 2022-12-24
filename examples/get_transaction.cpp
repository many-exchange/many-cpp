#include "../src/solana.h"

using namespace solana;

// TODO: test this example
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto transaction = connection.getTransaction("5WnkjRzRjb2sfCeU3JyE681pB8sMSEbhd9AqNgV8k3DryTeYzgnwCdoDuG2Jm3Qj2Tv6fHkJunZbvyxFtuunLkF1");

  std::cout << "slot = " << transaction.slot << std::endl << std::endl;
  std::cout << "signatures:" << std::endl;
  for (auto& signature : transaction.transaction.signatures) {
    std::cout << "  " << signature << std::endl;
  }
  std::cout << "accounts:" << std::endl;
  for (auto& accountKey : transaction.transaction.message.accountKeys) {
    std::cout << "  " << accountKey.toBase58() << std::endl;
  }
  std::cout << "recentBlockhash = " << transaction.transaction.message.recentBlockhash << std::endl;
  std::cout << "instructions:" << std::endl;
  for (auto& instruction : transaction.transaction.message.instructions) {
    std::cout << "  programId index = " << instruction.programIdIndex << std::endl;
    for (auto& account : instruction.accounts) {
      std::cout << "    account = " << account << std::endl;
    }
    for (auto& data : instruction.data) {
      std::cout << "    data = " << data << std::endl;
    }
  }
  std::cout << "block time = " << transaction.blockTime << std::endl;
  std::cout << "error = " << transaction.meta.err << std::endl;
  std::cout << "fee = " << transaction.meta.fee << std::endl;
  std::cout << "pre-balances:" << std::endl;
  for (auto& preBalance : transaction.meta.preBalances) {
    std::cout << "  " << preBalance << std::endl;
  }
  std::cout << "post-balances:" << std::endl;
  for (auto& postBalance : transaction.meta.postBalances) {
    std::cout << "  " << postBalance << std::endl;
  }
  std::cout << "inner instructions = " << transaction.meta.innerInstructions << std::endl;
  std::cout << "logs:" << std::endl;
  for (auto& log : transaction.meta.logMessages) {
    std::cout << "  " << log << std::endl;
  }
  std::cout << "rewards:" << std::endl;
  for (auto& reward : transaction.meta.rewards) {
    std::cout << "  recipient = " << reward.pubkey.toBase58() << std::endl;
    std::cout << "  lamports = " << reward.lamports << std::endl;
    std::cout << "  postBalance = " << reward.postBalance << std::endl;
    std::cout << "  rewardType = " << reward.rewardType << std::endl;
    std::cout << "  commission = " << reward.commission << std::endl;
    std::cout << "----------------" << std::endl;
  }
  std::cout << "loaded addresses:" << std::endl;
  std::cout << "  writable:" << std::endl;
  for (auto& address : transaction.meta.loadedAddresses.writable) {
    std::cout << "    " << address.toBase58() << std::endl;
  }
  std::cout << "  read only:" << std::endl;
  for (auto& address : transaction.meta.loadedAddresses.readOnly) {
    std::cout << "    " << address.toBase58() << std::endl;
  }
  std::cout << "return data:" << std::endl;
  std::cout << "  programId = " << transaction.meta.returnData.programId.toBase58() << std::endl;
  std::cout << "  data = " <<  transaction.meta.returnData.data  << std::endl;

  return 0;
}
