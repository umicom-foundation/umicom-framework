/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_inference_qualification.h
 *
 * PURPOSE:
 *   Qualify fixed-token incremental Transformer inference against two
 *   independent existing Framework observations: canonical decoder logits
 *   and its authoritative per-layer Q/K/V projection trace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_INFERENCE_QUALIFICATION_H
#define UMICOM_AI_TRAINING_STACKED_INFERENCE_QUALIFICATION_H

#include <stddef.h>
#include "umicom/ai/training/inference_parity.h"
#include "umicom/ai/training/stacked_model_identity.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedQualificationReport {
    UmiAiTrainingStackedModelIdentity checkpoint;
    size_t initialPromptTokens;
    size_t continuationTokens;
    size_t transformerLayers;
    size_t contextLength;
    size_t growingPrefixSteps;
    size_t fullWindowRebuilds;
    size_t independentCanonicalLogitsCompared;
    size_t incrementalLogitsCompared;
    size_t unchangedPrefixLogitsCompared;
    size_t incrementalKeyValueScalarsCompared;
    size_t retainedKeyValueScalarsCompared;
    double maximumLogitDifference;
    double maximumKeyValueDifference;
} UmiAiTrainingStackedQualificationReport;

/* Read-only qualification of the borrowed model and an ORIGINAL-CHECKPOINT
 * cache. Given a nonempty prefix and 1..64 externally supplied next-token IDs:
 * - compare each active canonical-logit row to projection-trace logits;
 * - while the prefix grows, independently compare the proposed NEW token's
 *   full vocabulary logits and every per-layer new K/V projection;
 * - verify causal invariance of all unchanged-position logit rows;
 * - after each append or absolute-position rollover, read back EVERY
 *   retained key/value scalar from Framework's existing store;
 * - at rollover rebuild every layer from the original canonical decoder.
 *
 * The continuation IDs are fixed test inputs, NOT sampled predictions. A
 * separate seeded generator regression must cover sampling equivalence.
 * No model weight, checkpoint, training tape algorithm, store contract or
 * sampling implementation is replaced. Caller serialises model operations.
 * Full context eviction is NOT claimed to use the incremental kernel.
 * Tolerances follow the companion inference_parity.h contract.
 * Invalid/mismatched/nonfinite data, model changes and failed operations
 * leave outReport byte-for-byte unchanged; all temporary cache state is
 * destroyed. The model remains owned by the caller.
 * This diagnostic measures numerical correctness, not elapsed-time speed. */
UmiStatus UmiAiTrainingStackedInferenceQualify(
    UmiAiTrainingStackedDecoder *model,
    const size_t *prefixTokenIds, size_t prefixTokenCount,
    const size_t *continuationTokenIds, size_t continuationTokenCount,
    const UmiAiTrainingInferenceParityTolerance *tolerance,
    UmiAiTrainingStackedQualificationReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_INFERENCE_QUALIFICATION_H */
