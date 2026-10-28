/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/federated_evidence_anchor.h
 *
 * PURPOSE:
 *   Bind a bounded set of approved federated-update receipt digests to a
 *   canonical, independently anchorable Framework record-chain checkpoint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_FEDERATED_EVIDENCE_ANCHOR_H
#define UMICOM_AI_TRAINING_FEDERATED_EVIDENCE_ANCHOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/record_chain/chain.h"
#include "umicom/record_chain/merkle.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Canonical version-one payload. No C struct dumps or native endian fields. */
#define UMI_AI_FEDERATED_EVIDENCE_ANCHOR_BYTES 160U
#define UMI_AI_FEDERATED_EVIDENCE_ANCHOR_KIND UINT32_C(0x55464241)

typedef struct UmiAiTrainingFederatedEvidenceSummary {
    uint64_t roundId;
    uint64_t submittedCount;
    size_t acceptedCount;
    uint8_t baseCheckpointSha256[32];
    uint8_t datasetPolicySha256[32];
    uint8_t heldOutEvaluationSha256[32];
} UmiAiTrainingFederatedEvidenceSummary;

typedef struct UmiAiTrainingFederatedEvidenceView {
    UmiAiTrainingFederatedEvidenceSummary summary;
    uint8_t merkleRoot[32];
} UmiAiTrainingFederatedEvidenceView;

/* Construct a count-bound Merkle root over 1..1024 ALREADY REVIEWED
 * 32-byte contribution-receipt digests. Commit the canonical 160-byte
 * summary to the existing Framework ordered record chain. Digests are
 * ordered as supplied. This does not decide whether any contribution is
 * legal, honest, independently evaluated or authorised.
 *
 * checkpoint is updated only after the entire operation succeeds. outRecord
 * and outPayload remain unchanged on rejection. All output regions must be
 * disjoint from one another and all borrowed inputs. No networking, heap
 * allocation, new hash implementation, signature or ledger consensus. */
UmiStatus UmiAiTrainingFederatedEvidenceAnchorCommit(
    UmiRecordChainCheckpoint *checkpoint,
    const UmiAiTrainingFederatedEvidenceSummary *summary,
    const uint8_t *acceptedReceiptDigests, size_t digestCount,
    UmiRecordChainRecord *outRecord,
    uint8_t outPayload[UMI_AI_FEDERATED_EVIDENCE_ANCHOR_BYTES]);

/* Construct a proof only when the original accepted leaf sequence matches
 * an already committed canonical payload. The payload is NOT itself a
 * trusted root. The proof must later be checked against an independent
 * record-chain checkpoint with FederatedEvidenceAnchorVerify. */
UmiStatus UmiAiTrainingFederatedEvidenceAnchorProve(
    const uint8_t *acceptedReceiptDigests, size_t digestCount,
    size_t selectedIndex,
    const uint8_t anchorPayload[UMI_AI_FEDERATED_EVIDENCE_ANCHOR_BYTES],
    UmiRecordChainMerkleProof *outProof);

/* Check (a) canonical record format, (b) complete recorded transition from
 * an independently trusted prior checkpoint to an independently trusted
 * expected tip, and (c) Merkle inclusion of the selected receipt digest.
 * The caller is responsible for authenticating both independently trusted
 * checkpoint sources. A hash proves byte commitment, NOT consensus,
 * signer identity, private-data rights or quality of model training.
 * outView is never modified by a failed verification. */
UmiStatus UmiAiTrainingFederatedEvidenceAnchorVerify(
    const UmiRecordChainCheckpoint *trustedPrior,
    const UmiRecordChainRecord *record,
    const uint8_t *anchorPayload, size_t payloadBytes,
    const UmiRecordChainCheckpoint *trustedExpectedTip,
    const uint8_t selectedReceiptDigest[32],
    const UmiRecordChainMerkleProof *proof,
    UmiAiTrainingFederatedEvidenceView *outView);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_FEDERATED_EVIDENCE_ANCHOR_H */
