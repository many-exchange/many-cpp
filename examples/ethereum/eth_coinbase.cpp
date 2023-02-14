#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_coinbase = provider.eth_coinbase();

  std::cout << "eth_coinbase.ok() = " << (eth_coinbase.ok() ? "Y" : "N") << std::endl;

  return 0;
}
