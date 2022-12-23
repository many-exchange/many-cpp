#include "../src/solana.h"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto version = connection.getVersion();

  std::cout << "version = " << version << std::endl << std::endl;

  return 0;
}
