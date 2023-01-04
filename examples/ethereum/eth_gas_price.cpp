#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_gas_price = provider.eth_gas_price();

  std::cout << "eth_gas_price = " << eth_gas_price.unwrap() << std::endl;

  return 0;
}
