/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/record_chain/merkle.h
 *
 * PURPOSE:
 *   Compute bounded domain-separated Merkle commitments and inclusion proofs
 *   for pre-hashed record digests, using the canonical Framework SHA-256.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RECORD_CHAIN_MERKLE_H
#define UMICOM_RECORD_CHAIN_MERKLE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_RECORD_CHAIN_MERKLE_DIGEST_BYTES 32U
#define UMI_RECORD_CHAIN_MERKLE_MAX_LEAVES 1024U
#define UMI_RECORD_CHAIN_MERKLE_MAX_LEVELS 10U

typedef struct UmiRecordChainMerkleProof {
    size_t leafIndex;
    size_t leafCount;
    size_t siblingCount;
    uint8_t siblings[UMI_RECORD_CHAIN_MERKLE_MAX_LEVELS][32];
} UmiRecordChainMerkleProof;

/* Input: exactly leafCount contiguous 32-byte, caller-owned digests (for
 * example the digest in each already-validated UmiRecordChainRecord).
 * SHA-256 domains separate leaves, internal nodes and roots. Odd node counts
 * duplicate their final node at that level, and the root binds leafCount in
 * big-endian bytes to distinguish a three-leaf list from a four-leaf list.
 * 1 <= leafCount <= 1024. Outputs are never written after a rejected call.
 * This is content inclusion only, NOT ledger consensus or model validation. */
UmiStatus umi_record_chain_merkle_root(
    const uint8_t *leafDigests, size_t leafCount,
    uint8_t outRoot[UMI_RECORD_CHAIN_MERKLE_DIGEST_BYTES]);

/* Build one bounded membership proof and its corresponding root together.
 * The output buffers must be disjoint from each other and from the input.
 * No heap allocation, process, network, signer, mining or consensus service. */
UmiStatus umi_record_chain_merkle_prove(
    const uint8_t *leafDigests, size_t leafCount, size_t leafIndex,
    UmiRecordChainMerkleProof *outProof,
    uint8_t outRoot[UMI_RECORD_CHAIN_MERKLE_DIGEST_BYTES]);

/* Check membership against an INDEPENDENTLY trusted 32-byte root. A malformed
 * proof is rejected; a well-formed nonmatching proof returns INVALID_STATE.
 * This does not establish the origin, authorisation or truth of a leaf. */
UmiStatus umi_record_chain_merkle_verify(
    const uint8_t leafDigest[UMI_RECORD_CHAIN_MERKLE_DIGEST_BYTES],
    const UmiRecordChainMerkleProof *proof,
    const uint8_t expectedRoot[UMI_RECORD_CHAIN_MERKLE_DIGEST_BYTES]);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_RECORD_CHAIN_MERKLE_H */
