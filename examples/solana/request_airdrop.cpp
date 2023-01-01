#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);

  std::string public_key;
  std::cout << "Enter public key: ";
  std::cin >> public_key;

  std::string tx_hash = connection.request_airdrop(PublicKey(public_key)).unwrap();

  std::cout << "tx hash = " << tx_hash << std::endl;

  return 0;
}
