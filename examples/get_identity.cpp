#include "../src/json.h"
#include "../src/solana.h"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto identity = connection.getIdentity();

  std::cout << "identity = " << identity.toBase58() << std::endl << std::endl;

  return 0;
}
