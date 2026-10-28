# Native Merkle membership over Umicom record digests

This standalone laboratory compiles the **actual** Umicom Framework `src/record_chain/merkle.c` and **existing** `src/base/sha256.c`; it does not contain an alternative hash implementation. Its binary32-byte leaves can be independently checked `UmiRecordChainRecord.digest` values or other already-hashed public data. The leaf domain, node domain and root domain are distinct SHA-256 inputs. The root binds the exact number of leaves, preventing a three-leaf batch from being confused with a four-leaf batch containing a duplicated final item.

The demonstration proves the inclusion of the last member in a five-leaf set and rejects a modified leaf. The regression tests additionally cover singletons, exact powers of two, odd counts, the 1,024-leaf limit, malformed proofs and publication atomicity. All buffers have explicit caller ownership, and implementation memory is bounded to a 32 KiB working array.

This provides **integrity evidence**, not permissionless consensus, actor authentication, independent finality, a live blockchain, a digital signature, or statistical verification that training helped a model. Consumers must independently authenticate/authorise contributors and anchor the expected root. Never publish confidential model deltas, data shards, bank transactions or predictable private identifiers just because a digest can be computed.

The source is MIT licensed; Sammy Hegab / Umicom Foundation.
