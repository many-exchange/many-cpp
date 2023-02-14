#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_block_number = provider.eth_block_number();

  std::cout << "eth_block_number = " << eth_block_number.unwrap() << std::endl;

  return 0;
}
