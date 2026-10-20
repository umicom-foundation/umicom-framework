/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_bigram.c
 * PURPOSE: Verify real next-token training without claiming a transformer.
 * AUTHOR: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <stdio.h>

#define REQUIRE(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "bigram test failed at line %d: %s\n", __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *inputs = NULL;
    UmiAiTrainingTensor *weights = NULL;
    /* The corpus is [umicom, framework, trains, umicom, framework, trains].
     * One-hot source tokens encode a deterministic three-symbol cycle.
     * The model must LEARN the destination class by SGD from zero weights. */
    const float oneHot[18] = {
        1.0F, 0.0F, 0.0F,
        0.0F, 1.0F, 0.0F,
        0.0F, 0.0F, 1.0F,
        1.0F, 0.0F, 0.0F,
        0.0F, 1.0F, 0.0F,
        0.0F, 0.0F, 1.0F
    };
    const float zeros[9] = {0.0F};
    const size_t nextToken[6] = {1U, 2U, 0U, 1U, 2U, 0U};
    float startLoss = 0.0F;
    float endLoss = 0.0F;
    float learned[9] = {0.0F};
    REQUIRE(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 6U, 3U, oneHot, 18U, 0, &inputs) == UMI_STATUS_OK);
    REQUIRE(UmiAiTrainingTensorCreate(tape, 3U, 3U, zeros, 9U, 1, &weights) == UMI_STATUS_OK);
    for (size_t epoch = 0U; epoch < 350U; ++epoch) {
        UmiAiTrainingTensor *logits = NULL;
        UmiAiTrainingTensor *loss = NULL;
        float observed = 0.0F;
        REQUIRE(UmiAiTrainingMatmul(tape, inputs, weights, &logits) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingCrossEntropy(tape, logits, nextToken, 6U, &loss) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingTensorValues(loss, &observed, 1U) == UMI_STATUS_OK);
        if (epoch == 0U) startLoss = observed;
        endLoss = observed;
        REQUIRE(UmiAiTrainingBackward(tape, loss) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingSgdStep(tape, weights, 0.7F) == UMI_STATUS_OK);
        REQUIRE(UmiAiTrainingTapeRewind(tape, 2U) == UMI_STATUS_OK);
    }
    REQUIRE(UmiAiTrainingTensorValues(weights, learned, 9U) == UMI_STATUS_OK);
    REQUIRE(startLoss > 1.09F && startLoss < 1.11F);
    REQUIRE(endLoss < 0.04F);
    for (size_t row = 0U; row < 3U; ++row) {
        const size_t expected = (row + 1U) % 3U;
        for (size_t col = 0U; col < 3U; ++col) {
            if (col != expected) REQUIRE(learned[row * 3U + expected] > learned[row * 3U + col]);
        }
    }
    printf("next-token CE: %.6f -> %.6f\n", (double)startLoss, (double)endLoss);
    UmiAiTrainingTapeDestroy(tape);
    return 0;
}
