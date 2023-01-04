#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto txpool_content = provider.txpool_content();

  std::cout << "txpool_content = " << txpool_content.unwrap() << std::endl;

  return 0;
}
