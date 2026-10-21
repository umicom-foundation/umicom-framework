/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/matrix_kernel.c
 *
 * PURPOSE:
 *   Portable, bounded C23 reference/tiled matrix multiplication for the
 *   canonical training-tape and incremental-inference implementations.
 *
 * ENGINEERING RATIONALE:
 *   The original operators repeated column-major dot-product loops. Adjacent
 *   outputs can share reads of contiguous weight rows without changing the
 *   mathematical result or associating additions in a different order.
 *   This scalar baseline allows future audited SIMD or Assembly backends to
 *   be compared with one authoritative numerical contract.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/matrix_kernel.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Retain the earlier small-result stack path without reserving the expanded
 * tensor maximum on every matrix call. Larger results use exact heap staging. */
#define UMI_AI_MATRIX_INLINE_ELEMENTS 4096U

static bool IsShapeValid(size_t rows, size_t inner, size_t columns)
{
    return rows > 0U && inner > 0U && columns > 0U &&
           rows <= UMI_AI_MATRIX_MAX_ELEMENTS / inner &&
           inner <= UMI_AI_MATRIX_MAX_ELEMENTS / columns &&
           rows <= UMI_AI_MATRIX_MAX_ELEMENTS / columns;
}

static bool CanRepresent(double value)
{
    return isfinite(value) && value >= -(double)FLT_MAX &&
           value <= (double)FLT_MAX;
}

UmiStatus UmiAiTrainingMatrixEstimateWork(
    size_t rows, size_t inner, size_t columns,
    UmiAiTrainingMatrixEstimate *outEstimate)
{
    UmiAiTrainingMatrixEstimate proposal;
    if (outEstimate == NULL || rows == 0U || inner == 0U || columns == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!IsShapeValid(rows, inner, columns))
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Validated bounded dimensions keep every product safely within uint64_t.
     * All counts describe the logical matrix footprint, not cache traffic. */
    proposal.multiplyAccumulateCount =
        (uint64_t)rows * (uint64_t)inner * (uint64_t)columns;
    proposal.nominalFloatingOperations =
        UINT64_C(2) * proposal.multiplyAccumulateCount;
    proposal.inputBytes = (uint64_t)rows * (uint64_t)inner * sizeof(float);
    proposal.weightBytes = (uint64_t)inner * (uint64_t)columns * sizeof(float);
    proposal.outputBytes = (uint64_t)rows * (uint64_t)columns * sizeof(float);
    proposal.temporaryBytes = proposal.outputBytes;
    *outEstimate = proposal;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingMatrixMultiply(
    const float *left, size_t rows, size_t inner,
    const float *right, size_t columns,
    UmiAiTrainingMatrixMethod method,
    float *outValues, size_t capacity)
{
    float inlineStorage[UMI_AI_MATRIX_INLINE_ELEMENTS];
    float *heapStorage = NULL;
    float *staging = inlineStorage;
    /* Historical fixed-stack workspace is retained as disabled reference.
     * Larger tensor shapes require an allocation sized to the actual result. */
#if 0
    float staging[UMI_AI_MATRIX_MAX_ELEMENTS];
#endif
    const size_t outputCount =
        rows > 0U && columns > 0U && rows <= UMI_AI_MATRIX_MAX_ELEMENTS / columns
            ? rows * columns : 0U;

    if (left == NULL || right == NULL || outValues == NULL ||
        (method != UMI_AI_MATRIX_REFERENCE &&
         method != UMI_AI_MATRIX_TILED && method != UMI_AI_MATRIX_AUTO) ||
        rows == 0U || inner == 0U || columns == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!IsShapeValid(rows, inner, columns))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (capacity < outputCount) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Small one-row projections can be quicker with the original scalar
     * loop. Auto-selection is shape-based and never alters accumulation
     * order; callers may still benchmark either implementation explicitly. */
    if (method == UMI_AI_MATRIX_AUTO) {
        method = (rows == 1U && inner <= 8U && columns >= 128U)
            ? UMI_AI_MATRIX_REFERENCE : UMI_AI_MATRIX_TILED;
    }

    /* Finite input checks precede all computation so malformed model weights
     * cannot cause a partially published output or depend on FP exceptions. */
    for (size_t i = 0U; i < rows * inner; ++i)
        if (!isfinite(left[i])) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < inner * columns; ++i)
        if (!isfinite(right[i])) return UMI_STATUS_INVALID_ARGUMENT;

    /* The original small-result stack fast path is unchanged in substance.
     * Beyond that ceiling allocate exactly the output footprint. Validation
     * above guarantees outputCount*sizeof(float) fits SIZE_MAX. Both paths
     * stage before publication to preserve aliases and failure atomicity. */
    if (outputCount > UMI_AI_MATRIX_INLINE_ELEMENTS) {
        heapStorage = malloc(outputCount * sizeof(*heapStorage));
        if (heapStorage == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        staging = heapStorage;
    }

    for (size_t row = 0U; row < rows; ++row) {
        if (method == UMI_AI_MATRIX_REFERENCE) {
            for (size_t col = 0U; col < columns; ++col) {
                double sum = 0.0;
                for (size_t k = 0U; k < inner; ++k) {
                    sum += (double)left[row * inner + k] *
                           (double)right[k * columns + col];
                }
                if (!CanRepresent(sum)) {
                    free(heapStorage);
                    return UMI_STATUS_INVALID_ARGUMENT;
                }
                staging[row * columns + col] = (float)sum;
            }
        } else {
            for (size_t first = 0U; first < columns;
                 first += UMI_AI_MATRIX_COLUMN_TILE) {
                /* The loop advances only within validated finite dimensions.
                 * Column tiling preserves the original inner summation order. */
                const size_t width = columns - first < UMI_AI_MATRIX_COLUMN_TILE
                    ? columns - first : UMI_AI_MATRIX_COLUMN_TILE;
                double totals[UMI_AI_MATRIX_COLUMN_TILE] = {0};
                for (size_t k = 0U; k < inner; ++k) {
                    const double value = (double)left[row * inner + k];
                    const float *weightRow = &right[k * columns + first];
                    for (size_t col = 0U; col < width; ++col)
                        totals[col] += value * (double)weightRow[col];
                }
                for (size_t col = 0U; col < width; ++col) {
                    if (!CanRepresent(totals[col])) {
                        free(heapStorage);
                        return UMI_STATUS_INVALID_ARGUMENT;
                    }
                    staging[row * columns + first + col] = (float)totals[col];
                }
            }
        }
    }
    memcpy(outValues, staging, outputCount * sizeof(*outValues));
    free(heapStorage);
    return UMI_STATUS_OK;
}
