#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"

#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

TEST_CASE("Encode base64") {
  std::string encoded = base64::encode("hello");
  ASSERT(encoded.size() == 8);
  //std::cout << encoded << std::endl;
  ASSERT(strcmp(encoded.c_str(), "aGVsbG8=") == 0);
}

TEST_CASE("Decode base64") {
  char buffer[1024];
  int length = base64::decode("aGVsbG8=", 8, buffer, sizeof(buffer));
  ASSERT(length == 5);
  ASSERT(memcmp(buffer, "hello", 5) == 0);
}
