// clang++ kitchen_sink.cpp -o kitchen_sink -std=c++17 -I ../../src/ -lssl -lcrypto -lsodium

#include <algorithm>
#include <chrono>

#include "solana.hpp"

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

int main() {
  // Initial timestamp
  auto start = std::chrono::high_resolution_clock::now();

  // Create connection to the cluster
  std::cout << "Connecting to cluster...";
  Connection connection(cluster_api_url(Cluster::Devnet), Commitment::Processed);
  std::cout << " done." << std::endl;

  // Get the current version
  std::cout << "get_version()...";
  std::string version = connection.get_version().unwrap().version;
  ASSERT(atoi(&version[0]) > 0);
  std::cout << " done." << std::endl;

  // Get the Pubkey for the current node
  std::cout << "get_identity()...";
  Identity identity = connection.get_identity().unwrap();
  ASSERT(identity.identity.to_base58() != PublicKey().to_base58());
  std::cout << " done." << std::endl;

  // Get the current slot
  std::cout << "get_slot()...";
  uint64_t current_slot = connection.get_slot().unwrap();
  ASSERT(current_slot > 0);
  std::cout << " done." << std::endl;

  // Create slot change subscription
  std::cout << "Creating slot change subscription...";
  uint64_t new_slot;
  int slot_subscription_id = connection.on_slot_change([&](Result<SlotInfo> result) {
    SlotInfo slot_info = result.unwrap();
    new_slot = slot_info.slot;
  });
  std::cout << " done." << std::endl;

  // Get the current slot leader
  std::cout << "get_slot_leader()...";
  PublicKey leader = connection.get_slot_leader().unwrap();
  ASSERT(leader.to_base58() != PublicKey().to_base58());
  std::cout << " done." << std::endl;

  // Get the schedule for the current leader
  std::cout << "get_leader_schedule()...";
  LeaderSchedule leader_schedule = connection.get_leader_schedule(leader).unwrap();
  ASSERT(leader_schedule.schedule.size() > 0);
  std::cout << " done." << std::endl;

  // Get all cluster nodes
  std::cout << "get_cluster_nodes()...";
  std::vector<ClusterNode> nodes = connection.get_cluster_nodes().unwrap();
  ASSERT(nodes.size() > 0);
  std::cout << " done." << std::endl;

  // Get the latest blockhash
  std::cout << "get_latest_blockhash()...";
  std::string blockhash = connection.get_latest_blockhash().unwrap().blockhash;
  ASSERT(blockhash.length() > 0);
  std::cout << " done." << std::endl;

  // Get the current supply of USDC
  std::cout << "get_token_supply()...";
  TokenBalance native_supply = connection.get_token_supply(USDC_MINT).unwrap();
  ASSERT(native_supply.amount > 0);
  std::cout << " done." << std::endl;

  // Generate a new keypair
  std::cout << "Generating a new keypair...";
  auto keypair = Keypair::generate();
  ASSERT(keypair.secret_key.size() > 0 && keypair.public_key.to_base58() != PublicKey().to_base58());
  std::cout << " done." << std::endl;

  // Create account change subscription
  std::cout << "Creating account change subscription...";
  uint64_t new_keypair_lamports = 0;
  int account_subscription_id = connection.on_account_change(keypair.public_key, [&](Result<Account> result) {
    Account account = result.unwrap();
    new_keypair_lamports = account.lamports;
  });
  std::cout << " done." << std::endl;

  // Request an airdrop
  std::cout << "request_airdrop()...";
  std::string tx_hash = connection.request_airdrop(keypair.public_key).unwrap();
  ASSERT(tx_hash.length() > 0);
  std::cout << " done." << std::endl;

  // Wait for the airdrop to complete and check balance
  std::cout << "get_balance()...";
  uint64_t current_keypair_lamports = 0;
  while (current_keypair_lamports == 0) {
    sleep(5);
    current_keypair_lamports = connection.get_balance(keypair.public_key).unwrap();
  }
  ASSERT(current_keypair_lamports > 0);
  std::cout << " done." << std::endl;

  // Fetch transaction using hash from the airdrop
  std::cout << "get_transaction()...";
  TransactionResponse tx_resp = connection.get_transaction(tx_hash).unwrap();
  ASSERT(std::find(tx_resp.transaction.message.account_keys.begin(), tx_resp.transaction.message.account_keys.end(), keypair.public_key) != tx_resp.transaction.message.account_keys.end());
  std::cout << " done." << std::endl;

  // Check that slot and account subscriptions fired
  ASSERT(current_slot < new_slot);
  current_slot = new_slot;
  ASSERT(current_keypair_lamports < new_keypair_lamports);
  current_keypair_lamports = new_keypair_lamports;

  // Create associated token account
  std::cout << "token::create_associated_token_account()...";
  PublicKey associated_token_account = token::create_associated_token_account(
    connection,
    keypair,
    NATIVE_MINT,
    keypair.public_key
  ).unwrap();
  std::cout << keypair.public_key.to_base58() << std::endl;
  std::cout << associated_token_account.to_base58() << std::endl;
  ASSERT(associated_token_account.to_base58() != PublicKey().to_base58());
  std::cout << " done." << std::endl;

  // Verify that the account was created
  std::cout << "get_account_info()...";
  for (int i = 0; i < 10; i++) {
    Result<Account> result = connection.get_account_info(associated_token_account);
    if (result.ok()) {
      Account account = result.unwrap();
      ASSERT(account.owner.to_base58() == ASSOCIATED_TOKEN_PROGRAM_ID.to_base58());
      goto DONE;
    }
    sleep(2);
  }
  throw std::runtime_error("Account not found");
DONE:
  std::cout << " done." << std::endl;

  // Get the balance of the associated token account (should be 0)
  std::cout << "get_token_account_balance()...";
  TokenBalance token_balance = connection.get_token_account_balance(associated_token_account).unwrap();
  ASSERT(token_balance.amount == 0);
  std::cout << " done." << std::endl;

  // Get all token accounts for the keypair, check for ownership
  std::cout << "get_token_accounts_by_owner()...";
  std::vector<TokenAccount> token_accounts = connection.get_token_accounts_by_owner(keypair.public_key).unwrap();
  ASSERT(token_accounts[0].account.data.parsed.info.owner.to_base58() ==  keypair.public_key.to_base58()
    && token_accounts[0].account.data.parsed.info.mint.to_base58() == NATIVE_MINT.to_base58());
  std::cout << " done." << std::endl;

  // Tests complete
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "All tests passed and completed in " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds" << std::endl;

  return 0;
}
