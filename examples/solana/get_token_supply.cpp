// clang++ get_token_supply.cpp -o get_token_supply -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);

  std::string mint;
  std::cout << "Enter token mint: ";
  std::cin >> mint;

  TokenBalance token_supply = connection.get_token_supply(PublicKey(mint)).unwrap();

  std::cout << "token supply = " << token_supply.amount << std::endl;

  return 0;
}
