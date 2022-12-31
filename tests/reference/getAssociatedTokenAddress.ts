import { Keypair, PublicKey } from "@solana/web3.js";
import { getAssociatedTokenAddressSync } from "@solana/spl-token";
import assert from "assert";

(async () => {

  const keypair = Keypair.generate();
  console.log(`keypair.publicKey = ${keypair.publicKey.toBase58()}`);

  const owner = keypair.publicKey;
  const programId = new PublicKey("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA");
  const mint = new PublicKey("So11111111111111111111111111111111111111112");
  const associatedTokenProgramId = new PublicKey("ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL");

  const associatedTokenAddress = getAssociatedTokenAddressSync(mint, owner);
  console.log("Associated token address: ", associatedTokenAddress.toBase58());

})();
