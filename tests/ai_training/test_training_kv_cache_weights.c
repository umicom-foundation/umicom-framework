/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_kv_cache_weights.c
 * PURPOSE: Reject weight-stale caches after training and verify rebind parity.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_cache.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {11U, 4U, 8U, 16U, 2U, UINT64_C(1007)};
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingKvCache *cache = NULL, *rebound = NULL, *reloaded = NULL;
    const size_t tokens[] = {3U, 2U, 8U, 4U};
    const size_t targets[] = {2U, 8U, 4U, 3U};
    float output[11], expected[44], checkpointOutput[11], loss = -1.0F;
    uint8_t *bytes;
    size_t size = 0U, written = 0U, inspected = 999U;
    uint64_t computed = 999U, rebuilds = 999U;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheCreate(model, &cache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCachePrefill(cache, tokens, 4U, output, 11U)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(model, tokens, targets, 4U,
                                        0.01F, &loss) == UMI_STATUS_OK);
    CHECK(isfinite(loss));
    for (size_t i = 0U; i < 11U; ++i) output[i] = -77.0F;
    CHECK(UmiAiTrainingKvCacheAppend(cache, 2U, output, 11U)
          == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingKvCachePrefill(cache, tokens, 4U, output, 11U)
          == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingKvCacheReset(cache) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &inspected, &computed, &rebuilds)
          == UMI_STATUS_INVALID_STATE);
    CHECK(inspected == 999U && computed == 999U && rebuilds == 999U);
    for (size_t i = 0U; i < 11U; ++i) CHECK(output[i] == -77.0F);
    CHECK(UmiAiTrainingKvCacheCreate(model, &rebound) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCachePrefill(rebound, tokens, 4U, output, 11U)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(model, tokens, 4U, expected, 44U)
          == UMI_STATUS_OK);
    for (size_t i = 0U; i < 11U; ++i)
        CHECK(fabsf(output[i] - expected[33U + i]) < 0.00003F);

    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &size) == UMI_STATUS_OK);
    bytes = malloc(size);
    CHECK(bytes != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, bytes, size, &written)
          == UMI_STATUS_OK);
    CHECK(size == written);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(bytes, size, &restored)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheCreate(restored, &reloaded) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCachePrefill(reloaded, tokens, 4U, checkpointOutput, 11U)
          == UMI_STATUS_OK);
    for (size_t i = 0U; i < 11U; ++i)
        CHECK(fabsf(output[i] - checkpointOutput[i]) < 0.00003F);
    free(bytes);
    UmiAiTrainingKvCacheDestroy(reloaded);
    UmiAiTrainingKvCacheDestroy(rebound);
    UmiAiTrainingKvCacheDestroy(cache);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    puts("updated weights invalidate caches, restored checkpoints rebuild parity");
    return 0;
}
