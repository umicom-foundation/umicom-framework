/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workflow/euler_step.c
 *
 * PURPOSE:
 *   Apply an allocation-free denoising update after complete numerical admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai_workflow/euler_step.h"
#include <math.h>
#include <float.h>
static double EulerValue(float sample, float denoised, double ratio)
{
    return ratio * (double)sample + (1.0 - ratio) * (double)denoised;
}
UmiStatus UmiAiEulerDenoisedStep(const float *sample, const float *denoised,
    size_t count, double sigma, double next_sigma, float *out, size_t capacity)
{
    if (!sample || !denoised || !out || count == 0 || count > UMI_AI_EULER_MAX_VALUES ||
        !isfinite(sigma) || !isfinite(next_sigma) || sigma <= 0 ||
        next_sigma < 0 || next_sigma >= sigma)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    const double ratio = next_sigma / sigma;
    for (size_t i=0; i<count; ++i) {
        if (!isfinite(sample[i]) || !isfinite(denoised[i])) return UMI_STATUS_INVALID_ARGUMENT;
        const double value = EulerValue(sample[i], denoised[i], ratio);
        if (!isfinite(value) || value > FLT_MAX || value < -FLT_MAX)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Recompute admitted values to avoid a tensor-sized scratch allocation.
     * Exact in-place updates are safe: each output reads only its own inputs. */
    for (size_t i=0; i<count; ++i) out[i]=(float)EulerValue(sample[i], denoised[i], ratio);
    return UMI_STATUS_OK;
}
