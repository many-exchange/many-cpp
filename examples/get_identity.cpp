#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto identity = connection.get_identity();

  std::cout << "identity = " << identity.to_base58() << std::endl << std::endl;

  return 0;
}
