#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_syncing = provider.eth_syncing();

  std::cout << "eth_syncing.ok() = " << (eth_syncing.ok() ? "Y" : "N") << std::endl;

  return 0;
}
