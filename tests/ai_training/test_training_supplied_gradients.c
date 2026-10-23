/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_supplied_gradients.c
 * PURPOSE:
 *   Check that externally accumulated gradients receive one atomic native
 *   parameter update, and malformed requests cannot change existing weights.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/gradient_accumulation.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *first = NULL, *second = NULL, *fixed = NULL;
    UmiAiTrainingTensor *derived = NULL;
    float a[] = { 2.0F, -3.0F, 1.0F, 4.0F };
    float b[] = { 5.0F, 6.0F };
    float ga[] = { 1.0F, -2.0F, 0.5F, 3.0F };
    float gb[] = { 2.0F, -3.0F };
    float invalid[] = { 2.0F, NAN };
    float beforeA[4], beforeB[2], afterA[4], afterB[2];
    UmiAiTrainingTensor *params[2];
    const float *gradients[2];

    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 2U, 2U, a, 4U, 1, &first)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 1U, 2U, b, 2U, 1, &second)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 1U, 2U, b, 2U, 0, &fixed)
          == UMI_STATUS_OK);
    params[0] = first; params[1] = second;
    gradients[0] = ga; gradients[1] = invalid;
    CHECK(UmiAiTrainingTensorValues(first, beforeA, 4U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(second, beforeB, 2U) == UMI_STATUS_OK);

    CHECK(UmiAiTrainingSgdStepWithGradients(tape, params, gradients, 2U, 0.1F)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingTensorValues(first, afterA, 4U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(second, afterB, 2U) == UMI_STATUS_OK);
    CHECK(memcmp(beforeA, afterA, sizeof(a)) == 0);
    CHECK(memcmp(beforeB, afterB, sizeof(b)) == 0);
    gradients[1] = gb;
    CHECK(UmiAiTrainingSgdStepWithGradients(tape, params, gradients, 2U, 0.0F)
          == UMI_STATUS_INVALID_ARGUMENT);
    params[1] = first;
    CHECK(UmiAiTrainingSgdStepWithGradients(tape, params, gradients, 2U, 0.1F)
          == UMI_STATUS_INVALID_ARGUMENT);
    params[1] = fixed;
    CHECK(UmiAiTrainingSgdStepWithGradients(tape, params, gradients, 2U, 0.1F)
          == UMI_STATUS_INVALID_ARGUMENT);
    params[1] = second;
    CHECK(UmiAiTrainingRelu(tape, first, &derived) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSgdStepWithGradients(tape, params, gradients, 2U, 0.1F)
          == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingTapeRewind(tape, 3U) == UMI_STATUS_OK);

    CHECK(UmiAiTrainingSgdStepWithGradients(tape, params, gradients, 2U, 0.1F)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(first, afterA, 4U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(second, afterB, 2U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 4U; ++i)
        CHECK(fabsf(afterA[i] - (a[i] - 0.1F * ga[i])) <= 1e-6F);
    for (size_t i = 0U; i < 2U; ++i)
        CHECK(fabsf(afterB[i] - (b[i] - 0.1F * gb[i])) <= 1e-6F);
    /* The previous backward state must not authorise an unrelated SGD step. */
    CHECK(UmiAiTrainingSgdStepMany(tape, params, 2U, 0.1F)
          == UMI_STATUS_INVALID_STATE);
    UmiAiTrainingTapeDestroy(tape);
    puts("supplied gradients: checked atomic multi-parameter SGD");
    return 0;
}
