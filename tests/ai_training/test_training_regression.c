/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_regression.c
 * PURPOSE: Prove training from random-like small initial weights reduces loss.
 * AUTHOR: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>

#define REQUIRE(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "regression test failed at line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *input = NULL;
    UmiAiTrainingTensor *weight = NULL;
    UmiAiTrainingTensor *bias = NULL;
    UmiAiTrainingTensor *target = NULL;
    const float x[] = {-1.0F, 0.0F, 1.0F, 2.0F};
    const float y[] = {-1.0F, 1.0F, 3.0F, 5.0F}; /* y = 2*x + 1 */
    const float initialWeight[] = {0.15F};
    const float initialBias[] = {-0.2F};
    float firstLoss = 0.0F;
    float lastLoss = 0.0F;
    float fittedWeight[1] = {0.0F};
    float fittedBias[1] = {0.0F};
    REQUIRE(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 4U, 1U, x, 4U, 0, &input) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, initialWeight, 1U, 1, &weight) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, initialBias, 1U, 1, &bias) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 4U, 1U, y, 4U, 0, &target) == UMI_STATUS_OK);

    for (size_t epoch = 0U; epoch < 140U; ++epoch) {
        UmiAiTrainingTensor *product = NULL;
        UmiAiTrainingTensor *prediction = NULL;
        UmiAiTrainingTensor *loss = NULL;
        float observed = 0.0F;
        REQUIRE(UmiAiTrainingMatmul(tape, input, weight, &product) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingAdd(tape, product, bias, &prediction) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingMeanSquaredError(tape, prediction, target, &loss) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingTensorValues(loss, &observed, 1U) == UMI_STATUS_OK);
        if (epoch == 0U) firstLoss = observed;
        lastLoss = observed;
        REQUIRE(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingSgdStep(tape, weight, 0.08F) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingSgdStep(tape, bias, 0.08F) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingTapeRewind(tape, 4U) == UMI_STATUS_OK);
    }
    REQUIRE(UmiAiTrainingTensorValues(weight, fittedWeight, 1U) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorValues(bias, fittedBias, 1U) == UMI_STATUS_OK);
    REQUIRE(firstLoss > 1.0F);
    REQUIRE(lastLoss < 0.0001F);
    REQUIRE(fabsf(fittedWeight[0] - 2.0F) < 0.02F);
    REQUIRE(fabsf(fittedBias[0] - 1.0F) < 0.02F);
    printf("regression MSE: %.6f -> %.6f, weight=%.4f bias=%.4f\n",
           (double)firstLoss, (double)lastLoss,
           (double)fittedWeight[0], (double)fittedBias[0]);
    UmiAiTrainingTapeDestroy(tape);
    return 0;
}
