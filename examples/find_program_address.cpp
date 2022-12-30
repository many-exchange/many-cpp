#include "../src/json.hpp"
#include "../src/solana.hpp"
#include "../src/tokens.hpp"

using namespace solana;

int main() {

  // auto mint = Keypair::generate();
  // auto owner = Keypair::generate();
  // std::cout << "mint = " << mint.publicKey.to_base58() << std::endl;
  // std::cout << "owner = " << owner.publicKey.to_base58() << std::endl;

  // const PublicKey associatedToken = token::get_associated_token_address(mint.publicKey, owner.publicKey);
  // std::cout << "associatedToken = " << associatedToken.to_base58() << std::endl;

  PublicKey programId("6Cust2JhvweKLh4CVo1dt21s2PJ86uNGkziudpkNPaCj");
  PublicKey programId2("BPFLoader1111111111111111111111111111111111");
  auto pda = PublicKey::find_program_address({ PublicKey("8VBafTNv1F8k5Bg7DTVwhitw3MGAMTmekHsgLuMJxLC8").to_buffer() }, programId);
  ASSERT(std::get<0>(pda).to_base58() == "FGnnqkzkXUGKD7wtgJCqTemU3WZ6yYqkYJ8xoQoXVvUG");
  ASSERT(std::get<1>(pda) == 255);

  auto pda2 = PublicKey::find_program_address({
    PublicKey("SeedPubey1111111111111111111111111111111111").to_buffer(),
    PublicKey("3gF2KMe9KiC6FNVBmfg9i267aMPvK37FewCip4eGBFcT").to_buffer(),
    PublicKey("HwRVBufQ4haG5XSgpspwKtNd3PC9GM9m1196uJW36vds").to_buffer(),
  }, programId2);
  ASSERT(std::get<0>(pda2).to_base58() == "GXLbx3CbJuTTtJDZeS1PGzwJJ5jGYVEqcXum7472kpUp");
  ASSERT(std::get<1>(pda2) == 254);

  return 0;
}
