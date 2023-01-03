#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace many::solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);

  std::string public_key;
  std::cout << "Enter public key: ";
  std::cin >> public_key;

  uint64_t balance = connection.get_balance(PublicKey(public_key)).unwrap();

  std::cout << "balance = " << balance << std::endl;

  return 0;
}
