#include "../src/solana.h"

using namespace solana;

// TODO: test this example when get_token_accounts_by_owner works.
int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenAccounts = connection.getTokenAccountsByOwner(PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"), NATIVE_MINT);
  if (tokenAccounts.size() == 0) {
    std::cout << "No token accounts found." << std::endl;
    return 1;
  }

  auto tokenBalance = connection.getTokenAccountBalance(tokenAccounts[0].pubkey);

  std::cout << "balance = " << tokenBalance.uiAmount << std::endl << std::endl;

  return 0;
}
