// clang++ basic-2.cpp -o basic-2 -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "../../src/anchor.hpp"

using namespace anchor;

int main() {
  Connection connection(cluster_api_url(Cluster::Localnet), Commitment::Processed);
  auto payer = Keypair::generate();
  Provider provider(&connection, &payer);

  auto program = Program::load(provider, "basic-2.json", PublicKey("uDgNTSMh9owwbjLZNtRHzpKiZMu7KMCRwJHDd4ikRbm"));

  //TODO
  /*
    await program.methods
      .create(provider.wallet.publicKey)
      .accounts({
        counter: counter.publicKey,
        user: provider.wallet.publicKey,
        systemProgram: SystemProgram.programId,
      })
      .signers([counter])
      .rpc();

    let counterAccount = await program.account.counter.fetch(counter.publicKey);

    assert.ok(counterAccount.authority.equals(provider.wallet.publicKey));
    assert.ok(counterAccount.count.toNumber() === 0);
  */

  /*
    await program.methods
      .increment()
      .accounts({
        counter: counter.publicKey,
        authority: provider.wallet.publicKey,
      })
      .rpc();

    const counterAccount = await program.account.counter.fetch(
      counter.publicKey
    );

    assert.ok(counterAccount.authority.equals(provider.wallet.publicKey));
    assert.ok(counterAccount.count.toNumber() == 1);
  */

  return 0;
}
