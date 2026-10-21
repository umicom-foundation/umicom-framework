/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_wide_matrix.c
 *
 * PURPOSE:
 *   Verify shape-sized CPU matrix workspaces above the original teaching
 *   ceiling without relaxing numerical parity, aliasing or failure atomicity.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/matrix_kernel.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "matrix workspace %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    const size_t rows = 48U, inner = 8U, columns = 260U;
    const size_t leftCount = rows * inner;
    const size_t rightCount = inner * columns;
    const size_t outputCount = rows * columns;
    UmiAiTrainingMatrixEstimate work = {0};
    float *left = calloc(leftCount, sizeof(*left));
    float *right = calloc(rightCount, sizeof(*right));
    float *reference = calloc(outputCount, sizeof(*reference));
    float *tiled = calloc(outputCount, sizeof(*tiled));
    float *automatic = calloc(outputCount, sizeof(*automatic));
    CHECK(left != NULL && right != NULL && reference != NULL &&
          tiled != NULL && automatic != NULL);
    CHECK(outputCount > 4096U && outputCount < UMI_AI_MATRIX_MAX_ELEMENTS);
    for (size_t i = 0U; i < leftCount; ++i)
        left[i] = (float)((int)(i % 13U) - 6) * 0.125F;
    for (size_t i = 0U; i < rightCount; ++i)
        right[i] = (float)((int)(i % 19U) - 9) * 0.0625F;

    CHECK(UmiAiTrainingMatrixEstimateWork(rows, inner, columns, &work)
          == UMI_STATUS_OK);
    CHECK(work.outputBytes == outputCount * sizeof(float));
    CHECK(work.temporaryBytes == work.outputBytes);
    CHECK(UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_REFERENCE, reference, outputCount) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_TILED, tiled, outputCount) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_AUTO, automatic, outputCount) == UMI_STATUS_OK);
    CHECK(memcmp(reference, tiled, work.outputBytes) == 0);
    CHECK(memcmp(reference, automatic, work.outputBytes) == 0);

    /* The larger staging area must not touch caller memory on a rejected
     * request; it also permits output to alias the caller's left input. */
    float *aliased = calloc(outputCount, sizeof(*aliased));
    CHECK(aliased != NULL);
    memcpy(aliased, left, leftCount * sizeof(*left));
    CHECK(UmiAiTrainingMatrixMultiply(aliased, rows, inner, right, columns,
        UMI_AI_MATRIX_TILED, aliased, outputCount) == UMI_STATUS_OK);
    CHECK(memcmp(reference, aliased, work.outputBytes) == 0);
    const float protectedValue = aliased[outputCount - 1U];
    CHECK(UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_TILED, aliased, outputCount - 1U)
        == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(aliased[outputCount - 1U] == protectedValue);
    left[0] = NAN;
    CHECK(UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_TILED, aliased, outputCount)
        == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(reference, aliased, work.outputBytes) == 0);
    left[0] = FLT_MAX;
    right[0] = FLT_MAX;
    CHECK(UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_REFERENCE, aliased, outputCount)
        == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(reference, aliased, work.outputBytes) == 0);
    CHECK(UmiAiTrainingMatrixEstimateWork(1U, UMI_AI_MATRIX_MAX_ELEMENTS + 1U,
        1U, &work) == UMI_STATUS_CAPACITY_EXCEEDED);

    free(aliased);
    free(automatic);
    free(tiled);
    free(reference);
    free(right);
    free(left);
    puts("shape-sized matrix workspace, parity and atomic rejection: PASS");
    return 0;
}
