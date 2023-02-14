#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto net_listening = provider.net_listening();

  std::cout << "net_listening = " << net_listening.unwrap() << std::endl;

  return 0;
}
