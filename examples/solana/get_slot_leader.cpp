#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto slot_leader = connection.get_slot_leader().unwrap();

  std::cout << "slot leader = " << slot_leader.to_base58() << std::endl;

  return 0;
}
