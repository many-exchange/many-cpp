#pragma once

#include "solana.hpp"

namespace solana {

  namespace token {

    /**
     * Returns an Instruction to create an Associated Token Account
     *
     * @param payer Payer of the initialization fees
     * @param associatedToken New associated token account
     * @param owner Owner of the new account
     * @param mint Token mint account
     * @param programId SPL Token program account
     * @param associatedTokenProgramId SPL Associated Token program account
     */
    Transaction::Message::Instruction create_associated_token_account_instruction(
      const PublicKey& payer,
      const PublicKey& associatedToken,
      const PublicKey& owner,
      const PublicKey& mint,
      const PublicKey& programId = TOKEN_PROGRAM_ID,
      const PublicKey& associatedTokenProgramId = ASSOCIATED_TOKEN_PROGRAM_ID
    ) {
      json accounts = {
        {
          { "pubkey", payer.to_base58() },
          { "isSigner", true },
          { "isWritable", true },
        },
        {
          { "pubkey", associatedToken.to_base58() },
          { "isSigner", false },
          { "isWritable", true },
        },
        {
          { "pubkey", owner.to_base58() },
          { "isSigner", false },
          { "isWritable", false },
        },
        {
          { "pubkey", mint.to_base58() },
          { "isSigner", false },
          { "isWritable", false },
        },
        {
          { "pubkey", SYSTEM_PROGRAM.to_base58() },
          { "isSigner", false },
          { "isWritable", false },
        },
        {
          { "pubkey", programId.to_base58() },
          { "isSigner", false },
          { "isWritable", false },
        },
        {
          { "pubkey", associatedTokenProgramId.to_base58() },
          { "isSigner", false },
          { "isWritable", false },
        },
      };

      return {
        programId,
        accounts,
        {}
      };
    }

    /**
     * Returns a Transaction to create an Associated Token Account
     *
     * @param payer Payer of the initialization fees
     * @param associatedToken New associated token account
     * @param owner Owner of the new account
     * @param mint Token mint account
     * @param programId SPL Token program account
     * @param associatedTokenProgramId SPL Associated Token program account
     */
    Transaction create_associated_token_account_transaction(
      const PublicKey& payer,
      const PublicKey& associatedToken,
      const PublicKey& owner,
      const PublicKey& mint,
      const PublicKey& programId = TOKEN_PROGRAM_ID,
      const PublicKey& associatedTokenProgramId = ASSOCIATED_TOKEN_PROGRAM_ID
    ) {
      Transaction tx;
      tx.add(
        create_associated_token_account_instruction(
          payer,
          associatedToken,
          owner,
          mint,
          programId,
          associatedTokenProgramId
        )
      );

      return tx;
    }

    /**
     * Get the address of the associated token account for a given mint and owner
     *
     * @param mint                     Token mint account
     * @param owner                    Owner of the new account
     * @param allowOwnerOffCurve       Allow the owner account to be a PDA (Program Derived Address)
     * @param programId                SPL Token program account
     * @param associatedTokenProgramId SPL Associated Token program account
     */
    PublicKey get_associated_token_address(
      const PublicKey& mint,
      const PublicKey& owner,
      const bool& allowOwnerOffCurve = false,
      const PublicKey& programId = TOKEN_PROGRAM_ID,
      const PublicKey& associatedTokenProgramId = ASSOCIATED_TOKEN_PROGRAM_ID
    ) {
      if (!allowOwnerOffCurve && !owner.is_on_curve()) {
        throw std::runtime_error("Token owner is off curve.");
      }

      std::tuple<PublicKey, uint8_t> pda = PublicKey::find_program_address(
        {
          owner.to_buffer(),
          programId.to_buffer(),
          mint.to_buffer()
        },
        associatedTokenProgramId
      );

      return std::get<0>(pda);
    }

    /**
    * Create and initialize a new associated token account
    *
    * @param connection Connection to use
    * @param payer Payer of the transaction and initialization fees
    * @param mint Mint for the account
    * @param owner Owner of the new account
    * @param confirmOptions Options for confirming the transaction
    * @param programId SPL Token program account
    * @param associatedTokenProgramId SPL Associated Token program account
    */
    PublicKey create_associated_token_account(
      const Connection& connection,
      const Keypair& payer,
      const PublicKey& mint,
      const PublicKey& owner,
      const ConfirmOptions& confirmOptions = {},
      const PublicKey& programId = TOKEN_PROGRAM_ID,
      const PublicKey& associatedTokenProgramId = ASSOCIATED_TOKEN_PROGRAM_ID
    ) {
      const PublicKey associatedToken = get_associated_token_address(mint, owner, false, programId, associatedTokenProgramId);

      Transaction tx = create_associated_token_account_transaction(
        payer.publicKey,
        associatedToken,
        owner,
        mint,
        programId,
        associatedTokenProgramId
      );

      std::string signedTx = connection.sign_transaction(tx, {payer});
      SendTransactionResult result = connection.send_transaction(signedTx);

      return associatedToken;
    }

  }

}
