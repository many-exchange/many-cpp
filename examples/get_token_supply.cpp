#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenSupply = connection.get_token_supply(PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v")).unwrap();

  std::cout << "token supply = " << tokenSupply.amount << std::endl << std::endl;

  return 0;
}
