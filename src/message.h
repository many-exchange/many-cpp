#pragma once

#include <iostream>
#include <vector>

namespace solana {

int decodeLength(std::vector<uint8_t> bytes) {
  int len = 0;
  int size = 0;
  for (;;) {
    int elem = bytes[0];
    len |= (elem & 0x7f) << (size * 7);
    size += 1;
    if ((elem & 0x80) == 0) {
      break;
    }
  }
  return len;
}

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

    return Message(MessageHeader(numRequiredSignatures, numReadonlySignedAccounts, numReadonlyUnsignedAccounts), accountKeys, b58encode(recentBlockhash), instructions);
  }

};

} // namespace solana
