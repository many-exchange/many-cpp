#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "./doctest.h"

#include "../src/json.hpp"
#include "../src/solana.hpp"
#include "../src/tokens.hpp"

using namespace solana;

TEST_CASE("Keypair::generate") {
  auto mint = Keypair::generate();
  auto owner = Keypair::generate();
  ASSERT(owner.publicKey.is_on_curve());

  std::cout << "mint = " << mint.publicKey.to_base58() << std::endl;
  std::cout << "owner = " << owner.publicKey.to_base58() << std::endl;

  const PublicKey associatedToken = token::get_associated_token_address(mint.publicKey, owner.publicKey);
  std::cout << "associatedToken = " << associatedToken.to_base58() << std::endl;
}
