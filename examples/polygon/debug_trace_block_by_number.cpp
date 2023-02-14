#include "../../src/polygon.hpp"

using namespace polygon;

int main() {
  JsonRpcProvider provider("https://rpc.ankr.com/polygon");

  auto debug_trace_block_by_number = provider.debug_trace_block_by_number();

  std::cout << "debug_trace_block_by_number = " << debug_trace_block_by_number.unwrap() << std::endl;

  return 0;
}
