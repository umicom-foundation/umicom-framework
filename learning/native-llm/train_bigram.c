/*-----------------------------------------------------------------------------
 * Umicom Framework Learning Laboratory
 * File: learning/native-llm/train_bigram.c
 *
 * PURPOSE:
 *   Demonstrate the forward -> loss -> backward -> SGD -> repeat lifecycle.
 *   A three-token model trains from zero weights and learns next-token
 *   transitions. It is a REAL trained probabilistic model, but NOT a
 *   transformer or a generally capable language model. Later lessons add
 *   token embeddings, positional encoding, self-attention and more layers.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include <stdio.h>

/* An error code is printed at the call boundary, not silently ignored. */
#define TRAIN_CHECK(expression) do { \
    UmiStatus trainingStatus = (expression); \
    if (trainingStatus != UMI_STATUS_OK) { \
        fprintf(stderr, "native LLM laboratory failed: %s (%d)\n", \
                #expression, (int)trainingStatus); \
        UmiAiTrainingTapeDestroy(tape); \
        return 1; \
    } \
} while (0)

int main(void)
{
    static const char *const vocabulary[3] = {"umicom", "framework", "trains"};
    static const float oneHot[18] = {
        1.0F, 0.0F, 0.0F,
        0.0F, 1.0F, 0.0F,
        0.0F, 0.0F, 1.0F,
        1.0F, 0.0F, 0.0F,
        0.0F, 1.0F, 0.0F,
        0.0F, 0.0F, 1.0F
    };
    static const size_t targets[6] = {1U, 2U, 0U, 1U, 2U, 0U};
    static const float zeroWeights[9] = {0.0F};
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *inputs = NULL;
    UmiAiTrainingTensor *weights = NULL;
    float learnedWeights[9] = {0.0F};

    TRAIN_CHECK(UmiAiTrainingTapeCreate(&tape));
    TRAIN_CHECK(UmiAiTrainingTensorCreate(tape, 6U, 3U, oneHot, 18U, 0, &inputs));
    TRAIN_CHECK(UmiAiTrainingTensorCreate(tape, 3U, 3U, zeroWeights, 9U, 1, &weights));

    puts("Umicom Framework — first native next-token training experiment");
    puts("Model: one-hot bigram, 3x3 weights, categorical cross-entropy");
    puts("This demonstrates training machinery, not transformer language ability.\n");

    for (size_t epoch = 0U; epoch < 350U; ++epoch) {
        UmiAiTrainingTensor *logits = NULL;
        UmiAiTrainingTensor *loss = NULL;
        float value = 0.0F;
        TRAIN_CHECK(UmiAiTrainingMatmul(tape, inputs, weights, &logits));
        TRAIN_CHECK(UmiAiTrainingCrossEntropy(tape, logits, targets, 6U, &loss));
        TRAIN_CHECK(UmiAiTrainingTensorValues(loss, &value, 1U));
        if (epoch == 0U || epoch == 49U || epoch == 99U || epoch == 199U || epoch == 349U) {
            printf("epoch %3zu: cross-entropy %.6f\n", epoch + 1U, (double)value);
        }
        TRAIN_CHECK(UmiAiTrainingBackward(tape, loss));
        TRAIN_CHECK(UmiAiTrainingSgdStep(tape, weights, 0.7F));
        TRAIN_CHECK(UmiAiTrainingTapeRewind(tape, 2U));
    }
    TRAIN_CHECK(UmiAiTrainingTensorValues(weights, learnedWeights, 9U));
    puts("\nLearned next-token predictions:");
    for (size_t source = 0U; source < 3U; ++source) {
        size_t winningClass = 0U;
        for (size_t candidate = 1U; candidate < 3U; ++candidate) {
            if (learnedWeights[source * 3U + candidate] >
                learnedWeights[source * 3U + winningClass]) winningClass = candidate;
        }
        printf("  %-10s -> %s\n", vocabulary[source], vocabulary[winningClass]);
    }
    UmiAiTrainingTapeDestroy(tape);
    return 0;
}
