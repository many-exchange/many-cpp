#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenSupply = connection.getTokenSupply(PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"));

  std::cout << "token supply = " << tokenSupply.amount << std::endl << std::endl;

  return 0;
}
