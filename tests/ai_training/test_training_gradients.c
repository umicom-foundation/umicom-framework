/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_gradients.c
 * PURPOSE: Check analytic reverse-mode gradients with independent expectations.
 * AUTHOR: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <math.h>
#include <stdio.h>

#define REQUIRE(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "gradient test failed at line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

static int Close(float actual, float expected, float tolerance)
{
    return fabsf(actual - expected) <= tolerance;
}

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *left = NULL;
    UmiAiTrainingTensor *weight = NULL;
    UmiAiTrainingTensor *bias = NULL;
    UmiAiTrainingTensor *target = NULL;
    UmiAiTrainingTensor *dot = NULL;
    UmiAiTrainingTensor *shifted = NULL;
    UmiAiTrainingTensor *relu = NULL;
    UmiAiTrainingTensor *loss = NULL;
    const float x[] = {1.0F, 2.0F};
    const float w[] = {2.0F};
    const float b[] = {1.0F};
    const float t[] = {0.0F, 0.0F};
    float dx[2] = {0.0F, 0.0F};
    float dw[1] = {0.0F};
    float db[1] = {0.0F};
    float dl[1] = {0.0F};

    REQUIRE(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 2U, 1U, x, 2U, 0, &left) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, w, 1U, 1, &weight) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 1U, b, 1U, 1, &bias) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 2U, 1U, t, 2U, 0, &target) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingMatmul(tape, left, weight, &dot) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingAdd(tape, dot, bias, &shifted) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingRelu(tape, shifted, &relu) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingMeanSquaredError(tape, relu, target, &loss) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorValues(loss, dl, 1U) == UMI_STATUS_OK);
    /* y = [3, 5], MSE = (9 + 25)/2 = 17. */
    REQUIRE(Close(dl[0], 17.0F, 0.0001F));
    REQUIRE(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorGradients(left, dx, 2U) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorGradients(weight, dw, 1U) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorGradients(bias, db, 1U) == UMI_STATUS_OK);
    /* dL/dy = [3,5], dW = 1*3 + 2*5, db = 3+5, dx=[6,10]. */
    REQUIRE(Close(dw[0], 13.0F, 0.0001F));
    REQUIRE(Close(db[0], 8.0F, 0.0001F));
    REQUIRE(Close(dx[0], 6.0F, 0.0001F));
    REQUIRE(Close(dx[1], 10.0F, 0.0001F));
    /* Second reverse pass clears old derivatives instead of accumulating. */
    REQUIRE(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorGradients(weight, dw, 1U) == UMI_STATUS_OK);
    REQUIRE(Close(dw[0], 13.0F, 0.0001F));
    REQUIRE(UmiAiTrainingSgdStep(tape, weight, 0.1F) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorValues(weight, dw, 1U) == UMI_STATUS_OK);
    REQUIRE(Close(dw[0], 0.7F, 0.0001F));
    REQUIRE(UmiAiTrainingTapeRewind(tape, 4U) == UMI_STATUS_OK);
    UmiAiTrainingTapeDestroy(tape);

    /* Independently verify the stable softmax-cross-entropy derivative. */
    tape = NULL;
    const float logitsValues[] = {0.0F, 0.0F, 0.0F};
    const size_t classes[] = {2U};
    UmiAiTrainingTensor *logits = NULL;
    UmiAiTrainingTensor *ce = NULL;
    float gr[3] = {0.0F, 0.0F, 0.0F};
    REQUIRE(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 1U, 3U, logitsValues, 3U, 1, &logits) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingCrossEntropy(tape, logits, classes, 1U, &ce) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingBackward(tape, ce) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorGradients(logits, gr, 3U) == UMI_STATUS_OK);
    REQUIRE(Close(gr[0], 1.0F / 3.0F, 0.0001F));
    REQUIRE(Close(gr[1], 1.0F / 3.0F, 0.0001F));
    REQUIRE(Close(gr[2], -2.0F / 3.0F, 0.0001F));
    UmiAiTrainingTapeDestroy(tape);
    puts("gradient tests passed");
    return 0;
}
