#include "../../src/polygon.hpp"

using namespace polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto net_listening = provider.net_listening();

  std::cout << "net_listening = " << net_listening.unwrap() << std::endl;

  return 0;
}
