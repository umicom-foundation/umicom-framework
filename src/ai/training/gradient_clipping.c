/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/gradient_clipping.c
 *
 * PURPOSE:
 *   Numerically stable, failure-atomic global gradient norm measurement.
 *   Reusable optimiser policy belongs to Framework, not individual apps.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/gradient_clipping.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

UmiStatus UmiAiTrainingGradientGlobalNorm(
    const float *const *gradients, const size_t *elementCounts,
    size_t arrayCount, float maxNorm,
    UmiAiTrainingGradientClipReport *outReport)
{
    UmiAiTrainingGradientClipReport candidate = {0};
    double scale = 0.0;
    double squares = 1.0;
    size_t total = 0U;

    if (gradients == NULL || elementCounts == NULL || outReport == NULL ||
        arrayCount == 0U || arrayCount > UMI_AI_TRAINING_MAX_TENSORS ||
        !isfinite(maxNorm) || maxNorm <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* BLAS-style scaled accumulation is stable even when |g| is close to the
     * largest finite FP32 value. No intermediary is squared in FP32. */
    for (size_t group = 0U; group < arrayCount; ++group) {
        size_t count = elementCounts[group];
        if (gradients[group] == NULL || count == 0U ||
            count > UMI_AI_TRAINING_MAX_ELEMENTS ||
            count > SIZE_MAX - total)
            return UMI_STATUS_INVALID_ARGUMENT;
        total += count;
        for (size_t element = 0U; element < count; ++element) {
            double absolute = fabs((double)gradients[group][element]);
            if (!isfinite(absolute)) return UMI_STATUS_INVALID_ARGUMENT;
            if (absolute == 0.0) continue;
            if (scale < absolute) {
                double ratio = scale / absolute;
                squares = 1.0 + squares * ratio * ratio;
                scale = absolute;
            } else {
                double ratio = absolute / scale;
                squares += ratio * ratio;
            }
        }
    }
    candidate.globalNorm = scale == 0.0 ? 0.0 : scale * sqrt(squares);
    if (!isfinite(candidate.globalNorm)) return UMI_STATUS_INVALID_STATE;
    candidate.wasClipped = candidate.globalNorm > (double)maxNorm;
    candidate.appliedScale = candidate.wasClipped ?
        (double)maxNorm / candidate.globalNorm : 1.0;
    if (!isfinite(candidate.appliedScale) ||
        candidate.appliedScale <= 0.0 || candidate.appliedScale > 1.0)
        return UMI_STATUS_INVALID_STATE;
    candidate.gradientElements = total;
    *outReport = candidate;
    return UMI_STATUS_OK;
}
