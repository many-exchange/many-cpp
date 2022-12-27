#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto currentLeader = connection.getSlotLeader();
  auto leaderSchedule = connection.getLeaderSchedule(currentLeader);

  std::cout << "leaderSchedule: " << leaderSchedule.dump() << std::endl << std::endl;

  return 0;
}
