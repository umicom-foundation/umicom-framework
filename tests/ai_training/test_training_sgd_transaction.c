/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_sgd_transaction.c
 * Purpose: Test failure-atomic multi-leaf SGD and incorrect lifecycle calls.
 * Author: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_sgd_transaction.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/tape.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *p1 = NULL, *p2 = NULL, *target = NULL, *loss = NULL;
    UmiAiTrainingTensor *params[2];
    float zero = 0.0F, one = 1.0F, hundred = 100.0F;
    float before1 = 0.0F, before2 = 0.0F, after = 0.0F;
    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 1, 1, &one, 1, 1, &p1) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 1, 1, &hundred, 1, 1, &p2) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 1, 1, &zero, 1, 0, &target) == UMI_STATUS_OK);
    params[0] = p1;
    params[1] = p2;
    CHECK(UmiAiTrainingSgdStepMany(tape, params, 2, 1.0F) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingMeanSquaredError(tape, p2, target, &loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(p1, &before1, 1) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(p2, &before2, 1) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSgdStepMany(tape, params, 2, 1e38F) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingTensorValues(p1, &after, 1) == UMI_STATUS_OK && after == before1);
    CHECK(UmiAiTrainingTensorValues(p2, &after, 1) == UMI_STATUS_OK && after == before2);
    UmiAiTrainingTensor *duplicates[2] = {p1, p1};
    CHECK(UmiAiTrainingSgdStepMany(tape, duplicates, 2, 0.001F) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingSgdStepMany(tape, params, 2, 0.001F) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(p1, &after, 1) == UMI_STATUS_OK && after == one);
    CHECK(UmiAiTrainingTensorValues(p2, &after, 1) == UMI_STATUS_OK && after < hundred);
    UmiAiTrainingTapeDestroy(tape);
    puts("SGD transaction: passed");
    return 0;
}
