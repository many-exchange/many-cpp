#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_protocol_version = provider.eth_protocol_version();

  std::cout << "eth_protocol_version = " << eth_protocol_version.unwrap() << std::endl;

  return 0;
}
