#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenAccounts = connection.getTokenAccountsByOwner(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"));

  for (TokenAccount tokenAccount : tokenAccounts) {
    std::cout << "pubkey = " << tokenAccount.pubkey.toBase58() << std::endl;
    std::cout << "owner = " << tokenAccount.account.owner.toBase58() << std::endl;
    std::cout << "mint = " << tokenAccount.account.data.parsed.info.mint.toBase58() << std::endl;
    std::cout << "tokens = " << tokenAccount.account.data.parsed.info.tokenAmount.as_tokens() << std::endl << std::endl;
  }

  return 0;
}
