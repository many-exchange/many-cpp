#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto currentLeader = connection.get_slot_leader().unwrap();
  auto leaderSchedule = connection.get_leader_schedule(currentLeader).unwrap();

  std::cout << "leaderSchedule: " << leaderSchedule.dump() << std::endl << std::endl;

  return 0;
}
