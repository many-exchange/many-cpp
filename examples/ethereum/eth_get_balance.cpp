#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_get_balance = provider.eth_get_balance("0x407d73d8a49eeb85d32cf465507dd71d507100c1");

  std::cout << "eth_get_balance = " << eth_get_balance.unwrap() << std::endl;

  return 0;
}
