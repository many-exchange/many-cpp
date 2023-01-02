#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace many::solana;

TEST_CASE("Keypair::generate") {
  auto mint = Keypair::generate();
  auto owner = Keypair::generate();
  ASSERT(owner.public_key.is_on_curve());

  std::cout << "mint = " << mint.public_key.to_base58() << std::endl;
  std::cout << "owner = " << owner.public_key.to_base58() << std::endl;

  const PublicKey associated_token = token::get_associated_token_address(mint.public_key, owner.public_key);
  std::cout << "associated token = " << associated_token.to_base58() << std::endl;
}
