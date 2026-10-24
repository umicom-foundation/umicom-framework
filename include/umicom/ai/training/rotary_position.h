/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/rotary_position.h
 *
 * PURPOSE:
 *   Define an experimental, standalone rotary-position (RoPE) mathematical
 *   primitive for a FUTURE separately trained decoder architecture. This is
 *   not an inference adapter for the existing learned-absolute checkpoint.
 *
 * ENGINEERING CONTRACT:
 *   The existing decoder, KV cache and checkpoint/position-admission services
 *   are not changed. Position indices are absolute, nonnegative token indices.
 *   Each consecutive pair of the first rotaryChannels receives the usual
 *   two-dimensional rotation. Remaining channels are copied unchanged.
 *   Inputs, including all components, must be finite. Callers serialise access
 *   to their own buffers; no state or global RNG is retained by this service.
 *
 *   Overlapping input/output vectors (including in-place calls) are supported
 *   through local staging. All rejections leave the output byte-for-byte
 *   unchanged. This is numerical research, not a checkpoint converter.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_ROTARY_POSITION_H
#define UMICOM_AI_TRAINING_ROTARY_POSITION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Match the bounded educational decoder's maximum channel budget without
 * changing that decoder's public constants or training tensor layout. */
#define UMI_AI_ROTARY_MAX_CHANNELS 32U
#define UMI_AI_ROTARY_MAX_CONTEXT 32U
#define UMI_AI_ROTARY_MAX_POSITION UINT64_C(1048576)

typedef struct UmiAiTrainingRotaryConfig {
    size_t headChannels;    /* 2..32 channels in one projected attention head */
    size_t rotaryChannels;  /* even, 2..headChannels; tail is untouched */
    double frequencyBase;   /* finite, 2..1000000; e.g. 10000 */
} UmiAiTrainingRotaryConfig;

UmiStatus UmiAiTrainingRotaryConfigValidate(
    const UmiAiTrainingRotaryConfig *config);

/* Rotates the first rotaryChannels as adjacent pairs (x0,x1), (x2,x3), ...
 * with angle = position / pow(frequencyBase, 2*pair/rotaryChannels).
 * The remaining channels are passed through exactly. capacity and inputCount
 * are FLOAT counts, not byte lengths. A result is published atomically.
 * Positions > MAX_POSITION are rejected to limit angular reduction error. */
UmiStatus UmiAiTrainingRotaryRotate(
    const UmiAiTrainingRotaryConfig *config,
    const float *input, size_t inputCount, uint64_t absolutePosition,
    float *outRotated, size_t capacity);

/* Return the unscaled attention dot product of two rotated vectors. A caller
 * implementing attention applies the 1/sqrt(headChannels) scale exactly once.
 * An equal shift of both absolute positions should leave the mathematical
 * dot product invariant; floating-point results may differ slightly.
 * Rejections preserve *outDot; no decoder is instantiated or modified. */
UmiStatus UmiAiTrainingRotaryRelativeDot(
    const UmiAiTrainingRotaryConfig *config,
    const float *query, const float *key, size_t channelCount,
    uint64_t queryPosition, uint64_t keyPosition, double *outDot);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_ROTARY_POSITION_H */
