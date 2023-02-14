#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_get_block_transaction_count_by_hash = provider.eth_get_block_transaction_count_by_hash("0xb903239f8543d04b5dc1ba6579132b143087c68db1b2168786408fcbce568238");

  std::cout << "eth_get_block_transaction_count_by_hash.ok() = " << (eth_get_block_transaction_count_by_hash.ok() ? "Y" : "N") << std::endl;

  return 0;
}
