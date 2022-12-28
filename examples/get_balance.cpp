#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto balance = connection.get_balance(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"));

  std::cout << "balance = " << balance << std::endl << std::endl;

  return 0;
}
