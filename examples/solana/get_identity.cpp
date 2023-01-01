#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO test this example
int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto identity = connection.get_identity().unwrap();

  std::cout << "identity = " << identity.identity.to_base58() << std::endl << std::endl;

  return 0;
}
