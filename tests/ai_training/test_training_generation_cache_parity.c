/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_generation_cache_parity.c
 * PURPOSE:
 *   Qualify cached nucleus generation against the existing full-context
 *   decoder, including BOS padding, context rollover and byte-token output.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_cache.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/checkpoint.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(98761)
    };
    const UmiAiTrainingNucleusOptions policies[] = {
        {1.0F, 1U, 1.0F},
        {0.80F, 8U, 1.0F},
        {0.90F, 12U, 0.90F}
    };
    const size_t prompt[] = {109U, 114U, 120U, 36U, 39U, 101U, 102U,
                             10U, 11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U};
    const size_t promptLengths[] = {0U, 1U, 3U, 8U, 16U};
    const uint8_t bytePrompt[] = {0U, 0x80U, 0xFFU, (uint8_t)'a'};
    UmiAiTrainingDecoder *model = NULL;
    size_t checkpointLength = 0U;
    size_t written = 0U;
    uint8_t *checkpointBefore = NULL, *checkpointAfter = NULL;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointLength) == UMI_STATUS_OK);
    checkpointBefore = malloc(checkpointLength);
    checkpointAfter = malloc(checkpointLength);
    CHECK(checkpointBefore != NULL && checkpointAfter != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, checkpointBefore,
        checkpointLength, &written) == UMI_STATUS_OK && written == checkpointLength);

    /* Every call uses the real canonical decoder and current parameter snapshot.
     * An unchanged seed and nucleus policy must return identical full sequences. */
    for (size_t p = 0U; p < sizeof policies / sizeof policies[0]; ++p) {
        for (size_t promptCase = 0U;
             promptCase < sizeof promptLengths / sizeof promptLengths[0]; ++promptCase) {
            size_t direct[20] = {0}, cached[20] = {0};
            size_t directCount = 0U, cachedCount = 0U;
            UmiAiTrainingGenerationCacheStats statistics = {0};
            const size_t length = promptLengths[promptCase];
            CHECK(UmiAiTrainingDecoderGenerateNucleus(model,
                length == 0U ? NULL : prompt, length, &policies[p],
                20U, 4444U, direct, 20U, &directCount) == UMI_STATUS_OK);
            CHECK(UmiAiTrainingDecoderGenerateCachedNucleus(model,
                length == 0U ? NULL : prompt, length, &policies[p],
                20U, 4444U, cached, 20U, &cachedCount,
                &statistics) == UMI_STATUS_OK);
            CHECK(cachedCount == 20U && directCount == cachedCount);
            if (memcmp(direct, cached, sizeof direct) != 0) {
                fprintf(stderr, "cache parity mismatch policy=%zu prompt=%zu\n", p, length);
                return 1;
            }
            CHECK(statistics.retainedTokens == (size_t)config.contextLength);
            CHECK(statistics.windowRebuilds == 19U);
            CHECK(statistics.computedRows >= (size_t)config.contextLength);
        }
    }

    {
        uint8_t direct[20] = {0}, cached[20] = {0};
        size_t directCount = 0U, cachedCount = 0U;
        UmiAiTrainingGenerationCacheStats statistics = {0};
        CHECK(UmiAiTrainingDecoderGenerateBytesNucleus(model, bytePrompt,
            sizeof bytePrompt, &policies[2], 20U, UINT64_C(778),
            direct, sizeof direct, &directCount) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingDecoderGenerateCachedBytesNucleus(model, bytePrompt,
            sizeof bytePrompt, &policies[2], 20U, UINT64_C(778),
            cached, sizeof cached, &cachedCount, &statistics) == UMI_STATUS_OK);
        CHECK(directCount == 20U && cachedCount == directCount);
        CHECK(memcmp(direct, cached, sizeof direct) == 0);
        CHECK(statistics.retainedTokens == (size_t)config.contextLength);
    }
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, checkpointAfter,
        checkpointLength, &written) == UMI_STATUS_OK && written == checkpointLength);
    CHECK(memcmp(checkpointBefore, checkpointAfter, checkpointLength) == 0);
    free(checkpointBefore);
    free(checkpointAfter);
    UmiAiTrainingDecoderDestroy(model);
    puts("native C23 cached nucleus/byte parity and checkpoint preservation: PASS");
    return 0;
}
