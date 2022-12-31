#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO test this example
int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);
  auto current_leader = connection.get_slot_leader().unwrap();
  auto leader_schedule = connection.get_leader_schedule(current_leader).unwrap();

  std::string string_schedule;
  for (auto& item : leader_schedule.schedule) {
    string_schedule += std::to_string(item) + ", ";
  }
  std::cout << "leader schedule: " << string_schedule << std::endl << std::endl;

  return 0;
}
