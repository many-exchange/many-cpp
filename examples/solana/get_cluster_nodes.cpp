// clang++ get_cluster_nodes.cpp -o get_cluster_nodes -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  std::vector<ClusterNode> nodes = connection.get_cluster_nodes().unwrap();

  for (auto& node : nodes) {
    std::cout << "node = " << node.pubkey.to_base58() << std::endl;
  }

  return 0;
}
