/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_limits.c
 * PURPOSE: Enforce finite values, finite losses, capacity and failure isolation.
 * AUTHOR: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <float.h>
#include <math.h>
#include <stdio.h>

#define REQUIRE(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "limits test failed at line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *a = NULL;
    UmiAiTrainingTensor *b = NULL;
    UmiAiTrainingTensor *result = NULL;
    const float veryLarge[1] = {FLT_MAX};
    const float negative[1] = {-FLT_MAX};
    const float one[1] = {1.0F};
    const size_t invalidClass[1] = {1U};
    REQUIRE(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, veryLarge, 1U, 1, &a) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, negative, 1U, 0, &b) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingAdd(tape, a, b, &result) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingMeanSquaredError(tape, a, b, &result) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingCrossEntropy(tape, a, invalidClass, 1U, &result) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingSgdStep(tape, a, NAN) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingSgdStep(tape, a, INFINITY) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiAiTrainingTapeRewind(tape, 2U) == UMI_STATUS_OK);
    UmiAiTrainingTapeDestroy(tape);

    /* The 257th 1x1 leaf must be refused and leave the caller's pointer intact. */
    tape = NULL;
    result = NULL;
    REQUIRE(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    for (size_t i = 0U; i < UMI_AI_TRAINING_MAX_TENSORS; ++i) {
        REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, one, 1U, 0, &result) == UMI_STATUS_OK);
    }
    UmiAiTrainingTensor *previous = result;
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, one, 1U, 0, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(result == previous);
    REQUIRE(UmiAiTrainingTapeRewind(tape, 0U) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, one, 1U, 0, &result) == UMI_STATUS_OK);
    UmiAiTrainingTapeDestroy(tape);
    puts("bounded capacity and error isolation tests passed");
    return 0;
}
