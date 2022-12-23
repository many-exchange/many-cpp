#include "../src/solana.h"

using namespace solana;

// TODO: This example is not working.
int main() {
  auto connection = Connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto tokenAccounts = connection.getTokenAccountsByOwner(PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"), NATIVE_MINT);
  if (tokenAccounts.size() == 0) {
    std::cout << "No token accounts found." << std::endl;
    return 1;
  }

  for (auto &tokenAccount : tokenAccounts) {
    std::cout << "public key = " << tokenAccount.pubkey.toBase58() << std::endl;
    std::cout << "owner = " << tokenAccount.account.owner.toBase58() << std::endl;
    std::cout << "program = " << tokenAccount.account.data.program.toBase58() << std::endl;
    std::cout << "account type = " << tokenAccount.account.data.parsed.accountType << std::endl;
    std::cout << "token amount = " << tokenAccount.account.data.parsed.info.tokenAmount.uiAmount << std::endl;
    std::cout << "delegate = " << tokenAccount.account.data.parsed.info.delegate.toBase58() << std::endl;
    std::cout << "delegated amount = " << tokenAccount.account.data.parsed.info.delegatedAmount.uiAmount << std::endl;
    std::cout << "state = " << tokenAccount.account.data.parsed.info.state << std::endl;
    std::cout << "is native = " << tokenAccount.account.data.parsed.info.isNative << std::endl;
    std::cout << "mint = " << tokenAccount.account.data.parsed.info.mint.toBase58() << std::endl;
    std::cout << "space = " << tokenAccount.account.data.space << std::endl;
    std::cout << "executable = " << tokenAccount.account.executable << std::endl;
    std::cout << "lamports = " << tokenAccount.account.lamports << std::endl;
    std::cout << "rent epoch = " << tokenAccount.account.rentEpoch << std::endl << std::endl;
  }

  return 0;
}
