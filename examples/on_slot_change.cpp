#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Localnet), Commitment::Processed);
  int subscriptionId = connection.on_slot_change([&](Context context, SlotInfo slotInfo) {
    std::cout << "slot = " << slotInfo.slot << std::endl << std::endl;
  });
  ASSERT(connection.is_connected());

  for (int i = 0; i < 10; i++) {
    connection.poll();
    sleep(1);
  }

  connection.remove_slot_change_listener(subscriptionId);
  return 0;
}
