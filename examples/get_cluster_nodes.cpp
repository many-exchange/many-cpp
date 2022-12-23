#include "../src/solana.h"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto nodes = connection.getClusterNodes();

  std::cout << "nodes = " << nodes.dump() << std::endl << std::endl;

  return 0;
}
