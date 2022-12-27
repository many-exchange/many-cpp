#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO: this example is not working.
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto tx = connection.requestAirdrop(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"));

  std::cout << "tx hash = " << tx << std::endl << std::endl;

  return 0;
}
