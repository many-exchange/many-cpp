// clang++ verify_keypair.cpp -o verify_keypair -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  PublicKey public_key = PublicKey("24PNhTaNtomHhoy3fTRaMhAFCRj4uHqhZEEoWrKDbR5p");

  const uint8_t bytes[] = {
    174, 47, 154, 16, 202, 193, 206, 113, 199, 190, 53, 133, 169, 175, 31, 56, 222, 53, 138,
    189, 224, 216, 117, 173, 10, 149, 53, 45, 73, 251, 237, 246, 15, 185, 186, 82, 177, 240,
    148, 69, 241, 227, 167, 80, 141, 89, 240, 121, 121, 35, 172, 247, 68, 251, 226, 218, 48,
    63, 176, 109, 168, 89, 238, 135,
  };
  Keypair key_pair = Keypair::from_seed(bytes);
  std::cout << "Created Keypair with Public Key: " << key_pair.public_key.to_base58() << std::endl;

  std::cout << (public_key.to_base58() == key_pair.public_key.to_base58()) << std::endl;
}