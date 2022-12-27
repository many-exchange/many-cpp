#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO: not sure if this is working.
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenAccounts = connection.getTokenAccountsByOwner(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"), NATIVE_MINT);
  if (tokenAccounts.size() == 0) {
    std::cout << "No token accounts found." << std::endl;
    return 1;
  }

  for (auto &tokenAccount : tokenAccounts) {
    std::cout << "public key = " << tokenAccount.pubkey.toBase58() << std::endl;
    std::cout << "  owner = " << tokenAccount.account.owner.toBase58() << std::endl;
    std::cout << "  token amount = " << tokenAccount.account.data.parsed.info.tokenAmount.as_tokens() << std::endl << std::endl;
  }

  return 0;
}
