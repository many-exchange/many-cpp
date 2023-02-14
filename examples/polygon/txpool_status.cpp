#include "../../src/polygon.hpp"

using namespace polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto txpool_status = provider.txpool_status();

  std::cout << "txpool_status = " << txpool_status.unwrap() << std::endl;

  return 0;
}
