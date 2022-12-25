#include "../src/json.h"
#include "../src/solana.h"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto accountInfo = connection.getAccountInfo(PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"));

  std::cout << "owner = " << accountInfo.owner.toBase58() << std::endl;
  std::cout << "lamports = " << accountInfo.lamports << std::endl;
  std::cout << "data = " << accountInfo.data << std::endl;
  std::cout << "executable = " << (accountInfo.executable ? "true" : "false") << std::endl << std::endl;

  return 0;
}
