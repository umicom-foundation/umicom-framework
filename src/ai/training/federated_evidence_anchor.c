/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/federated_evidence_anchor.c
 *
 * PURPOSE:
 *   Compose canonical Framework record chains and Merkle membership into
 *   failure-atomic, verifiable federated training evidence commitments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/federated_evidence_anchor.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>

_Static_assert(CHAR_BIT == 8, "The canonical evidence format requires 8-bit bytes");

static int NonzeroDigest(const uint8_t digest[32])
{
    uint8_t combined = 0U;
    for (size_t i = 0U; i < 32U; ++i) combined |= digest[i];
    return combined != 0U;
}

/* Compare buffer ranges conservatively before any borrowed pointer is read
 * or any caller-owned result is published. Integer pointer overflow rejects. */
static int Overlaps(const void *a, size_t aBytes, const void *b, size_t bBytes)
{
    uintptr_t x = (uintptr_t)a, y = (uintptr_t)b;
    if (x > UINTPTR_MAX - aBytes || y > UINTPTR_MAX - bBytes) return 1;
    return x < y + bBytes && y < x + aBytes;
}

static void Write64(uint8_t *out, uint64_t n)
{
    for (size_t i = 0U; i < 8U; ++i) {
        out[7U - i] = (uint8_t)(n & UINT64_C(255));
        n >>= 8U;
    }
}

static uint64_t Read64(const uint8_t *in)
{
    uint64_t n = UINT64_C(0);
    for (size_t i = 0U; i < 8U; ++i) n = (n << 8U) | (uint64_t)in[i];
    return n;
}

static int ValidSummary(const UmiAiTrainingFederatedEvidenceSummary *summary)
{
    return summary != NULL && summary->roundId != UINT64_C(0) &&
        summary->acceptedCount > 0U &&
        summary->acceptedCount <= UMI_RECORD_CHAIN_MERKLE_MAX_LEAVES &&
        summary->submittedCount >= (uint64_t)summary->acceptedCount &&
        NonzeroDigest(summary->baseCheckpointSha256) &&
        NonzeroDigest(summary->datasetPolicySha256) &&
        NonzeroDigest(summary->heldOutEvaluationSha256);
}

/* The fixed layout binds the schema, round, counts and digest identities.
 * Payload[128..159] holds the existing count-bound Merkle root. */
static void Encode(const UmiAiTrainingFederatedEvidenceSummary *summary,
                   const uint8_t root[32], uint8_t payload[160])
{
    static const uint8_t magic[8] = {'U','F','B','A',0U,1U,0U,0U};
    memcpy(payload, magic, sizeof magic);
    Write64(payload + 8U, summary->roundId);
    Write64(payload + 16U, summary->submittedCount);
    Write64(payload + 24U, (uint64_t)summary->acceptedCount);
    memcpy(payload + 32U, summary->baseCheckpointSha256, 32U);
    memcpy(payload + 64U, summary->datasetPolicySha256, 32U);
    memcpy(payload + 96U, summary->heldOutEvaluationSha256, 32U);
    memcpy(payload + 128U, root, 32U);
}

static UmiStatus Decode(const uint8_t *payload, size_t bytes,
                        UmiAiTrainingFederatedEvidenceView *out)
{
    static const uint8_t magic[8] = {'U','F','B','A',0U,1U,0U,0U};
    UmiAiTrainingFederatedEvidenceView trial = {0};
    if (payload == NULL || out == NULL || bytes != 160U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (memcmp(payload, magic, sizeof magic) != 0)
        return UMI_STATUS_PARSE_ERROR;
    trial.summary.roundId = Read64(payload + 8U);
    trial.summary.submittedCount = Read64(payload + 16U);
    const uint64_t accepted = Read64(payload + 24U);
    if (accepted == 0U || accepted > UMI_RECORD_CHAIN_MERKLE_MAX_LEAVES)
        return UMI_STATUS_PARSE_ERROR;
    trial.summary.acceptedCount = (size_t)accepted;
    memcpy(trial.summary.baseCheckpointSha256, payload + 32U, 32U);
    memcpy(trial.summary.datasetPolicySha256, payload + 64U, 32U);
    memcpy(trial.summary.heldOutEvaluationSha256, payload + 96U, 32U);
    memcpy(trial.merkleRoot, payload + 128U, 32U);
    if (!ValidSummary(&trial.summary) || !NonzeroDigest(trial.merkleRoot))
        return UMI_STATUS_PARSE_ERROR;
    *out = trial;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedEvidenceAnchorCommit(
    UmiRecordChainCheckpoint *checkpoint,
    const UmiAiTrainingFederatedEvidenceSummary *summary,
    const uint8_t *acceptedReceiptDigests, size_t digestCount,
    UmiRecordChainRecord *outRecord, uint8_t outPayload[160])
{
    uint8_t root[32], payload[160];
    UmiRecordChainRecord trialRecord = {0};
    UmiRecordChainCheckpoint trialCheckpoint;
    UmiStatus status;
    if (checkpoint == NULL || summary == NULL || acceptedReceiptDigests == NULL ||
        outRecord == NULL || outPayload == NULL || !ValidSummary(summary) ||
        digestCount != summary->acceptedCount) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t inputBytes = 32U * digestCount;
    if (Overlaps(checkpoint, sizeof *checkpoint, outRecord, sizeof *outRecord) ||
        Overlaps(checkpoint, sizeof *checkpoint, outPayload, 160U) ||
        Overlaps(outRecord, sizeof *outRecord, outPayload, 160U) ||
        Overlaps(summary, sizeof *summary, outRecord, sizeof *outRecord) ||
        Overlaps(summary, sizeof *summary, outPayload, 160U) ||
        Overlaps(acceptedReceiptDigests, inputBytes, outRecord, sizeof *outRecord) ||
        Overlaps(acceptedReceiptDigests, inputBytes, outPayload, 160U) ||
        Overlaps(acceptedReceiptDigests, inputBytes, checkpoint, sizeof *checkpoint) ||
        Overlaps(summary, sizeof *summary, checkpoint, sizeof *checkpoint))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_record_chain_merkle_root(acceptedReceiptDigests, digestCount, root);
    if (status != UMI_STATUS_OK) return status;
    Encode(summary, root, payload);
    trialCheckpoint = *checkpoint;
    status = umi_record_chain_append(&trialCheckpoint,
        UMI_AI_FEDERATED_EVIDENCE_ANCHOR_KIND, payload, sizeof payload,
        &trialRecord);
    if (status != UMI_STATUS_OK) return status;
    /* All three owned outputs commit together, after the underlying chain
     * has accepted the exact canonical evidence bytes. */
    *outRecord = trialRecord;
    memcpy(outPayload, payload, sizeof payload);
    *checkpoint = trialCheckpoint;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedEvidenceAnchorProve(
    const uint8_t *acceptedReceiptDigests, size_t digestCount,
    size_t selectedIndex, const uint8_t anchorPayload[160],
    UmiRecordChainMerkleProof *outProof)
{
    UmiAiTrainingFederatedEvidenceView view;
    UmiRecordChainMerkleProof trialProof = {0};
    uint8_t calculatedRoot[32];
    UmiStatus status;
    if (acceptedReceiptDigests == NULL || anchorPayload == NULL ||
        outProof == NULL || selectedIndex >= digestCount ||
        digestCount == 0U || digestCount > UMI_RECORD_CHAIN_MERKLE_MAX_LEAVES)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (Overlaps(outProof, sizeof *outProof, anchorPayload, 160U) ||
        Overlaps(outProof, sizeof *outProof, acceptedReceiptDigests, 32U * digestCount))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = Decode(anchorPayload, 160U, &view);
    if (status != UMI_STATUS_OK) return status;
    if (view.summary.acceptedCount != digestCount) return UMI_STATUS_INVALID_STATE;
    status = umi_record_chain_merkle_prove(acceptedReceiptDigests,
        digestCount, selectedIndex, &trialProof, calculatedRoot);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(calculatedRoot, view.merkleRoot, 32U) != 0)
        return UMI_STATUS_INVALID_STATE;
    *outProof = trialProof;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingFederatedEvidenceAnchorVerify(
    const UmiRecordChainCheckpoint *trustedPrior,
    const UmiRecordChainRecord *record,
    const uint8_t *anchorPayload, size_t payloadBytes,
    const UmiRecordChainCheckpoint *trustedExpectedTip,
    const uint8_t selectedReceiptDigest[32],
    const UmiRecordChainMerkleProof *proof,
    UmiAiTrainingFederatedEvidenceView *outView)
{
    UmiRecordChainCheckpoint candidate;
    UmiAiTrainingFederatedEvidenceView view;
    UmiStatus status;
    if (trustedPrior == NULL || record == NULL || anchorPayload == NULL ||
        trustedExpectedTip == NULL || selectedReceiptDigest == NULL ||
        proof == NULL || outView == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (record->kind != UMI_AI_FEDERATED_EVIDENCE_ANCHOR_KIND)
        return UMI_STATUS_INVALID_STATE;
    if (Overlaps(outView, sizeof *outView, record, sizeof *record) ||
        Overlaps(outView, sizeof *outView, anchorPayload, payloadBytes) ||
        Overlaps(outView, sizeof *outView, trustedPrior, sizeof *trustedPrior) ||
        Overlaps(outView, sizeof *outView, trustedExpectedTip, sizeof *trustedExpectedTip) ||
        Overlaps(outView, sizeof *outView, selectedReceiptDigest, 32U) ||
        Overlaps(outView, sizeof *outView, proof, sizeof *proof))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = Decode(anchorPayload, payloadBytes, &view);
    if (status != UMI_STATUS_OK) return status;
    if (proof->leafCount != view.summary.acceptedCount)
        return UMI_STATUS_INVALID_STATE;
    candidate = *trustedPrior;
    status = umi_record_chain_accept(&candidate, record, anchorPayload, payloadBytes);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_record_chain_checkpoint_equal(&candidate, trustedExpectedTip))
        return UMI_STATUS_INVALID_STATE;
    status = umi_record_chain_merkle_verify(selectedReceiptDigest, proof,
        view.merkleRoot);
    if (status != UMI_STATUS_OK) return status;
    *outView = view;
    return UMI_STATUS_OK;
}
