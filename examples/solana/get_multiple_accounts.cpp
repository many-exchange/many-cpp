// clang++ get_multiple_accounts.cpp -o get_multiple_accounts -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);

  std::vector<PublicKey> public_keys;
  std::string public_key;
  std::cout << "enter public keys: " << std::endl;
  while (std::cin >> public_key) {
      public_keys.push_back(PublicKey(public_key));
  }

  std::vector<Account> accounts = connection.get_multiple_accounts(public_keys).unwrap();

  for (Account account : accounts) {
    std::cout << "owner = " << account.owner.to_base58() << std::endl;
    std::cout << "lamports = " << account.lamports << std::endl;
    std::cout << "data = " << account.data[0] << std::endl;
    std::cout << "executable = " << (account.executable ? "true" : "false") << std::endl;
    std::cout << std::endl;
  }

  return 0;
}
