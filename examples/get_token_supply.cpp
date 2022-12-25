#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO: this example is not working.
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenSupply = connection.getTokenSupply(NATIVE_MINT);

  std::cout << "token supply = " << tokenSupply.uiAmount << std::endl << std::endl;

  return 0;
}
