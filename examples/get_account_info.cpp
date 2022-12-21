
#include "../src/solana.h"

using namespace solana;

int main() {

  auto connection = Connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);

  auto accountInfo = connection.getAccountInfo("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v");

  return 0;
}
