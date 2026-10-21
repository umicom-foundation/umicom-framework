/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/matrix_kernel.h
 *
 * PURPOSE:
 *   Define a reusable CPU matrix multiplication kernel for native model
 *   training and inference. Expose deterministic reference and cache-aware
 *   column-tiled implementations behind one stable C23 ABI.
 *
 * NUMERICAL AND LIFETIME CONTRACT:
 *   Matrices are contiguous, row-major float32 arrays. Each output dot
 *   product accumulates in double in increasing inner-index order, then
 *   rounds once to float32, matching the existing training tape.
 *   No global configuration, heap allocation, threads, SIMD instruction
 *   requirement, or application-specific implementation is introduced.
 *   All inputs and outputs are caller-owned. Output publication is atomic:
 *   failures leave the output and optional work estimate unchanged. Aliased
 *   input/output arrays are supported through bounded staging storage.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_MATRIX_KERNEL_H
#define UMICOM_AI_TRAINING_MATRIX_KERNEL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_MATRIX_MAX_ELEMENTS 4096U
#define UMI_AI_MATRIX_COLUMN_TILE 16U

typedef enum UmiAiTrainingMatrixMethod {
    UMI_AI_MATRIX_REFERENCE = 1,
    UMI_AI_MATRIX_TILED = 2,
    /* Stable conservative dispatch; never changes numerical semantics. */
    UMI_AI_MATRIX_AUTO = 3
} UmiAiTrainingMatrixMethod;

typedef struct UmiAiTrainingMatrixEstimate {
    /* Algorithmic arithmetic estimate; it is not a CPU instruction count. */
    uint64_t multiplyAccumulateCount;
    uint64_t nominalFloatingOperations;
    uint64_t inputBytes;
    uint64_t weightBytes;
    uint64_t outputBytes;
    uint64_t temporaryBytes;
} UmiAiTrainingMatrixEstimate;

/* Compute the nominal work and bounded temporary output size for
 * [rows, inner] x [inner, columns] without reading matrix memory.
 * All three matrices must contain 1..UMI_AI_MATRIX_MAX_ELEMENTS values.
 * Rejection never changes the caller's estimate. */
UmiStatus UmiAiTrainingMatrixEstimateWork(
    size_t rows, size_t inner, size_t columns,
    UmiAiTrainingMatrixEstimate *outEstimate);

/* Produce a complete row-major [rows, columns] result; capacity counts
 * FLOATS. The reference path preserves the original column-by-column loop.
 * The tiled path processes adjacent columns together to improve weight-row
 * locality, while preserving each dot product's summation order.
 * AUTO prefers the reference loop for short single-row wide projections and
 * the tiled loop otherwise. This conservative CPU heuristic is not a promise
 * of superior elapsed time on every machine.
 * This function only calculates forward values: the training tape remains
 * authoritative for automatic differentiation and optimiser updates. */
UmiStatus UmiAiTrainingMatrixMultiply(
    const float *left, size_t rows, size_t inner,
    const float *right, size_t columns,
    UmiAiTrainingMatrixMethod method,
    float *outValues, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_MATRIX_KERNEL_H */
