#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  std::vector<TokenAccount> token_accounts = connection.get_token_accounts_by_owner(PublicKey("CFAkMBwYYpzVHkSzJ4bWYYV1GatZP1YjSkfvPfLq2ZzA")).unwrap();

  for (TokenAccount token_account : token_accounts) {
    std::cout << "pubkey = " << token_account.pubkey.to_base58() << std::endl;
    std::cout << "owner = " << token_account.account.owner.to_base58() << std::endl;
    std::cout << "mint = " << token_account.account.data.parsed.info.mint.to_base58() << std::endl;
    std::cout << "tokens = " << token_account.account.data.parsed.info.tokenAmount.tokens() << std::endl;
  }

  return 0;
}
