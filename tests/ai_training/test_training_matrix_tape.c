/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_matrix_tape.c
 * PURPOSE: Keep forward matmul and backward gradient contracts unchanged.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *left = NULL, *right = NULL;
    UmiAiTrainingTensor *product = NULL, *target = NULL, *loss = NULL;
    const float a[6] = {1, 2, 3, 4, 5, 6};
    const float b[6] = {7, 8, 9, 10, 11, 12};
    const float zeros[4] = {0};
    const float expected[4] = {58, 64, 139, 154};
    float values[4] = {0};
    float gradientsLeft[6] = {0};
    float gradientsRight[6] = {0};

    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 2U, 3U, a, 6U, 1,
        &left) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 3U, 2U, b, 6U, 1,
        &right) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingMatmul(tape, left, right,
        &product) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(product, values, 4U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 4U; ++i) CHECK(values[i] == expected[i]);
    CHECK(UmiAiTrainingTensorCreate(tape, 2U, 2U, zeros, 4U, 0,
        &target) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingMeanSquaredError(tape, product, target,
        &loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorGradients(left, gradientsLeft, 6U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorGradients(right, gradientsRight, 6U) == UMI_STATUS_OK);
    /* d/dA mean((AB)^2) = (2/(MN)) (AB) B^T
     * d/dB mean((AB)^2) = (2/(MN)) A^T (AB) */
    for (size_t r = 0U; r < 2U; ++r) {
        for (size_t k = 0U; k < 3U; ++k) {
            double expectedGradient = 0.0;
            for (size_t c = 0U; c < 2U; ++c) {
                expectedGradient += (double)expected[r * 2U + c] * 0.5 *
                                    (double)b[k * 2U + c];
            }
            CHECK(fabs((double)gradientsLeft[r * 3U + k] -
                       expectedGradient) < 0.0001);
        }
    }
    for (size_t k = 0U; k < 3U; ++k) {
        for (size_t c = 0U; c < 2U; ++c) {
            double expectedGradient = 0.0;
            for (size_t r = 0U; r < 2U; ++r) {
                expectedGradient += (double)a[r * 3U + k] *
                                    (double)expected[r * 2U + c] * 0.5;
            }
            CHECK(fabs((double)gradientsRight[k * 2U + c] -
                       expectedGradient) < 0.0001);
        }
    }
    UmiAiTrainingTapeDestroy(tape);
    puts("shared matrix kernel retains tape gradients: PASS");
    return 0;
}
