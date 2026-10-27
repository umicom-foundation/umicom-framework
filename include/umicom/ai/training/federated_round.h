/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/federated_round.h
 *
 * PURPOSE:
 *   Declare a bounded, checkpoint-bound FedAvg delta aggregation round
 *   without implementing or duplicating model training, transport,
 *   cryptography or consensus.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_FEDERATED_ROUND_H
#define UMICOM_AI_TRAINING_FEDERATED_ROUND_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_FEDERATED_DIGEST_BYTES 32U
#define UMI_AI_FEDERATED_MAX_CONTRIBUTORS 64U
#define UMI_AI_FEDERATED_MAX_PARAMETERS 4096U

typedef struct UmiAiTrainingFederatedRound UmiAiTrainingFederatedRound;

typedef struct UmiAiTrainingFederatedRoundConfig {
    size_t parameterCount;
    size_t maximumContributors;
    float maximumDeltaL2Norm;
    uint8_t baseCheckpointSha256[UMI_AI_FEDERATED_DIGEST_BYTES];
} UmiAiTrainingFederatedRoundConfig;

typedef struct UmiAiTrainingFederatedContribution {
    uint8_t contributorId[UMI_AI_FEDERATED_DIGEST_BYTES];
    uint8_t baseCheckpointSha256[UMI_AI_FEDERATED_DIGEST_BYTES];
    uint8_t datasetCommitmentSha256[UMI_AI_FEDERATED_DIGEST_BYTES];
    uint64_t distinctSampleCount;
    const float *weightSpaceDelta;
    size_t deltaCount;
} UmiAiTrainingFederatedContribution;

typedef struct UmiAiTrainingFederatedRoundReport {
    size_t acceptedContributors;
    size_t parameterCount;
    uint64_t totalDistinctSamples;
    double maximumObservedDeltaL2Norm;
    int finalised;
} UmiAiTrainingFederatedRoundReport;

/* A round is caller-owned and caller-serialised. Initialise *outRound=NULL.
 * The base digest must be a nonzero 32-byte SHA-256 supplied and independently
 * verified using the established Framework checkpoint-identity APIs. No
 * cryptographic verification, model training, network exchange, or privacy
 * guarantee is implemented by this arithmetic service. */
UmiStatus UmiAiTrainingFederatedRoundCreate(
    const UmiAiTrainingFederatedRoundConfig *config,
    UmiAiTrainingFederatedRound **outRound);
void UmiAiTrainingFederatedRoundDestroy(UmiAiTrainingFederatedRound *round);

/* Stage one explicitly authorised weight-space delta from a contributor.
 * Values are NOT LoRA factor matrices; aggregate B*A deltas, not separately
 * averaged B and A factors. Sample counts are caller-certified distinct
 * examples; this function cannot verify dataset rights, counts or claims.
 * Reject duplicate contributor IDs, zero commitments, mismatched bases,
 * nonfinite/excessive deltas and counter overflow without mutating the round.
 * This is FedAvg ARITHMETIC only, not Byzantine-robust aggregation. */
UmiStatus UmiAiTrainingFederatedRoundSubmit(
    UmiAiTrainingFederatedRound *round,
    const UmiAiTrainingFederatedContribution *contribution);

/* Finalise weighted mean delta: sum(n_i * delta_i) / sum(n_i).
 * Caller must add it to the pinned base model only after independently
 * evaluating quality, governance and checkpoint approval. The output array
 * contains parameterCount FP32 values; capacity counts FLOAT elements.
 * On rejection, all caller outputs and round state remain unchanged.
 * A successful finalisation closes Submit and allows repeat result reads.
 * outMeanDelta and non-NULL outReport must be disjoint regions. */
UmiStatus UmiAiTrainingFederatedRoundFinalise(
    UmiAiTrainingFederatedRound *round,
    float *outMeanDelta, size_t capacity,
    UmiAiTrainingFederatedRoundReport *outReport);
UmiStatus UmiAiTrainingFederatedRoundInspect(
    const UmiAiTrainingFederatedRound *round,
    UmiAiTrainingFederatedRoundReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_FEDERATED_ROUND_H */
