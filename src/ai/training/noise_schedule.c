/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/noise_schedule.c
 *
 * PURPOSE:
 *   Validate an entire caller-selected noise schedule before publishing cumulative signal coefficients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/denoising.h"
#include <float.h>
#include <math.h>

UmiStatus UmiAiDenoisingScheduleFromBetas(const float *betas, size_t count,
    float *outAlphaBars, size_t capacity)
{
    if (betas == NULL || outAlphaBars == NULL || count == 0U ||
        count > UMI_AI_DENOISING_MAX_STEPS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Framework owns the conversion so consumers cannot confuse the step's
     * beta with cumulative signal. Double intermediates reduce accumulation
     * error; rejecting underflow makes the float output contract explicit.
     * Validate first, then repeat this bounded arithmetic without allocating. */
    double product = 1.0;
    for (size_t i = 0U; i < count; ++i) {
        if (!isfinite(betas[i]) || betas[i] < 0.0F || betas[i] >= 1.0F)
            return UMI_STATUS_INVALID_ARGUMENT;
        product *= 1.0 - (double)betas[i];
        if (!isfinite(product) || product < (double)FLT_MIN)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    product = 1.0;
    for (size_t i = 0U; i < count; ++i) {
        product *= 1.0 - (double)betas[i];
        outAlphaBars[i] = (float)product;
    }
    return UMI_STATUS_OK;
}
