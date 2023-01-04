#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto txpool_inspect = provider.txpool_inspect();

  std::cout << "txpool_inspect = " << txpool_inspect.unwrap() << std::endl;

  return 0;
}
