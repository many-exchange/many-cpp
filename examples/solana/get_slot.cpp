#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto slot = connection.get_slot().unwrap();

  std::cout << "slot = " << slot << std::endl;

  return 0;
}
