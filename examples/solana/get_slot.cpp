#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto slot = connection.get_slot().unwrap();

  std::cout << "slot = " << slot << std::endl << std::endl;

  return 0;
}
