#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace many::solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);
  std::vector<AccountInfo> program_accounts = connection.get_program_accounts(PublicKey("gSbePebfvPy7tRqimPoVecS2UsBvYv46ynrzWocc92s")).unwrap();

  for (AccountInfo program_account : program_accounts) {
    std::cout << "owner = " << program_account.account.owner.to_base58() << std::endl;
    std::cout << "lamports = " << program_account.account.lamports << std::endl;
    std::cout << "data = " << program_account.account.data[0] << std::endl;
    std::cout << "executable = " << (program_account.account.executable ? "true" : "false") << std::endl;
  }

  return 0;
}
