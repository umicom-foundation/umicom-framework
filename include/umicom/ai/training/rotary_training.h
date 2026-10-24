/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/rotary_training.h
 *
 * PURPOSE:
 *   Qualify the differentiable RoPE boundary before adding a separately
 *   trained rotary decoder. This is a mathematical forward/adjoint adapter
 *   for projected query/key tensors, NOT a second neural-network trainer.
 *
 * ARCHITECTURE AND OWNERSHIP:
 *   The existing Framework RoPE primitive owns rotations. This module
 *   composes that primitive over bounded [batch*time, heads*headChannels]
 *   tensors and computes the analytical gradient of a rotary query/key dot.
 *   A later training-tape operator can use the same derivative contract.
 *   No model weights, checkpoints, tokeniser, optimiser, RNG or historic
 *   generation behaviour are changed. The existing tape/decoder remain the
 *   sole owners of their respective training and model semantics.
 *
 *   Buffers belong to the caller; inputs may alias their own output because
 *   computed tensors are staged until validation succeeds. For DotBackward,
 *   the TWO output ranges must not overlap each other. All other rejected
 *   calls preserve caller outputs byte-for-byte. No global state is retained.
 *
 * LIMITS:
 *   The educational shape is bounded by the Framework tensor maximum,
 *   at most 32 total channels and at most 32 time steps per sequence.
 *   All absolute positions are bounded by the existing RoPE service.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_ROTARY_TRAINING_H
#define UMICOM_AI_TRAINING_ROTARY_TRAINING_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/rotary_position.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_ROTARY_TRAINING_MAX_ELEMENTS 16384U

/* Differentiate the unscaled scalar product:
 *   d = dot(R(posQ) * query, R(posK) * key)
 * d/dquery = upstream * R(posQ)^T * R(posK) * key
 * d/dkey   = upstream * R(posK)^T * R(posQ) * query.
 * All gradients and inputs are float32 vectors; accumulation is in double.
 * Upstream is the finite dLoss/dDot scalar from the owning training graph.
 * The two output ranges must not overlap. Inputs may alias outputs; publication
 * occurs only after both candidates are complete and representable as FP32. */
UmiStatus UmiAiTrainingRotaryDotBackward(
    const UmiAiTrainingRotaryConfig *config,
    const float *query, const float *key, size_t channelCount,
    uint64_t queryPosition, uint64_t keyPosition, double upstream,
    float *outQueryGradient, size_t queryCapacity,
    float *outKeyGradient, size_t keyCapacity);

/* Apply the same RoPE mathematical owner to a projected multi-head tensor.
 * Layout: [batchCount * sequenceLength, attentionHeads * headChannels].
 * Each sequence begins at its corresponding startPositions[batchIndex].
 * NULL startPositions with startCount=0 starts EVERY sequence at position 0.
 * startCount otherwise MUST equal batchCount. Input/output may alias.
 * This function creates no training-tape node; the next milestone attaches
 * the validated pair of forward and adjoint operations to the canonical tape.
 */
UmiStatus UmiAiTrainingRotaryTensorForward(
    const UmiAiTrainingRotaryConfig *config,
    const float *projected, size_t tokenRows,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    float *outRotated, size_t capacity);

/* Jacobian-transpose action for the same tensor shape and position policy.
 * Input is dLoss/dRotated, output is dLoss/dProjected. For each time/head,
 * this is the TRANSPOSE of the forward rotation, not a second forward RoPE.
 * The numerical operation is exact up to float32 rounding and is validated
 * by finite-difference and inner-product regression tests. */
UmiStatus UmiAiTrainingRotaryTensorBackward(
    const UmiAiTrainingRotaryConfig *config,
    const float *rotatedGradient, size_t tokenRows,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    float *outProjectedGradient, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_ROTARY_TRAINING_H */
