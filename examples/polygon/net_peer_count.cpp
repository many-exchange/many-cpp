#include "../../src/polygon.hpp"

using namespace polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto net_peer_count = provider.net_peer_count();

  std::cout << "net_peer_count = " << net_peer_count.unwrap() << std::endl;

  return 0;
}
