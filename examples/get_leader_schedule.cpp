#include "../src/solana.h"

using namespace solana;

// TODO: this example is not working.
int main() {
  auto connection = Connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto leaderSchedule = connection.getLeaderSchedule();

  return 0;
}
