#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);

  connection.on_slot_change([&](Context context, SlotInfo slotInfo) {
    std::cout << "slot = " << slotInfo.slot << std::endl;
  });

  for (int i = 0; i < 10; i++) {
    connection.poll();

    sleep(1);
  }

  return 0;
}
