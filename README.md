# Solana C++ SDK
A Solana C++ SDK built on the [JSON RPC API](https://docs.solana.com/apps/jsonrpc-api)<br>
*You may notice small differences where we saw a benefit to changing the API.*

### Installation
```
$ TODO: add setup instructions
```

### Dependencies
You'll need to install both [libsodium](https://formulae.brew.sh/formula/libsodium#default) and [openssl@3](https://formulae.brew.sh/formula/openssl@3) TODO finish this section + json.hpp

### Usage
#### TODO break down key structs/classes, more examples
```c++
#include "../src/json.hpp"
#include "../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(clusterApiUrl(Cluster::MainnetBeta), Commitment::Processed);
  auto balance = connection.get_balance(PublicKey("{your_pubkey}"));

  std::cout << "balance = " << balance << std::endl;

  return 0;
}
```

### Contributing
If you'd like to help contribute or report a problem, submit a PR or email us at [many.defi@gmail.com](mailto:many.defi@gmail.com) 🤘
