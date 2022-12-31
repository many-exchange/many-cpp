#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO test this example
int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto currentLeader = connection.get_slot_leader().unwrap();
  auto leaderSchedule = connection.get_leader_schedule(currentLeader).unwrap();

  std::string stringSchedule;
  for (auto& item : leaderSchedule) {
    stringSchedule += std::to_string(item) + ", ";
  }
  std::cout << "leader schedule: " << stringSchedule << std::endl << std::endl;

  return 0;
}
