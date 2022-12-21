#pragma once

#include <assert.h>

#include "../src/websocket.h"

using namespace solana;

TEST_CASE("Test websocket") {
  WebSocketClient client("wss://api.devnet.solana.com/", 443);
  client.connect();
}

/*
  // Set up the client to connect to a local Solana node
  solana_sdk::Client client;
  client.connect("127.0.0.1:8001");

  // Load the private key for your wallet from a file
  solana_sdk::PrivateKey private_key = solana_sdk::PrivateKey::load_from_file("wallet.key");

  // Set up the MEV transaction
  solana_sdk::Transaction transaction;
  transaction.recent_blockhash = client.get_recent_blockhash();
  transaction.signer_key = private_key;
  transaction.min_fee = client.get_min_transaction_fee();

  // Add the MEV instruction to the transaction
  solana_sdk::Instruction instruction;
  instruction.program_id = solana_sdk::pubkey_of(solana_sdk::MEV_PROGRAM_ID);
  instruction.data = solana_sdk::encode_mev_request(solana_sdk::MevRequest {});
  transaction.instructions.push_back(instruction);

  // Send the transaction to the network
  client.send_transaction(transaction);

  // Disconnect from the node
  client.disconnect();
*/
