// clang++ on_slot_change.cpp -o on_slot_change -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  int subscriptionId = connection.on_slot_change([&](Result<SlotInfo> result) {
    SlotInfo slotInfo = result.unwrap();
    std::cout << "slot = " << slotInfo.slot << std::endl;
  });
  ASSERT(connection.is_connected());

  for (int i = 0; i < 10; i++) {
    connection.poll();
    sleep(1);
  }

  connection.remove_slot_change_listener(subscriptionId);
  return 0;
}
