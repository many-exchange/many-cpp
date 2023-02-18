// clang++ basic-1.cpp -o basic-1 -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

//#include "anchor.hpp"
#include "../../src/anchor.hpp"

using namespace anchor;

int main() {
  Connection connection(cluster_api_url(Cluster::Localnet), Commitment::Processed);
  auto payer = Keypair::generate();
  Provider provider(&connection, &payer);

  auto program = Program::load(provider, "basic-1.json", PublicKey("2f7rR9YhMhwadX5huTuWLjRKUXiLyGxsPDUP36Uxvtyr"));

  connection.require_airdrop(payer.public_key, 1000000000);

  // The Account to create.
  auto my_account = Keypair::generate();
  std::cout << my_account.public_key.to_base58() << std::endl;

  // Create the new account and initialize it with the program.
  program.rpc["initialize"]({{ "data", { 1234 } }, { "accounts", {
      { "myAccount", my_account.public_key },
      { "user", payer.public_key },
      { "systemProgram", SystemProgram::program_id },
    }
  }}, { payer, my_account });

  auto account = program.account["MyAccount"].fetch(my_account.public_key);
  while (account.is_null()) {
    sleep(1);
    account = program.account["MyAccount"].fetch(my_account.public_key);
  }

  // Check it's state was initialized.
  assert(account["data"] == 1234);

  // Invoke the update rpc.
  program.rpc["update"]({{ "data", { 4321 } }, { "accounts", {
      { "myAccount", my_account.public_key },
    }
  }}, { payer });

  // Fetch the newly updated account.
  //TODO
  sleep(30);
  account = program.account["MyAccount"].fetch(my_account.public_key);

  // Check it's state was initialized.
  assert(account["data"] == 4321);

  return 0;
}
