#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto nodes = connection.get_cluster_nodes().unwrap();

  for (auto& node : nodes) {
    std::cout << "node = " << node.pubkey.to_base58() << std::endl;
  }

  return 0;
}
