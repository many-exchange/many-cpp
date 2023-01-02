#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace many::solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  std::vector<Account> accounts = connection.get_multiple_accounts({PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"), NATIVE_MINT}).unwrap();

  for (Account account : accounts) {
    std::cout << "owner = " << account.owner.to_base58() << std::endl;
    std::cout << "lamports = " << account.lamports << std::endl;
    std::cout << "data = " << account.data[0] << std::endl;
    std::cout << "executable = " << (account.executable ? "true" : "false") << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
