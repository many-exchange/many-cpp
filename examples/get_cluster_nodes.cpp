#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto nodes = connection.get_cluster_nodes();

  std::cout << "nodes = " << nodes.dump() << std::endl << std::endl;

  return 0;
}
