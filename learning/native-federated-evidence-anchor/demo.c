/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-federated-evidence-anchor/demo.c
 *
 * PURPOSE:
 *   Teach anchored, count-bound federated receipt inclusion with real C23
 *   Framework record-chain and Merkle services and synthetic input digests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_evidence_anchor.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    uint8_t chainId[32] = {0x42U};
    uint8_t receipts[3U * 32U] = {0};
    uint8_t payload[UMI_AI_FEDERATED_EVIDENCE_ANCHOR_BYTES] = {0};
    UmiAiTrainingFederatedEvidenceSummary summary = {0};
    UmiAiTrainingFederatedEvidenceView verified = {0};
    UmiRecordChainCheckpoint prior, tip;
    UmiRecordChainRecord record = {0};
    UmiRecordChainMerkleProof proof = {0};
    for (size_t i = 0U; i < 3U; ++i) receipts[i * 32U] = (uint8_t)(0x10U + i);
    summary.roundId = 7U;
    summary.submittedCount = 4U;
    summary.acceptedCount = 3U;
    summary.baseCheckpointSha256[0] = 0x01U;
    summary.datasetPolicySha256[0] = 0x02U;
    summary.heldOutEvaluationSha256[0] = 0x03U;
    if (umi_record_chain_start(chainId, &prior) != UMI_STATUS_OK) return 1;
    tip = prior;
    if (UmiAiTrainingFederatedEvidenceAnchorCommit(&tip, &summary,
        receipts, 3U, &record, payload) != UMI_STATUS_OK) return 2;
    if (UmiAiTrainingFederatedEvidenceAnchorProve(receipts, 3U,
        1U, payload, &proof) != UMI_STATUS_OK) return 3;
    if (UmiAiTrainingFederatedEvidenceAnchorVerify(&prior, &record,
        payload, sizeof payload, &tip, receipts + 32U,
        &proof, &verified) != UMI_STATUS_OK) return 4;
    printf("Round %llu: %zu accepted of %llu submitted; anchored leaf index %zu\n",
        (unsigned long long)verified.summary.roundId,
        verified.summary.acceptedCount,
        (unsigned long long)verified.summary.submittedCount,
        proof.leafIndex);
    puts("Commitment and inclusion verified. No consensus, signature, training or reward was performed.");
    return 0;
}
