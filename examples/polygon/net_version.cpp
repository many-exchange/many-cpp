#include "../../src/polygon.hpp"

using namespace polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto net_version = provider.net_version();

  std::cout << "net_version = " << net_version.unwrap() << std::endl;

  return 0;
}
