// clang++ basic-4.cpp -o basic-4 -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "../../src/anchor.hpp"

using namespace anchor;

int main() {
  Connection connection(cluster_api_url(Cluster::Localnet), Commitment::Processed);
  auto payer = Keypair::generate();
  Provider provider(&connection, &payer);

  auto program = Program::load(provider, "basic-4.json", PublicKey("CwrqeMj2U8tFr1Rhkgwc84tpAsqbt9pTt2a4taoTADPr"));

//TODO
/*
  const program = anchor.workspace.Basic4,
    counterSeed = anchor.utils.bytes.utf8.encode("counter");

  let counterPubkey;

  before(async () => {
    [counterPubkey] = await anchor.web3.PublicKey.findProgramAddress(
      [counterSeed],
      program.programId
    );
  });

  it("Is runs the constructor", async () => {
    // Initialize the program's state struct.
    await program.methods
      .initialize()
      .accounts({
        counter: counterPubkey,
        authority: provider.wallet.publicKey,
        systemProgram: anchor.web3.SystemProgram.programId,
      })
      .rpc();

    // Fetch the state struct from the network.
    const counterAccount = await program.account.counter.fetch(counterPubkey);

    assert.ok(counterAccount.count.eq(new anchor.BN(0)));
  });

  it("Executes a method on the program", async () => {
    await program.methods
      .increment()
      .accounts({
        counter: counterPubkey,
        authority: provider.wallet.publicKey,
      })
      .rpc();

    const counterAccount = await program.account.counter.fetch(counterPubkey);
    assert.ok(counterAccount.count.eq(new anchor.BN(1)));
  });
});
*/

  return 0;
}
