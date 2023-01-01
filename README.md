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
#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

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
```
