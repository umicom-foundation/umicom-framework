/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/record_chain/merkle.c
 *
 * PURPOSE:
 *   Build and verify finite Merkle membership evidence by composing the
 *   existing Framework SHA-256, with no competing hashing implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/record_chain/merkle.h"
#include "umicom/base/sha256.h"

#include <stdint.h>
#include <string.h>

static const char leafDomain[] = "UMICOM-MERKLE-LEAF:1\n";
static const char nodeDomain[] = "UMICOM-MERKLE-NODE:1\n";
static const char rootDomain[] = "UMICOM-MERKLE-ROOT:1\n";

/* This conservative address-range guard rejects output/input aliasing before
 * any read or write, and refuses wrapped uintptr_t ranges. */
static int Intersects(const void *a, size_t aSize, const void *b, size_t bSize)
{
    uintptr_t x = (uintptr_t)a, y = (uintptr_t)b;
    if (x > UINTPTR_MAX - aSize || y > UINTPTR_MAX - bSize) return 1;
    return x < y + bSize && y < x + aSize;
}

static UmiStatus HashParts(const char *domain, size_t domainBytes,
                           const void *first, size_t firstBytes,
                           const void *second, size_t secondBytes,
                           uint8_t output[32])
{
    UmiSha256 sha;
    UmiStatus status;
    UmiSha256Init(&sha);
    status = UmiSha256Update(&sha, domain, domainBytes);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&sha, first, firstBytes);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Update(&sha, second, secondBytes);
    if (status == UMI_STATUS_OK)
        status = UmiSha256Final(&sha, output);
    return status;
}

static UmiStatus HashLeaf(const uint8_t digest[32], uint8_t out[32])
{
    return HashParts(leafDomain, sizeof leafDomain - 1U,
                     digest, 32U, NULL, 0U, out);
}

static UmiStatus HashNode(const uint8_t left[32], const uint8_t right[32],
                          uint8_t out[32])
{
    return HashParts(nodeDomain, sizeof nodeDomain - 1U,
                     left, 32U, right, 32U, out);
}

static UmiStatus HashRoot(size_t leafCount, const uint8_t node[32],
                          uint8_t out[32])
{
    uint8_t countBytes[8];
    uint64_t count = (uint64_t)leafCount;
    for (size_t i = 0U; i < 8U; ++i) {
        countBytes[7U - i] = (uint8_t)(count & UINT64_C(255));
        count >>= 8U;
    }
    return HashParts(rootDomain, sizeof rootDomain - 1U,
                     countBytes, sizeof countBytes, node, 32U, out);
}

static int InputsValid(const uint8_t *digests, size_t count)
{
    return digests != NULL && count > 0U &&
           count <= UMI_RECORD_CHAIN_MERKLE_MAX_LEAVES;
}

/* All intermediate layers fit in 32 KiB of stack memory by contract.
 * In-place parent construction is safe: parent i overwrites only child i,
 * which has already been consumed by the time it is written. */
static UmiStatus Build(const uint8_t *digests, size_t count,
                       size_t proofIndex, UmiRecordChainMerkleProof *proof,
                       uint8_t root[32])
{
    uint8_t nodes[UMI_RECORD_CHAIN_MERKLE_MAX_LEAVES][32];
    size_t cursor = proofIndex;
    for (size_t i = 0U; i < count; ++i) {
        UmiStatus status = HashLeaf(digests + 32U * i, nodes[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    while (count > 1U) {
        if (proof != NULL) {
            const size_t sibling = (cursor ^ (size_t)1U) < count
                ? (cursor ^ (size_t)1U) : cursor;
            memcpy(proof->siblings[proof->siblingCount], nodes[sibling], 32U);
            ++proof->siblingCount;
            cursor /= 2U;
        }
        size_t nextCount = (count + 1U) / 2U;
        for (size_t i = 0U; i < nextCount; ++i) {
            size_t left = i * 2U;
            size_t right = left + 1U < count ? left + 1U : left;
            uint8_t candidate[32];
            UmiStatus status = HashNode(nodes[left], nodes[right], candidate);
            if (status != UMI_STATUS_OK) return status;
            memcpy(nodes[i], candidate, sizeof candidate);
        }
        count = nextCount;
    }
    return HashRoot(proof != NULL ? proof->leafCount : proofIndex,
                    nodes[0], root);
}

UmiStatus umi_record_chain_merkle_root(
    const uint8_t *leafDigests, size_t leafCount, uint8_t outRoot[32])
{
    uint8_t candidate[32];
    UmiStatus status;
    if (!InputsValid(leafDigests, leafCount) || outRoot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (Intersects(leafDigests, 32U * leafCount, outRoot, 32U))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = Build(leafDigests, leafCount, leafCount, NULL, candidate);
    if (status == UMI_STATUS_OK) memcpy(outRoot, candidate, 32U);
    return status;
}

UmiStatus umi_record_chain_merkle_prove(
    const uint8_t *leafDigests, size_t leafCount, size_t leafIndex,
    UmiRecordChainMerkleProof *outProof, uint8_t outRoot[32])
{
    UmiRecordChainMerkleProof candidate = {0};
    uint8_t candidateRoot[32];
    UmiStatus status;
    if (!InputsValid(leafDigests, leafCount) || leafIndex >= leafCount ||
        outProof == NULL || outRoot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (Intersects(leafDigests, 32U * leafCount, outProof, sizeof *outProof) ||
        Intersects(leafDigests, 32U * leafCount, outRoot, 32U) ||
        Intersects(outRoot, 32U, outProof, sizeof *outProof))
        return UMI_STATUS_INVALID_ARGUMENT;
    candidate.leafIndex = leafIndex;
    candidate.leafCount = leafCount;
    status = Build(leafDigests, leafCount, leafIndex, &candidate, candidateRoot);
    if (status != UMI_STATUS_OK) return status;
    *outProof = candidate;
    memcpy(outRoot, candidateRoot, 32U);
    return UMI_STATUS_OK;
}

UmiStatus umi_record_chain_merkle_verify(
    const uint8_t leafDigest[32], const UmiRecordChainMerkleProof *proof,
    const uint8_t expectedRoot[32])
{
    uint8_t current[32], candidate[32], boundRoot[32];
    size_t count, index, levels = 0U;
    UmiStatus status;
    if (leafDigest == NULL || proof == NULL || expectedRoot == NULL ||
        proof->leafCount == 0U ||
        proof->leafCount > UMI_RECORD_CHAIN_MERKLE_MAX_LEAVES ||
        proof->leafIndex >= proof->leafCount) return UMI_STATUS_INVALID_ARGUMENT;
    count = proof->leafCount;
    while (count > 1U) { count = (count + 1U) / 2U; ++levels; }
    if (proof->siblingCount != levels) return UMI_STATUS_INVALID_ARGUMENT;
    /* Canonical proof does not carry arbitrary unused bytes. */
    for (size_t i = levels; i < UMI_RECORD_CHAIN_MERKLE_MAX_LEVELS; ++i)
        for (size_t j = 0U; j < 32U; ++j)
            if (proof->siblings[i][j] != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = HashLeaf(leafDigest, current);
    if (status != UMI_STATUS_OK) return status;
    count = proof->leafCount;
    index = proof->leafIndex;
    for (size_t level = 0U; level < levels; ++level) {
        const uint8_t *sibling = proof->siblings[level];
        /* Duplicate-last is deterministic, so a wrong duplicated sibling is
         * not an acceptable alternate proof for the same root/count. */
        if ((index ^ (size_t)1U) >= count &&
            memcmp(sibling, current, 32U) != 0)
            return UMI_STATUS_INVALID_STATE;
        if ((index & (size_t)1U) == 0U)
            status = HashNode(current, sibling, candidate);
        else
            status = HashNode(sibling, current, candidate);
        if (status != UMI_STATUS_OK) return status;
        memcpy(current, candidate, 32U);
        index /= 2U;
        count = (count + 1U) / 2U;
    }
    status = HashRoot(proof->leafCount, current, boundRoot);
    if (status != UMI_STATUS_OK) return status;
    return memcmp(boundRoot, expectedRoot, 32U) == 0
        ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
