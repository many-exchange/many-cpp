// clang++ get_slot_leader.cpp -o get_slot_leader -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto slot_leader = connection.get_slot_leader().unwrap();

  std::cout << "slot leader = " << slot_leader.to_base58() << std::endl;

  return 0;
}
