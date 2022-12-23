#include "../src/solana.h"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto leader = connection.getSlotLeader();
  auto leaderSchedule = connection.getLeaderSchedule(leader);

  std::cout << "leaderSchedule: " << leaderSchedule.dump() << std::endl << std::endl;

  return 0;
}
