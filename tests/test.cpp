#include <chrono>

#include "../src/json.hpp"
#include "../src/solana.hpp"
#include "../src/tokens.hpp"

using namespace solana;

#define USDC_MINT PublicKey("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v")

bool json_includes_pubkey(const std::vector<json>& array, const PublicKey& pubkey) {
  for (auto& item : array) {
    if (item["pubkey"].get<std::string>() == pubkey.to_base58()) {
      return true;
    }
  }
  return false;
}

bool pubkeys_include_pubkey(const std::vector<PublicKey>& array, const PublicKey& pubkey) {
  for (auto& item : array) {
    if (item.to_base58() == pubkey.to_base58()) {
      return true;
    }
  }
  return false;
}

int main() {
  // Initial timestamp
  auto start = std::chrono::high_resolution_clock::now();

  // Create connections to the clusters
  std::cout << "Connecting to clusters...";
  Connection mainnet_beta_connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  Connection devnet_connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);
  std::cout << " done." << std::endl;

  // Get the current version
  std::cout << "get_version()...";
  std::string version = mainnet_beta_connection.get_version().unwrap().version;
  assert(atoi(&version[0]) > 0);
  std::cout << " done." << std::endl;

  // Get the Pubkey for the current node
  std::cout << "get_identity()...";
  PublicKey node = mainnet_beta_connection.get_identity().unwrap().identity;
  assert(node.to_base58() != PublicKey().to_base58());
  std::cout << " done." << std::endl;

  // Get the current slot
  std::cout << "get_slot()...";
  uint64_t slot = mainnet_beta_connection.get_slot().unwrap();
  assert(slot > 0);
  std::cout << " done." << std::endl;

  // Get the current slot leader
  std::cout << "get_slot_leader()...";
  PublicKey leader = mainnet_beta_connection.get_slot_leader().unwrap();
  assert(leader.to_base58() != PublicKey().to_base58());
  std::cout << " done." << std::endl;

  // Get the schedule for the current leader
  std::cout << "get_leader_schedule()...";
  json schedule = mainnet_beta_connection.get_leader_schedule(leader).unwrap();
  assert(schedule[leader.to_base58()].size() > 0);
  std::cout << " done." << std::endl;

  // Get all cluster nodes
  std::cout << "get_cluster_nodes()...";
  std::vector<json> nodes = mainnet_beta_connection.get_cluster_nodes().unwrap();
  assert(nodes.size() > 0 && json_includes_pubkey(nodes, leader) && json_includes_pubkey(nodes, node));
  std::cout << " done." << std::endl;

  // Get the latest blockhash
  std::cout << "get_latest_blockhash()...";
  std::string blockhash = mainnet_beta_connection.get_latest_blockhash().unwrap().blockhash;
  assert(blockhash.length() > 0);
  std::cout << " done." << std::endl;

  // Get the current supply of USDC
  std::cout << "get_token_supply()...";
  TokenBalance native_supply = mainnet_beta_connection.get_token_supply(USDC_MINT).unwrap();
  assert(native_supply.amount > 0);
  std::cout << " done." << std::endl;

  // Generate a new keypair
  std::cout << "Generating a new keypair...";
  auto keypair = Keypair::generate();
  assert(keypair.secretKey.size() > 0 && keypair.publicKey.to_base58() != PublicKey().to_base58());
  std::cout << " done." << std::endl;

  // Request an airdrop
  std::cout << "request_airdrop()...";
  std::string tx_hash = devnet_connection.request_airdrop(keypair.publicKey).unwrap();
  assert(tx_hash.length() > 0);
  std::cout << " done." << std::endl;

  // Wait for the airdrop to complete and check balance
  std::cout << "get_balance()...";
  uint64_t lamports = 0;
  while (lamports == 0) {
    sleep(5);
    lamports = devnet_connection.get_balance(keypair.publicKey).unwrap();
  }
  assert(lamports > 0);
  std::cout << " done." << std::endl;

  // Fetch transaction using hash from the airdrop
  std::cout << "get_transaction()...";
  TransactionResponse txResp;
  while (txResp.slot == 0) {
    sleep(5);
    txResp = devnet_connection.get_transaction(tx_hash).unwrap();
  }
  assert(pubkeys_include_pubkey(txResp.transaction.message.accountKeys, keypair.publicKey));
  std::cout << " done." << std::endl;

  // Create associated token account
  std::cout << "token::create_associated_token_account()...";
  PublicKey associatedTokenAccount = token::create_associated_token_account(
    devnet_connection,
    keypair,
    NATIVE_MINT,
    keypair.publicKey
  );
  assert(associatedTokenAccount.to_base58() != PublicKey().to_base58());
  std::cout << " done." << std::endl;

  // Verify that the account was created
  std::cout << "get_account_info()...";
  AccountInfo accountInfo = devnet_connection.get_account_info(associatedTokenAccount).unwrap();
  assert(accountInfo.owner.to_base58() == ASSOCIATED_TOKEN_PROGRAM_ID.to_base58());
  std::cout << " done." << std::endl;

  // Get the balance of the associated token account (should be 0)
  std::cout << "get_token_account_balance()...";
  TokenBalance tokenBalance = devnet_connection.get_token_account_balance(associatedTokenAccount).unwrap();
  assert(tokenBalance.amount == 0);
  std::cout << " done." << std::endl;

  // Get all token accounts for the keypair, check for ownership
  std::cout << "get_token_accounts_by_owner()...";
  std::vector<TokenAccount> tokenAccounts = devnet_connection.get_token_accounts_by_owner(keypair.publicKey).unwrap();
  assert(tokenAccounts[0].account.data.parsed.info.owner.to_base58() ==  keypair.publicKey.to_base58()
    && tokenAccounts[0].account.data.parsed.info.mint.to_base58() == NATIVE_MINT.to_base58());
  std::cout << " done." << std::endl;

  // Tests complete
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "All tests passed and completed in " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds" << std::endl;

  return 0;
}
