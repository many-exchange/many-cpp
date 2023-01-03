#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace many::solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);

  std::string tx_signature;
  std::cout << "Enter transaction signature: ";
  std::cin >> tx_signature;

  TransactionResponse transaction = connection.get_transaction(tx_signature).unwrap();

  std::cout << "slot = " << transaction.slot << std::endl;
  std::cout << "fee = " << transaction.meta.fee << std::endl;
  std::cout << "instructions: " << std::endl;
  for (auto &instruction : transaction.transaction.message.instructions) {
    std::cout << "  programId index = " << instruction.program_id_index << std::endl;
    std::cout << "  data = " << std::string(instruction.data.begin(), instruction.data.end()) << std::endl;
  }
  std::cout << "logs: " << std::endl;
  for (std::string &log : transaction.meta.log_messages) {
    std::cout << "  " << log << std::endl;
  }

  return 0;
}
