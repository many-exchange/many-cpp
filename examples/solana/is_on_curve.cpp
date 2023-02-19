// clang++ is_on_curve.cpp -o is_on_curve -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  auto public_key = PublicKey("5oNDL3swdJJF1g9DzJiZ4ynHXgszjAEpUkxVYejchzrY");

  std::cout << public_key.is_on_curve() << std::endl;

  return 0;
}