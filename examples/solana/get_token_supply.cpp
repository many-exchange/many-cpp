#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace many::solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  TokenBalance token_supply = connection.get_token_supply(PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v")).unwrap();

  std::cout << "token supply = " << token_supply.amount << std::endl;

  return 0;
}
