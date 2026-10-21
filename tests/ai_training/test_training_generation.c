/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_generation.c
 * PURPOSE: Autoregressive rolling context, determinism, and output isolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(99887)};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingSamplingOptions greedy = {1.0F, 1U};
    UmiAiTrainingSamplingOptions sample = {0.8F, 12U};
    size_t prompt[3] = {109U, 114U, 120U}; /* byte encoding of "int" */
    size_t first[32] = {0}, second[32] = {0};
    size_t produced = 999U;
    UmiAiTrainingDecoderConfig inspected;
    uint64_t stepCount = UINT64_MAX;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &greedy, 24U, 0U, first, 32U, &produced) == UMI_STATUS_OK);
    CHECK(produced == 24U);
    for (size_t i = 0U; i < produced; ++i)
        CHECK(first[i] >= UMI_AI_BYTE_OFFSET && first[i] < UMI_AI_BYTE_VOCABULARY);
    produced = 0U;
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &greedy, 24U, 1234U, second, 32U, &produced) == UMI_STATUS_OK);
    CHECK(produced == 24U && memcmp(first, second, 24U * sizeof(size_t)) == 0);
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &sample, 24U, 42U, first, 32U, &produced) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &sample, 24U, 42U, second, 32U, &produced) == UMI_STATUS_OK);
    CHECK(memcmp(first, second, 24U * sizeof(size_t)) == 0);
    CHECK(UmiAiTrainingDecoderInspect(model, &inspected, &stepCount) == UMI_STATUS_OK);
    CHECK(stepCount == 0U && inspected.contextLength == 8U);

    size_t guarded[32];
    for (size_t i = 0U; i < 32U; ++i) guarded[i] = SIZE_MAX;
    produced = 999U;
    /* The requested count is within the published ceiling, but larger than
     * the provided buffer: neither buffer nor count can be partially written. */
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &greedy, 33U, 0U, guarded, 32U, &produced) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(produced == 999U);
    for (size_t i = 0U; i < 32U; ++i) CHECK(guarded[i] == SIZE_MAX);
    size_t invalidPrompt[] = {UMI_AI_BYTE_VOCABULARY};
    CHECK(UmiAiTrainingDecoderGenerate(model, invalidPrompt, 1U,
                &greedy, 1U, 0U, guarded, 32U, &produced) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(produced == 999U && guarded[0] == SIZE_MAX);
    CHECK(UmiAiTrainingDecoderGenerate(model, prompt, 3U,
                &greedy, 0U, 0U, guarded, 32U, &produced) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(guarded[0] == SIZE_MAX);

    /* The decoder owns its neural parameters; generation cannot change them. */
    size_t checkpointSize = 0U, written = 0U;
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize) == UMI_STATUS_OK);
    uint8_t *before = malloc(checkpointSize), *after = malloc(checkpointSize);
    CHECK(before != NULL && after != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before,
                checkpointSize, &written) == UMI_STATUS_OK && written == checkpointSize);
    CHECK(UmiAiTrainingDecoderGenerate(model, NULL, 0U,
                &greedy, 4U, 1U, first, 32U, &produced) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
                checkpointSize, &written) == UMI_STATUS_OK && written == checkpointSize);
    CHECK(memcmp(before, after, checkpointSize) == 0);
    free(after);
    free(before);
    UmiAiTrainingDecoderDestroy(model);
    puts("decoder generation, context, determinism, atomic results: PASS");
    return 0;
}
