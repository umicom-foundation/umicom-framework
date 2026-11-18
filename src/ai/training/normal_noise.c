/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/normal_noise.c
 *
 * PURPOSE:
 *   Transform the shared seeded integer RNG into bounded transactional normal samples.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/initialisation.h"
#include "umicom/ai/training/tape.h"
#include "umicom/security/secrets.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* A midpoint-like open interval avoids log(0) without retrying draws or using
 * a platform RNG. The denominator is exactly represented in double. */
static double NextNormalOpenUnit(UmiAiTrainingRandomState *randomState)
{
    uint64_t sample = 0U;
    /* Valid state and a fixed nonzero power-of-two bound make this request
     * infallible. Reuse the existing integer RNG instead of duplicating it. */
    (void)UmiAiTrainingRandomIndex(randomState, UINT64_C(16777216), &sample);
    return ((double)sample + 1.0) / 16777217.0;
}

UmiStatus UmiAiTrainingRandomFillNormal(UmiAiTrainingRandomState *randomState,
    float mean, float standardDeviation, float *values, size_t count)
{
    if (randomState == NULL || !isfinite(mean) || !isfinite(standardDeviation) ||
        standardDeviation < 0.0F || (count > 0U && values == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_AI_TRAINING_MAX_ELEMENTS || count > SIZE_MAX / sizeof(float))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count == 0U) return UMI_STATUS_OK;
    float *candidate = malloc(count * sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiAiTrainingRandomState proposed = *randomState;
    UmiStatus status = UMI_STATUS_OK;
    const double fullTurn = 6.283185307179586476925286766559;
    for (size_t index = 0U; index < count;) {
        double first = 0.0, second = 0.0;
        if (standardDeviation > 0.0F) {
            double radius = sqrt(-2.0 * log(NextNormalOpenUnit(&proposed)));
            double angle = fullTurn * NextNormalOpenUnit(&proposed);
            first = radius * cos(angle);
            second = radius * sin(angle);
        }
        /* Evaluate both pair positions locally, publishing only after every
         * requested sample is representable. Discard an unused odd partner. */
        for (size_t member = 0U; member < 2U && index < count; ++member, ++index) {
            double normal = member == 0U ? first : second;
            double value = (double)mean + (double)standardDeviation * normal;
            if (!isfinite(value) || value < -(double)FLT_MAX || value > (double)FLT_MAX) {
                status = UMI_STATUS_INVALID_ARGUMENT;
                goto finished_normal;
            }
            candidate[index] = (float)value;
        }
    }
    memcpy(values, candidate, count * sizeof(*values));
    *randomState = proposed;
finished_normal:
    /* Reuse Framework clearing across C runtimes that do not yet provide
     * the C23 explicit-memory-clear library function. */
    umi_secret_clear(candidate, count * sizeof(*candidate));
    free(candidate);
    return status;
}
