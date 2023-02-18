// clang++ basic-0.cpp -o basic-0 -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include "anchor.hpp"

using namespace anchor;
using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::Localnet), Commitment::Processed);
  auto payer = Keypair::generate();
  Provider provider(&connection, &payer);

  auto program = Program::load(provider, "basic-0.json", PublicKey("DcGPyWLy2f7ScEK5SuqNQbkxboAQ7crBbnBmf3ZNbzqv"));

  connection.require_airdrop(payer.public_key, 1000000000);

  auto result = program.rpc["initialize"]({{ "accounts", {} }});

  return 0;
}
