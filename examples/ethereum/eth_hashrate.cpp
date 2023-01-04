#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_hashrate = provider.eth_hashrate();

  std::cout << "eth_hashrate.ok() = " << (eth_hashrate.ok() ? "Y" : "N") << std::endl;

  return 0;
}
