/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_cache_compact_lifetime.c
 * PURPOSE: Verify stale cache rejection and checkpoint-size consistency.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/kv_cache.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); \
    return 1; } } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        17U, 3U, 4U, 8U, 2U, UINT64_C(24)
    };
    const size_t inputs[3U] = {4U, 5U, 6U};
    const size_t targets[3U] = {5U, 6U, 7U};
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingKvCache *cache = NULL, *restoredCache = NULL;
    uint8_t *bytes = NULL;
    size_t capacity = 0U, written = 0U, expectedBytes = 0U;
    size_t cacheBytes = 0U, restoredBytes = 0U;
    float output[17U];
    float loss = 0.0F;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheCreate(model, &cache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheAllocatedBytes(cache, &cacheBytes) ==
          UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheAllocationForConfig(&config, &expectedBytes) ==
          UMI_STATUS_OK && cacheBytes == expectedBytes);
    CHECK(UmiAiTrainingKvCachePrefill(cache, inputs, 3U, output, 17U) ==
          UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(model, inputs, targets, 3U,
                                         0.01F, &loss) == UMI_STATUS_OK);
    size_t tokens = 0U;
    uint64_t computed = 0U, resets = 0U;
    CHECK(UmiAiTrainingKvCacheInspect(cache, &tokens, &computed, &resets) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingKvCachePrefill(cache, inputs, 3U, output, 17U) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &capacity) == UMI_STATUS_OK);
    bytes = malloc(capacity);
    CHECK(bytes != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, bytes, capacity,
                                                  &written) == UMI_STATUS_OK);
    CHECK(written == capacity);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(bytes, written,
                                                  &restored) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheCreate(restored, &restoredCache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheAllocatedBytes(restoredCache,
                                               &restoredBytes) == UMI_STATUS_OK);
    CHECK(cacheBytes == restoredBytes);
    CHECK(UmiAiTrainingKvCachePrefill(restoredCache, inputs, 3U,
                                        output, 17U) == UMI_STATUS_OK);
    UmiAiTrainingKvCacheDestroy(restoredCache);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingKvCacheDestroy(cache);
    UmiAiTrainingDecoderDestroy(model);
    free(bytes);
    puts("compact cache stale-state and restored-weight contracts: PASS");
    return 0;
}
