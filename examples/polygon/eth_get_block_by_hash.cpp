#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto eth_get_block_by_hash = provider.eth_get_block_by_hash();

  std::cout << "eth_get_block_by_hash = " << eth_get_block_by_hash.unwrap() << std::endl;

  return 0;
}
