#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_sign = provider.eth_sign("0x9b2055d370f73ec7d8a03e965129118dc8f5bf83", "0xdeadbeaf");

  std::cout << "eth_sign = " << (eth_sign.ok() ? "Y" : "N") << std::endl;

  return 0;
}
