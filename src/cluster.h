#pragma once

#include <stdexcept>
#include <string>

namespace solana {

enum class Cluster {
  Devnet,
  Testnet,
  MainnetBeta
};

std::string clusterApiUrl(Cluster cluster) {
  switch (cluster) {
    case Cluster::Devnet:
      return "https://api.devnet.solana.com";
    case Cluster::Testnet:
      return "https://api.testnet.solana.com";
    case Cluster::MainnetBeta:
      return "https://api.mainnet-beta.solana.com";
    default:
      throw std::runtime_error("Invalid cluster.");
  }
}

}
