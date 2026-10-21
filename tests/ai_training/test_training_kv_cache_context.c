/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_kv_cache_context.c
 * PURPOSE: Test short prompts, full-window rebuild and multihead dimensions.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_cache.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig configs[] = {
        {5U, 1U, 4U, 8U, 1U, UINT64_C(5)},
        {16U, 8U, 8U, 12U, 4U, UINT64_C(11)},
        {32U, 16U, 16U, 32U, 8U, UINT64_C(19)},
        {260U, 8U, 8U, 16U, 2U, UINT64_C(43)}
    };
    for (size_t caseId = 0U; caseId < sizeof(configs)/sizeof(configs[0]); ++caseId) {
        const UmiAiTrainingDecoderConfig *config = &configs[caseId];
        const size_t context = (size_t)config->contextLength;
        const size_t vocabulary = (size_t)config->vocabularySize;
        UmiAiTrainingDecoder *model = NULL;
        UmiAiTrainingKvCache *cache = NULL;
        size_t history[UMI_AI_DECODER_MAX_CONTEXT];
        float output[UMI_AI_DECODER_MAX_VOCABULARY];
        float expected[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_VOCABULARY];
        CHECK(UmiAiTrainingDecoderCreate(config, &model) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingKvCacheCreate(model, &cache) == UMI_STATUS_OK);
        for (size_t index = 0U; index < context + 4U; ++index) {
            size_t next = (index * 13U + 3U) % vocabulary;
            if (index < context) {
                history[index] = next;
            } else {
                for (size_t pos = 1U; pos < context; ++pos)
                    history[pos - 1U] = history[pos];
                history[context - 1U] = next;
            }
            CHECK(UmiAiTrainingKvCacheAppend(cache, next, output, vocabulary)
                  == UMI_STATUS_OK);
            size_t populated = index + 1U < context ? index + 1U : context;
            for (size_t fill = populated; fill < context; ++fill)
                history[fill] = 1U;
            CHECK(UmiAiTrainingDecoderLogits(model, history, context, expected,
                                             context * vocabulary) == UMI_STATUS_OK);
            for (size_t col = 0U; col < vocabulary; ++col) {
                float reference = expected[(populated - 1U) * vocabulary + col];
                CHECK(fabsf(output[col] - reference) < 0.00004F);
            }
        }
        UmiAiTrainingKvCacheDestroy(cache);
        UmiAiTrainingDecoderDestroy(model);
    }
    puts("cached decoder handles multiple head counts and positional rebasing");
    return 0;
}
