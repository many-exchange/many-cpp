#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO: test this
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto accountInfos = connection.getMultipleAccounts({PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"), NATIVE_MINT});

  for (AccountInfo accountInfo : accountInfos) {
    std::cout << "owner = " << accountInfo.owner.toBase58() << std::endl;
    std::cout << "lamports = " << accountInfo.lamports << std::endl;
    std::cout << "data = " << accountInfo.data[0] << std::endl;
    std::cout << "executable = " << (accountInfo.executable ? "true" : "false") << std::endl << std::endl;
  }

  return 0;
}
