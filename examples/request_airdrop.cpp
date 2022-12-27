#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::Devnet), Commitment::Processed);
  auto tx = connection.requestAirdrop(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"));

  std::cout << "tx hash = " << tx << std::endl << std::endl;

  return 0;
}
