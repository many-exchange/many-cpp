#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

// TODO test this example
int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  int subscriptionId = connection.on_account_change(TOKEN_PROGRAM_ID, [&](Context context, AccountInfo accountInfo) {
    std::cout << "pubkey = " << accountInfo.pubkey.to_base58() << std::endl;
    std::cout << "owner = " << accountInfo.account.owner.to_base58() << std::endl;
    std::cout << "lamports = " << accountInfo.account.lamports << std::endl;
    std::cout << "data = " << accountInfo.account.data << std::endl;
    std::cout << "executable = " << (accountInfo.account.executable ? "true" : "false") << std::endl << std::endl;
  });

  for (int i = 0; i < 10; i++) {
    connection.poll();
    sleep(1);
  }

  connection.remove_account_listener(subscriptionId);
  return 0;
}
