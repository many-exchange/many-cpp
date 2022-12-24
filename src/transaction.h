#pragma once

#include <set>
#include <vector>

#include "keypair.h"
#include "message.h"

namespace solana {

#define SIGNATURE_LENGTH 64

const std::vector<uint8_t> DEFAULT_SIGNATURE = {0};

/**
 * Account metadata used to define instructions
 */
struct AccountMeta {
  /** An account's public key */
  PublicKey pubkey;
  /** True if an instruction requires a transaction signature matching `pubkey` */
  bool isSigner;
  /** True if the `pubkey` can be loaded as a read-write account. */
  bool isWritable;

  AccountMeta(PublicKey pubkey, bool isSigner, bool isWritable)
    : pubkey(pubkey), isSigner(isSigner), isWritable(isWritable) { }

  AccountMeta(PublicKey pubkey, bool isSigner)
    : pubkey(pubkey), isSigner(isSigner), isWritable(false) { }
};

/**
 * Transaction Instruction class
 */
struct TransactionInstruction {
  /**
   * Public keys to include in this transaction
   * Boolean represents whether this pubkey needs to sign the transaction
   */
  std::vector<AccountMeta> keys;
  /**
   * Program Id to execute
   */
  PublicKey programId;
  /**
   * Program input
   */
  std::vector<uint8_t> data;

  TransactionInstruction(
    std::vector<AccountMeta> keys,
    PublicKey programId,
    std::vector<uint8_t> data
  ) : keys(keys), programId(programId), data(data) { }

  TransactionInstruction(
    std::vector<AccountMeta> keys,
    PublicKey programId,
    Message message
  ) : keys(keys), programId(programId), data(message.serialize()) { }

};

/**
 * Pair of signature and corresponding public key
 */
struct SignaturePubkeyPair {
  std::string signature;
  PublicKey publicKey;
};

/**
 * Nonce information to be used to build an offline Transaction.
 */
struct NonceInformation {
  /** The current blockhash stored in the nonce */
  std::string nonce;
  TransactionInstruction nonceInstruction;
};

/**
 * Transaction class
 */
struct Transaction {
  /**
   * Signatures for the transaction.  Typically created by invoking the
   * `sign()` method
   */
  std::vector<SignaturePubkeyPair> signatures;

  /**
   * The transaction fee payer
   */
  PublicKey feePayer;

  /**
   * The instructions to atomically execute
   */
  std::vector<TransactionInstruction> instructions;

  /**
   * A recent transaction id. Must be populated by the caller
   */
  std::string recentBlockhash;

  /**
   * Optional Nonce information. If populated, transaction will use a durable
   * Nonce hash instead of a recentBlockhash. Must be populated by the caller
   */
  NonceInformation* nonceInfo = nullptr;

  Transaction() { }

  Transaction(
    PublicKey feePayer,
    std::vector<TransactionInstruction> instructions,
    std::string recentBlockhash
  ) : feePayer(feePayer), instructions(instructions), recentBlockhash(recentBlockhash) { }

  /**
   * Add one or more instructions to this Transaction
   */
  Transaction* add(
    std::vector<TransactionInstruction> items
  ) {
    if (items.size() == 0) {
      throw std::runtime_error("No instructions");
    }
    for (auto item : items) {
      instructions.push_back(item);
    }
    return this;
  }

  /**
   * Compile transaction data
   */
  Message compile_message() {
    /*
    if (nonceInfo && instructions[0] != nonceInfo->nonceInstruction) {
      recentBlockhash = nonceInfo->nonce;
      instructions.insert(instructions.begin(), nonceInfo->nonceInstruction);
    }
    if (recentBlockhash == nullptr) {
      throw std::runtime_error("Transaction recentBlockhash required");
    }
    if (instructions.size() < 1) {
      std::cout << "No instructions provided";
    }
    PublicKey* feePayer;
    if (this->feePayer) {
      feePayer = this->feePayer;
    } else if (signatures.size() > 0 && signatures[0].publicKey) {
      // Use implicit fee payer
      feePayer = signatures[0].publicKey;
    } else {
      throw std::runtime_error("Transaction fee payer required");
    }
    for (int i = 0; i < instructions.size(); i++) {
      if (instructions[i].programId == nullptr) {
        throw std::runtime_error(
          "Transaction instruction index " + std::to_string(i) + " has undefined program id"
        );
      }
    }

    std::vector<std::string> programIds;
    std::vector<AccountMeta> accountMetas;
    for (auto instruction : instructions) {
      for (auto accountMeta : instruction.keys) {
        accountMetas.push_back(accountMeta);
      }
      std::string programId = instruction.programId->toBase58();
      if (std::find(programIds.begin(), programIds.end(), programId) == programIds.end()) {
        programIds.push_back(programId);
      }
    }
    std::vector<CompiledInstruction> compiled_instructions;
    for (auto instruction : instructions) {
      std::vector<uint8_t> accounts;
      for (auto accountMeta : instruction.keys) {
        int index = 0;
        for (auto meta : accountMetas) {
          if (meta.pubkey->toBase58() == accountMeta.pubkey->toBase58()) {
            accounts.push_back(index);
            break;
          }
          index++;
        }
      }
      std::string programId = instruction.programId->toBase58();
      int index = 0;
      for (auto id : programIds) {
        if (id == programId) {
          break;
        }
        index++;
      }
      compiled_instructions.push_back(CompiledInstruction(
        index,
        accounts,
        instruction.data
      ));
    }

    return Message(
      MessageHeader(
        1,
        0,
        0
      ),
      std::vector<PublicKey>{*feePayer},
      recentBlockhash,
      compiled_instructions
    );
    */
    throw std::runtime_error("Not implemented");
  }

  /**
   * Sign the Transaction with the specified signers. Multiple signatures may
   * be applied to a Transaction. The first signature is considered "primary"
   * and is used identify and confirm transactions.
   *
   * If the Transaction `feePayer` is not set, the first signer will be used
   * as the transaction fee payer account.
   *
   * Transaction fields should not be modified after the first call to `sign`,
   * as doing so may invalidate the signature and cause the Transaction to be
   * rejected.
   *
   * The Transaction must be assigned a valid `recentBlockhash` before invoking this method
   */
  void sign(
    std::vector<Keypair> signers
  ) {
    if (signers.size() == 0) {
      throw std::runtime_error("No signers");
    }

    // Dedupe signers
    std::set<std::string> seen;
    /*
    std::vector<Signer*> uniqueSigners;
    for (auto signer : signers) {
      std::string key = signer->publicKey->toBase58();
      if (seen.find(key) != seen.end()) {
        continue;
      } else {
        seen.insert(key);
        uniqueSigners.push_back(signer);
      }
    }

    for (auto signer : uniqueSigners) {
      signatures.push_back(SignaturePubkeyPair(
        std::vector<uint8_t>{},
        *signer->publicKey
      ));
    }

    Message message = compile_message();
    partial_sign(message, uniqueSigners);
    verify_signatures(message.serialize(), true);
    */
    throw std::runtime_error("Not implemented");
  }

  /**
   * Serialize the Transaction in the wire format.
   */
  std::vector<uint8_t> serialize() const {
    /*
    Message message = compile_message();
    return message.serialize();
    */
    throw std::runtime_error("Not implemented");
  }

  /**
   * Parse a wire transaction into a Transaction object.
   */
  static Transaction from(
    std::vector<uint8_t> buffer
  ) {
    // Slice up wire data
    std::vector<uint8_t> byteArray = buffer;

    int signatureCount = decodeLength(byteArray);
    std::vector<std::string> signatures;
    for (int i = 0; i < signatureCount; i++) {
      std::vector<uint8_t> signature = std::vector<uint8_t>(byteArray.begin(), byteArray.begin() + SIGNATURE_LENGTH);
      byteArray.erase(byteArray.begin(), byteArray.begin() + SIGNATURE_LENGTH);
      signatures.push_back(b58encode(signature));
    }

    return populate(Message::from(byteArray), signatures);
  }

  /**
   * Populate Transaction object from message and signatures
   */
  static Transaction populate(
    Message message,
    std::vector<std::string> signatures = std::vector<std::string>()
  ) {
    Transaction transaction;
    transaction.recentBlockhash = message.recentBlockhash;
    if (message.header.numRequiredSignatures > 0) {
      transaction.feePayer = message.accountKeys[0];
    }
    for (auto signature : signatures) {
      SignaturePubkeyPair sigPubkeyPair;
      if (signature == b58encode(DEFAULT_SIGNATURE)) {
        sigPubkeyPair.signature = "";
      } else {
        sigPubkeyPair.signature = b58decode(signature);
      }
      sigPubkeyPair.publicKey = message.accountKeys[0];
      transaction.signatures.push_back(sigPubkeyPair);
    }

    for (auto instruction : message.instructions) {
      std::vector<AccountMeta> keys;
      for (auto account : instruction.accounts) {
        keys.push_back(AccountMeta(
          message.accountKeys[account],
          false
        ));
      }
      transaction.instructions.push_back(TransactionInstruction(
        keys,
        message.accountKeys[instruction.programIdIndex],
        instruction.data
      ));
    }

    return transaction;
  }

};

}
