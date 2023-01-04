#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_mining = provider.eth_mining();

  std::cout << "eth_mining.ok() = " << (eth_mining.ok() ? "Y" : "N") << std::endl;

  return 0;
}
