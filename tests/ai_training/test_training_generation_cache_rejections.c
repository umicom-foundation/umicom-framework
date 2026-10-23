/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_generation_cache_rejections.c
 * PURPOSE:
 *   Verify complete-output publication, invalid-policy rejection and
 *   bounded context/output limits in the KV cache generation adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_cache.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(55)
    };
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingNucleusOptions policy = {0.85F, 8U, 0.95F};
    UmiAiTrainingGenerationCacheStats stats = {777U, 888U, 999U};
    size_t output[16];
    uint8_t byteOutput[16];
    size_t prompt[] = {7U, 9U, 11U};
    size_t written = 123U;
    const uint8_t bytes[] = {1U, 2U, 3U};

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 16U; ++i) output[i] = SIZE_MAX;
    memset(byteOutput, 0xAB, sizeof byteOutput);

    CHECK(UmiAiTrainingDecoderGenerateCachedNucleus(model, prompt, 3U,
        &policy, 16U, 9U, output, 15U, &written, &stats) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(written == 123U && stats.computedRows == 777U && output[0] == SIZE_MAX);
    CHECK(UmiAiTrainingDecoderGenerateCachedNucleus(model, NULL, 1U,
        &policy, 3U, 9U, output, 16U, &written, &stats) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderGenerateCachedNucleus(model, prompt, 3U,
        &policy, 0U, 9U, output, 16U, &written, &stats) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(written == 123U && output[0] == SIZE_MAX);

    size_t badPrompt[] = {UMI_AI_BYTE_VOCABULARY};
    CHECK(UmiAiTrainingDecoderGenerateCachedNucleus(model, badPrompt, 1U,
        &policy, 3U, 9U, output, 16U, &written, &stats) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(written == 123U && output[0] == SIZE_MAX);

    policy.topP = 0.0F;
    CHECK(UmiAiTrainingDecoderGenerateCachedNucleus(model, prompt, 3U,
        &policy, 3U, 9U, output, 16U, &written, &stats) == UMI_STATUS_INVALID_ARGUMENT);
    policy.topP = 1.1F;
    CHECK(UmiAiTrainingDecoderGenerateCachedNucleus(model, prompt, 3U,
        &policy, 3U, 9U, output, 16U, &written, &stats) == UMI_STATUS_INVALID_ARGUMENT);
    policy.topP = 0.95F;
    policy.topK = 0U;
    CHECK(UmiAiTrainingDecoderGenerateCachedNucleus(model, prompt, 3U,
        &policy, 3U, 9U, output, 16U, &written, &stats) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(written == 123U && stats.windowRebuilds == 888U && output[0] == SIZE_MAX);

    policy.topK = 8U;
    CHECK(UmiAiTrainingDecoderGenerateCachedBytesNucleus(model, bytes,
        sizeof bytes, &policy, 16U, 4U, byteOutput, 15U, &written,
        &stats) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(written == 123U && stats.retainedTokens == 999U && byteOutput[0] == 0xABU);
    CHECK(UmiAiTrainingDecoderGenerateCachedBytesNucleus(model, NULL,
        1U, &policy, 3U, 4U, byteOutput, 16U, &written,
        &stats) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderGenerateCachedBytesNucleus(model, bytes,
        sizeof bytes, &policy, 3U, 4U, byteOutput, 16U, &written,
        &stats) == UMI_STATUS_OK);
    CHECK(written == 3U && stats.retainedTokens == 8U);

    UmiAiTrainingDecoderDestroy(model);
    puts("native C23 cached generation failure contracts: PASS");
    return 0;
}
