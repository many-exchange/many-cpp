#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO: test this example
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto transaction = connection.getTransaction("5WnkjRzRjb2sfCeU3JyE681pB8sMSEbhd9AqNgV8k3DryTeYzgnwCdoDuG2Jm3Qj2Tv6fHkJunZbvyxFtuunLkF1");

  std::cout << transaction << std::endl;

  return 0;
}
