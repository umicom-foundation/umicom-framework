/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_selected_loss.c
 * PURPOSE: Verify response-only loss equals an independent numerical
 *          reference and propagates no gradient through ignored logits.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/selected_loss.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "selected loss:%d: %s\n", __LINE__, #x); \
    return 1; } } while (0)

static double ExpectedRowLoss(const float *row, size_t vocabulary, size_t target)
{
    double maximum = -INFINITY;
    for (size_t i = 0U; i < vocabulary; ++i)
        if ((double)row[i] > maximum) maximum = row[i];
    double sum = 0.0;
    for (size_t i = 0U; i < vocabulary; ++i)
        sum += exp((double)row[i] - maximum);
    return log(sum) + maximum - (double)row[target];
}

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *logits = NULL, *loss = NULL;
    const float data[16] = {
        1.0F, 0.2F, -1.0F, 0.5F,
        -0.5F, 1.0F, 0.1F, -1.0F,
        0.3F, -0.1F, 1.5F, 0.2F,
        0.4F, -0.3F, 0.0F, 1.2F
    };
    const size_t labels[4] = {SIZE_MAX, 1U, SIZE_MAX, 3U};
    const uint8_t mask[4] = {0U, 1U, 0U, 1U};
    const uint8_t emptyMask[4] = {0U, 0U, 0U, 0U};
    const uint8_t invalidMask[4] = {0U, 2U, 0U, 1U};
    size_t checkpoint = 0U, afterCount = 0U;
    float measured = -1.0F, gradients[16] = {0};

    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 4U, 4U, data, 16U,
                                     1, &logits) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeCheckpoint(tape, &checkpoint) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingSelectedCrossEntropy(tape, logits, labels, 4U,
              mask, 4U, &loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorValues(loss, &measured, 1U) == UMI_STATUS_OK);
    double expected = 0.5 * (ExpectedRowLoss(&data[4], 4U, 1U) +
                             ExpectedRowLoss(&data[12], 4U, 3U));
    CHECK(fabs((double)measured - expected) < 0.000005);
    CHECK(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorGradients(logits, gradients, 16U) == UMI_STATUS_OK);
    for (size_t col = 0U; col < 4U; ++col) {
        CHECK(gradients[col] == 0.0F);
        CHECK(gradients[8U + col] == 0.0F);
    }
    CHECK(fabsf(gradients[5]) > 0.0001F);
    CHECK(fabsf(gradients[15]) > 0.0001F);
    CHECK(UmiAiTrainingTapeRollback(tape, checkpoint) == UMI_STATUS_OK);

    loss = logits; /* A sentinel: invalid calls cannot overwrite it. */
    CHECK(UmiAiTrainingSelectedCrossEntropy(tape, logits, labels, 4U,
              emptyMask, 4U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingSelectedCrossEntropy(tape, logits, labels, 4U,
              invalidMask, 4U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingSelectedCrossEntropy(tape, logits, labels, 4U,
              mask, 3U, &loss) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(loss == logits);
    CHECK(UmiAiTrainingTapeCheckpoint(tape, &afterCount) == UMI_STATUS_OK);
    CHECK(afterCount == checkpoint);
    UmiAiTrainingTapeDestroy(tape);
    printf("Selected token loss %.7f; ignored gradients exactly zero\n",
           (double)measured);
    return 0;
}
