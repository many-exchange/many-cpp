#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);
  std::string tx_hash = connection.request_airdrop(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA")).unwrap();

  std::cout << "tx hash = " << tx_hash << std::endl;

  return 0;
}
