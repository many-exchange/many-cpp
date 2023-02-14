// clang++ get_token_accounts_by_owner.cpp -o get_token_accounts_by_owner -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);

  std::string public_key;
  std::cout << "Enter public key: ";
  std::cin >> public_key;

  std::vector<TokenAccount> token_accounts = connection.get_token_accounts_by_owner(PublicKey(public_key)).unwrap();

  for (TokenAccount token_account : token_accounts) {
    std::cout << "pubkey = " << token_account.pubkey.to_base58() << std::endl;
    std::cout << "owner = " << token_account.account.owner.to_base58() << std::endl;
    std::cout << "mint = " << token_account.account.data.parsed.info.mint.to_base58() << std::endl;
    std::cout << "tokens = " << token_account.account.data.parsed.info.token_amount.tokens() << std::endl;
  }

  return 0;
}
