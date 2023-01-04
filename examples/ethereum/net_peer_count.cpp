#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto net_peer_count = provider.net_peer_count();

  std::cout << "net_peer_count.ok() = " << (net_peer_count.ok() ? "Y" : "N") << std::endl;

  return 0;
}
