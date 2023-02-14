#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_get_block_transaction_count_by_number = provider.eth_get_block_transaction_count_by_number(from_hex("0xe8"));

  std::cout << "eth_get_block_transaction_count_by_number.ok() = " << (eth_get_block_transaction_count_by_number.ok() ? "Y" : "N") << std::endl;

  return 0;
}
