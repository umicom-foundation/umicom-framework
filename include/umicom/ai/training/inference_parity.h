/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/inference_parity.h
 *
 * PURPOSE:
 *   Compare complete FP32 inference vectors with explicit absolute and
 *   relative tolerances. This diagnostic does not change model mathematics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_INFERENCE_PARITY_H
#define UMICOM_AI_TRAINING_INFERENCE_PARITY_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingInferenceParityTolerance {
    float absoluteTolerance;
    float relativeTolerance;
} UmiAiTrainingInferenceParityTolerance;

typedef struct UmiAiTrainingInferenceParityReport {
    size_t valuesCompared;
    size_t mismatchedValues;
    size_t firstMismatchIndex; /* SIZE_MAX if all values qualify. */
    double maximumAbsoluteDifference;
    double maximumRelativeDifference;
    int withinTolerance;
} UmiAiTrainingInferenceParityReport;

/* Compare EVERY supplied element, never only the argmax or sampled token.
 * A component qualifies when |candidate-reference| <= absoluteTolerance +
 * relativeTolerance * max(|candidate|,|reference|). Calculations use double;
 * finite FP32 input and nonnegative finite tolerances are mandatory.
 * A legitimate numerical mismatch returns UMI_STATUS_OK with
 * withinTolerance=0 and diagnostic counts; invalid arguments/nonfinite values
 * return a non-OK status and leave *outReport unchanged. Buffers must not
 * overlap the report object. No allocations or global state are used. */
UmiStatus UmiAiTrainingInferenceParityCompare(
    const float *reference, const float *candidate, size_t elementCount,
    const UmiAiTrainingInferenceParityTolerance *tolerance,
    UmiAiTrainingInferenceParityReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_INFERENCE_PARITY_H */
