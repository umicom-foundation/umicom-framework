/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_matrix_contract.c
 * PURPOSE: Verify portable numerical parity, bounds and failure-atomic writes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/matrix_kernel.h"
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

static int CompareMethods(size_t rows, size_t inner, size_t columns)
{
    float left[UMI_AI_MATRIX_MAX_ELEMENTS];
    float right[UMI_AI_MATRIX_MAX_ELEMENTS];
    float reference[UMI_AI_MATRIX_MAX_ELEMENTS];
    float tiled[UMI_AI_MATRIX_MAX_ELEMENTS];
    float automatic[UMI_AI_MATRIX_MAX_ELEMENTS];
    const size_t count = rows * columns;
    for (size_t i = 0U; i < rows * inner; ++i)
        left[i] = (float)((int)(i % 13U) - 6) * 0.125F;
    for (size_t i = 0U; i < inner * columns; ++i)
        right[i] = (float)((int)(i % 19U) - 9) * 0.0625F;
    CHECK(UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_REFERENCE, reference, count) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_TILED, tiled, count) == UMI_STATUS_OK);
    CHECK(memcmp(reference, tiled, count * sizeof(float)) == 0);
    CHECK(UmiAiTrainingMatrixMultiply(left, rows, inner, right, columns,
        UMI_AI_MATRIX_AUTO, automatic, count) == UMI_STATUS_OK);
    CHECK(memcmp(reference, automatic, count * sizeof(float)) == 0);
    return 0;
}

int main(void)
{
    const float left[6] = {1.0F, 2.0F, 3.0F, 4.0F, 5.0F, 6.0F};
    const float right[6] = {7.0F, 8.0F, 9.0F, 10.0F, 11.0F, 12.0F};
    const float expected[4] = {58.0F, 64.0F, 139.0F, 154.0F};
    float result[6] = {-7.0F, -7.0F, -7.0F, -7.0F, -7.0F, -7.0F};
    UmiAiTrainingMatrixEstimate work = {0};
    UmiAiTrainingMatrixEstimate unchanged = {0};
    CHECK(UmiAiTrainingMatrixEstimateWork(2U, 3U, 2U, &work) == UMI_STATUS_OK);
    CHECK(work.multiplyAccumulateCount == 12U);
    CHECK(work.nominalFloatingOperations == 24U);
    CHECK(work.inputBytes == 24U && work.weightBytes == 24U);
    CHECK(work.outputBytes == 16U && work.temporaryBytes == 16U);

    CHECK(UmiAiTrainingMatrixMultiply(left, 2U, 3U, right, 2U,
        UMI_AI_MATRIX_TILED, result, 6U) == UMI_STATUS_OK);
    CHECK(memcmp(result, expected, sizeof(expected)) == 0);
    CHECK(result[4] == -7.0F && result[5] == -7.0F);

    CHECK(CompareMethods(1U, 1U, 1U) == 0);
    CHECK(CompareMethods(2U, 3U, 5U) == 0);
    CHECK(CompareMethods(4U, 8U, 260U) == 0);
    CHECK(CompareMethods(1U, 32U, 32U) == 0);
    CHECK(CompareMethods(16U, 8U, 8U) == 0);
    CHECK(CompareMethods(1U, 260U, 8U) == 0);

    /* Staging permits the right-hand or left-hand matrix to be overwritten
     * only after the complete, validated result has been calculated. */
    float aliased[4] = {1.0F, 2.0F, 3.0F, 4.0F};
    const float identity[4] = {1.0F, 0.0F, 0.0F, 1.0F};
    CHECK(UmiAiTrainingMatrixMultiply(aliased, 2U, 2U, identity, 2U,
        UMI_AI_MATRIX_TILED, aliased, 4U) == UMI_STATUS_OK);
    CHECK(aliased[0] == 1.0F && aliased[1] == 2.0F &&
          aliased[2] == 3.0F && aliased[3] == 4.0F);
    CHECK(UmiAiTrainingMatrixMultiply(identity, 2U, 2U, aliased, 2U,
        UMI_AI_MATRIX_TILED, aliased, 4U) == UMI_STATUS_OK);
    CHECK(aliased[0] == 1.0F && aliased[3] == 4.0F);

    /* A failed operation must preserve every byte of caller output. */
    float refused[4] = {-313.0F, -313.0F, -313.0F, -313.0F};
    CHECK(UmiAiTrainingMatrixMultiply(left, 2U, 3U, right, 2U,
        UMI_AI_MATRIX_TILED, refused, 3U) == UMI_STATUS_CAPACITY_EXCEEDED);
    for (size_t i = 0U; i < 4U; ++i) CHECK(refused[i] == -313.0F);
    CHECK(UmiAiTrainingMatrixMultiply(left, 2U, 3U, right, 2U,
        (UmiAiTrainingMatrixMethod)999, refused, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingMatrixMultiply(left, 4097U, 1U, right, 1U,
        UMI_AI_MATRIX_TILED, refused, 4U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingMatrixMultiply(left, SIZE_MAX, 2U, right, 2U,
        UMI_AI_MATRIX_TILED, refused, 4U) == UMI_STATUS_CAPACITY_EXCEEDED);
    float nonfinite[1] = {NAN};
    CHECK(UmiAiTrainingMatrixMultiply(nonfinite, 1U, 1U, right, 1U,
        UMI_AI_MATRIX_TILED, refused, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    const float excessive[1] = {FLT_MAX};
    CHECK(UmiAiTrainingMatrixMultiply(excessive, 1U, 1U, excessive, 1U,
        UMI_AI_MATRIX_TILED, refused, 4U) == UMI_STATUS_INVALID_ARGUMENT);
    for (size_t i = 0U; i < 4U; ++i) CHECK(refused[i] == -313.0F);

    unchanged = work;
    CHECK(UmiAiTrainingMatrixEstimateWork(0U, 1U, 1U,
        &work) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingMatrixEstimateWork(1U, 4097U, 1U,
        &work) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&work, &unchanged, sizeof(work)) == 0);
    puts("matrix multiplication numerical parity and contracts: PASS");
    return 0;
}
