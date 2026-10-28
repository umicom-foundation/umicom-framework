/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-merkle-ledger/demo.c
 *
 * PURPOSE:
 *   Demonstrate a five-record digest batch, a compact inclusion proof and
 *   rejection of a modified leaf against an independently supplied root.
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

int main(void)
{
    uint8_t digests[5U * 32U] = {0};
    uint8_t root[32], altered[32];
    UmiRecordChainMerkleProof proof = {0};
    for (size_t i = 0U; i < 5U; ++i)
        digests[i * 32U] = (uint8_t)(i + 1U);
    if (umi_record_chain_merkle_prove(digests, 5U, 4U, &proof, root) != UMI_STATUS_OK)
        return 1;
    if (umi_record_chain_merkle_verify(digests + 4U * 32U, &proof, root) != UMI_STATUS_OK)
        return 1;
    for (size_t i = 0U; i < 32U; ++i) altered[i] = digests[4U * 32U + i];
    altered[0] ^= 1U;
    if (umi_record_chain_merkle_verify(altered, &proof, root) != UMI_STATUS_INVALID_STATE)
        return 1;
    printf("Five synthetic digests: leaf 4 verified; changed leaf rejected; %zu sibling digests\n",
           proof.siblingCount);
    puts("Inclusion is not consensus, authentication or proof of useful model training.");
    return 0;
}
