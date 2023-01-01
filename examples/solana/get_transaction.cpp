#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  TransactionResponse transaction = connection.get_transaction("5WnkjRzRjb2sfCeU3JyE681pB8sMSEbhd9AqNgV8k3DryTeYzgnwCdoDuG2Jm3Qj2Tv6fHkJunZbvyxFtuunLkF1").unwrap();

  std::cout << "slot = " << transaction.slot << std::endl;
  std::cout << "fee = " << transaction.meta.fee << std::endl;
  std::cout << "instructions: " << std::endl;
  for (auto &instruction : transaction.transaction.message.instructions) {
    std::cout << "  programId index = " << instruction.programIdIndex << std::endl;
    std::cout << "  data = " << std::string(instruction.data.begin(), instruction.data.end()) << std::endl;
  }
  std::cout << "logs: " << std::endl;
  for (std::string &log : transaction.meta.logMessages) {
    std::cout << "  " << log << std::endl;
  }

  return 0;
}
