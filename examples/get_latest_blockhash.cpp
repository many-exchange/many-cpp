#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto latestBlockhash = connection.get_latest_blockhash();

  std::cout << "latest blockhash = " << latestBlockhash << std::endl << std::endl;

  return 0;
}
