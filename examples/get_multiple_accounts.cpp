#include "../src/solana.h"

using namespace solana;

// TODO: test this
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto accountInfos = connection.getMultipleAccounts({
    PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"), 
    PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA")});
    
  for (auto accountInfo : accountInfos) {
    std::cout << "owner = " << accountInfo.owner.toBase58() << std::endl;
    std::cout << "lamports = " << accountInfo.lamports << std::endl;
    std::cout << "data = " << accountInfo.data << std::endl;
    std::cout << "executable = " << (accountInfo.executable ? "true" : "false") << std::endl << std::endl;
  }

  return 0;
}