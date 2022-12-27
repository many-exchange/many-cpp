#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::Devnet), Commitment::Processed);
  auto programAccounts = connection.getProgramAccounts(TOKEN_PROGRAM_ID);

  for (AccountInfo programAccount : programAccounts) {
    std::cout << "owner = " << programAccount.owner.to_base58() << std::endl;
    std::cout << "lamports = " << programAccount.lamports << std::endl;
    std::cout << "data = " << programAccount.data[0] << std::endl;
    std::cout << "executable = " << (programAccount.executable ? "true" : "false") << std::endl << std::endl;
  }

  return 0;
}