#include "../src/solana.h"

using namespace solana;

// TODO: this example is not working.
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto leaderSchedule = connection.getLeaderSchedule();

  std::cout << "leaderSchedule: " << leaderSchedule << std::endl << std::endl;

  return 0;
}
