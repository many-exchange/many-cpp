// clang++ request_airdrop.cpp -o request_airdrop -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection("https://api.devnet.solana.com");

  auto key_pair = Keypair::generate();

  std::string tx_hash = connection.request_airdrop(key_pair.public_key).unwrap();

  std::cout << "tx hash = " << tx_hash << std::endl;

  return 0;
}
