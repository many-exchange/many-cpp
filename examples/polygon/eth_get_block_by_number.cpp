#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto eth_get_block_by_number = provider.eth_get_block_by_number();

  std::cout << "eth_get_block_by_number = " << eth_get_block_by_number.unwrap() << std::endl;

  return 0;
}
