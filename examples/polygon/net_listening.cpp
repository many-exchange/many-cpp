#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto net_listening = provider.net_listening();

  std::cout << "net_listening = " << net_listening.unwrap() << std::endl;

  return 0;
}
