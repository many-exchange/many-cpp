#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto net_peer_count = provider.net_peer_count();

  std::cout << "net_peer_count = " << net_peer_count.unwrap() << std::endl;

  return 0;
}
