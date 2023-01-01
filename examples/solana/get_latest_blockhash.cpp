#include "../../src/json.hpp"

using json = nlohmann::json;

#include "../../src/solana.hpp"

using namespace solana;

int main() {
  Connection connection(cluster_api_url(Cluster::MainnetBeta), Commitment::Processed);
  auto latestBlockhash = connection.get_latest_blockhash().unwrap();

  std::cout << "latest blockhash = " << latestBlockhash.blockhash << std::endl;

  return 0;
}
