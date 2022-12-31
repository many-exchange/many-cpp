#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto accountInfos = connection.get_multiple_accounts({PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"), NATIVE_MINT}).unwrap();

  for (AccountInfo accountInfo : accountInfos) {
    std::cout << "pubkey = " << accountInfo.pubkey.to_base58() << std::endl;
    std::cout << "owner = " << accountInfo.account.owner.to_base58() << std::endl;
    std::cout << "lamports = " << accountInfo.account.lamports << std::endl;
    std::cout << "data = " << accountInfo.account.data[0] << std::endl;
    std::cout << "executable = " << (accountInfo.account.executable ? "true" : "false") << std::endl << std::endl;
  }

  return 0;
}
