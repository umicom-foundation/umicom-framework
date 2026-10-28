# Federated Evidence Anchors — Native Framework Learning Laboratory

This laboratory composes the canonical Framework `UmiSha256`, `umi_record_chain_*` and `umi_record_chain_merkle_*` services. It does not implement its own SHA-256, Merkle algorithm, ledger consensus or numerical training engine. The source is written in C23 and uses exact-size canonical byte formats.

A coordinator that has already **authorised, evaluated and accepted** a group of worker contribution receipts can commit their ordered SHA-256 digests. `FederatedEvidenceAnchorCommit` builds a count-bound Merkle root and embeds it in a fixed 160-byte record-chain payload containing a round ID, submitted and accepted counts, base checkpoint digest, dataset-policy digest and held-out evaluation digest. These fields are *claims* bound to bytes, not self-proving decisions. Separate, independent authorisation and quality review are essential.

`FederatedEvidenceAnchorProve` creates an inclusion witness for one digest. `FederatedEvidenceAnchorVerify` checks the witness and a complete ordered record-chain step from an independently trusted prior checkpoint to an independently trusted expected tip. A matching root from the untrusted record itself is insufficient to establish trust: the expected chain checkpoint must be distributed by an authenticated, independent mechanism.

## Learning tasks

1. Compare a three-leaf proof against a four-leaf proof and explain why leaf counts are bound to roots.
2. Change the held-out evaluation digest in a committed payload and observe rejection.
3. Replace one sibling hash in a proof and demonstrate that the anchored root no longer verifies.
4. Explain why an authenticated record chain is still not a Byzantine-fault-tolerant consensus ledger.
5. Analyse privacy: public hashes of guessable sensitive records can leak membership by dictionary attack.

The standalone CMake project compiles the **existing Framework source files** for SHA-256 and ordered chain and the previously added Merkle implementation alongside this additive component. Existing Python/PowerShell teaching alternatives are not modified.

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT
