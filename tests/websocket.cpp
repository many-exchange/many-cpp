#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "./doctest.h"

#include <assert.h>
#include <stdio.h>

#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

TEST_CASE("Test websocket") {
  websockets::WebSocketClient client("wss://api.devnet.solana.com/", 443);
  client.connect();
}
