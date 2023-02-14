#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

TEST_CASE("find_program_address") {
  PublicKey program_id("6Cust2JhvweKLh4CVo1dt21s2PJ86uNGkziudpkNPaCj");
  auto pda = PublicKey::find_program_address({ PublicKey("8VBafTNv1F8k5Bg7DTVwhitw3MGAMTmekHsgLuMJxLC8").to_buffer() }, program_id);
  ASSERT(std::get<0>(pda).to_base58() == "FGnnqkzkXUGKD7wtgJCqTemU3WZ6yYqkYJ8xoQoXVvUG");
  ASSERT(std::get<1>(pda) == 255);
}

TEST_CASE("find_program_address with a nonce") {
  PublicKey program_id("BPFLoader1111111111111111111111111111111111");
  auto pda = PublicKey::find_program_address({
    PublicKey("SeedPubey1111111111111111111111111111111111").to_buffer(),
    PublicKey("3gF2KMe9KiC6FNVBmfg9i267aMPvK37FewCip4eGBFcT").to_buffer(),
    PublicKey("HwRVBufQ4haG5XSgpspwKtNd3PC9GM9m1196uJW36vds").to_buffer(),
  }, program_id);
  ASSERT(std::get<0>(pda).to_base58() == "GXLbx3CbJuTTtJDZeS1PGzwJJ5jGYVEqcXum7472kpUp");
  ASSERT(std::get<1>(pda) == 254);
}
