#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO: this example is not working
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto programAccounts = connection.getProgramAccounts(TOKEN_PROGRAM_ID);

  for (AccountInfo programAccount : programAccounts) {
    std::cout << "owner = " << programAccount.owner.toBase58() << std::endl;
    std::cout << "lamports = " << programAccount.lamports << std::endl;
    std::cout << "data = " << programAccount.data[0] << std::endl;
    std::cout << "executable = " << (programAccount.executable ? "true" : "false") << std::endl << std::endl;
  }

  return 0;
}
