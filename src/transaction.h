#pragma once

#include <set>
#include <vector>

namespace solana {

  /**
   * The message header, identifying signed and read-only account
   */
  struct MessageHeader {
    /**
     * The number of signatures required for this message to be considered valid. The
     * signatures must match the first `numRequiredSignatures` of `accountKeys`.
     */
    uint8_t numRequiredSignatures;
    /** The last `numReadonlySignedAccounts` of the signed keys are read-only accounts */
    uint8_t numReadonlySignedAccounts;
    /** The last `numReadonlySignedAccounts` of the unsigned keys are read-only accounts */
    uint8_t numReadonlyUnsignedAccounts;

    MessageHeader() {
      numRequiredSignatures = 0;
      numReadonlySignedAccounts = 0;
      numReadonlyUnsignedAccounts = 0;
    }

    MessageHeader(uint8_t numRequiredSignatures, uint8_t numReadonlySignedAccounts, uint8_t numReadonlyUnsignedAccounts) {
      this->numRequiredSignatures = numRequiredSignatures;
      this->numReadonlySignedAccounts = numReadonlySignedAccounts;
      this->numReadonlyUnsignedAccounts = numReadonlyUnsignedAccounts;
    }
  };

  /**
   * An instruction to execute by a program
   *
   * @property {number} programIdIndex
   * @property {number[]} accounts
   * @property {string} data
   */
  struct CompiledInstruction {
    /** Index into the transaction keys array indicating the program account that executes this instruction */
    uint8_t programIdIndex;
    /** Ordered indices into the transaction keys array indicating which accounts to pass to the program */
    std::vector<uint8_t> accounts;
    /** The program input data encoded as base 58 */
    std::vector<uint8_t> data;

    CompiledInstruction() {
      programIdIndex = 0;
      accounts = std::vector<uint8_t>();
      data = std::vector<uint8_t>();
    }

    CompiledInstruction(uint8_t programIdIndex, std::vector<uint8_t> accounts, std::vector<uint8_t> data)
      : programIdIndex(programIdIndex), accounts(accounts), data(data) { }
  };

  /**
   * List of instructions to be processed atomically
   */
  struct Message {
    /** The message header, identifying signed and read-only `accountKeys` */
    MessageHeader header;
    /** All the account keys used by this transaction */
    std::vector<PublicKey> accountKeys;
    /** The hash of a recent ledger block */
    std::string recentBlockhash;
    /** Instructions that will be executed in sequence and committed in one atomic transaction if all succeed. */
    std::vector<CompiledInstruction> instructions;

    Message() {
      header = MessageHeader();
      accountKeys = std::vector<PublicKey>();
      recentBlockhash = "";
      instructions = std::vector<CompiledInstruction>();
    }

    Message(MessageHeader header, std::vector<PublicKey> accountKeys, std::string recentBlockhash, std::vector<CompiledInstruction> instructions)
      : header(header), accountKeys(accountKeys), recentBlockhash(recentBlockhash), instructions(instructions) { }

    std::vector<uint8_t> serialize() {
      int numKeys = accountKeys.size();

      /*
      let keyCount: number[] = [];
      shortvec.encodeLength(keyCount, numKeys);

      const instructions = this.instructions.map(instruction => {
        const {accounts, programIdIndex} = instruction;
        const data = bs58.decode(instruction.data);

        let keyIndicesCount: number[] = [];
        shortvec.encodeLength(keyIndicesCount, accounts.length);

        let dataCount: number[] = [];
        shortvec.encodeLength(dataCount, data.length);

        return {
          programIdIndex,
          keyIndicesCount: Buffer.from(keyIndicesCount),
          keyIndices: Buffer.from(accounts),
          dataLength: Buffer.from(dataCount),
          data,
        };
      });

      let instructionCount: number[] = [];
      shortvec.encodeLength(instructionCount, instructions.length);
      let instructionBuffer = Buffer.alloc(PACKET_DATA_SIZE);
      Buffer.from(instructionCount).copy(instructionBuffer);
      let instructionBufferLength = instructionCount.length;

      instructions.forEach(instruction => {
        const instructionLayout = BufferLayout.struct([
          BufferLayout.u8('programIdIndex'),

          BufferLayout.blob(
            instruction.keyIndicesCount.length,
            'keyIndicesCount',
          ),
          BufferLayout.seq(
            BufferLayout.u8('keyIndex'),
            instruction.keyIndices.length,
            'keyIndices',
          ),
          BufferLayout.blob(instruction.dataLength.length, 'dataLength'),
          BufferLayout.seq(
            BufferLayout.u8('userdatum'),
            instruction.data.length,
            'data',
          ),
        ]);

        const instructionBufferData = Buffer.alloc(instructionLayout.span);
        instructionLayout.encode(
          {
            programIdIndex: instruction.programIdIndex,
            keyIndicesCount: instruction.keyIndicesCount,
            keyIndices: instruction.keyIndices,
            dataLength: instruction.dataLength,
            data: instruction.data,
          },
          instructionBufferData,
        );

        instructionBufferData.copy(instructionBuffer, instructionBufferLength);
        instructionBufferLength += instructionBufferData.length;
      });

      const messageLayout = BufferLayout.struct([
        BufferLayout.u8('numRequiredSignatures'),
        BufferLayout.u8('numReadonlySignedAccounts'),
        BufferLayout.u8('numReadonlyUnsignedAccounts'),

        BufferLayout.blob(keyCount.length, 'keyCount'),
        BufferLayout.seq(
          BufferLayout.blob(PublicKey.LENGTH, 'key'),
          numKeys,
          'keys',
        ),

        BufferLayout.blob(32, 'recentBlockhash'),

        BufferLayout.blob(instructionCount.length, 'instructionCount'),
        BufferLayout.seq(
          BufferLayout.u8('instruction'),
          instructionBufferLength - instructionCount.length,
          'instructions',
        ),
      ]);

      const messageBufferData = Buffer.alloc(messageLayout.span);
      messageLayout.encode(
        {
          numRequiredSignatures: this.header.numRequiredSignatures,
          numReadonlySignedAccounts: this.header.numReadonlySignedAccounts,
          numReadonlyUnsignedAccounts: this.header.numReadonlyUnsignedAccounts,
          keyCount: Buffer.from(keyCount),
          keys: this.accountKeys.map(key => key.toBuffer()),
          recentBlockhash: bs58.decode(this.recentBlockhash),
          instructionCount: Buffer.from(instructionCount),
          instructions: instructionBuffer,
        },
        messageBufferData,
      );

      return messageBufferData;
      */
      throw std::runtime_error("Not implemented");
    }

    /**
     * Decode a compiled message into a Message object.
     */
    static Message from(std::vector<uint8_t> buffer) {
      // Slice up wire data
      std::vector<uint8_t> byteArray = buffer;

      uint8_t numRequiredSignatures = byteArray[0];
      byteArray.erase(byteArray.begin());
      uint8_t numReadonlySignedAccounts = byteArray[0];
      byteArray.erase(byteArray.begin());
      uint8_t numReadonlyUnsignedAccounts = byteArray[0];
      byteArray.erase(byteArray.begin());

      int accountCount = decodeLength(byteArray);
      std::vector<PublicKey> accountKeys;
      for (int i = 0; i < accountCount; i++) {
        std::vector<uint8_t> account = std::vector<uint8_t>(byteArray.begin(), byteArray.begin() + PUBLIC_KEY_LENGTH);
        byteArray.erase(byteArray.begin(), byteArray.begin() + PUBLIC_KEY_LENGTH);
        accountKeys.push_back(PublicKey(account.data()));
      }

      std::vector<uint8_t> recentBlockhash = std::vector<uint8_t>(byteArray.begin(), byteArray.begin() + PUBLIC_KEY_LENGTH);
      byteArray.erase(byteArray.begin(), byteArray.begin() + PUBLIC_KEY_LENGTH);

      int instructionCount = decodeLength(byteArray);
      std::vector<CompiledInstruction> instructions;
      for (int i = 0; i < instructionCount; i++) {
        uint8_t programIdIndex = byteArray[0];
        byteArray.erase(byteArray.begin());

        int keyIndicesCount = decodeLength(byteArray);
        std::vector<uint8_t> accounts;
        for (int j = 0; j < keyIndicesCount; j++) {
          accounts.push_back(byteArray[0]);
          byteArray.erase(byteArray.begin());
        }

        int dataLength = decodeLength(byteArray);
        std::vector<uint8_t> data = std::vector<uint8_t>(byteArray.begin(), byteArray.begin() + dataLength);
        byteArray.erase(byteArray.begin(), byteArray.begin() + dataLength);

        instructions.push_back(CompiledInstruction(programIdIndex, accounts, data));
      }

      return Message(MessageHeader(numRequiredSignatures, numReadonlySignedAccounts, numReadonlyUnsignedAccounts), accountKeys, base58::b58encode(recentBlockhash), instructions);
    }

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
        signatures.push_back(base58::b58encode(signature));
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
        if (signature == base58::b58encode(DEFAULT_SIGNATURE)) {
          sigPubkeyPair.signature = "";
        } else {
          sigPubkeyPair.signature = base58::b58decode(signature);
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
