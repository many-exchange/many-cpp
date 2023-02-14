// clang++ get_version.cpp -o get_version -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  Version version = connection.get_version().unwrap();

  std::cout << "version = " << version.version << std::endl;

  return 0;
}
