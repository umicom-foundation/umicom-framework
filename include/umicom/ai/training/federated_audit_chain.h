/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/federated_audit_chain.h
 *
 * PURPOSE:
 *   Commit canonical, non-secret federated learning decision metadata through
 *   the existing Framework record-chain service without implementing consensus.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_FEDERATED_AUDIT_CHAIN_H
#define UMICOM_AI_TRAINING_FEDERATED_AUDIT_CHAIN_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/record_chain/chain.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES 224U
#define UMI_AI_FEDERATED_AUDIT_KIND UINT32_C(0x41494644) /* ASCII AIFD */

/* The outcome is a declared governance event, not a computed validity proof. */
typedef enum UmiAiTrainingFederatedAuditOutcome {
    UMI_AI_FEDERATED_AUDIT_ACCEPTED = 1,
    UMI_AI_FEDERATED_AUDIT_REJECTED = 2,
    UMI_AI_FEDERATED_AUDIT_QUARANTINED = 3
} UmiAiTrainingFederatedAuditOutcome;

typedef struct UmiAiTrainingFederatedAuditEvidence {
    uint64_t roundId;
    uint64_t workerSequence;
    uint64_t distinctSamples;
    UmiAiTrainingFederatedAuditOutcome outcome;
    uint8_t contributorId[32];
    uint8_t baseCheckpointSha256[32];
    uint8_t datasetCommitmentSha256[32];
    uint8_t policySha256[32];
    uint8_t updateCommitmentSha256[32];
    uint8_t evaluationCommitmentSha256[32];
} UmiAiTrainingFederatedAuditEvidence;

/* Start a distinct, nonzero 32-byte audit history, using the *existing* record
 * chain genesis routine. The chain id is public metadata, not an access key.
 * A rejected call leaves outCheckpoint unchanged. */
UmiStatus UmiAiTrainingFederatedAuditStart(
    const uint8_t chainId[32], UmiRecordChainCheckpoint *outCheckpoint);

/* A 224-byte canonical payload commits to round[8], sequence[8], samples[8],
 * decision[4], reserved[4]=0 and six 32-byte digests in the structure's
 * declared order. All multi-byte integer fields use network byte order.
 * Digests are *claims* of object identity and are not signatures, secrets,
 * authorisation decisions, training receipts or proofs of statistical quality.
 * Inputs/outputs must remain disjoint; failure leaves checkpoint, record and
 * payload byte-for-byte unchanged. The caller publishes/stores both record and
 * payload, and anchors the expected final checkpoint independently. */
UmiStatus UmiAiTrainingFederatedAuditAppend(
    UmiRecordChainCheckpoint *checkpoint,
    const UmiAiTrainingFederatedAuditEvidence *evidence,
    UmiRecordChainRecord *outRecord,
    uint8_t outPayload[UMI_AI_FEDERATED_AUDIT_PAYLOAD_BYTES]);

/* Recheck the record and payload against an independently held predecessor,
 * then parse the canonical payload. A skipped or reordered record is rejected.
 * Neither the checkpoint nor the parsed output changes on rejection. */
UmiStatus UmiAiTrainingFederatedAuditAccept(
    UmiRecordChainCheckpoint *checkpoint,
    const UmiRecordChainRecord *record,
    const uint8_t *payload, size_t payloadBytes,
    UmiAiTrainingFederatedAuditEvidence *outEvidence);

/* The authoritative record-chain verifier performs all hash-link checks.
 * This wrapper also checks the federated payload contract for each record and
 * requires the independently known expected tip, so a valid shorter prefix
 * cannot be mistakenly reported as the complete research history. */
UmiStatus UmiAiTrainingFederatedAuditVerify(
    UmiRecordChainCheckpoint *checkpoint,
    const UmiRecordChainInput *inputs, size_t count,
    const UmiRecordChainCheckpoint *expectedTip);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_FEDERATED_AUDIT_CHAIN_H */
