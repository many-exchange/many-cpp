#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenAccounts = connection.get_token_accounts_by_owner(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"), NATIVE_MINT);
  if (tokenAccounts.size() == 0) {
    std::cout << "No token accounts found." << std::endl;
    return 1;
  }
  auto tokenBalance = connection.get_token_account_balance(tokenAccounts[0].pubkey);

  std::cout << "balance = " << tokenBalance.as_tokens() << std::endl << std::endl;

  return 0;
}
