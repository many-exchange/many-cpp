#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace many::solana;

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
