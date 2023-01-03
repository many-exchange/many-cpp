#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  Provider provider("https://rpc.ankr.com/eth");

  auto eth_get_balance = provider.eth_get_balance();

  std::cout << "eth_get_balance = " << eth_get_balance["result"] << std::endl;

  return 0;
}
