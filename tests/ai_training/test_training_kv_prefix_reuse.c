/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_kv_prefix_reuse.c
 * PURPOSE: Qualify real decoder growing-prefix reuse and nonmutation.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_prefix_reuse.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expr); \
    return 1; } } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig configs[] = {
        {5U, 1U, 4U, 8U, 1U, UINT64_C(101)},
        {16U, 8U, 8U, 12U, 4U, UINT64_C(202)},
        {260U, 8U, 8U, 16U, 2U, UINT64_C(303)},
        {32U, 16U, 16U, 32U, 8U, UINT64_C(404)}
    };
    for (size_t k = 0U; k < sizeof(configs) / sizeof(configs[0]); ++k) {
        const UmiAiTrainingDecoderConfig *config = &configs[k];
        UmiAiTrainingDecoder *model = NULL;
        UmiAiTrainingKvPrefixPerformance report = {0};
        UmiAiTrainingKvPrefixWorkEstimate estimate = {0};
        size_t tokens[UMI_AI_DECODER_MAX_CONTEXT];
        size_t checkpointSize = 0U, written = 0U;
        uint8_t *before = NULL, *after = NULL;
        const size_t context = (size_t)config->contextLength;
        const size_t lengths[] = {1U, 2U, 5U, 8U, 16U};

        CHECK(UmiAiTrainingDecoderCreate(config, &model) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize) == UMI_STATUS_OK);
        before = malloc(checkpointSize);
        after = malloc(checkpointSize);
        CHECK(before != NULL && after != NULL);
        CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before,
            checkpointSize, &written) == UMI_STATUS_OK && written == checkpointSize);
        for (size_t i = 0U; i < context; ++i)
            tokens[i] = (i * 11U + 3U) % (size_t)config->vocabularySize;
        for (size_t i = 0U; i < sizeof(lengths) / sizeof(lengths[0]); ++i) {
            const size_t count = lengths[i];
            uint64_t baseline;
            if (count > context) continue;
            baseline = (uint64_t)count * ((uint64_t)count + 1U) / 2U;
            CHECK(UmiAiTrainingKvPrefixWorkEstimateBuild(config,
                count, &estimate) == UMI_STATUS_OK);
            CHECK(estimate.referenceComputedRows == baseline);
            CHECK(estimate.incrementalComputedRows == count);
            CHECK(estimate.computedRowsAvoided == baseline - count);
            CHECK(estimate.singleCacheHeapBytes > 0U);
            CHECK(UmiAiTrainingKvPrefixPerformanceCompare(model,
                tokens, count, &report) == UMI_STATUS_OK);
            CHECK(report.matchingPrefixes == count);
            CHECK(report.observedReferenceRows == baseline);
            CHECK(report.observedIncrementalRows == count);
            CHECK(report.observedWindowRebuilds == 0U);
            CHECK(report.maximumAbsoluteLogitDifference >= 0.0F);
            CHECK(report.maximumAbsoluteLogitDifference <= 0.00004F);
            CHECK(report.estimate.singleCacheHeapBytes == estimate.singleCacheHeapBytes);
            CHECK(report.cpuClockAvailable == 0 || report.cpuClockAvailable == 1);
            CHECK(report.referenceProcessCpuSeconds >= 0.0);
            CHECK(report.incrementalProcessCpuSeconds >= 0.0);
        }
        CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after,
            checkpointSize, &written) == UMI_STATUS_OK && written == checkpointSize);
        CHECK(memcmp(before, after, checkpointSize) == 0);
        free(before); free(after);
        UmiAiTrainingDecoderDestroy(model);
    }
    puts("KV growing-prefix reuse: real decoder logits, work and checkpoint parity PASS");
    return 0;
}
