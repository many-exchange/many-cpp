// clang++ get_token_account_balance.cpp -o get_token_account_balance -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);

  std::string public_key;
  std::cout << "Enter public key: ";
  std::cin >> public_key;

  std::vector<TokenAccount> token_accounts = connection.get_token_accounts_by_owner(PublicKey(public_key), NATIVE_MINT).unwrap();
  if (token_accounts.size() == 0) {
    std::cout << "No token accounts found." << std::endl;
    return 1;
  }

  TokenBalance token_balance = connection.get_token_account_balance(token_accounts[0].pubkey).unwrap();

  std::cout << "balance = " << token_balance.tokens() << std::endl;

  return 0;
}
