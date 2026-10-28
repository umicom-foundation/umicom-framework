/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-federated-audit/demo.c
 *
 * PURPOSE:
 *   Demonstrate canonical federated decision metadata and an independently
 *   verifiable record-chain checkpoint using synthetic educational identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_audit_chain.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    uint8_t chainId[32] = {0x55U, 0x4dU, 0x49U};
    UmiRecordChainCheckpoint checkpoint = {0};
    UmiRecordChainCheckpoint independent = {0};
    UmiRecordChainRecord receipt = {0};
    uint8_t payload[UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES] = {0};
    UmiAiTrainingFederatedAuditEvidence e = {0}, readback = {0};
    e.roundId = 1U;
    e.workerSequence = 1U;
    e.distinctSamples = 50U;
    e.outcome = UMI_AI_FEDERATED_AUDIT_ACCEPTED;
    memset(e.contributorId, 1, 32U);
    memset(e.baseCheckpointSha256, 2, 32U);
    memset(e.datasetCommitmentSha256, 3, 32U);
    memset(e.policySha256, 4, 32U);
    memset(e.updateCommitmentSha256, 5, 32U);
    memset(e.evaluationCommitmentSha256, 6, 32U);
    if (UmiAiTrainingFederatedAuditStart(chainId, &checkpoint) != UMI_STATUS_OK)
        return 1;
    independent = checkpoint;
    if (UmiAiTrainingFederatedAuditAppend(&checkpoint, &e,
        &receipt, payload) != UMI_STATUS_OK) return 2;
    if (UmiAiTrainingFederatedAuditAccept(&independent, &receipt,
        payload, sizeof(payload), &readback) != UMI_STATUS_OK) return 3;
    if (!umi_record_chain_checkpoint_equal(&checkpoint, &independent)) return 4;
    printf("Verified record sequence %llu; round %llu; synthetic worker samples %llu\n",
        (unsigned long long)receipt.sequence,
        (unsigned long long)readback.roundId,
        (unsigned long long)readback.distinctSamples);
    puts("This is a tamper-evident local record chain, not a blockchain consensus network.");
    return 0;
}
