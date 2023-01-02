# Low-latency C++ for DeFi
**EVM support coming soon*<br>
If you'd like to help contribute or report a problem, submit a PR or email us at [many.defi@gmail.com](mailto:many.defi@gmail.com) 🤘

# Solana
A Solana C++ SDK built on the [JSON RPC API](https://docs.solana.com/apps/jsonrpc-api)<br>
*You may notice small differences where we saw a benefit to changing the API.*

#### Dependencies
You'll need to make sure you have [libsodium](https://formulae.brew.sh/formula/libsodium#default) and [openssl@3](https://formulae.brew.sh/formula/openssl@3) installed, as well as a copy of [JSON for Modern C++](https://github.com/nlohmann/json).

#### Usage
`solana.hpp` is the main header file and contains the `Connection` class, which connects to and interacts with with Solana's JSON RPC API.
Refer to their [docs](https://docs.solana.com/apps/jsonrpc-api) or look through the header file to see what's currently supported.<br>
`tokens.hpp` can be used in conjunction for easy interaction with the SPL Token Program.

#### Example
```c++
#include "../src/json.hpp"
#include "../src/solana.hpp"
#include "../src/tokens.hpp"

using namespace many::solana;

int main() {
  // Create a connection to the cluster
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);

  // Generate a new keypair
  auto keypair = Keypair::generate();

  // Request an airdrop
  std::string tx_hash = connection.request_airdrop(keypair.public_key);
  std::cout << "tx_hash = " << tx_hash << std::endl;

  uint64_t lamports = 0;
  while (lamports == 0) {
    sleep(3);
    lamports = connection.get_balance(keypair.public_key);
  }
  std::cout << "lamports = " << lamports << std::endl;

  // Create associated token account
  PublicKey associatedTokenAccount = token::create_associated_token_account(
    connection,
    keypair,
    NATIVE_MINT,
    keypair.public_key
  );
  std::cout << "associatedTokenAccount = " << associatedTokenAccount.to_base58() << std::endl;

  // Verify that the account was created
  AccountInfo accountInfo = connection.get_account_info(associatedTokenAccount);
  std::cout << "pubkey = " << accountInfo.pubkey.to_base58() << std::endl;

  return 0;
}
```
