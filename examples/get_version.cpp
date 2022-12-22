#include "../src/solana.h"

using namespace solana;

int main() {

  auto connection = Connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);

  auto version = connection.getVersion();

  return 0;
}
