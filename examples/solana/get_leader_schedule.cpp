// clang++ get_leader_schedule.cpp -o get_leader_schedule -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);
  PublicKey current_leader = connection.get_slot_leader().unwrap();
  LeaderSchedule leader_schedule = connection.get_leader_schedule(current_leader).unwrap();

  std::string string_schedule;
  for (auto& item : leader_schedule.schedule) {
    string_schedule += std::to_string(item) + ", ";
  }
  std::cout << "leader schedule: " << string_schedule << std::endl;

  return 0;
}
