#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

//TODO not working
int main() {
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);
  AccountInfo programAccountInfo = connection.get_program_accounts(PublicKey("gSbePebfvPy7tRqimPoVecS2UsBvYv46ynrzWocc92s")).unwrap()[0];
  int subscriptionId = connection.on_program_account_change(programAccountInfo.pubkey, [&](Result<Account> result) {
    Account account = result.unwrap();
    std::cout << "owner = " << account.owner.to_base58() << std::endl;
    std::cout << "lamports = " << account.lamports << std::endl;
    std::cout << "data = " << account.data << std::endl;
    std::cout << "executable = " << (account.executable ? "true" : "false") << std::endl;
  });
  ASSERT(connection.is_connected());

  for (int i = 0; i < 10; i++) {
    connection.poll();
    sleep(1);
  }

  connection.remove_program_account_change_listnener(subscriptionId);
  return 0;
}
