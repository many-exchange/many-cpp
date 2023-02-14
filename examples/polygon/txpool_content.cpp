#include "../../src/polygon.hpp"

using namespace polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto txpool_content = provider.txpool_content();

  std::cout << "txpool_content = " << txpool_content.unwrap() << std::endl;

  return 0;
}
