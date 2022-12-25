#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto slot = connection.getSlot();

  std::cout << "slot = " << slot << std::endl << std::endl;

  return 0;
}
