#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO: this example is not working
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenAccounts = connection.getAllTokenAccountsByOwner(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"));

  for (TokenAccount tokenAccount : tokenAccounts) {
    std::cout << "pubkey = " << tokenAccount.pubkey.toBase58() << std::endl;
    std::cout << "  lamports = " << tokenAccount.account.lamports << std::endl;
    std::cout << "  owner = " << tokenAccount.account.owner.toBase58() << std::endl << std::endl;
  }

  return 0;
}
