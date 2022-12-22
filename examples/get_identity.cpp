#include "../src/solana.h"

using namespace solana;

int main() {

  auto connection = Connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);

  auto identity = connection.getIdentity();

  std::cout << "identity = " << identity.toBase58() << std::endl;

  return 0;
}
