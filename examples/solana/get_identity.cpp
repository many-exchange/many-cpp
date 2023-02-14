// clang++ get_identity.cpp -o get_identity -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

// TODO test this example
int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  Identity identity = connection.get_identity().unwrap();

  std::cout << "identity = " << identity.identity.to_base58() << std::endl;

  return 0;
}
