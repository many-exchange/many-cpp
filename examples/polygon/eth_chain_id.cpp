#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto eth_chain_Id = provider.eth_chain_Id();

  std::cout << "eth_chain_Id = " << from_hex(eth_chain_Id.unwrap()) << std::endl;

  return 0;
}
