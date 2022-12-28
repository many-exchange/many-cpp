#include "../src/json.hpp"
#include "../src/solana.hpp"
#include "../src/tokens.hpp"

using namespace solana;

int main() {
  // Create a connection to the cluster
  Connection connection(clusterApiUrl(Cluster::Devnet), Commitment::Processed);

  // Generate a new keypair
  auto keypair = Keypair::generate();

  // // Request an airdrop
  // std::string tx_hash = connection.requestAirdrop(keypair.publicKey);
  // std::cout << "tx_hash = " << tx_hash << std::endl << std::endl;

  // uint64_t lamports = 0;
  // while (lamports == 0) {
  //   sleep(3);
  //   lamports = connection.getBalance(keypair.publicKey);
  // }
  // std::cout << "lamports = " << lamports << std::endl << std::endl;

  // Create associated token account
  PublicKey associatedTokenAccount = token::create_associated_token_account(
    connection,
    keypair,
    NATIVE_MINT,
    keypair.publicKey
  );
  std::cout << "associatedTokenAccount = " << associatedTokenAccount.to_base58() << std::endl << std::endl;

  // sleep(3);

  // // Verify that the account was created
  // AccountInfo accountInfo = connection.getAccountInfo(associatedTokenAccount);
  // std::cout << "accountInfo owner = " << accountInfo.owner.to_base58() << std::endl << std::endl;

  return 0;
}
