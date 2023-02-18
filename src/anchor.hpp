//  _____ _____ ____ _   _ _____ _____
// |  _  |   | |  __| |_| |     |  _  |  Solana C++ SDK
// |     | | | | |__|  _  |  |  |    \   version 0.0.1
// |__|__|_|___|____|_| |_|_____|__|__|  https://github.com/many-exchange/many-exchange-cpp
//
// Copyright (c) 2022-2023 Many Exchange
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License.
// You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include "solana.hpp"

using namespace solana;

namespace anchor {

  /**
   * @brief IdlType provides the IDL deserialized client representation of an Anchor type.
   */
  enum IdlType {
    boolean,
    u8,
    i8,
    u16,
    i16,
    u32,
    i32,
    f32,
    u64,
    i64,
    f64,
    u128,
    i128,
    u256,
    i256,
    bytes,
    string,
    publicKey,
    defined,
    option,
    //coption,
    vec,
    array
  };

  void from_json(const json& j, IdlType& type) {
    if (j == "bool") {
      type = IdlType::boolean;
    } else if (j == "u8") {
      type = IdlType::u8;
    } else if (j == "i8") {
      type = IdlType::i8;
    } else if (j == "u16") {
      type = IdlType::u16;
    } else if (j == "i16") {
      type = IdlType::i16;
    } else if (j == "u32") {
      type = IdlType::u32;
    } else if (j == "i32") {
      type = IdlType::i32;
    } else if (j == "f32") {
      type = IdlType::f32;
    } else if (j == "u64") {
      type = IdlType::u64;
    } else if (j == "i64") {
      type = IdlType::i64;
    } else if (j == "f64") {
      type = IdlType::f64;
    } else if (j == "u128") {
      type = IdlType::u128;
    } else if (j == "i128") {
      type = IdlType::i128;
    } else if (j == "u256") {
      type = IdlType::u256;
    } else if (j == "i256") {
      type = IdlType::i256;
    } else if (j == "bytes") {
      type = IdlType::bytes;
    } else if (j == "string") {
      type = IdlType::string;
    } else if (j == "publicKey") {
      type = IdlType::publicKey;
    } else if (j.contains("defined")) {
      type = IdlType::defined; //TODO
    } else if (j.contains("option")) {
      type = IdlType::option; //TODO
    } else if (j.contains("vec")) {
      type = IdlType::vec; //TODO
    } else if (j.contains("array")) {
      type = IdlType::array; //TODO
    } else {
      throw std::runtime_error("Unknown type: " + j.dump());
    }
  }

  struct IdlField {
    std::string name;
    IdlType type;
  };

  void from_json(const json& j, IdlField& field) {
    field.name = j["name"].get<std::string>();
    field.type = j["type"].get<IdlType>();
  }

  struct IdlTypeDefTyStruct {
    std::vector<IdlField> fields;
  };

  void from_json(const json& j, IdlTypeDefTyStruct& struct_) {
    for (auto it = j["fields"].begin(); it!= j["fields"].end(); ++it) {
      struct_.fields.push_back(it.value().get<IdlField>());
    }
  }

  /**
   * @brief IdlAccountDef provides the IDL deserialized client representation of an Anchor account definition.
   */
  struct IdlAccountDef {
    std::string name;
    IdlTypeDefTyStruct type;
  };

  void from_json(const json& j, IdlAccountDef& account) {
    account.name = j["name"].get<std::string>();
    account.type = j["type"].get<IdlTypeDefTyStruct>();
  }

  /**
   * @brief IdlConstant provides the IDL deserialized client representation of an Anchor constant.
   */
  struct IdlConstant {
    std::string name;
    IdlType type;
    std::string value;
  };

  void from_json(const json& j, IdlConstant& constant) {
    constant.name = j["name"].get<std::string>();
    constant.type = j["type"].get<IdlType>();
    constant.value = j["value"].get<std::string>();
  }

  /**
   * @brief IdlErrorCode provides the IDL deserialized client representation of an Anchor error code.
   */
  struct IdlErrorCode {
    int code;
    std::string name;
    std::optional<std::string> msg;
  };

  void from_json(const json& j, IdlErrorCode& error) {
    error.code = j["code"].get<int>();
    error.name = j["name"].get<std::string>();
    if (j.contains("msg")) {
      error.msg = j["msg"].get<std::string>();
    }
  }

  /**
   * @brief IdlEventField provides the IDL deserialized client representation of an Anchor event field.
   */
  struct IdlEventField {
    std::string name;
    IdlType type;
    bool index;
  };

  void from_json(const json& j, IdlEventField& event_field) {
    event_field.name = j["name"].get<std::string>();
    event_field.type = j["type"].get<IdlType>();
    event_field.index = j["index"].get<bool>();
  }

  /**
   * @brief IdlEvent provides the IDL deserialized client representation of an Anchor event.
   */
  struct IdlEvent {
    std::string name;
    std::vector<IdlEventField> fields;
  };

  void from_json(const json& j, IdlEvent& event) {
    event.name = j["name"];
    for (auto it = j.begin(); it != j.end(); ++it) {
      event.fields.push_back(it.value().get<IdlEventField>());
    }
  }

  /**
   * @brief IdlAccount provides the IDL deserialized client representation of an Anchor account passed into an instruction.
   */
  struct IdlAccount {
    std::string name;
    bool isMut;
    bool isSigner;
    bool isOptional;
  };

  void from_json(const json& j, IdlAccount& account) {
    account.name = j["name"];
    account.isMut = j["isMut"];
    account.isSigner = j["isSigner"];
    account.isOptional = j.contains("isOptional") ? j["isOptional"].get<bool>() : false;
  }

  /**
   * @brief IdlInstruction provides the IDL deserialized client representation of an Anchor instruction.
   */
  struct IdlInstruction {
    std::string name;
    std::vector<IdlAccount> accounts;
    std::vector<IdlField> args;
    std::optional<IdlType> returns;
  };

  void from_json(const json& j, IdlInstruction& instruction) {
    instruction.name = j["name"].get<std::string>();
    if (j.contains("accounts")) {
      for (auto it = j["accounts"].begin(); it != j["accounts"].end(); ++it) {
        instruction.accounts.push_back(it.value().get<IdlAccount>());
      }
    }
    if (j.contains("args")) {
      for (auto it = j["args"].begin(); it != j["args"].end(); ++it) {
        instruction.args.push_back(it.value().get<IdlField>());
      }
    }
    if (j.contains("returns")) {
      instruction.returns = j["returns"]["type"];
    }
  }

  // struct IdlEnumField {
  //   IdlField named;
  //   IdlType tuple;
  // };

  // void from_json(const json& j, IdlEnumField& enum_field) {
  //   //TODO
  // }

  struct IdlEnumVariant {
    std::string name;
    //TODO std::vector<IdlEnumField> fields;
  };

  void from_json(const json& j, IdlEnumVariant& variant) {
    variant.name = j["name"].get<std::string>();
  }

  struct IdlTypeDefTyEnum {
    std::vector<IdlEnumVariant> variants;
  };

  void from_json(const json& j, IdlTypeDefTyEnum& enum_) {
    for (auto it = j["variants"].begin(); it != j["variants"].end(); ++it) {
      enum_.variants.push_back(it.value().get<IdlEnumVariant>());
    }
  }

  /**
   * @brief IdlTypeDefTy provides the IDL deserialized client representation of an Anchor type definition.
   */
  struct IdlTypeDefTy {
    std::string kind;
    std::optional<IdlTypeDefTyEnum> enum_;
    std::optional<IdlTypeDefTyStruct> struct_;
  };

  void from_json(const json& j, IdlTypeDefTy& type) {
    type.kind = j["kind"].get<std::string>();
    if (type.kind == "enum") {
      type.enum_ = j.get<IdlTypeDefTyEnum>();
    } else if (type.kind == "struct") {
      type.struct_ = j.get<IdlTypeDefTyStruct>();
    }
  }

  /**
   * @brief IdlTypeDef provides the IDL deserialized client representation of an Anchor type definition.
   */
  struct IdlTypeDef {
    std::string name;
    IdlTypeDefTy type;
  };

  void from_json(const json& j, IdlTypeDef& type) {
    type.name = j["name"].get<std::string>();
    type.type = j["type"].get<IdlTypeDefTy>();
  }

  /**
   * @brief Idl provides the IDL representation of an Anchor IDL.
   */
  struct Idl {
    json idl;

    std::string version;
    std::string name;
    std::vector<IdlInstruction> instructions;
    std::vector<IdlAccountDef> accounts;
    std::vector<IdlTypeDef> types;
    std::vector<IdlEvent> events;
    std::vector<IdlErrorCode> errors;
    std::vector<IdlConstant> constants;

    Idl(json idl) : idl(idl) {
      name = idl["name"];
      version = idl["version"];

      for (auto it = idl["instructions"].begin(); it != idl["instructions"].end(); ++it) {
        instructions.push_back(it.value().get<IdlInstruction>());
      }

      if (idl.find("accounts") != idl.end()) {
        for (auto it = idl["accounts"].begin(); it != idl["accounts"].end(); ++it) {
          accounts.push_back(it.value().get<IdlAccountDef>());
        }
      }

      if (idl.find("types") != idl.end()) {
        for (auto it = idl["types"].begin(); it != idl["types"].end(); ++it) {
          types.push_back(it.value().get<IdlTypeDef>());
        }
      }

      if (idl.find("events") != idl.end()) {
        for (auto it = idl["events"].begin(); it != idl["events"].end(); ++it) {
          events.push_back(it.value().get<IdlEvent>());
        }
      }

      if (idl.find("errors") != idl.end()) {
        for (auto it = idl["errors"].begin(); it != idl["errors"].end(); ++it) {
          errors.push_back(it.value().get<IdlErrorCode>());
        }
      }

      if (idl.find("constants") != idl.end()) {
        for (auto it = idl["constants"].begin(); it != idl["constants"].end(); ++it) {
          constants.push_back(it.value().get<IdlConstant>());
        }
      }
    }

  };

  std::ostream& operator<<(std::ostream& stream, const Idl& idl) {
    stream << "Version: " << idl.version << std::endl;
    stream << "Name: " << idl.name << std::endl;
    if (idl.instructions.size() > 0) {
      stream << "Instructions:" << std::endl;
      for (auto it = idl.instructions.begin(); it != idl.instructions.end(); ++it) {
        stream << "  " << it->name << std::endl;
        if (it->accounts.size() > 0) {
          stream << "    Accounts:" << std::endl;
          for (auto it2 = it->accounts.begin(); it2 != it->accounts.end(); ++it2) {
            stream << "  " << it2->name << std::endl;
          }
        }
        if (it->args.size() > 0) {
          stream << "    Args:" << std::endl;
          for (auto it2 = it->args.begin(); it2 != it->args.end(); ++it2) {
            stream << "      " << it2->name << ": " << it2->type << std::endl;
          }
        }
        if (it->returns.has_value()) {
          stream << "    Returns: " << *it->returns << std::endl;
        }
      }
    }
    if (idl.accounts.size() > 0) {
      stream << "Accounts:" << std::endl;
      for (auto it = idl.accounts.begin(); it != idl.accounts.end(); ++it) {
        stream << "  " << it->name << std::endl;
        for (auto it2 = it->type.fields.begin(); it2 != it->type.fields.end(); ++it2) {
          stream << "    " << it2->name << ": " << it2->type << std::endl;
        }
      }
    }
    if (idl.types.size() > 0) {
      stream << "Types:" << std::endl;
      for (auto it = idl.types.begin(); it != idl.types.end(); ++it) {
        stream << "  " << it->name << ": " << it->type.kind << std::endl;
        if (it->type.kind == "struct") {
          for (auto it2 = it->type.struct_->fields.begin(); it2 != it->type.struct_->fields.end(); ++it2) {
            stream << "    " << it2->name << ": " << it2->type << std::endl;
          }
        } else if (it->type.kind == "enum") {
          for (auto it2 = it->type.enum_->variants.begin(); it2 != it->type.enum_->variants.end(); ++it2) {
            stream << "    " << it2->name << std::endl;
          }
        }
      }
    }
    if (idl.events.size() > 0) {
      stream << "Events:" << std::endl;
      for (auto it = idl.events.begin(); it != idl.events.end(); ++it) {
        stream << "  " << it->name << std::endl;
        for (auto it2 = it->fields.begin(); it2 != it->fields.end(); ++it2) {
          stream << "    " << it2->name << ": " << it2->type << std::endl;
        }
      }
    }
    if (idl.errors.size() > 0) {
      stream << "Errors:" << std::endl;
      for (auto it = idl.errors.begin(); it != idl.errors.end(); ++it) {
        stream << "  " << it->code << ": " << it->name << std::endl;
        if (it->msg.has_value()) {
          stream << "    " << *it->msg << std::endl;
        }
      }
    }
    if (idl.constants.size() > 0) {
      stream << "Constants:" << std::endl;
      for (auto it = idl.constants.begin(); it != idl.constants.end(); ++it) {
        stream << "  " << it->name << ": " << it->type << " = " << it->value << std::endl;
      }
    }
    return stream;
  }

  class AccountHandler {
    Connection* connection;
    IdlAccountDef accountDef;
  public:
    AccountHandler() { }
    AccountHandler(Connection* connection, IdlAccountDef accountDef) : connection(connection), accountDef(accountDef) { }
    AccountHandler(const AccountHandler& other) : connection(other.connection), accountDef(other.accountDef) { }
    AccountHandler& operator=(const AccountHandler& other) {
      connection = other.connection;
      accountDef = other.accountDef;
      return *this;
    }

    json decode(std::string b64) const {
      auto buffer = base64::decode(b64);
      json account = json::object();
      int offset = 0;
      // check discriminator
      // if (memcmp(buffer.data(), accountDef.name.c_str(), 8) != 0) {
      //   throw std::runtime_error("Invalid account discriminator");
      // }
      offset += 8;
      for (auto field : accountDef.type.fields) {
        account[field.name] = decode_field(field, buffer.data(), offset, buffer.size());
      }
      return account;
    }

    json fetch(PublicKey account) const {
      auto account_info = connection->get_account_info(account);
      if (account_info.ok()) {
        return decode(account_info.unwrap().data);
      }
      return nullptr;
    }

  private:
    json decode_field(IdlField field, uint8_t* buffer, int& offset, int length) const {
      switch (field.type) {
        case IdlType::boolean: {
          bool value = buffer[0] == 1;
          offset += 1;
          return value;
        }
        case IdlType::u8: {
          uint8_t value = buffer[offset];
          offset += 1;
          return value;
        }
        case IdlType::u16: {
          uint16_t value = (uint16_t)buffer[offset + 1] | ((uint16_t)buffer[offset] << 8);
          offset += 2;
          return value;
        }
        case IdlType::u32: {
          uint32_t value = (uint32_t)buffer[offset + 3] | ((uint32_t)buffer[offset + 2] << 8) | ((uint32_t)buffer[offset + 1] << 16) | ((uint32_t)buffer[offset] << 24);
          offset += 4;
          return value;
        }
        case IdlType::u64: {
          uint64_t value = (uint64_t)buffer[offset + 7] | ((uint64_t)buffer[offset + 6] << 8) | ((uint64_t)buffer[offset + 5] << 16) | ((uint64_t)buffer[offset + 4] << 24) | ((uint64_t)buffer[offset + 3] << 32) | ((uint64_t)buffer[offset + 2] << 40) | ((uint64_t)buffer[offset + 1] << 48) | ((uint64_t)buffer[offset] << 56);
          offset += 8;
          return value;
        }
        default: {
          throw std::runtime_error("Unsupported type");
        }
      }
    }
  };

  class InstructionHandler {
    IdlInstruction instruction;
    PublicKey program_id;
    std::string discriminator;
  public:
    InstructionHandler() { }
    InstructionHandler(IdlInstruction instruction, PublicKey program_id) : instruction(instruction), program_id(program_id) {
      std::vector<uint8_t> sighash(32);
      std::string preimage = "global:" + to_snake_case(instruction.name);
      crypto_hash_sha256(sighash.data(), (const unsigned char*)preimage.data(), preimage.size());
      this->discriminator = std::string(sighash.begin(), sighash.begin() + 8);
    }
    InstructionHandler(const InstructionHandler& other) : instruction(other.instruction), program_id(other.program_id), discriminator(other.discriminator) { }
    InstructionHandler& operator=(const InstructionHandler& other) {
      instruction = other.instruction;
      program_id = other.program_id;
      discriminator = other.discriminator;
      return *this;
    }

    Transaction::Message::Instruction operator()(json j) const {
      return {
        program_id,
        to_account_metas(j.contains("accounts") ? j["accounts"] : json::object()),
        encode(j.contains("data") ? j["data"] : json::array()),
      };
    }

  private:
    std::vector<uint8_t> encode(json data) const {
      std::vector<uint8_t> buffer;
      buffer.insert(buffer.end(), discriminator.begin(), discriminator.begin() + 8);
      for (int i = 0; i < instruction.args.size(); i++) {
        IdlField arg = instruction.args[i];
        switch (arg.type) {
          case IdlType::boolean: buffer.push_back(data[i].get<bool>() ? 1 : 0); break;
          case IdlType::u8: buffer.push_back(data[i].get<uint8_t>()); break;
          case IdlType::u16: {
            uint16_t v = data[i].get<uint16_t>();
            buffer.push_back(v >> 8);
            buffer.push_back(v & 0xff);
            break;
          }
          case IdlType::u32: {
            uint32_t v = data[i].get<uint32_t>();
            buffer.push_back(v >> 24);
            buffer.push_back((v >> 16) & 0xff);
            buffer.push_back((v >> 8) & 0xff);
            buffer.push_back(v & 0xff);
            break;
          }
          case IdlType::u64: {
            uint64_t v = data[i].get<uint64_t>();
            buffer.push_back(v >> 56);
            buffer.push_back((v >> 48) & 0xff);
            buffer.push_back((v >> 40) & 0xff);
            buffer.push_back((v >> 32) & 0xff);
            buffer.push_back((v >> 24) & 0xff);
            buffer.push_back((v >> 16) & 0xff);
            buffer.push_back((v >> 8) & 0xff);
            buffer.push_back(v & 0xff);
            break;
          }
          case IdlType::i8: buffer.push_back(data[i].get<int8_t>()); break;
          case IdlType::i16: {
            int16_t v = data[i].get<int16_t>();
            buffer.push_back(v >> 8);
            buffer.push_back(v & 0xff);
            break;
          }
          case IdlType::i32: {
            int32_t v = data[i].get<int32_t>();
            buffer.push_back(v >> 24);
            buffer.push_back((v >> 16) & 0xff);
            buffer.push_back((v >> 8) & 0xff);
            buffer.push_back(v & 0xff);
            break;
          }
          case IdlType::i64: {
            int64_t v = data[i].get<int64_t>();
            buffer.push_back(v >> 56);
            buffer.push_back((v >> 48) & 0xff);
            buffer.push_back((v >> 40) & 0xff);
            buffer.push_back((v >> 32) & 0xff);
            buffer.push_back((v >> 24) & 0xff);
            buffer.push_back((v >> 16) & 0xff);
            buffer.push_back((v >> 8) & 0xff);
            buffer.push_back(v & 0xff);
            break;
          }
          case IdlType::f32: {
            float v = data[i].get<float>();
            uint32_t u = *(uint32_t*)&v;
            buffer.push_back(u >> 24);
            buffer.push_back((u >> 16) & 0xff);
            buffer.push_back((u >> 8) & 0xff);
            buffer.push_back(u & 0xff);
            break;
          }
          case IdlType::f64: {
            double v = data[i].get<double>();
            uint64_t u = *(uint64_t*)&v;
            buffer.push_back(u >> 56);
            buffer.push_back((u >> 48) & 0xff);
            buffer.push_back((u >> 40) & 0xff);
            buffer.push_back((u >> 32) & 0xff);
            buffer.push_back((u >> 24) & 0xff);
            buffer.push_back((u >> 16) & 0xff);
            buffer.push_back((u >> 8) & 0xff);
            buffer.push_back(u & 0xff);
            break;
          }
          case IdlType::string: {
            std::string s = data[i].get<std::string>();
            buffer.push_back(s.size() >> 8);
            buffer.push_back(s.size() & 0xff);
            buffer.insert(buffer.end(), s.begin(), s.end());
            break;
          }
          default: throw std::runtime_error("unsupported type");
        }
      }
      return buffer;
    }

    json to_account_metas(json accounts) const {
      json account_metas = json::array();
      for (int i = 0; i < instruction.accounts.size(); i++) {
        const IdlAccount& account = instruction.accounts[i];
        account_metas.push_back({{"pubkey", accounts[to_camel_case(account.name)].get<PublicKey>().to_base58()}, {"isSigner", account.isSigner}, {"isWritable", account.isMut}});
      }
      return account_metas;
    }

    std::string to_camel_case(std::string str) const {
      std::string result;
      bool next_upper = false;
      for (auto c : str) {
        if (c == '_') {
          next_upper = true;
        } else if (next_upper) {
          result += c - 'a' + 'A';
          next_upper = false;
        } else {
          result += c;
        }
      }
      return result;
    }

    std::string to_snake_case(std::string str) const {
      std::string result;
      for (auto c : str) {
        if (c >= 'A' && c <= 'Z') {
          if (!result.empty()) {
            result += '_';
          }
          result += c - 'A' + 'a';
        } else {
          result += c;
        }
      }
      return result;
    }
  };

  class TransactionHandler {
    InstructionHandler instruction_handler;
  public:
    TransactionHandler() { }
    TransactionHandler(IdlInstruction& instruction, PublicKey program_id) : instruction_handler(instruction, program_id) { }
    TransactionHandler(const TransactionHandler& other) : instruction_handler(other.instruction_handler) { }

    TransactionHandler& operator=(const TransactionHandler& other) {
      instruction_handler = other.instruction_handler;
      return *this;
    }

    Transaction operator()(json::object_t j) const {
      Transaction tx;
      tx.add(instruction_handler(j));
      return tx;
    }
  };

  class RpcHandler {
    Connection* connection;
    Keypair* payer;
    TransactionHandler transaction_handler;
  public:
    RpcHandler() { }
    RpcHandler(Connection* connection, Keypair* payer, IdlInstruction& instruction, PublicKey program_id) : connection(connection), payer(payer), transaction_handler(instruction, program_id) { }
    RpcHandler(const RpcHandler& other) : connection(other.connection), payer(other.payer), transaction_handler(other.transaction_handler) { }

    std::string operator()(json::object_t j, std::vector<Keypair> signers = {}) const {
      Transaction transaction = transaction_handler(j);
      if (signers.size() == 0) {
        signers.push_back(*payer);
      }
      std::string txid = connection->sign_and_send_transaction(transaction, signers).unwrap();
      //TODO confirm the transaction.
      return txid;
    }
  };

  /**
   * @brief Provider holds a Solana connection and payer.
   */
  struct Provider {
    Connection* connection;
    Keypair* payer;
    Provider(Connection* connection, Keypair* payer) : connection(connection), payer(payer) { }
    Provider() = delete;
    Provider(const Provider& other) = delete;
    Provider& operator=(const Provider& other) = delete;
  };

  /**
   * @brief Program provides the IDL deserialized client representation of an Anchor program.
   */
  struct Program {
    Idl idl;
    PublicKey program_id;

    std::map<std::string, AccountHandler> account;
    std::map<std::string, InstructionHandler> instruction;
    std::map<std::string, RpcHandler> rpc;
    std::map<std::string, TransactionHandler> transaction;

    Program(Provider& provider, json idl, PublicKey program_id) : idl(idl), program_id(program_id) {
      for (auto& accountDef : this->idl.accounts) {
        account.emplace(accountDef.name, AccountHandler(provider.connection, accountDef));
      }
      for (auto& instruction : this->idl.instructions) {
        this->instruction.emplace(instruction.name, InstructionHandler(instruction, program_id));
        this->transaction.emplace(instruction.name, TransactionHandler(instruction, program_id));
        this->rpc.emplace(instruction.name, RpcHandler(provider.connection, provider.payer, instruction, program_id));
      }
    }

    /**
     * @brief Load a program from an IDL file.
     *
     * @param idlPath
     * @param programId
     * @return Program
     */
    static Program load(Provider& provider, std::string idlPath, PublicKey programId) {
      std::ifstream idlFile(idlPath);
      json idl;
      idlFile >> idl;
      return Program(provider, idl, programId);
    }
  };

}
