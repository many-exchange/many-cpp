#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/polygon.hpp"

using namespace many;
using namespace many::polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto web3_client_version = provider.web3_client_version();

  std::cout << "web3_client_version = " << web3_client_version.unwrap() << std::endl;

  return 0;
}
