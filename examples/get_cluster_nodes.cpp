#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto nodes = connection.get_cluster_nodes();

  std::cout << "nodes = " << nodes.dump() << std::endl << std::endl;

  return 0;
}
