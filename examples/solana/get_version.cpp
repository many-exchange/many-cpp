#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto version = connection.get_version().unwrap();

  std::cout << "version = " << version.version << std::endl << std::endl;

  return 0;
}