/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/record_chain/test_merkle.c
 *
 * PURPOSE:
 *   Exercise Merkle inclusion, odd-leaf canonicalisation, root/count binding,
 *   mutation rejection, full bounds and failure-atomic publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/record_chain/merkle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr,"Merkle test failure at %d: %s\n", __LINE__, #x); \
    return 1; } } while (0)

static int ExerciseCount(size_t count)
{
    uint8_t *leaves = calloc(count, 32U);
    uint8_t root[32] = {0}, root2[32] = {0};
    UmiRecordChainMerkleProof proof = {0};
    CHECK(leaves != NULL);
    for (size_t i = 0U; i < count * 32U; ++i)
        leaves[i] = (uint8_t)((i * 17U + 9U) & 255U);
    CHECK(umi_record_chain_merkle_root(leaves, count, root) == UMI_STATUS_OK);
    if (count == 3U) {
        char hex[65];
        static const char alphabet[] = "0123456789abcdef";
        for (size_t j = 0U; j < 32U; ++j) {
            hex[2U * j] = alphabet[root[j] >> 4U];
            hex[2U * j + 1U] = alphabet[root[j] & 15U];
        }
        hex[64] = '\0';
        /* Independently calculated SHA-256 reference for exactly three leaves. */
        CHECK(strcmp(hex,
          "3593fad2b970cc8f681de35521c0c061d6628693e9ba3908f373f8d2ad14f10b") == 0);
    }
    for (size_t i = 0U; i < count; ++i) {
        CHECK(umi_record_chain_merkle_prove(leaves, count, i, &proof, root2) == UMI_STATUS_OK);
        CHECK(memcmp(root, root2, 32U) == 0);
        CHECK(umi_record_chain_merkle_verify(leaves + i * 32U, &proof, root) == UMI_STATUS_OK);
        uint8_t corrupted[32];
        memcpy(corrupted, leaves + i * 32U, 32U);
        corrupted[0] ^= 1U;
        CHECK(umi_record_chain_merkle_verify(corrupted, &proof, root) == UMI_STATUS_INVALID_STATE);
    }
    /* Root must bind the declared leaf count, including duplicate-last. */
    if (count == 3U) {
        uint8_t extended[128];
        memcpy(extended, leaves, 96U);
        memcpy(extended + 96U, leaves + 64U, 32U);
        CHECK(umi_record_chain_merkle_root(extended, 4U, root2) == UMI_STATUS_OK);
        CHECK(memcmp(root, root2, 32U) != 0);
        CHECK(umi_record_chain_merkle_prove(leaves, count, 2U, &proof, root2) == UMI_STATUS_OK);
        proof.siblings[0][0] ^= 1U;
        CHECK(umi_record_chain_merkle_verify(leaves + 64U, &proof, root2) == UMI_STATUS_INVALID_STATE);
    }
    free(leaves);
    return 0;
}

int main(void)
{
    CHECK(ExerciseCount(1U) == 0);
    CHECK(ExerciseCount(2U) == 0);
    CHECK(ExerciseCount(3U) == 0);
    CHECK(ExerciseCount(5U) == 0);
    CHECK(ExerciseCount(9U) == 0);
    CHECK(ExerciseCount(1024U) == 0);
    uint8_t leaves[64] = {1U,2U,3U};
    uint8_t root[32];
    UmiRecordChainMerkleProof proof;
    memset(root, 0xa5, sizeof(root));
    memset(&proof, 0x5a, sizeof(proof));
    uint8_t previousRoot[32];
    UmiRecordChainMerkleProof previousProof;
    memcpy(previousRoot, root, 32U);
    memcpy(&previousProof, &proof, sizeof(proof));
    CHECK(umi_record_chain_merkle_prove(leaves, 2U, 2U, &proof, root) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(root, previousRoot, 32U) == 0);
    CHECK(memcmp(&proof, &previousProof, sizeof(proof)) == 0);
    CHECK(umi_record_chain_merkle_root(leaves, 0U, root) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(root, previousRoot, 32U) == 0);
    CHECK(umi_record_chain_merkle_root(leaves, 2U, leaves) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_chain_merkle_prove(leaves, 2U, 0U, &proof, root) == UMI_STATUS_OK);
    proof.siblingCount = 0U;
    CHECK(umi_record_chain_merkle_verify(leaves, &proof, root) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_record_chain_merkle_prove(leaves, 2U, 0U, &proof, root) == UMI_STATUS_OK);
    proof.siblings[9][0] = 1U;
    CHECK(umi_record_chain_merkle_verify(leaves, &proof, root) == UMI_STATUS_INVALID_ARGUMENT);
    puts("Native Merkle: 1/2/3/5/9/1024 leaves, membership, mutation, size binding, atomic errors passed");
    return 0;
}
