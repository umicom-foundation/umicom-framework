/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_generation_performance.c
 * PURPOSE: Real decoder generation parity plus honest KV-cache work accounting.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_performance.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expr); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(20261023)
    };
    const UmiAiTrainingNucleusOptions policies[] = {
        {1.0F, 1U, 1.0F},
        {0.85F, 12U, 0.90F}
    };
    const size_t prompt[] = {109U, 114U, 120U, 37U, 45U, 63U,
        78U, 88U, 95U, 112U, 131U, 150U};
    const size_t promptCounts[] = {0U, 3U, 12U};
    const size_t generatedCounts[] = {1U, 6U, 12U};
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingGenerationPerformance report = {0};
    UmiAiTrainingGenerationWorkEstimate estimate = {0};
    uint8_t *before = NULL, *after = NULL;
    size_t checkpointSize = 0U, written = 0U;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize) == UMI_STATUS_OK);
    before = malloc(checkpointSize); after = malloc(checkpointSize);
    CHECK(before != NULL && after != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before,
        checkpointSize, &written) == UMI_STATUS_OK && written == checkpointSize);

    CHECK(UmiAiTrainingGenerationWorkEstimateBuild(&config, 0U, 1U,
        &estimate) == UMI_STATUS_OK);
    CHECK(estimate.referenceComputedRows == 8U && estimate.cachedComputedRows == 8U);
    CHECK(estimate.cachedWindowRebuilds == 0U && estimate.cacheHeapBytes > 0U);
    CHECK(UmiAiTrainingGenerationWorkEstimateBuild(&config, 12U, 12U,
        &estimate) == UMI_STATUS_OK);
    CHECK(estimate.referenceComputedRows == 96U && estimate.cachedComputedRows == 96U);
    CHECK(estimate.cachedWindowRebuilds == 11U);

    for (size_t k = 0U; k < 2U; ++k) {
        for (size_t p = 0U; p < 3U; ++p) {
            for (size_t n = 0U; n < 3U; ++n) {
                const size_t count = generatedCounts[n];
                const size_t promptLength = promptCounts[p];
                CHECK(UmiAiTrainingGenerationPerformanceCompare(model,
                    promptLength == 0U ? NULL : prompt, promptLength,
                    &policies[k], count, UINT64_C(7801), &report) == UMI_STATUS_OK);
                CHECK(report.matchingTokens == count);
                CHECK(report.estimate.generatedTokens == count);
                CHECK(report.estimate.promptTokens == promptLength);
                CHECK(report.estimate.contextTokens == 8U);
                CHECK(report.observedRetainedTokens == 8U);
                CHECK(report.estimate.referenceComputedRows == 8U * count);
                CHECK(report.observedCachedRows == 8U * count);
                CHECK(report.observedWindowRebuilds == count - 1U);
                CHECK(report.estimate.cacheHeapBytes > 0U);
                CHECK(report.cpuClockAvailable == 0 || report.cpuClockAvailable == 1);
                CHECK(report.referenceProcessCpuSeconds >= 0.0);
                CHECK(report.cachedProcessCpuSeconds >= 0.0);
            }
        }
    }
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
        checkpointSize, &written) == UMI_STATUS_OK && written == checkpointSize);
    CHECK(memcmp(before, after, checkpointSize) == 0);
    free(after); free(before);
    UmiAiTrainingDecoderDestroy(model);
    puts("native generation workload: real decoder parity and nonmutating measurement PASS");
    return 0;
}
