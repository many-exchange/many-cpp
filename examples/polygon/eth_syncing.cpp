#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto eth_syncing = provider.eth_syncing();

  std::cout << "eth_syncing = " << eth_syncing.unwrap() << std::endl;

  return 0;
}
