/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/inference_parity.c
 *
 * PURPOSE:
 *   Provide a bounded and failure-atomic numerical comparison for canonical
 *   and incremental inference evidence in native C23.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_parity.h"

#include <math.h>
#include <stdint.h>

static int ParityOverlaps(const void *left, size_t leftBytes,
                          const void *right, size_t rightBytes)
{
    const uintptr_t a = (uintptr_t)left;
    const uintptr_t b = (uintptr_t)right;
    if (a > UINTPTR_MAX - leftBytes || b > UINTPTR_MAX - rightBytes)
        return 1;
    return a < b + rightBytes && b < a + leftBytes;
}

UmiStatus UmiAiTrainingInferenceParityCompare(
    const float *reference, const float *candidate, size_t elementCount,
    const UmiAiTrainingInferenceParityTolerance *tolerance,
    UmiAiTrainingInferenceParityReport *outReport)
{
    UmiAiTrainingInferenceParityReport staged = {0};
    if (reference == NULL || candidate == NULL || tolerance == NULL ||
        outReport == NULL || elementCount == 0U ||
        elementCount > SIZE_MAX / sizeof(float) ||
        !isfinite(tolerance->absoluteTolerance) ||
        !isfinite(tolerance->relativeTolerance) ||
        tolerance->absoluteTolerance < 0.0F ||
        tolerance->relativeTolerance < 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    const size_t dataBytes = elementCount * sizeof(float);
    if (ParityOverlaps(reference, dataBytes, outReport, sizeof(*outReport)) ||
        ParityOverlaps(candidate, dataBytes, outReport, sizeof(*outReport)))
        return UMI_STATUS_INVALID_ARGUMENT;
    staged.valuesCompared = elementCount;
    staged.firstMismatchIndex = SIZE_MAX;
    staged.withinTolerance = 1;
    for (size_t i = 0U; i < elementCount; ++i) {
        if (!isfinite(reference[i]) || !isfinite(candidate[i]))
            return UMI_STATUS_INVALID_ARGUMENT;
        const double a = (double)reference[i];
        const double b = (double)candidate[i];
        const double delta = fabs(b - a);
        const double scale = fmax(fabs(a), fabs(b));
        const double relative = scale == 0.0 ? 0.0 : delta / scale;
        const double permitted = (double)tolerance->absoluteTolerance +
            (double)tolerance->relativeTolerance * scale;
        if (delta > staged.maximumAbsoluteDifference)
            staged.maximumAbsoluteDifference = delta;
        if (relative > staged.maximumRelativeDifference)
            staged.maximumRelativeDifference = relative;
        if (delta > permitted) {
            if (staged.mismatchedValues == 0U)
                staged.firstMismatchIndex = i;
            ++staged.mismatchedValues;
            staged.withinTolerance = 0;
        }
    }
    *outReport = staged;
    return UMI_STATUS_OK;
}
