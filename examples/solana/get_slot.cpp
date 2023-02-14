// clang++ get_slot.cpp -o get_slot -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  uint64_t slot = connection.get_slot().unwrap();

  std::cout << "slot = " << slot << std::endl;

  return 0;
}
