#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_get_block_by_number = provider.eth_get_block_by_number(from_hex("0x1b4"));

  std::cout << "eth_get_block_by_number = " << eth_get_block_by_number.unwrap() << std::endl;

  return 0;
}
