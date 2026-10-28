/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/federated_audit_chain.c
 *
 * PURPOSE:
 *   Serialize public federated audit metadata and reuse the canonical
 *   tamper-evident record-chain owner; do not implement a competing ledger.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_audit_chain.h"
#include <string.h>
#include <stdint.h>

/* Field layout is intentionally stable across CPU endianness and C ABIs. */
#define AUDIT_DIGEST_OFFSET 32U
#define AUDIT_DIGEST_COUNT 6U

static int NonzeroDigest(const uint8_t digest[32])
{
    unsigned int aggregate = 0U;
    for (size_t i = 0U; i < 32U; ++i) aggregate |= digest[i];
    return aggregate != 0U;
}

static int Valid(const UmiAiTrainingFederatedAuditEvidence *e)
{
    if (e == NULL || e->roundId == 0U || e->workerSequence == 0U ||
        e->outcome < UMI_AI_FEDERATED_AUDIT_ACCEPTED ||
        e->outcome > UMI_AI_FEDERATED_AUDIT_QUARANTINED ||
        (e->outcome == UMI_AI_FEDERATED_AUDIT_ACCEPTED && e->distinctSamples == 0U))
        return 0;
    return NonzeroDigest(e->contributorId) &&
           NonzeroDigest(e->baseCheckpointSha256) &&
           NonzeroDigest(e->datasetCommitmentSha256) &&
           NonzeroDigest(e->policySha256) &&
           NonzeroDigest(e->updateCommitmentSha256) &&
           NonzeroDigest(e->evaluationCommitmentSha256);
}

static void Put(uint8_t *p, uint64_t n, size_t width)
{
    for (size_t i = 0U; i < width; ++i) {
        p[width - 1U - i] = (uint8_t)(n & UINT64_C(255));
        n >>= 8U;
    }
}

static uint64_t Get(const uint8_t *p, size_t width)
{
    uint64_t n = 0U;
    for (size_t i = 0U; i < width; ++i) n = (n << 8U) | p[i];
    return n;
}

/* Detect overlap conservatively without dereferencing any caller pointers. */
static int Overlap(const void *a, size_t na, const void *b, size_t nb)
{
    const uintptr_t p = (uintptr_t)a, q = (uintptr_t)b;
    if (p > UINTPTR_MAX - na || q > UINTPTR_MAX - nb) return 1;
    return p < q + nb && q < p + na;
}

static void Encode(const UmiAiTrainingFederatedAuditEvidence *e,
                   uint8_t b[UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES])
{
    Put(b, e->roundId, 8U);
    Put(b + 8U, e->workerSequence, 8U);
    Put(b + 16U, e->distinctSamples, 8U);
    Put(b + 24U, (uint64_t)e->outcome, 4U);
    Put(b + 28U, 0U, 4U); /* Always zero; protects future schema extension. */
    memcpy(b + AUDIT_DIGEST_OFFSET, e->contributorId, 32U);
    memcpy(b + AUDIT_DIGEST_OFFSET + 32U, e->baseCheckpointSha256, 32U);
    memcpy(b + AUDIT_DIGEST_OFFSET + 64U, e->datasetCommitmentSha256, 32U);
    memcpy(b + AUDIT_DIGEST_OFFSET + 96U, e->policySha256, 32U);
    memcpy(b + AUDIT_DIGEST_OFFSET + 128U, e->updateCommitmentSha256, 32U);
    memcpy(b + AUDIT_DIGEST_OFFSET + 160U, e->evaluationCommitmentSha256, 32U);
}

static UmiStatus Decode(const uint8_t *b, size_t length,
                        UmiAiTrainingFederatedAuditEvidence *out)
{
    UmiAiTrainingFederatedAuditEvidence candidate = {0};
    if (b == NULL || out == NULL || length != UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (Get(b + 28U, 4U) != 0U) return UMI_STATUS_PARSE_ERROR;
    candidate.roundId = Get(b, 8U);
    candidate.workerSequence = Get(b + 8U, 8U);
    candidate.distinctSamples = Get(b + 16U, 8U);
    const uint64_t decision = Get(b + 24U, 4U);
    if (decision < 1U || decision > 3U) return UMI_STATUS_PARSE_ERROR;
    candidate.outcome = (UmiAiTrainingFederatedAuditOutcome)decision;
    memcpy(candidate.contributorId, b + AUDIT_DIGEST_OFFSET, 32U);
    memcpy(candidate.baseCheckpointSha256, b + AUDIT_DIGEST_OFFSET + 32U, 32U);
    memcpy(candidate.datasetCommitmentSha256, b + AUDIT_DIGEST_OFFSET + 64U, 32U);
    memcpy(candidate.policySha256, b + AUDIT_DIGEST_OFFSET + 96U, 32U);
    memcpy(candidate.updateCommitmentSha256, b + AUDIT_DIGEST_OFFSET + 128U, 32U);
    memcpy(candidate.evaluationCommitmentSha256, b + AUDIT_DIGEST_OFFSET + 160U, 32U);
    if (!Valid(&candidate)) return UMI_STATUS_PARSE_ERROR;
    *out = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedAuditStart(const uint8_t chainId[32],
                                           UmiRecordChainCheckpoint *outCheckpoint)
{
    return umi_record_chain_start(chainId, outCheckpoint);
}

UmiStatus UmiAiTrainingFederatedAuditAppend(
    UmiRecordChainCheckpoint *checkpoint,
    const UmiAiTrainingFederatedAuditEvidence *evidence,
    UmiRecordChainRecord *outRecord,
    uint8_t outPayload[UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES])
{
    UmiRecordChainRecord record = {0};
    UmiRecordChainCheckpoint trial;
    uint8_t payload[UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES] = {0};
    UmiStatus status;
    if (checkpoint == NULL || outRecord == NULL || outPayload == NULL || !Valid(evidence))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (Overlap(checkpoint, sizeof(*checkpoint), outRecord, sizeof(*outRecord)) ||
        Overlap(checkpoint, sizeof(*checkpoint), outPayload, sizeof(payload)) ||
        Overlap(outRecord, sizeof(*outRecord), outPayload, sizeof(payload)) ||
        Overlap(evidence, sizeof(*evidence), outPayload, sizeof(payload)) ||
        Overlap(evidence, sizeof(*evidence), outRecord, sizeof(*outRecord)))
        return UMI_STATUS_INVALID_ARGUMENT;
    trial = *checkpoint;
    Encode(evidence, payload);
    status = umi_record_chain_append(&trial, UMI_AI_FEDERATED_AUDIT_KIND,
                                     payload, sizeof(payload), &record);
    if (status != UMI_STATUS_OK) return status;
    *outRecord = record;
    memcpy(outPayload, payload, sizeof(payload));
    *checkpoint = trial;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedAuditAccept(
    UmiRecordChainCheckpoint *checkpoint, const UmiRecordChainRecord *record,
    const uint8_t *payload, size_t payloadBytes,
    UmiAiTrainingFederatedAuditEvidence *outEvidence)
{
    UmiAiTrainingFederatedAuditEvidence decoded = {0};
    UmiRecordChainCheckpoint candidate;
    UmiStatus status;
    if (checkpoint == NULL || record == NULL || outEvidence == NULL ||
        record->kind != UMI_AI_FEDERATED_AUDIT_KIND)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (Overlap(checkpoint, sizeof(*checkpoint), outEvidence, sizeof(*outEvidence)) ||
        Overlap(record, sizeof(*record), outEvidence, sizeof(*outEvidence)) ||
        (payload != NULL && Overlap(payload, payloadBytes, outEvidence, sizeof(*outEvidence))))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = Decode(payload, payloadBytes, &decoded);
    if (status != UMI_STATUS_OK) return status;
    candidate = *checkpoint;
    status = umi_record_chain_accept(&candidate, record, payload, payloadBytes);
    if (status != UMI_STATUS_OK) return status;
    *outEvidence = decoded;
    *checkpoint = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedAuditVerify(
    UmiRecordChainCheckpoint *checkpoint, const UmiRecordChainInput *inputs,
    size_t count, const UmiRecordChainCheckpoint *expectedTip)
{
    UmiStatus status;
    UmiAiTrainingFederatedAuditEvidence decoded = {0};
    if (checkpoint == NULL || expectedTip == NULL ||
        (inputs == NULL && count != 0U)) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_RECORD_CHAIN_BATCH_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i) {
        if (inputs[i].record == NULL ||
            inputs[i].record->kind != UMI_AI_FEDERATED_AUDIT_KIND)
            return UMI_STATUS_INVALID_ARGUMENT;
        status = Decode(inputs[i].payload, inputs[i].payload_size, &decoded);
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_record_chain_verify(checkpoint, inputs, count, expectedTip);
}
