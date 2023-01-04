#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto eth_block_number = provider.eth_block_number();

  std::cout << "eth_block_number = " << eth_block_number.unwrap() << std::endl;

  return 0;
}
