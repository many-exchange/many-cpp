#pragma once

#include <assert.h>

#include "../src/websocket.h"

using namespace solana;

TEST_CASE("Test websocket") {
  WebSocketClient client("wss://api.devnet.solana.com/", 443);
  client.connect();
}
