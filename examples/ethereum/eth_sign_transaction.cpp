#include "../../src/ethereum.hpp"

using namespace ethereum;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/eth");

  auto from = "0xb60e8dd61c5d32be8058bb8eb970870f07233155";
  auto to = "0xd46e8dd67c5d32be8058bb8eb970870f07244567";
  auto gas = 90000;
  auto gas_price = 20000000000;
  auto value = 1000000000000000000;
  auto data = "0xd46e8dd67c5d32be8d46e8dd67c5d32be8058bb8eb970870f072445675058bb8eb970870f072445675";
  auto nonce = 0;

  auto eth_sign_transaction = provider.eth_sign_transaction(from, data, to, gas, gas_price, value, nonce);

  std::cout << "eth_sign_transaction.ok() = " << (eth_sign_transaction.ok() ? "Y" : "N") << std::endl;

  return 0;
}
