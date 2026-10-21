/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/initialisation.c
 *
 * PURPOSE:
 *   Seeded SplitMix64 training parameter initialisation in portable C23.
 *
 * ENGINEERING NOTES:
 *   Integer overflow is deliberate and defined for uint64_t. SplitMix64
 *   provides deterministic teaching samples, NOT secret or secure entropy.
 *   The top 24 bits become an FP32-friendly uniform fraction; the entire
 *   requested array is calculated before the caller's state/buffer is updated.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/initialisation.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define UMICOM_TRAINING_INIT_MAX_VALUES 4096U

static uint64_t NextRandom(UmiAiTrainingRandomState *randomState)
{
    uint64_t value = (randomState->state += UINT64_C(0x9e3779b97f4a7c15));
    value = (value ^ (value >> 30U)) * UINT64_C(0xbf58476d1ce4e5b9);
    value = (value ^ (value >> 27U)) * UINT64_C(0x94d049bb133111eb);
    return value ^ (value >> 31U);
}

/* Only a 24-bit integer is converted to floating point. This is representable
 * exactly in float and avoids different floating-point rounding of uint64_t. */
static float NextUnit(UmiAiTrainingRandomState *randomState)
{
    uint32_t top24 = (uint32_t)(NextRandom(randomState) >> 40U);
    return (float)top24 * (1.0F / 16777216.0F);
}

static bool IsRangeValid(float minimum, float maximum)
{
    return isfinite(minimum) && isfinite(maximum) && minimum < maximum;
}

static UmiStatus MakeSample(UmiAiTrainingRandomState *randomState,
                            float minimum, float maximum, float *outValue)
{
    /* Double intermediates prevent FP32 overflow of a wide valid interval. */
    double fraction = (double)NextUnit(randomState);
    double result = (double)minimum +
                    ((double)maximum - (double)minimum) * fraction;
    if (!isfinite(result)) return UMI_STATUS_INVALID_ARGUMENT;
    float value = (float)result;
    if (!isfinite(value)) return UMI_STATUS_INVALID_ARGUMENT;
    *outValue = value;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRandomSeed(UmiAiTrainingRandomState *randomState,
                                  uint64_t seed)
{
    if (randomState == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    randomState->state = seed;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRandomUniform(UmiAiTrainingRandomState *randomState,
                                     float minimum, float maximum,
                                     float *outValue)
{
    UmiAiTrainingRandomState proposed;
    float value;
    UmiStatus status;
    if (randomState == NULL || outValue == NULL ||
        !IsRangeValid(minimum, maximum)) return UMI_STATUS_INVALID_ARGUMENT;
    proposed = *randomState;
    status = MakeSample(&proposed, minimum, maximum, &value);
    if (status != UMI_STATUS_OK) return status;
    *outValue = value;
    *randomState = proposed;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRandomFillUniform(UmiAiTrainingRandomState *randomState,
                                         float minimum, float maximum,
                                         float *values, size_t count)
{
    UmiAiTrainingRandomState proposed;
    float *temporary;
    if (randomState == NULL || !IsRangeValid(minimum, maximum) ||
        (count > 0U && values == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMICOM_TRAINING_INIT_MAX_VALUES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (count == 0U) return UMI_STATUS_OK;
    temporary = malloc(count * sizeof(*temporary));
    if (temporary == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    proposed = *randomState;
    for (size_t index = 0U; index < count; ++index) {
        UmiStatus status = MakeSample(&proposed, minimum, maximum,
                                     &temporary[index]);
        if (status != UMI_STATUS_OK) {
            free(temporary);
            return status;
        }
    }
    memcpy(values, temporary, count * sizeof(*values));
    *randomState = proposed;
    free(temporary);
    return UMI_STATUS_OK;
}
