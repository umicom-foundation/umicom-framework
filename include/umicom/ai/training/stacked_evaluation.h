/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_evaluation.h
 * PURPOSE: Independently score held-out masked next-token examples in C23.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_EVALUATION_H
#define UMICOM_AI_TRAINING_STACKED_EVALUATION_H

#include "umicom/ai/training/stacked_model_identity.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_STACKED_EVALUATION_MAX_CASES 64U

typedef struct UmiAiTrainingStackedEvaluationCase {
    const size_t *inputIds;
    const size_t *targetIds;
    const uint8_t *lossMask;
    size_t tokenCount;
    /* Caller-supplied provenance label; a digest alone does NOT certify
     * training rights, originality, redaction, or split integrity. */
    uint8_t originalSourceSha256[UMI_SHA256_BYTES];
} UmiAiTrainingStackedEvaluationCase;

typedef struct UmiAiTrainingStackedEvaluationReport {
    size_t exampleCount;
    size_t supervisedTargets;
    size_t correctTopOneTargets;
    double meanSelectedCrossEntropy;
    double topOneAccuracy;
    double perplexity;          /* Zero if exponentiation is unavailable. */
    int perplexityAvailable;    /* Explicitly distinguish overflow from zero. */
    uint8_t orderedCaseSha256[UMI_SHA256_BYTES];
    UmiAiTrainingStackedModelIdentity modelIdentity;
} UmiAiTrainingStackedEvaluationReport;

/* Evaluate 1..64 independently REVIEWED, held-out examples against the actual
 * stacked decoder. All contexts must match the model's fixed context length;
 * every mask is 0/1 with at least one selected target per example. Targets
 * at ignored positions need not be valid vocabulary IDs.
 *
 * Cross-entropy is delegated to StackedDecoderEvaluateSelected. Top-one
 * accuracy is calculated from StackedDecoderLogits on the SAME input, with
 * lowest-token-ID tie breaking. Only SELECTED targets enter either metric.
 * Aggregate loss is weighted by number of selected tokens, not example count.
 * The reported case digest includes ordered source labels, input IDs, targets
 * and masks, canonicalised to fixed-width little-endian integers. It is NOT
 * proof of copyright permission, source authenticity or a signed release.
 *
 * The complete model's SHA-256 is checked before and after scoring. No
 * training occurs. Callers must serialise access and keep inputs immutable.
 * Failure preserves outReport byte-for-byte. No I/O or logging of raw text. */
UmiStatus UmiAiTrainingStackedEvaluationRun(
    UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedEvaluationCase *examples,
    size_t exampleCount, UmiAiTrainingStackedEvaluationReport *outReport);

#ifdef __cplusplus
}
#endif
#endif
