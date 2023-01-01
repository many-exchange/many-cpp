#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  std::vector<ClusterNode> nodes = connection.get_cluster_nodes().unwrap();

  for (auto& node : nodes) {
    std::cout << "node = " << node.pubkey.to_base58() << std::endl;
  }

  return 0;
}
