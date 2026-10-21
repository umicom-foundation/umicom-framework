/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_kv_cache_rejection.c
 * PURPOSE: Reject malformed calls without modifying cached state or outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_cache.h"
#include <stdint.h>
#include <stdio.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {9U, 4U, 4U, 8U, 2U, UINT64_C(1204)};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingKvCache *cache = NULL;
    size_t prefix[] = {2U, 3U, 4U, 5U, 6U};
    float logits[9];
    size_t count = 777U;
    uint64_t computed = 777U, rebuilds = 777U;
    for (size_t i = 0U; i < 9U; ++i) logits[i] = -123.0F;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheCreate(model, &cache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCachePrefill(cache, prefix, 5U, logits, 9U)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvCachePrefill(cache, prefix, 2U, logits, 8U)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    prefix[1] = 9U;
    CHECK(UmiAiTrainingKvCachePrefill(cache, prefix, 2U, logits, 9U)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvCacheAppend(cache, 9U, logits, 9U)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingKvCacheAppend(cache, 3U, logits, 8U)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    for (size_t i = 0U; i < 9U; ++i) CHECK(logits[i] == -123.0F);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &count, &computed, &rebuilds)
          == UMI_STATUS_OK);
    CHECK(count == 0U && computed == 0U && rebuilds == 0U);
    CHECK(UmiAiTrainingKvCacheInspect(cache, NULL, &computed, &rebuilds)
          == UMI_STATUS_INVALID_ARGUMENT);
    prefix[1] = 3U;
    CHECK(UmiAiTrainingKvCachePrefill(cache, prefix, 4U, logits, 9U)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &count, &computed, &rebuilds)
          == UMI_STATUS_OK);
    CHECK(count == 4U && computed == 4U && rebuilds == 0U);
    /* A rejected rolling-window append may not drop an old token. */
    CHECK(UmiAiTrainingKvCacheAppend(cache, 3U, logits, 8U)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &count, &computed, &rebuilds)
          == UMI_STATUS_OK);
    CHECK(count == 4U && computed == 4U && rebuilds == 0U);
    UmiAiTrainingKvCacheDestroy(cache);
    UmiAiTrainingDecoderDestroy(model);
    puts("cached inference validates capacity and tokens before changing state");
    return 0;
}
