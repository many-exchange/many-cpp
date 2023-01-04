#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/ethereum.hpp"

using namespace many::ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto eth_accounts = provider.eth_accounts();

  std::cout << "eth_accounts = " << eth_accounts.unwrap().size() << std::endl;

  return 0;
}
