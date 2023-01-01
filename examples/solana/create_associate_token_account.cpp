#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  // Create a connection to the cluster
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);

  // Generate a new keypair
  auto keypair = Keypair::generate();

  // Request an airdrop
  std::string tx_hash = connection.request_airdrop(keypair.publicKey).unwrap();
  std::cout << "tx_hash = " << tx_hash << std::endl;

  uint64_t lamports = 0;
  while (lamports == 0) {
    sleep(3);
    lamports = connection.get_balance(keypair.publicKey).unwrap();
  }
  std::cout << "lamports = " << lamports << std::endl;

  // Create associated token account
  PublicKey associatedTokenAccount = token::create_associated_token_account(
    connection,
    keypair,
    NATIVE_MINT,
    keypair.publicKey
  ).unwrap();
  std::cout << "associatedTokenAccount = " << associatedTokenAccount.to_base58() << std::endl;

  for (int i = 0; i < 10; i++) {
    // Verify that the account was created
    Result<Account> result = connection.get_account_info(associatedTokenAccount);

    if (result.result) { //TODO I don't like this syntax
      Account& account = result.result.value(); //TODO I don't like this syntax
      std::cout << "owner = " << account.owner.to_base58() << std::endl;
      std::cout << "lamports = " << account.lamports << std::endl;
      std::cout << "data = " << account.data << std::endl;
      std::cout << "executable = " << (account.executable ? "true" : "false") << std::endl;
      return 0;
    }

    sleep(2);
  }

  throw std::runtime_error("Failed to create associated token account");
  return 1;
}
