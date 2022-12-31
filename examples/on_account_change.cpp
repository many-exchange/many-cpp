#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);
  int subscriptionId = connection.on_account_change(PublicKey("J83w4HKfqxwcq3BEMMkPFSppX3gqekLyLJBexebFVkix"), [&](Context context, AccountInfo accountInfo) {
    std::cout << "pubkey = " << accountInfo.pubkey.to_base58() << std::endl;
    std::cout << "owner = " << accountInfo.account.owner.to_base58() << std::endl;
    std::cout << "lamports = " << accountInfo.account.lamports << std::endl;
    std::cout << "data = " << accountInfo.account.data << std::endl;
    std::cout << "executable = " << (accountInfo.account.executable ? "true" : "false") << std::endl << std::endl;
  });
  ASSERT(connection.is_connected());

  for (int i = 0; i < 10; i++) {
    connection.poll();
    sleep(1);
  }

  connection.remove_account_listener(subscriptionId);
  return 0;
}
