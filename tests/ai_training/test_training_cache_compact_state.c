/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_cache_compact_state.c
 * PURPOSE: Verify exact cached logits, rollback and retained token semantics.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/kv_cache.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); \
    return 1; } } while (0)

/* Reference uses all configured positions with unused FUTURE positions filled
 * with BOS, preserving the prefix's absolute positional embeddings. */
static int MatchesReference(UmiAiTrainingDecoder *model,
                            const size_t *ids, size_t consumed,
                            const float *actual)
{
    size_t tokens[6U];
    float logits[6U * 17U];
    const size_t retained = consumed < 6U ? consumed : 6U;
    for (size_t index = 0U; index < 6U; ++index) {
        tokens[index] = index < retained ?
            ids[consumed - retained + index] : 1U;
    }
    CHECK(UmiAiTrainingDecoderLogits(model, tokens, 6U, logits,
                                      6U * 17U) == UMI_STATUS_OK);
    for (size_t token = 0U; token < 17U; ++token) {
        const float expected = logits[(retained - 1U) * 17U + token];
        CHECK(isfinite(actual[token]));
        CHECK(fabs((double)expected - (double)actual[token]) < 0.00005);
    }
    return 0;
}

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        17U, 6U, 8U, 16U, 2U, UINT64_C(42)
    };
    const size_t tokens[] = {4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingKvCache *cache = NULL;
    float logits[17U];
    size_t size = 0U, retained = 0U;
    uint64_t rows = 0U, rebuilt = 0U;
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheCreate(model, &cache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheAllocatedBytes(cache, &size) == UMI_STATUS_OK);
    CHECK(size > 0U);
    CHECK(UmiAiTrainingKvCachePrefill(cache, tokens, 3U, logits, 17U) ==
          UMI_STATUS_OK);
    CHECK(MatchesReference(model, tokens, 3U, logits) == 0);
    for (size_t i = 3U; i < 10U; ++i) {
        CHECK(UmiAiTrainingKvCacheAppend(cache, tokens[i], logits, 17U) ==
              UMI_STATUS_OK);
        CHECK(MatchesReference(model, tokens, i + 1U, logits) == 0);
    }
    CHECK(UmiAiTrainingKvCacheInspect(cache, &retained, &rows, &rebuilt) ==
          UMI_STATUS_OK);
    CHECK(retained == 6U && rebuilt == 4U && rows == 30U);
    const size_t previousCount = retained;
    const uint64_t previousRows = rows;
    const uint64_t previousRebuilds = rebuilt;
    float outputBefore[17U];
    memcpy(outputBefore, logits, sizeof(logits));
    CHECK(UmiAiTrainingKvCacheAppend(cache, 17U, logits, 17U) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(outputBefore, logits, sizeof(logits)) == 0);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &retained, &rows, &rebuilt) ==
          UMI_STATUS_OK);
    CHECK(retained == previousCount && rows == previousRows &&
          rebuilt == previousRebuilds);
    CHECK(UmiAiTrainingKvCacheReset(cache) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingKvCacheInspect(cache, &retained, &rows, &rebuilt) ==
          UMI_STATUS_OK);
    CHECK(retained == 0U && rows == 0U && rebuilt == 0U);
    CHECK(UmiAiTrainingKvCachePrefill(cache, tokens, 1U, logits, 17U) ==
          UMI_STATUS_OK);
    CHECK(MatchesReference(model, tokens, 1U, logits) == 0);
    CHECK(UmiAiTrainingKvCacheAllocatedBytes(cache, &retained) ==
          UMI_STATUS_OK && retained == size);
    UmiAiTrainingKvCacheDestroy(cache);
    UmiAiTrainingDecoderDestroy(model);
    puts("compact cache parity, rollover, rollback and reset: PASS");
    return 0;
}
