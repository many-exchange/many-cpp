// clang++ vanity_keypair.cpp -o vanity_keypair -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  auto key_pair = Keypair::generate();

  while (key_pair.public_key.to_base58().substr(0, 1) != "A") {
    key_pair = Keypair::generate();
  }

  std::cout << "Created Keypair with Public Key: " << key_pair.public_key.to_base58() << std::endl;
}
