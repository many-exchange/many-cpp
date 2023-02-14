// clang++ get_account_info.cpp -o get_account_info -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);

  std::string public_key;
  std::cout << "Enter public key: ";
  std::cin >> public_key;

  Account account = connection.get_account_info(PublicKey(public_key)).unwrap();

  std::cout << "owner = " << account.owner.to_base58() << std::endl;
  std::cout << "lamports = " << account.lamports << std::endl;
  std::cout << "data = " << account.data << std::endl;
  std::cout << "executable = " << (account.executable ? "true" : "false") << std::endl;

  return 0;
}
