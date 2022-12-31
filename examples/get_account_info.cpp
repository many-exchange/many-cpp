#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto account = connection.get_account_info(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA")).unwrap();

  std::cout << "owner = " << account.owner.to_base58() << std::endl;
  std::cout << "lamports = " << account.lamports << std::endl;
  std::cout << "data = " << account.data << std::endl;
  std::cout << "executable = " << (account.executable ? "true" : "false") << std::endl << std::endl;

  return 0;
}
