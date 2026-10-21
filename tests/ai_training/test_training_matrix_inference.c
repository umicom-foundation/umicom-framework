/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_matrix_inference.c
 * PURPOSE: Verify tiled projections retain canonical and cached logits parity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/kv_cache.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {9U, 4U, 4U, 8U, 2U, UINT64_C(773)};
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingKvCache *cache = NULL;
    const size_t input[4] = {1U, 3U, 2U, 7U};
    float canonical[4U * 9U] = {0};
    float projected[9U] = {0};
    size_t retained = 0U;
    uint64_t computed = 0U, rebuilds = 0U;

    CHECK(UmiAiTrainingDecoderCreate(&config, &decoder) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheCreate(decoder, &cache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(decoder, input, 4U, canonical,
        4U * 9U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCachePrefill(cache, input, 4U, projected,
        9U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 9U; ++i)
        CHECK(fabs((double)projected[i] -
                   (double)canonical[3U * 9U + i]) < 0.00008);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &retained, &computed,
        &rebuilds) == UMI_STATUS_OK);
    CHECK(retained == 4U && computed == 4U && rebuilds == 0U);

    CHECK(UmiAiTrainingKvCacheReset(cache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCachePrefill(cache, input, 1U, projected,
        9U) == UMI_STATUS_OK);
    for (size_t time = 1U; time < 4U; ++time)
        CHECK(UmiAiTrainingKvCacheAppend(cache, input[time], projected,
            9U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 9U; ++i)
        CHECK(fabs((double)projected[i] -
                   (double)canonical[3U * 9U + i]) < 0.00008);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &retained, &computed,
        &rebuilds) == UMI_STATUS_OK);
    CHECK(retained == 4U && computed == 4U && rebuilds == 0U);

    /* Learned absolute positions force rebuilding after a full-window shift. */
    size_t shifted[4] = {3U, 2U, 7U, 8U};
    CHECK(UmiAiTrainingKvCacheAppend(cache, 8U, projected,
        9U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(decoder, shifted, 4U, canonical,
        4U * 9U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 9U; ++i)
        CHECK(fabs((double)projected[i] -
                   (double)canonical[3U * 9U + i]) < 0.00008);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &retained, &computed,
        &rebuilds) == UMI_STATUS_OK);
    CHECK(retained == 4U && computed == 8U && rebuilds == 1U);
    UmiAiTrainingKvCacheDestroy(cache);
    UmiAiTrainingDecoderDestroy(decoder);
    puts("shared tiled kernel retains cached decoder logits parity: PASS");
    return 0;
}
