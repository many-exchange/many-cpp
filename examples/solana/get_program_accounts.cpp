#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);

  std::string program_id;
  std::cout << "Enter program ID: ";
  std::cin >> program_id;

  std::vector<AccountInfo> program_accounts = connection.get_program_accounts(PublicKey(program_id)).unwrap();

  for (AccountInfo program_account : program_accounts) {
    std::cout << "owner = " << program_account.account.owner.to_base58() << std::endl;
    std::cout << "lamports = " << program_account.account.lamports << std::endl;
    std::cout << "data = " << program_account.account.data[0] << std::endl;
    std::cout << "executable = " << (program_account.account.executable ? "true" : "false") << std::endl;
  }

  return 0;
}
