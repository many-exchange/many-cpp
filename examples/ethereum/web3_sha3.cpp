#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto web3_sha3 = provider.web3_sha3("0x68656c6c6f20776f726c64");

  std::cout << "web3_sha3 = " << web3_sha3.unwrap() << std::endl;

  return 0;
}
