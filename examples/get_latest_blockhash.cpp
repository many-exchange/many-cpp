#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto latestBlockhash = connection.getLatestBlockhash();

  std::cout << "latest blockhash = " << latestBlockhash << std::endl << std::endl;

  return 0;
}
