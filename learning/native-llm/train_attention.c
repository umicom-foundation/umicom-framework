/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/train_attention.c
 *
 * PURPOSE:
 *   Demonstrate trainable token embeddings, Q/K/V projections, single-head
 *   causal attention, categorical cross-entropy and native SGD on a tiny
 *   context-sensitive educational corpus.
 *
 * SCOPE:
 *   This is a trained attention-based classifier, not yet a complete GPT,
 *   transformer block, multi-head system or generally capable language model.
 *   The two training sequences are intentionally tiny and are NOT held-out
 *   evaluation. The example is for studying the forward/backward mechanism.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/tape.h"
#include "umicom/ai/training/initialisation.h"
#include "umicom/ai/training/sequence_shape.h"

#include <stdio.h>

#define TRAIN_VERIFY(expression) do { \
    UmiStatus status = (expression); \
    if (status != UMI_STATUS_OK) { \
        fprintf(stderr, "native attention training failed at line %d: %s (status=%d)\n", \
                __LINE__, #expression, (int)status); \
        UmiAiTrainingTapeDestroy(tape); \
        return 1; \
    } \
} while (0)

int main(void)
{
    /* Both input sequences end with the same token. Their next-token targets
     * differ, so no context-free rule can perfectly predict the second row. */
    static const char *const vocabulary[4] = {
        "framework", "learns", "calypso", "C23"
    };
    static const size_t tokens[4] = {0U, 1U, 2U, 1U};
    static const size_t targets[4] = {1U, 3U, 1U, 0U};
    enum { VOCABULARY = 4, CHANNELS = 4, WEIGHT_COUNT = 16, PARAMETERS = 5 };
    UmiAiTrainingRandomState randomState;
    UmiAiTrainingSequenceShape shape;
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *parameters[PARAMETERS] = {NULL};
    float initialWeights[WEIGHT_COUNT] = {0};
    float firstLoss = -1.0F;
    float lastLoss = -1.0F;
    size_t sequenceRows = 0U;
    size_t sequenceChannels = 0U;

    TRAIN_VERIFY(UmiAiTrainingSequenceShapeCreate(2U, 2U, CHANNELS, &shape));
    TRAIN_VERIFY(UmiAiTrainingSequenceFlatten(&shape,
                                              &sequenceRows, &sequenceChannels));
    if (sequenceRows != 4U || sequenceChannels != CHANNELS) return 1;
    TRAIN_VERIFY(UmiAiTrainingRandomSeed(&randomState, UINT64_C(20261020)));
    TRAIN_VERIFY(UmiAiTrainingTapeCreate(&tape));

    for (size_t parameter = 0U; parameter < PARAMETERS; ++parameter) {
        TRAIN_VERIFY(UmiAiTrainingRandomFillUniform(&randomState, -0.3F,
                    0.3F, initialWeights, WEIGHT_COUNT));
        TRAIN_VERIFY(UmiAiTrainingTensorCreate(tape, CHANNELS, CHANNELS,
                    initialWeights, WEIGHT_COUNT, 1, &parameters[parameter]));
    }

    puts("Umicom — native, seeded, trainable single-head causal attention");
    puts("Two sequences; the same final token requires different next tokens.");
    puts("This demonstrates attention training, not a complete transformer.\n");

    for (size_t epoch = 0U; epoch < 1100U; ++epoch) {
        UmiAiTrainingTensor *embedded = NULL;
        UmiAiTrainingTensor *query = NULL;
        UmiAiTrainingTensor *key = NULL;
        UmiAiTrainingTensor *value = NULL;
        UmiAiTrainingTensor *attended = NULL;
        UmiAiTrainingTensor *logits = NULL;
        UmiAiTrainingTensor *loss = NULL;
        float lossValue = 0.0F;
        TRAIN_VERIFY(UmiAiTrainingEmbeddingLookup(tape, parameters[0],
                                                 tokens, 4U, &embedded));
        TRAIN_VERIFY(UmiAiTrainingMatmul(tape, embedded, parameters[1], &query));
        TRAIN_VERIFY(UmiAiTrainingMatmul(tape, embedded, parameters[2], &key));
        TRAIN_VERIFY(UmiAiTrainingMatmul(tape, embedded, parameters[3], &value));
        TRAIN_VERIFY(UmiAiTrainingCausalAttention(tape, query, key, value,
                                                 shape.time, &attended));
        TRAIN_VERIFY(UmiAiTrainingMatmul(tape, attended, parameters[4], &logits));
        TRAIN_VERIFY(UmiAiTrainingCrossEntropy(tape, logits, targets, 4U, &loss));
        TRAIN_VERIFY(UmiAiTrainingTensorValues(loss, &lossValue, 1U));
        if (epoch == 0U) firstLoss = lossValue;
        lastLoss = lossValue;
        if (epoch == 0U || epoch == 49U || epoch == 199U ||
            epoch == 499U || epoch == 1099U) {
            printf("epoch %4zu: training cross-entropy %.6f\n",
                   epoch + 1U, (double)lossValue);
        }
        TRAIN_VERIFY(UmiAiTrainingBackward(tape, loss));
        for (size_t parameter = 0U; parameter < PARAMETERS; ++parameter) {
            TRAIN_VERIFY(UmiAiTrainingSgdStep(tape, parameters[parameter], 0.35F));
        }
        TRAIN_VERIFY(UmiAiTrainingTapeRewind(tape, PARAMETERS));
    }
    printf("\nCorpus: '%s %s' => '%s', '%s %s' => '%s'\n",
           vocabulary[0], vocabulary[1], vocabulary[3],
           vocabulary[2], vocabulary[1], vocabulary[0]);
    printf("training loss %.6f -> %.6f\n", (double)firstLoss, (double)lastLoss);
    UmiAiTrainingTapeDestroy(tape);
    if (!(lastLoss < firstLoss * 0.9F)) {
        fputs("training did not improve enough: inspect the training components\n", stderr);
        return 1;
    }
    puts("Training loss decreased. This does not measure generalisation.");
    return 0;
}
