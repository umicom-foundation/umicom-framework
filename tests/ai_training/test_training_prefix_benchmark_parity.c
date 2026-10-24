/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_prefix_benchmark_parity.c
 * PURPOSE:
 *   Exercise REAL canonical-decoder vs cached-prefix sampling, reproducible
 *   seeds, immutable checkpoint bytes and independently predicted KV work.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/prefix_benchmark.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/checkpoint.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; \
} } while (0)

static int RunCases(const UmiAiTrainingDecoderConfig *config)
{
    const UmiAiTrainingNucleusOptions policies[] = {
        {1.0F, 1U, 1.0F},
        {0.85F, 12U, 0.90F}
    };
    const size_t prompt[] = {109U, 114U, 120U, 35U, 64U, 74U, 83U, 93U};
    const size_t lengths[] = {1U, 3U};
    const size_t generationCounts[] = {1U, 10U};
    UmiAiTrainingDecoder *model = NULL;
    uint8_t *before = NULL, *after = NULL;
    size_t checkpointSize = 0U, written = 0U;

    CHECK(UmiAiTrainingDecoderCreate(config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize)
          == UMI_STATUS_OK);
    before = malloc(checkpointSize);
    after = malloc(checkpointSize);
    CHECK(before != NULL && after != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before,
          checkpointSize, &written) == UMI_STATUS_OK);
    CHECK(written == checkpointSize);

    for (size_t p = 0U; p < sizeof policies / sizeof policies[0]; ++p) {
        for (size_t n = 0U; n < sizeof generationCounts / sizeof generationCounts[0]; ++n) {
            for (size_t l = 0U; l < sizeof lengths / sizeof lengths[0]; ++l) {
                UmiAiTrainingPrefixBenchmarkPlan plan = {
                    generationCounts[n], 1U, 3U, UINT64_C(20261024)
                };
                UmiAiTrainingPrefixBenchmarkResult result = {0};
                UmiAiTrainingPrefixBenchmarkResult again = {0};
                const size_t promptCount = lengths[l];
                CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt,
                      promptCount, &policies[p], &plan, &result) == UMI_STATUS_OK);
                CHECK(UmiAiTrainingPrefixBenchmarkRun(model, prompt,
                      promptCount, &policies[p], &plan, &again) == UMI_STATUS_OK);
                CHECK(result.tokenFingerprint == again.tokenFingerprint);
                CHECK(result.verifiedPairs == 4U);
                CHECK(result.comparedTokens == 4U * plan.generatedTokens);
                CHECK(result.checkpointUnchanged == 1);
                CHECK(result.checkpointBytes == checkpointSize);
                CHECK(result.estimate.canonicalComputedRows ==
                      (uint64_t)config->contextLength * plan.generatedTokens);
                CHECK(result.observedCache.computedRows ==
                      result.estimate.cachedComputedRows);
                CHECK(result.observedCache.windowRebuilds ==
                      result.estimate.cachedWindowRebuilds);
                CHECK(result.estimate.computedRowsAvoided ==
                      result.estimate.canonicalComputedRows -
                      result.estimate.cachedComputedRows);
                CHECK(result.wallClockAvailable == 0 || result.wallClockAvailable == 1);
                CHECK(result.wallClockMonotonic == 0 || result.wallClockMonotonic == 1);
                CHECK(result.cpuClockAvailable == 0 || result.cpuClockAvailable == 1);
                if (result.wallClockAvailable) {
                    CHECK(result.canonical.minimumWallSeconds > 0.0);
                    CHECK(result.canonical.minimumWallSeconds <=
                          result.canonical.medianWallSeconds);
                    CHECK(result.canonical.medianWallSeconds <=
                          result.canonical.maximumWallSeconds);
                    CHECK(result.incremental.minimumWallSeconds > 0.0);
                    CHECK(result.incremental.minimumWallSeconds <=
                          result.incremental.medianWallSeconds);
                    CHECK(result.incremental.medianWallSeconds <=
                          result.incremental.maximumWallSeconds);
                }
                if (result.cpuClockAvailable) {
                    CHECK(result.canonical.medianProcessCpuSeconds > 0.0);
                    CHECK(result.incremental.medianProcessCpuSeconds > 0.0);
                }
            }
        }
    }
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
          checkpointSize, &written) == UMI_STATUS_OK);
    CHECK(written == checkpointSize);
    CHECK(memcmp(before, after, checkpointSize) == 0);
    free(after); free(before);
    UmiAiTrainingDecoderDestroy(model);
    return 0;
}

int main(void)
{
    const UmiAiTrainingDecoderConfig configs[] = {
        {UMI_AI_BYTE_VOCABULARY, 4U, 8U, 16U, 2U, UINT64_C(73)},
        {UMI_AI_BYTE_VOCABULARY, 8U, 8U, 16U, 2U, UINT64_C(74)}
    };
    for (size_t i = 0U; i < sizeof configs / sizeof configs[0]; ++i)
        CHECK(RunCases(&configs[i]) == 0);
    puts("prefix benchmark: canonical token parity, workload, repeatability and checkpoint preservation PASS");
    return 0;
}
