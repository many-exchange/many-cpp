// clang++ basic-3.cpp -o basic-3 -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "../../src/anchor.hpp"

using namespace anchor;

int main() {
  Connection connection(cluster_api_url(Cluster::Localnet), Commitment::Processed);
  auto payer = Keypair::generate();
  Provider provider(&connection, &payer);

  auto puppet_program = Program::load(provider, "basic-3-puppet.json", PublicKey("Fg6PaFpoGXkYsidMpWTK6W2BeZ7FEfcYkg476zPFsLnS"));
  auto puppet_master_program = Program::load(provider, "basic-3-puppet-master.json", PublicKey("HmbTLCmaGvZhKnn1Zfa1JVnp7vkMV4DYVxPLWBVoN65L"));

  //TODO
  /*
    // Initialize a new puppet account.
    const newPuppetAccount = anchor.web3.Keypair.generate();
    const tx = await puppet.methods
      .initialize()
      .accounts({
        puppet: newPuppetAccount.publicKey,
        user: provider.wallet.publicKey,
        systemProgram: SystemProgram.programId,
      })
      .signers([newPuppetAccount])
      .rpc();

    // Invoke the puppet master to perform a CPI to the puppet.
    await puppetMaster.methods
      .pullStrings(new anchor.BN(111))
      .accounts({
        puppet: newPuppetAccount.publicKey,
        puppetProgram: puppet.programId,
      })
      .rpc();

    // Check the state updated.
    puppetAccount = await puppet.account.data.fetch(newPuppetAccount.publicKey);
    assert.ok(puppetAccount.data.eq(new anchor.BN(111)));
  */

  return 0;
}
