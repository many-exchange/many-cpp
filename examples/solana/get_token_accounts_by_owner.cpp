#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenAccounts = connection.get_token_accounts_by_owner(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA")).unwrap();

  for (TokenAccount tokenAccount : tokenAccounts) {
    std::cout << "pubkey = " << tokenAccount.pubkey.to_base58() << std::endl;
    std::cout << "owner = " << tokenAccount.account.owner.to_base58() << std::endl;
    std::cout << "mint = " << tokenAccount.account.data.parsed.info.mint.to_base58() << std::endl;
    std::cout << "tokens = " << tokenAccount.account.data.parsed.info.tokenAmount.tokens() << std::endl << std::endl;
  }

  return 0;
}
