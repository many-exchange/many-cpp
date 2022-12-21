#pragma once

namespace solana {

  //TODO
  /*
/**
 * @internal
 */
/*
export type InstructionType = {
  // The Instruction index (from solana upstream program)
  index: number;
  // The BufferLayout to use to build data
  layout: BufferLayout.Layout;
};
 */

/**
 * Populate a buffer of instruction data using an InstructionType
 * @internal
 */
/*
export function encodeData(type: InstructionType, fields?: any): Buffer {
  const allocLength =
    type.layout.span >= 0 ? type.layout.span : Layout.getAlloc(type, fields);
  const data = Buffer.alloc(allocLength);
  const layoutFields = Object.assign({instruction: type.index}, fields);
  type.layout.encode(layoutFields, data);
  return data;
}
  */

/**
 * Decode instruction data buffer using an InstructionType
 * @internal
 */
/*
export function decodeData(type: InstructionType, buffer: Buffer): any {
  let data;
  try {
    data = type.layout.decode(buffer);
  } catch (err) {
    throw new Error('invalid instruction; ' + err);
  }

  if (data.instruction !== type.index) {
    throw new Error(
      `invalid instruction; instruction index mismatch ${data.instruction} != ${type.index}`,
    );
  }

  return data;
}
  */


  /*
/**
 * Factory class for transactions to interact with the System program
 */
/*
export class SystemProgram {
*/

  /**
   * Public key that identifies the System program
   */
  /*
  static programId: PublicKey = new PublicKey(
    '11111111111111111111111111111111',
  );
  */

  /**
   * Generate a transaction instruction that creates a new account
   */
  /*
  static createAccount(params: CreateAccountParams): TransactionInstruction {
    const type = SYSTEM_INSTRUCTION_LAYOUTS.Create;
    const data = encodeData(type, {
      lamports: params.lamports,
      space: params.space,
      programId: toBuffer(params.programId.toBuffer()),
    });

    return new TransactionInstruction({
      keys: [
        {pubkey: params.fromPubkey, isSigner: true, isWritable: true},
        {pubkey: params.newAccountPubkey, isSigner: true, isWritable: true},
      ],
      programId: this.programId,
      data,
    });
  }
  */

  /**
   * Generate a transaction instruction that transfers lamports from one account to another
   */
  /*
  static transfer(
    params: TransferParams | TransferWithSeedParams,
  ): TransactionInstruction {
    let data;
    let keys;
    if ('basePubkey' in params) {
      const type = SYSTEM_INSTRUCTION_LAYOUTS.TransferWithSeed;
      data = encodeData(type, {
        lamports: params.lamports,
        seed: params.seed,
        programId: toBuffer(params.programId.toBuffer()),
      });
      keys = [
        {pubkey: params.fromPubkey, isSigner: false, isWritable: true},
        {pubkey: params.basePubkey, isSigner: true, isWritable: false},
        {pubkey: params.toPubkey, isSigner: false, isWritable: true},
      ];
    } else {
      const type = SYSTEM_INSTRUCTION_LAYOUTS.Transfer;
      data = encodeData(type, {lamports: params.lamports});
      keys = [
        {pubkey: params.fromPubkey, isSigner: true, isWritable: true},
        {pubkey: params.toPubkey, isSigner: false, isWritable: true},
      ];
    }

    return new TransactionInstruction({
      keys,
      programId: this.programId,
      data,
    });
  }
  */

  /**
   * Generate a transaction instruction that assigns an account to a program
   */
  /*
  static assign(
    params: AssignParams | AssignWithSeedParams,
  ): TransactionInstruction {
    let data;
    let keys;
    if ('basePubkey' in params) {
      const type = SYSTEM_INSTRUCTION_LAYOUTS.AssignWithSeed;
      data = encodeData(type, {
        base: toBuffer(params.basePubkey.toBuffer()),
        seed: params.seed,
        programId: toBuffer(params.programId.toBuffer()),
      });
      keys = [
        {pubkey: params.accountPubkey, isSigner: false, isWritable: true},
        {pubkey: params.basePubkey, isSigner: true, isWritable: false},
      ];
    } else {
      const type = SYSTEM_INSTRUCTION_LAYOUTS.Assign;
      data = encodeData(type, {
        programId: toBuffer(params.programId.toBuffer()),
      });
      keys = [{pubkey: params.accountPubkey, isSigner: true, isWritable: true}];
    }

    return new TransactionInstruction({
      keys,
      programId: this.programId,
      data,
    });
  }
  */

  /**
   * Generate a transaction instruction that creates a new account at
   *   an address generated with `from`, a seed, and programId
   */
  /*
  static createAccountWithSeed(
    params: CreateAccountWithSeedParams,
  ): TransactionInstruction {
    const type = SYSTEM_INSTRUCTION_LAYOUTS.CreateWithSeed;
    const data = encodeData(type, {
      base: toBuffer(params.basePubkey.toBuffer()),
      seed: params.seed,
      lamports: params.lamports,
      space: params.space,
      programId: toBuffer(params.programId.toBuffer()),
    });
    let keys = [
      {pubkey: params.fromPubkey, isSigner: true, isWritable: true},
      {pubkey: params.newAccountPubkey, isSigner: false, isWritable: true},
    ];
    if (params.basePubkey != params.fromPubkey) {
      keys.push({pubkey: params.basePubkey, isSigner: true, isWritable: false});
    }

    return new TransactionInstruction({
      keys,
      programId: this.programId,
      data,
    });
  }
  */

  /**
   * Generate a transaction that creates a new Nonce account
   */
  /*
  static createNonceAccount(
    params: CreateNonceAccountParams | CreateNonceAccountWithSeedParams,
  ): Transaction {
    const transaction = new Transaction();
    if ('basePubkey' in params && 'seed' in params) {
      transaction.add(
        SystemProgram.createAccountWithSeed({
          fromPubkey: params.fromPubkey,
          newAccountPubkey: params.noncePubkey,
          basePubkey: params.basePubkey,
          seed: params.seed,
          lamports: params.lamports,
          space: NONCE_ACCOUNT_LENGTH,
          programId: this.programId,
        }),
      );
    } else {
      transaction.add(
        SystemProgram.createAccount({
          fromPubkey: params.fromPubkey,
          newAccountPubkey: params.noncePubkey,
          lamports: params.lamports,
          space: NONCE_ACCOUNT_LENGTH,
          programId: this.programId,
        }),
      );
    }

    const initParams = {
      noncePubkey: params.noncePubkey,
      authorizedPubkey: params.authorizedPubkey,
    };

    transaction.add(this.nonceInitialize(initParams));
    return transaction;
  }
  */

  /**
   * Generate an instruction to initialize a Nonce account
   */
  /*
  static nonceInitialize(
    params: InitializeNonceParams,
  ): TransactionInstruction {
    const type = SYSTEM_INSTRUCTION_LAYOUTS.InitializeNonceAccount;
    const data = encodeData(type, {
      authorized: toBuffer(params.authorizedPubkey.toBuffer()),
    });
    const instructionData = {
      keys: [
        {pubkey: params.noncePubkey, isSigner: false, isWritable: true},
        {
          pubkey: SYSVAR_RECENT_BLOCKHASHES_PUBKEY,
          isSigner: false,
          isWritable: false,
        },
        {pubkey: SYSVAR_RENT_PUBKEY, isSigner: false, isWritable: false},
      ],
      programId: this.programId,
      data,
    };
    return new TransactionInstruction(instructionData);
  }
  */

  /**
   * Generate an instruction to advance the nonce in a Nonce account
   */
  /*
  static nonceAdvance(params: AdvanceNonceParams): TransactionInstruction {
    const type = SYSTEM_INSTRUCTION_LAYOUTS.AdvanceNonceAccount;
    const data = encodeData(type);
    const instructionData = {
      keys: [
        {pubkey: params.noncePubkey, isSigner: false, isWritable: true},
        {
          pubkey: SYSVAR_RECENT_BLOCKHASHES_PUBKEY,
          isSigner: false,
          isWritable: false,
        },
        {pubkey: params.authorizedPubkey, isSigner: true, isWritable: false},
      ],
      programId: this.programId,
      data,
    };
    return new TransactionInstruction(instructionData);
  }
  */

  /**
   * Generate a transaction instruction that withdraws lamports from a Nonce account
   */
  /*
  static nonceWithdraw(params: WithdrawNonceParams): TransactionInstruction {
    const type = SYSTEM_INSTRUCTION_LAYOUTS.WithdrawNonceAccount;
    const data = encodeData(type, {lamports: params.lamports});

    return new TransactionInstruction({
      keys: [
        {pubkey: params.noncePubkey, isSigner: false, isWritable: true},
        {pubkey: params.toPubkey, isSigner: false, isWritable: true},
        {
          pubkey: SYSVAR_RECENT_BLOCKHASHES_PUBKEY,
          isSigner: false,
          isWritable: false,
        },
        {
          pubkey: SYSVAR_RENT_PUBKEY,
          isSigner: false,
          isWritable: false,
        },
        {pubkey: params.authorizedPubkey, isSigner: true, isWritable: false},
      ],
      programId: this.programId,
      data,
    });
  }
  */

  /**
   * Generate a transaction instruction that authorizes a new PublicKey as the authority
   * on a Nonce account.
   */
  /*
  static nonceAuthorize(params: AuthorizeNonceParams): TransactionInstruction {
    const type = SYSTEM_INSTRUCTION_LAYOUTS.AuthorizeNonceAccount;
    const data = encodeData(type, {
      authorized: toBuffer(params.newAuthorizedPubkey.toBuffer()),
    });

    return new TransactionInstruction({
      keys: [
        {pubkey: params.noncePubkey, isSigner: false, isWritable: true},
        {pubkey: params.authorizedPubkey, isSigner: true, isWritable: false},
      ],
      programId: this.programId,
      data,
    });
  }
  */

  /**
   * Generate a transaction instruction that allocates space in an account without funding
   */
  /*
  static allocate(
    params: AllocateParams | AllocateWithSeedParams,
  ): TransactionInstruction {
    let data;
    let keys;
    if ('basePubkey' in params) {
      const type = SYSTEM_INSTRUCTION_LAYOUTS.AllocateWithSeed;
      data = encodeData(type, {
        base: toBuffer(params.basePubkey.toBuffer()),
        seed: params.seed,
        space: params.space,
        programId: toBuffer(params.programId.toBuffer()),
      });
      keys = [
        {pubkey: params.accountPubkey, isSigner: false, isWritable: true},
        {pubkey: params.basePubkey, isSigner: true, isWritable: false},
      ];
    } else {
      const type = SYSTEM_INSTRUCTION_LAYOUTS.Allocate;
      data = encodeData(type, {
        space: params.space,
      });
      keys = [{pubkey: params.accountPubkey, isSigner: true, isWritable: true}];
    }

    return new TransactionInstruction({
      keys,
      programId: this.programId,
      data,
    });
  }
}
  */

}
