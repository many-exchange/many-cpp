#include "../../src/polygon.hpp"

using namespace polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto eth_syncing = provider.eth_syncing();

  std::cout << "eth_syncing = " << eth_syncing.unwrap() << std::endl;

  return 0;
}
