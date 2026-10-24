/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/rotary_position.c
 * PURPOSE:
 *   Bounded C23 RoPE kernel for the opt-in positional-research laboratory.
 *   It does not reinterpret old positional embeddings or legacy checkpoints.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_position.h"

#include <float.h>
#include <math.h>
#include <string.h>

UmiStatus UmiAiTrainingRotaryConfigValidate(
    const UmiAiTrainingRotaryConfig *config)
{
    if (config == NULL ||
        config->headChannels < 2U ||
        config->headChannels > UMI_AI_ROTARY_MAX_CHANNELS ||
        config->rotaryChannels < 2U ||
        config->rotaryChannels > config->headChannels ||
        (config->rotaryChannels % 2U) != 0U ||
        !isfinite(config->frequencyBase) ||
        config->frequencyBase < 2.0 ||
        config->frequencyBase > 1000000.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRotaryRotate(
    const UmiAiTrainingRotaryConfig *config,
    const float *input, size_t inputCount, uint64_t absolutePosition,
    float *outRotated, size_t capacity)
{
    float proposed[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    UmiStatus status = UmiAiTrainingRotaryConfigValidate(config);
    if (status != UMI_STATUS_OK) return status;
    if (input == NULL || outRotated == NULL ||
        inputCount != config->headChannels ||
        absolutePosition > UMI_AI_ROTARY_MAX_POSITION)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < config->headChannels) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < inputCount; ++i)
        if (!isfinite(input[i])) return UMI_STATUS_INVALID_ARGUMENT;

    memcpy(proposed, input, inputCount * sizeof(float));
    for (size_t i = 0U; i < config->rotaryChannels; i += 2U) {
        /* Materialise every output as FP32. No change to existing decoder's
         * projection arithmetic or its learned position embedding matrices. */
        const double exponent = (double)i / (double)config->rotaryChannels;
        const double angularFrequency = pow(config->frequencyBase, -exponent);
        const double angle = (double)absolutePosition * angularFrequency;
        const double sine = sin(angle);
        const double cosine = cos(angle);
        const double x = (double)input[i];
        const double y = (double)input[i + 1U];
        const double a = x * cosine - y * sine;
        const double b = x * sine + y * cosine;
        if (!isfinite(a) || !isfinite(b) ||
            a < -(double)FLT_MAX || a > (double)FLT_MAX ||
            b < -(double)FLT_MAX || b > (double)FLT_MAX)
            return UMI_STATUS_INVALID_STATE;
        proposed[i] = (float)a;
        proposed[i + 1U] = (float)b;
    }
    memcpy(outRotated, proposed, inputCount * sizeof(float));
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRotaryRelativeDot(
    const UmiAiTrainingRotaryConfig *config,
    const float *query, const float *key, size_t channelCount,
    uint64_t queryPosition, uint64_t keyPosition, double *outDot)
{
    float queryRotated[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float keyRotated[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    double dot = 0.0;
    UmiStatus status;
    if (outDot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingRotaryRotate(config, query, channelCount,
                                      queryPosition, queryRotated,
                                      UMI_AI_ROTARY_MAX_CHANNELS);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingRotaryRotate(config, key, channelCount,
                                      keyPosition, keyRotated,
                                      UMI_AI_ROTARY_MAX_CHANNELS);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < channelCount; ++i)
        dot += (double)queryRotated[i] * (double)keyRotated[i];
    if (!isfinite(dot)) return UMI_STATUS_INVALID_STATE;
    *outDot = dot;
    return UMI_STATUS_OK;
}

/* The transpose of each 2x2 rotary matrix is its inverse. This adjoint is
 * NOT a learned-position gradient or a new inference kernel. It is the exact
 * derivative required when an enclosing Framework training operator must
 * propagate gradients through RoPE. The established forward function above
 * is intentionally preserved verbatim for compatibility review. */
UmiStatus UmiAiTrainingRotaryRotateAdjoint(
    const UmiAiTrainingRotaryConfig *config,
    const float *rotatedGradient, size_t channelCount,
    uint64_t absolutePosition, float *outInputGradient, size_t capacity)
{
    float staged[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    UmiStatus status = UmiAiTrainingRotaryConfigValidate(config);
    if (status != UMI_STATUS_OK) return status;
    if (rotatedGradient == NULL || outInputGradient == NULL ||
        channelCount != config->headChannels ||
        absolutePosition > UMI_AI_ROTARY_MAX_POSITION)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < channelCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < channelCount; ++i)
        if (!isfinite(rotatedGradient[i])) return UMI_STATUS_INVALID_ARGUMENT;

    /* A detached staging array permits output=input without changing input
     * before all numerical and capacity checks have completed. */
    memcpy(staged, rotatedGradient, channelCount * sizeof(float));
    for (size_t i = 0U; i < config->rotaryChannels; i += 2U) {
        /* This is the same positional frequency used by Rotate above.
         * Maintain a single canonical positional convention in this source. */
        const double exponent = (double)i / (double)config->rotaryChannels;
        const double angularFrequency = pow(config->frequencyBase, -exponent);
        const double angle = (double)absolutePosition * angularFrequency;
        const double sine = sin(angle);
        const double cosine = cos(angle);
        const double x = (double)rotatedGradient[i];
        const double y = (double)rotatedGradient[i + 1U];
        const double a = x * cosine + y * sine;
        const double b = -x * sine + y * cosine;
        if (!isfinite(a) || !isfinite(b) ||
            a < -(double)FLT_MAX || a > (double)FLT_MAX ||
            b < -(double)FLT_MAX || b > (double)FLT_MAX)
            return UMI_STATUS_INVALID_STATE;
        staged[i] = (float)a;
        staged[i + 1U] = (float)b;
    }
    memcpy(outInputGradient, staged, channelCount * sizeof(float));
    return UMI_STATUS_OK;
}
