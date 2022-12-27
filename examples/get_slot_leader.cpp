#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto slotLeader = connection.getSlotLeader();

  std::cout << "slot leader = " << slotLeader.to_base58() << std::endl << std::endl;

  return 0;
}
