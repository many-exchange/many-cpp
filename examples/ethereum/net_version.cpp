#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto net_version = provider.net_version();

  std::cout << "net_version = " << net_version.unwrap() << std::endl;

  return 0;
}
