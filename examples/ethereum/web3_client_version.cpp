#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto web3_client_version = provider.web3_client_version();

  std::cout << "web3_client_version = " << web3_client_version.unwrap() << std::endl;

  return 0;
}
