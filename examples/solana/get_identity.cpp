#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace many::solana;

// TODO test this example
int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  Identity identity = connection.get_identity().unwrap();

  std::cout << "identity = " << identity.identity.to_base58() << std::endl;

  return 0;
}
