/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_federated_audit_chain.c
 *
 * PURPOSE:
 *   Check canonical C23 learning receipts against the real record-chain and
 *   SHA-256 services, including tampering, ordering, truncation and atomicity.
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

#define CHECK(e) do { if (!(e)) { fprintf(stderr, "audit test line %d: %s\n", __LINE__, #e); return 1; } } while (0)

static UmiAiTrainingFederatedAuditEvidence Example(uint64_t sequence,
    UmiAiTrainingFederatedAuditOutcome decision)
{
    UmiAiTrainingFederatedAuditEvidence e = {0};
    e.roundId = UINT64_C(23);
    e.workerSequence = sequence;
    e.distinctSamples = decision == UMI_AI_FEDERATED_AUDIT_ACCEPTED ? 100U : 0U;
    e.outcome = decision;
    for (size_t i = 0U; i < 32U; ++i) {
        e.contributorId[i] = (uint8_t)(i + 1U);
        e.baseCheckpointSha256[i] = (uint8_t)(i + 2U);
        e.datasetCommitmentSha256[i] = (uint8_t)(i + 3U);
        e.policySha256[i] = (uint8_t)(i + 4U);
        e.updateCommitmentSha256[i] = (uint8_t)(i + 5U);
        e.evaluationCommitmentSha256[i] = (uint8_t)(i + 6U);
    }
    return e;
}

int main(void)
{
    const uint8_t chainId[32] = {1U, 4U, 9U, 16U};
    const uint8_t badId[32] = {0U};
    UmiRecordChainCheckpoint start = {0}, tip = {0}, recovered = {0};
    UmiRecordChainCheckpoint unchanged = {0};
    UmiRecordChainRecord a = {0}, b = {0};
    uint8_t p1[UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES] = {0};
    uint8_t p2[UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES] = {0};
    uint8_t tampered[UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES] = {0};
    UmiAiTrainingFederatedAuditEvidence first = Example(1U, UMI_AI_FEDERATED_AUDIT_ACCEPTED);
    UmiAiTrainingFederatedAuditEvidence second = Example(2U, UMI_AI_FEDERATED_AUDIT_REJECTED);
    UmiAiTrainingFederatedAuditEvidence readback;
    memset(&readback, 0xa5, sizeof(readback));
    UmiAiTrainingFederatedAuditEvidence sentinel = readback;

    CHECK(UmiAiTrainingFederatedAuditStart(badId, &start) != UMI_STATUS_OK);
    CHECK(start.sequence == 0U);
    CHECK(UmiAiTrainingFederatedAuditStart(chainId, &start) == UMI_STATUS_OK);
    tip = start;
    CHECK(UmiAiTrainingFederatedAuditAppend(&tip, &first, &a, p1) == UMI_STATUS_OK);
    CHECK(tip.sequence == 1U && a.kind == UMI_AI_FEDERATED_AUDIT_KIND);
    CHECK(UmiAiTrainingFederatedAuditAppend(&tip, &second, &b, p2) == UMI_STATUS_OK);
    CHECK(tip.sequence == 2U && b.sequence == 2U);
    CHECK(p1[7] == 23U && p1[15] == 1U && p1[23] == 100U);
    CHECK(p1[27] == 1U && p1[28] == 0U && p1[31] == 0U);

    recovered = start;
    CHECK(UmiAiTrainingFederatedAuditAccept(&recovered, &a, p1, sizeof(p1),
        &readback) == UMI_STATUS_OK);
    CHECK(readback.roundId == 23U && readback.workerSequence == 1U &&
        readback.distinctSamples == 100U && recovered.sequence == 1U);
    unchanged = recovered;
    readback = sentinel;
    CHECK(UmiAiTrainingFederatedAuditAccept(&recovered, &a, p1, sizeof(p1),
        &readback) != UMI_STATUS_OK);
    CHECK(memcmp(&recovered, &unchanged, sizeof(recovered)) == 0);
    CHECK(memcmp(&readback, &sentinel, sizeof(readback)) == 0);
    CHECK(UmiAiTrainingFederatedAuditAccept(&recovered, &b, p2,
        sizeof(p2), &readback) == UMI_STATUS_OK);
    CHECK(umi_record_chain_checkpoint_equal(&recovered, &tip));
    CHECK(readback.outcome == UMI_AI_FEDERATED_AUDIT_REJECTED);

    memcpy(tampered, p2, sizeof(tampered));
    tampered[60] ^= 1U;
    recovered = start;
    CHECK(UmiAiTrainingFederatedAuditAccept(&recovered, &b, tampered,
        sizeof(tampered), &readback) != UMI_STATUS_OK);
    CHECK(umi_record_chain_checkpoint_equal(&recovered, &start));

    const UmiRecordChainInput list[2] = {{&a, p1, sizeof(p1)},
                                        {&b, p2, sizeof(p2)}};
    recovered = start;
    CHECK(UmiAiTrainingFederatedAuditVerify(&recovered, list, 2U, &tip) == UMI_STATUS_OK);
    CHECK(umi_record_chain_checkpoint_equal(&recovered, &tip));
    recovered = start;
    CHECK(UmiAiTrainingFederatedAuditVerify(&recovered, list, 1U, &tip) ==
          UMI_STATUS_INVALID_STATE); /* A valid prefix is not the expected tip. */
    CHECK(umi_record_chain_checkpoint_equal(&recovered, &start));
    const UmiRecordChainInput reversed[2] = {{&b, p2, sizeof(p2)},
                                             {&a, p1, sizeof(p1)}};
    CHECK(UmiAiTrainingFederatedAuditVerify(&recovered, reversed, 2U, &tip) !=
          UMI_STATUS_OK);
    CHECK(umi_record_chain_checkpoint_equal(&recovered, &start));
    UmiAiTrainingFederatedAuditEvidence invalid = first;
    invalid.updateCommitmentSha256[0] = 0U;
    memset(invalid.updateCommitmentSha256, 0, 32U);
    UmiRecordChainRecord untouched = a;
    unchanged = recovered;
    CHECK(UmiAiTrainingFederatedAuditAppend(&recovered, &invalid,
        &untouched, p1) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&untouched, &a, sizeof(a)) == 0);
    CHECK(umi_record_chain_checkpoint_equal(&recovered, &unchanged));
    CHECK(UmiAiTrainingFederatedAuditAccept(&recovered, &a,
        p1, sizeof(p1) - 1U, &readback) != UMI_STATUS_OK);
    puts("Canonical federated receipts: ordered, bounded, tamper-evident, not consensus");
    return 0;
}
