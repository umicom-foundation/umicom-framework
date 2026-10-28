# Federated audit records: portable C23 laboratory

**Sammy Hegab · Umicom Foundation · MIT**

A cryptographic hash-linked audit trail is a useful building block for training governance, but it is not a distributed consensus mechanism. This laboratory uses the **existing** `umicom/record_chain/chain.h` service and its canonical SHA-256 implementation. It does not create a cryptocurrency, validate model weights, authenticate a worker, implement proof-of-stake or assert on-chain finality.

## Learning route

1. Start a chain with a nonzero public chain identity.
2. Canonically encode a 224-byte public research decision containing round, worker sequence, accepted/rejected/quarantined outcome and six 32-byte commitments.
3. Append using `umi_record_chain_append` to obtain a committed record and new checkpoint.
4. Persist the corresponding payload **separately** through an authorised storage service. Independently anchor the expected final checkpoint; otherwise an attacker controlling the entire storage history can replace the chain and its tip.
5. Verify the ordered records with `umi_record_chain_verify`, including an independently trusted expected tip.
6. Test mutation, truncation, reorder and stale checkpoint rejection.

All identities, hashes and counts in the demonstration are **synthetic**. A content hash identifies bytes, not a copyright permission, authenticated identity, privacy guarantee or measured model improvement. Hashes of guessable sensitive data may themselves disclose information.

## Portable isolated build

From this directory, using CMake and a supported native C23 compiler:

```text
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

This isolated CMake project composes the Framework's original SHA-256 and record-chain sources under the existing include hierarchy. For a full Framework SDK integration, the normal build must supply `Umicom::record_chain` and include `cmake/UmicomAiFederatedAuditChain.cmake` as an opt-in component.

## Conceptual comparison

| Property | Local record chain | Permissioned distributed ledger | Public proof-of-work/stake blockchain |
|---|---|---|---|
| Hash-linked history | Yes | Often | Yes |
| Multiple independent administrators | Not inherently | Yes | Yes |
| Network consensus | No | Yes | Yes |
| Authenticated contributor policy | Separate | Governed membership | Protocol-specific |
| Proof of useful model training | No | No | No |

See NISTIR 8202, the Bitcoin whitepaper, Ethereum proof-of-stake documentation and Hyperledger Fabric ordering-service documentation for independently published descriptions of these architectures.
