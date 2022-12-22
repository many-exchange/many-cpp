
#include "../src/solana.h"

using namespace solana;

int main() {

  auto connection = Connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);

  auto balance = connection.getBalance(PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"));

  std::cout << "balance = " << balance << std::endl;

  return 0;
}
