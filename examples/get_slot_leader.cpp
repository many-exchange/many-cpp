#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto slotLeader = connection.get_slot_leader();

  std::cout << "slot leader = " << slotLeader.to_base58() << std::endl << std::endl;

  return 0;
}
