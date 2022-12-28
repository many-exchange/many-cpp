#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto currentLeader = connection.get_slot_leader();
  auto leaderSchedule = connection.get_leader_schedule(currentLeader);

  std::cout << "leaderSchedule: " << leaderSchedule.dump() << std::endl << std::endl;

  return 0;
}
