#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "./doctest.h"

#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

PublicKey programId("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA");
PublicKey mint("So11111111111111111111111111111111111111112");
PublicKey associatedTokenProgramId("ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL");

TEST_CASE("get_associated_token_address CsfCXcswe5pjoW6M7rgAhXz1GRPAGAYsESozFFPg6AeY") {
  PublicKey owner("CsfCXcswe5pjoW6M7rgAhXz1GRPAGAYsESozFFPg6AeY");
  std::tuple<PublicKey, uint8_t> pda = PublicKey::find_program_address({ owner.to_buffer(), programId.to_buffer(), mint.to_buffer() }, associatedTokenProgramId);
  ASSERT(std::get<0>(pda) == PublicKey("A8QkUQWihGvC1cgFsJudjQwcv2Ubb3i4MTUmwPybpurm"));
}

TEST_CASE("get_associated_token_address nisxRxKkjLXt1qyJ1VfwYX9AjyyRehEhQTdmFEiUo26") {
  PublicKey owner("nisxRxKkjLXt1qyJ1VfwYX9AjyyRehEhQTdmFEiUo26");
  std::tuple<PublicKey, uint8_t> pda = PublicKey::find_program_address({ owner.to_buffer(), programId.to_buffer(), mint.to_buffer() }, associatedTokenProgramId);
  ASSERT(std::get<0>(pda) == PublicKey("ApMFHdM1fohWjMpYEWWRsbxGJDFL3FqA6oe9GEGa8dbQ"));
}

TEST_CASE("get_associated_token_address 2V42XoRgBR5aimAB9RDRzPS18yZcjAiN28pWqMcU8Fmq") {
  PublicKey owner("2V42XoRgBR5aimAB9RDRzPS18yZcjAiN28pWqMcU8Fmq");
  std::tuple<PublicKey, uint8_t> pda = PublicKey::find_program_address({ owner.to_buffer(), programId.to_buffer(), mint.to_buffer() }, associatedTokenProgramId);
  ASSERT(std::get<0>(pda) == PublicKey("DEAGK4KynGGwihPzZDgngtcnSjJAfNE2gW9n5gei7p9n"));
}

TEST_CASE("get_associated_token_address GM4Z8CQ4z3vZayM8mX76nhcqoskufz2U58N3FGo8ZVMW") {
  PublicKey owner("GM4Z8CQ4z3vZayM8mX76nhcqoskufz2U58N3FGo8ZVMW");
  std::tuple<PublicKey, uint8_t> pda = PublicKey::find_program_address({ owner.to_buffer(), programId.to_buffer(), mint.to_buffer() }, associatedTokenProgramId);
  ASSERT(std::get<0>(pda) == PublicKey("5LcqH2Yitqu2R2kzVWKJh5ttK1ZoxrX7V1tpXYFDUNRn"));
}

TEST_CASE("get_associated_token_address 7TrFa6HZCve8eWRg24Q895kDAzg1pK6pWXisZffY44Mo") {
  PublicKey owner("7TrFa6HZCve8eWRg24Q895kDAzg1pK6pWXisZffY44Mo");
  std::tuple<PublicKey, uint8_t> pda = PublicKey::find_program_address({ owner.to_buffer(), programId.to_buffer(), mint.to_buffer() }, associatedTokenProgramId);
  ASSERT(std::get<0>(pda) == PublicKey("G5HV3HSNg5rv4ARm5EW75KtRP9e28StqUvpAvF7kWkao"));
}
