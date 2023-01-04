#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto net_version = provider.net_version();

  std::cout << "net_version = " << net_version.unwrap() << std::endl;

  return 0;
}
