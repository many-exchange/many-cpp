#pragma once

#include <assert.h>

#include "../src/websocket.h"

using namespace solana;

TEST_CASE("Test websocket") {
  std::string protocol = "wss://";
  std::string hostname = "stream.binance.com";
  std::string resource = "/stream";
  int port = 9443;

  WebSocketClient client(hostname, port);
  client.connect();
  /*
  client.Send("Hello, server!");
  client.Run();
  */
}

/*
TCPClient client("127.0.0.1", 1234);
client.sendMessage("Hello, world!");
std::string message = client.receiveMessage();
std::cout << "Received message: " << message << std::endl;
*/
