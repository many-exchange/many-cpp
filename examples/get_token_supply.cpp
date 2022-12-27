#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO: not sure if this is working.
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenSupply = connection.getTokenSupply(NATIVE_MINT);

  std::cout << "token supply = " << tokenSupply.as_tokens() << std::endl << std::endl;

  return 0;
}
