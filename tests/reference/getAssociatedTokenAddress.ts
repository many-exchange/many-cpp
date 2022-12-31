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


// (async () => {

//   let publicKey = new PublicKey("7shNxW9FhomrTpYVzFuUFqxCtGrY7t4g6AmeU2TAupif");
//   console.log(`${publicKey.toBase58()} = ${PublicKey.isOnCurve(publicKey.toBuffer())}`);

//   publicKey = new PublicKey("GhWpf4AyL2NWNdSuaCDDyt2csTs6HzgpgFXrRNbY6C1B");
//   console.log(`${publicKey.toBase58()} = ${PublicKey.isOnCurve(publicKey.toBuffer())}`);

//   publicKey = new PublicKey("FPmgj6Ty6LQCv65ejJdfB3w5SgChxbwHjUvPqW3FQFUK");
//   console.log(`${publicKey.toBase58()} = ${PublicKey.isOnCurve(publicKey.toBuffer())}`);

//   publicKey = new PublicKey("2eQzsJbQnDYzRvNqwoBJeXfBeWsrbNfa1pXdNbe1JBS8");
//   console.log(`${publicKey.toBase58()} = ${PublicKey.isOnCurve(publicKey.toBuffer())}`);

//   publicKey = new PublicKey("5MdBSu6D5CNrocD4sETfsFfDrQ4HjyahaWVsp5D1eJK2");
//   console.log(`${publicKey.toBase58()} = ${PublicKey.isOnCurve(publicKey.toBuffer())}`);

//   publicKey = new PublicKey("HsF8NnhYGXFBC5bfNdD1TYfJUj8fCU72qgxLQvhcaFYj");
//   console.log(`${publicKey.toBase58()} = ${PublicKey.isOnCurve(publicKey.toBuffer())}`);

//   publicKey = new PublicKey("ApMFHdM1fohWjMpYEWWRsbxGJDFL3FqA6oe9GEGa8dbQ");
//   console.log(`${publicKey.toBase58()} = ${PublicKey.isOnCurve(publicKey.toBuffer())}`);

// })();

