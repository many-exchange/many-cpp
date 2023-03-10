#include "../../src/polygon.hpp"

using namespace polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto eth_gas_price = provider.eth_gas_price();

  std::cout << "eth_gas_price = " << from_hex(eth_gas_price.unwrap()) << std::endl;

  return 0;
}
