/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_memory_auxiliary.c
 * PURPOSE: Account for persistent attention-probability and target-index
 *          allocations in addition to values, gradients and node metadata.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_memory_auxiliary.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/tape.h"
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *emb = NULL, *embedded = NULL, *logits = NULL;
    UmiAiTrainingTensor *attention = NULL, *loss = NULL;
    UmiAiTrainingTapeMemory before = {0}, after = {0};
    float table[] = {1, 0, 0, 1, 0.3F, 0.7F};
    float scores[] = {2, 1, 0, 1, 2, 0};
    size_t ids[] = {0, 1};
    size_t labels[] = {0, 1};

    CHECK(UmiAiTrainingTapeCreate(&tape) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTensorCreate(tape, 3, 2, table, 6, 1, &emb)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &before) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingEmbeddingLookup(tape, emb, ids, 2, &embedded)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.auxiliaryBytes == 2U * sizeof(size_t));
    CHECK(after.tensorValuesBytes == before.tensorValuesBytes + 4U * sizeof(float));

    CHECK(UmiAiTrainingTensorCreate(tape, 2, 3, scores, 6, 1, &logits)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingCrossEntropy(tape, logits, labels, 2, &loss)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.auxiliaryBytes == 4U * sizeof(size_t));

    /* Two rows, one head, sequence length two: four retained probabilities.
     * The same embedding is deliberately reused for query/key/value. */
    CHECK(UmiAiTrainingCausalAttention(tape, embedded, embedded, embedded,
                                      2U, &attention) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.auxiliaryBytes ==
          4U * sizeof(size_t) + 4U * sizeof(float));
    CHECK(after.currentBytes > after.auxiliaryBytes +
                              after.tensorValuesBytes + after.tensorGradientBytes);
    CHECK(UmiAiTrainingTapeRewind(tape, 0U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingTapeMemoryInspect(tape, &after) == UMI_STATUS_OK);
    CHECK(after.tensorCount == 0U && after.auxiliaryBytes == 0U &&
          after.tensorValuesBytes == 0U && after.tensorGradientBytes == 0U);
    UmiAiTrainingTapeDestroy(tape);
    puts("Targets, attention probabilities and gradient payloads: PASS");
    return 0;
}
