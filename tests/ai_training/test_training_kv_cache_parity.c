/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_kv_cache_parity.c
 * PURPOSE: Compare each native cached decode with canonical decoder logits.
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

static int Matches(UmiAiTrainingDecoder *model,
                   const size_t *sequence, size_t count,
                   const float *cached, size_t context, size_t vocabulary)
{
    size_t padded[UMI_AI_DECODER_MAX_CONTEXT] = {0};
    float reference[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_VOCABULARY];
    /* A causal row is independent of tokens to its right. For a short prefix
     * provide a complete canonical input and compare its final prefix row. */
    if (count > context) return 0;
    for (size_t i = 0U; i < count; ++i) padded[i] = sequence[i];
    for (size_t i = count; i < context; ++i) padded[i] = 1U;
    if (UmiAiTrainingDecoderLogits(model, padded, context,
            reference, context * vocabulary) != UMI_STATUS_OK) return 0;
    for (size_t col = 0U; col < vocabulary; ++col) {
        const float expected = reference[(count - 1U) * vocabulary + col];
        if (!isfinite(cached[col]) || fabsf(cached[col] - expected) > 0.00003F) {
            fprintf(stderr, "position=%zu class=%zu cached=%f expected=%f\n",
                    count, col, (double)cached[col], (double)expected);
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    UmiAiTrainingDecoderConfig config = {17U, 6U, 8U, 16U, 2U, UINT64_C(15333)};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingKvCache *cache = NULL;
    const size_t sequence[] = {3U, 4U, 6U, 7U, 4U, 3U, 10U, 15U, 12U, 9U};
    float output[17];
    size_t retained = 0U;
    uint64_t computed = 0U, rebuilds = 0U;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheCreate(model, &cache) == UMI_STATUS_OK);
    for (size_t index = 0U; index < sizeof(sequence) / sizeof(sequence[0]); ++index) {
        size_t start = index + 1U > 6U ? index + 1U - 6U : 0U;
        size_t count = index + 1U - start;
        CHECK(UmiAiTrainingKvCacheAppend(cache, sequence[index], output, 17U)
              == UMI_STATUS_OK);
        CHECK(Matches(model, &sequence[start], count, output, 6U, 17U));
        CHECK(UmiAiTrainingKvCacheInspect(cache, &retained, &computed, &rebuilds)
              == UMI_STATUS_OK);
        CHECK(retained == count);
        CHECK(rebuilds == (index >= 6U ? (uint64_t)(index - 5U) : 0U));
        CHECK(computed == (index < 6U ? (uint64_t)(index + 1U) :
                            (uint64_t)(6U + (index - 5U) * 6U)));
    }
    /* Rebuilding from a prefix must leave no old attention history. */
    CHECK(UmiAiTrainingKvCachePrefill(cache, &sequence[2], 3U,
                                      output, 17U) == UMI_STATUS_OK);
    CHECK(Matches(model, &sequence[2], 3U, output, 6U, 17U));
    CHECK(UmiAiTrainingKvCacheInspect(cache, &retained, &computed, &rebuilds)
          == UMI_STATUS_OK);
    CHECK(retained == 3U && computed == 3U && rebuilds == 0U);
    CHECK(UmiAiTrainingKvCacheReset(cache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &retained, &computed, &rebuilds)
          == UMI_STATUS_OK);
    CHECK(retained == 0U && computed == 0U && rebuilds == 0U);
    UmiAiTrainingKvCacheDestroy(cache);
    UmiAiTrainingDecoderDestroy(model);
    puts("causal cached logits match canonical decoder before and after window rebasing");
    return 0;
}
