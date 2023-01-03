#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  Provider provider("https://rpc.ankr.com/eth");

  auto eth_block_number = provider.eth_block_number();

  std::cout << "eth_block_number = " << eth_block_number["result"] << std::endl;

  return 0;
}
