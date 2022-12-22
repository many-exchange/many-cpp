#include "../src/solana.h"

using namespace solana;

int main() {
  auto connection = Connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto nodes = connection.getClusterNodes();

  std::cout << "nodes = " << nodes.dump() << std::endl;

  return 0;
}
