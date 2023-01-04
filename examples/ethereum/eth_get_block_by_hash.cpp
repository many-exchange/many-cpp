#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_get_block_by_hash = provider.eth_get_block_by_hash("0xdc0818cf78f21a8e70579cb46a43643f78291264dda342ae31049421c82d21ae");

  std::cout << "eth_get_block_by_hash = " << eth_get_block_by_hash.unwrap() << std::endl;

  return 0;
}
