#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);
  auto programAccounts = connection.get_program_accounts(PublicKey("gSbePebfvPy7tRqimPoVecS2UsBvYv46ynrzWocc92s")).unwrap();

  for (AccountInfo programAccount : programAccounts) {
    std::cout << "owner = " << programAccount.account.owner.to_base58() << std::endl;
    std::cout << "lamports = " << programAccount.account.lamports << std::endl;
    std::cout << "data = " << programAccount.account.data[0] << std::endl;
    std::cout << "executable = " << (programAccount.account.executable ? "true" : "false") << std::endl << std::endl;
  }

  return 0;
}
