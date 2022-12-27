#include "../src/json.hpp"
#include "../src/solana.hpp"
#include "../src/tokens.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::Devnet), Commitment::Processed);

  auto keypair = Keypair::generate();
  std::string tx_hash = connection.requestAirdrop(keypair.publicKey);
  std::cout << "tx_hash = " << tx_hash << std::endl << std::endl;

  sleep(3);

  uint64_t lamports = connection.getBalance(keypair.publicKey);
  std::cout << "lamports = " << lamports << std::endl << std::endl;

  PublicKey associatedTokenAccount = token::create_associated_token_account(
    connection,
    keypair,
    NATIVE_MINT,
    keypair.publicKey
  );
  std::cout << "associatedTokenAccount = " << associatedTokenAccount.to_base58() << std::endl << std::endl;

  sleep(3);

  AccountInfo accountInfo = connection.getAccountInfo(associatedTokenAccount);
  std::cout << "accountInfo owner = " << accountInfo.owner.to_base58() << std::endl << std::endl;

  return 0;
}
