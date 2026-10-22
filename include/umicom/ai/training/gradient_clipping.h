/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/gradient_clipping.h
 *
 * PURPOSE:
 *   Measure a global gradient L2 norm and plan a bounded clipping factor
 *   across the canonical training tape's parameter arrays.
 *
 * OWNERSHIP:
 *   The borrowed arrays remain owned by the caller. This inspection does not
 *   allocate, modify gradients, update model weights or consume random state.
 *   Non-finite inputs and invalid shapes leave the result unchanged.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_GRADIENT_CLIPPING_H
#define UMICOM_AI_TRAINING_GRADIENT_CLIPPING_H

#include <stddef.h>
#include "umicom/ai/training/tape.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingGradientClipReport {
    double globalNorm;
    double appliedScale;
    size_t gradientElements;
    int wasClipped;
} UmiAiTrainingGradientClipReport;

/* Compute the global Euclidean norm over all supplied finite FP32 gradients.
 * The scaled sum-of-squares algorithm avoids overflow in intermediate sums;
 * all vector entries remain FP32 and the calculation uses double precision.
 *
 * maxNorm must be positive and finite. Scale = min(1, maxNorm / norm), with
 * scale=1 for all-zero gradients. Dimensions obey the tape's tensor bounds.
 * A rejected call does not modify any input or *outReport.
 * This function reports a prospective clipping decision; it does not apply
 * an optimiser update or claim to detect whether training is converging. */
UmiStatus UmiAiTrainingGradientGlobalNorm(
    const float *const *gradients, const size_t *elementCounts,
    size_t arrayCount, float maxNorm,
    UmiAiTrainingGradientClipReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_GRADIENT_CLIPPING_H */
