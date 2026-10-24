/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/rotary_tape.c
 *
 * PURPOSE:
 *   Native C23 trainable rotary tape composition with automatic derivatives.
 *
 * ENGINEERING:
 *   Existing tape operators implement graph edges and reverse-mode gradients;
 *   this module never accesses opaque tape internals. One basis-vector RoPE
 *   call per matrix input column constructs a constant row-specific rotation
 *   matrix. EmbeddingLookup selects one row, Matmul rotates it, and a one-hot
 *   selector/matrix product restores its position in the full tensor.
 *   Summing those rows produces a normal tape tensor: Backward then reaches
 *   upstream projected tensors and their trainable weights automatically.
 *
 *   This intentionally O(rows^2) composition is for qualification, not an
 *   inference speedup. The original tape, transformer, decoder, checkpoint,
 *   and RoPE research services are neither deleted nor altered.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_tape.h"
#include <string.h>

/* A conservative tape-node bound ensures there is room for a teaching loss
 * and at least one other head; real dynamic node capacity is checked by the
 * canonical tape and failure-atomic rollback owns intermediate cleanup. */
static UmiStatus CheckLayout(
    const UmiAiTrainingRotaryConfig *rotary,
    size_t rows, size_t columns,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount)
{
    if (UmiAiTrainingRotaryConfigValidate(rotary) != UMI_STATUS_OK ||
        rows == 0U || rows > UMI_AI_ROTARY_TAPE_MAX_ROWS ||
        sequenceLength == 0U || sequenceLength > rows ||
        rows % sequenceLength != 0U ||
        attentionHeads == 0U ||
        attentionHeads > UMI_AI_ROTARY_TAPE_MAX_CHANNELS / rotary->headChannels ||
        columns != attentionHeads * rotary->headChannels ||
        columns > UMI_AI_ROTARY_TAPE_MAX_CHANNELS)
        return UMI_STATUS_INVALID_ARGUMENT;
    const size_t batchCount = rows / sequenceLength;
    if ((startPositions == NULL && startCount != 0U) ||
        (startPositions != NULL && startCount != batchCount))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t batch = 0U; batch < startCount; ++batch) {
        if (startPositions[batch] >
            UMI_AI_ROTARY_MAX_POSITION - (uint64_t)(sequenceLength - 1U))
            return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

/* Compute M = R(position)^T for the row-vector convention y=x*M.
 * R itself is NOT duplicated here: for every source basis vector e_i,
 * UmiAiTrainingRotaryRotate returns R*e_i; its jth entry equals M[i,j]. */
static UmiStatus BuildRotationMatrix(
    const UmiAiTrainingRotaryConfig *rotary,
    size_t attentionHeads, uint64_t position,
    float matrix[UMI_AI_ROTARY_TAPE_MAX_CHANNELS * UMI_AI_ROTARY_TAPE_MAX_CHANNELS])
{
    const size_t width = rotary->headChannels;
    const size_t channels = width * attentionHeads;
    float basis[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float transformed[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    memset(matrix, 0, sizeof(float) *
                    UMI_AI_ROTARY_TAPE_MAX_CHANNELS * UMI_AI_ROTARY_TAPE_MAX_CHANNELS);
    for (size_t head = 0U; head < attentionHeads; ++head) {
        for (size_t inputChannel = 0U; inputChannel < width; ++inputChannel) {
            UmiStatus status;
            memset(basis, 0, sizeof(basis));
            basis[inputChannel] = 1.0F;
            status = UmiAiTrainingRotaryRotate(rotary,
                basis, width, position, transformed, width);
            if (status != UMI_STATUS_OK) return status;
            const size_t inputIndex = head * width + inputChannel;
            for (size_t outputChannel = 0U; outputChannel < width; ++outputChannel) {
                const size_t outputIndex = head * width + outputChannel;
                matrix[inputIndex * channels + outputIndex] = transformed[outputChannel];
            }
        }
    }
    return UMI_STATUS_OK;
}

/* Add nodes but do not publish an output handle until the entire requested
 * tensor is complete. The caller holds a tape checkpoint and rolls back on
 * any error. Every tape operator is Framework-owned. */
static UmiStatus ComposeRotation(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *projected,
    const UmiAiTrainingRotaryConfig *rotary,
    size_t rows, size_t columns,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions,
    UmiAiTrainingTensor **outCandidate)
{
    UmiAiTrainingTensor *accumulated = NULL;
    for (size_t row = 0U; row < rows; ++row) {
        const size_t batch = row / sequenceLength;
        const uint64_t start = startPositions == NULL ? 0U : startPositions[batch];
        const uint64_t absolutePosition = start + (uint64_t)(row % sequenceLength);
        float matrix[UMI_AI_ROTARY_TAPE_MAX_CHANNELS * UMI_AI_ROTARY_TAPE_MAX_CHANNELS];
        float selector[UMI_AI_ROTARY_TAPE_MAX_ROWS] = {0};
        UmiAiTrainingTensor *selectedRow = NULL, *rotation = NULL;
        UmiAiTrainingTensor *rotatedRow = NULL, *mask = NULL;
        UmiAiTrainingTensor *placed = NULL, *sum = NULL;
        UmiStatus status = BuildRotationMatrix(rotary, attentionHeads,
                                              absolutePosition, matrix);
        if (status != UMI_STATUS_OK) return status;
        selector[row] = 1.0F;
        status = UmiAiTrainingEmbeddingLookup(tape, projected,
                     &row, 1U, &selectedRow);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingTensorCreate(tape, columns, columns,
                     matrix, columns * columns, 0, &rotation);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingMatmul(tape, selectedRow, rotation, &rotatedRow);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingTensorCreate(tape, rows, 1U,
                     selector, rows, 0, &mask);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingMatmul(tape, mask, rotatedRow, &placed);
        if (status != UMI_STATUS_OK) return status;
        if (accumulated == NULL) {
            accumulated = placed;
        } else {
            status = UmiAiTrainingAdd(tape, accumulated, placed, &sum);
            if (status != UMI_STATUS_OK) return status;
            accumulated = sum;
        }
    }
    *outCandidate = accumulated;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRotaryTapeApply(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *projected,
    const UmiAiTrainingRotaryConfig *rotary,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    UmiAiTrainingTensor **outRotated)
{
    UmiAiTrainingTensor *candidate = NULL;
    size_t rows = 0U, columns = 0U, checkpoint = 0U;
    UmiStatus status;
    if (tape == NULL || projected == NULL || outRotated == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTensorShape(projected, &rows, &columns);
    if (status != UMI_STATUS_OK) return status;
    status = CheckLayout(rotary, rows, columns, sequenceLength,
                         attentionHeads, startPositions, startCount);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTapeCheckpoint(tape, &checkpoint);
    if (status != UMI_STATUS_OK) return status;
    status = ComposeRotation(tape, projected, rotary, rows, columns,
                             sequenceLength, attentionHeads, startPositions, &candidate);
    if (status == UMI_STATUS_OK) {
        *outRotated = candidate;
        return UMI_STATUS_OK;
    }
    /* TapeRollback preserves the graph prefix, but invalidates any previously
     * computed gradients. This is deliberately documented in the public API. */
    (void)UmiAiTrainingTapeRollback(tape, checkpoint);
    return status;
}

UmiStatus UmiAiTrainingRotaryTapeAttention(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *queries,
    const UmiAiTrainingTensor *keys,
    const UmiAiTrainingTensor *values,
    const UmiAiTrainingRotaryConfig *rotary,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    UmiAiTrainingTensor **outAttended)
{
    UmiAiTrainingTensor *rotatedQueries = NULL;
    UmiAiTrainingTensor *rotatedKeys = NULL;
    UmiAiTrainingTensor *attended = NULL;
    size_t rows = 0U, columns = 0U, keyRows = 0U, keyColumns = 0U;
    size_t valueRows = 0U, valueColumns = 0U, checkpoint = 0U;
    UmiStatus status;
    if (tape == NULL || queries == NULL || keys == NULL ||
        values == NULL || outAttended == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTensorShape(queries, &rows, &columns);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTensorShape(keys, &keyRows, &keyColumns);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTensorShape(values, &valueRows, &valueColumns);
    if (status != UMI_STATUS_OK) return status;
    if (rows != keyRows || rows != valueRows ||
        columns != keyColumns || columns != valueColumns)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = CheckLayout(rotary, rows, columns, sequenceLength,
                         attentionHeads, startPositions, startCount);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTapeCheckpoint(tape, &checkpoint);
    if (status != UMI_STATUS_OK) return status;
    status = ComposeRotation(tape, queries, rotary, rows, columns,
                             sequenceLength, attentionHeads, startPositions, &rotatedQueries);
    if (status != UMI_STATUS_OK) goto rollback;
    status = ComposeRotation(tape, keys, rotary, rows, columns,
                             sequenceLength, attentionHeads, startPositions, &rotatedKeys);
    if (status != UMI_STATUS_OK) goto rollback;
    status = UmiAiTrainingMultiHeadCausalAttention(tape, rotatedQueries,
                       rotatedKeys, values, sequenceLength,
                       attentionHeads, &attended);
    if (status != UMI_STATUS_OK) goto rollback;
    *outAttended = attended;
    return UMI_STATUS_OK;
rollback:
    (void)UmiAiTrainingTapeRollback(tape, checkpoint);
    return status;
}
