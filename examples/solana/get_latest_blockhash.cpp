#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  Blockhash latest_blockhash = connection.get_latest_blockhash().unwrap();

  std::cout << "latest blockhash = " << latest_blockhash.blockhash << std::endl;

  return 0;
}
