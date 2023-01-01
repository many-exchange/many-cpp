#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenAccounts = connection.get_token_accounts_by_owner(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA"), NATIVE_MINT).unwrap();
  if (tokenAccounts.size() == 0) {
    std::cout << "No token accounts found." << std::endl;
    return 1;
  }
  auto tokenBalance = connection.get_token_account_balance(tokenAccounts[0].pubkey).unwrap();

  std::cout << "balance = " << tokenBalance.tokens() << std::endl << std::endl;

  return 0;
}
