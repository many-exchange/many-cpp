#include "../src/json.hpp"
#include "../src/solana.hpp"
#include "../src/tokens.hpp"

using namespace solana;

int main() {

  auto mint = Keypair::generate();
  auto owner = Keypair::generate();
  std::cout << "mint = " << mint.publicKey.to_base58() << std::endl;
  std::cout << "owner = " << owner.publicKey.to_base58() << std::endl;

  const PublicKey associatedToken = token::get_associated_token_address(mint.publicKey, owner.publicKey);
  std::cout << "associatedToken = " << associatedToken.to_base58() << std::endl;

  return 0;
}
