/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/rotary_training.c
 * PURPOSE:
 *   Native C23 differentiable rotary query/key mathematics and bounded
 *   multi-head tensor orchestration for a future trainable RoPE decoder.
 *
 * ENGINEERING:
 *   Forward and adjoint rotations are delegated to rotary_position.c. No
 *   attention softmax, optimiser, sampling or decoder is copied here. The
 *   existing training tape will be the owner of trainable graph integration;
 *   this module proves its required forward/adjoint mathematics beforehand.
 *   All caller outputs are failure-atomic and no global state is retained.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_training.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Unlike equality-only checks, reject PARTIALLY overlapping output ranges.
 * Input/output aliasing is safe because every result is staged first. */
static int OutputRangesOverlap(const float *left, const float *right,
                               size_t elements)
{
    const uintptr_t a = (uintptr_t)(const void *)left;
    const uintptr_t b = (uintptr_t)(const void *)right;
    const size_t bytes = elements * sizeof(float); /* max 32 * sizeof(float) */
    return (a >= b ? (a - b) < bytes : (b - a) < bytes);
}

UmiStatus UmiAiTrainingRotaryDotBackward(
    const UmiAiTrainingRotaryConfig *config,
    const float *query, const float *key, size_t channelCount,
    uint64_t queryPosition, uint64_t keyPosition, double upstream,
    float *outQueryGradient, size_t queryCapacity,
    float *outKeyGradient, size_t keyCapacity)
{
    float rotatedQuery[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float rotatedKey[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float queryGradient[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float keyGradient[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    UmiStatus status = UmiAiTrainingRotaryConfigValidate(config);
    if (status != UMI_STATUS_OK) return status;
    if (query == NULL || key == NULL || outQueryGradient == NULL ||
        outKeyGradient == NULL || channelCount != config->headChannels ||
        queryPosition > UMI_AI_ROTARY_MAX_POSITION ||
        keyPosition > UMI_AI_ROTARY_MAX_POSITION || !isfinite(upstream))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (queryCapacity < channelCount || keyCapacity < channelCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (OutputRangesOverlap(outQueryGradient, outKeyGradient, channelCount))
        return UMI_STATUS_INVALID_ARGUMENT;

    /* For d=dot(Rq*q,Rk*k), d(d)/dq = Rq^T*(Rk*k).
     * The Framework's existing rotary owner performs every rotation. */
    status = UmiAiTrainingRotaryRotate(config, query, channelCount,
               queryPosition, rotatedQuery, channelCount);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingRotaryRotate(config, key, channelCount,
               keyPosition, rotatedKey, channelCount);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingRotaryRotateAdjoint(config, rotatedKey,
               channelCount, queryPosition, queryGradient, channelCount);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingRotaryRotateAdjoint(config, rotatedQuery,
               channelCount, keyPosition, keyGradient, channelCount);
    if (status != UMI_STATUS_OK) return status;

    for (size_t i = 0U; i < channelCount; ++i) {
        const double q = upstream * (double)queryGradient[i];
        const double k = upstream * (double)keyGradient[i];
        if (!isfinite(q) || !isfinite(k) ||
            q < -(double)FLT_MAX || q > (double)FLT_MAX ||
            k < -(double)FLT_MAX || k > (double)FLT_MAX)
            return UMI_STATUS_INVALID_STATE;
        queryGradient[i] = (float)q;
        keyGradient[i] = (float)k;
    }
    /* At this point both complete candidate gradients are valid. */
    memcpy(outQueryGradient, queryGradient, channelCount * sizeof(float));
    memcpy(outKeyGradient, keyGradient, channelCount * sizeof(float));
    return UMI_STATUS_OK;
}

/* Return shape data only if every dimension, position timeline, and byte
 * allocation is safely representable. No values or outputs are touched. */
static UmiStatus ValidateTensorCall(
    const UmiAiTrainingRotaryConfig *config,
    const float *input, size_t tokenRows,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    float *output, size_t capacity,
    size_t *outColumns, size_t *outElements)
{
    UmiStatus status = UmiAiTrainingRotaryConfigValidate(config);
    if (status != UMI_STATUS_OK) return status;
    if (input == NULL || output == NULL || outColumns == NULL ||
        outElements == NULL || tokenRows == 0U || sequenceLength == 0U ||
        sequenceLength > UMI_AI_ROTARY_MAX_CONTEXT ||
        attentionHeads == 0U ||
        attentionHeads > UMI_AI_ROTARY_MAX_CHANNELS / config->headChannels ||
        tokenRows % sequenceLength != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;

    const size_t batches = tokenRows / sequenceLength;
    if ((startPositions == NULL && startCount != 0U) ||
        (startPositions != NULL && startCount != batches))
        return UMI_STATUS_INVALID_ARGUMENT;
    const size_t columns = attentionHeads * config->headChannels;
    if (tokenRows > UMI_AI_ROTARY_TRAINING_MAX_ELEMENTS / columns)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t elements = tokenRows * columns;
    if (capacity < elements) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (startPositions != NULL) {
        for (size_t batch = 0U; batch < batches; ++batch) {
            if (startPositions[batch] >
                UMI_AI_ROTARY_MAX_POSITION - (uint64_t)(sequenceLength - 1U))
                return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    *outColumns = columns;
    *outElements = elements;
    return UMI_STATUS_OK;
}

static UmiStatus ApplyTensor(
    const UmiAiTrainingRotaryConfig *config,
    const float *input, size_t tokenRows,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    float *output, size_t capacity, int backward)
{
    size_t columns = 0U, elements = 0U;
    float *candidate;
    UmiStatus status = ValidateTensorCall(config, input, tokenRows,
        sequenceLength, attentionHeads, startPositions, startCount,
        output, capacity, &columns, &elements);
    if (status != UMI_STATUS_OK) return status;
    candidate = malloc(elements * sizeof(float));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    for (size_t row = 0U; row < tokenRows; ++row) {
        const size_t sequence = row / sequenceLength;
        const uint64_t first = startPositions == NULL ? 0U :
                                   startPositions[sequence];
        const uint64_t absolutePosition =
            first + (uint64_t)(row % sequenceLength);
        for (size_t head = 0U; head < attentionHeads; ++head) {
            const size_t offset = row * columns + head * config->headChannels;
            if (backward) {
                status = UmiAiTrainingRotaryRotateAdjoint(config,
                    &input[offset], config->headChannels,
                    absolutePosition, &candidate[offset], config->headChannels);
            } else {
                status = UmiAiTrainingRotaryRotate(config,
                    &input[offset], config->headChannels,
                    absolutePosition, &candidate[offset], config->headChannels);
            }
            if (status != UMI_STATUS_OK) {
                free(candidate);
                return status;
            }
        }
    }
    /* A complete row/head tensor commits once. In-place calls are safe. */
    memcpy(output, candidate, elements * sizeof(float));
    free(candidate);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRotaryTensorForward(
    const UmiAiTrainingRotaryConfig *config,
    const float *projected, size_t tokenRows,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    float *outRotated, size_t capacity)
{
    return ApplyTensor(config, projected, tokenRows, sequenceLength,
        attentionHeads, startPositions, startCount,
        outRotated, capacity, 0);
}

UmiStatus UmiAiTrainingRotaryTensorBackward(
    const UmiAiTrainingRotaryConfig *config,
    const float *rotatedGradient, size_t tokenRows,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    float *outProjectedGradient, size_t capacity)
{
    return ApplyTensor(config, rotatedGradient, tokenRows, sequenceLength,
        attentionHeads, startPositions, startCount,
        outProjectedGradient, capacity, 1);
}
