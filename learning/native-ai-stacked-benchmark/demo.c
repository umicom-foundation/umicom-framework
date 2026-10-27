/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-stacked-benchmark/demo.c
 *
 * PURPOSE:
 *   Native comparative inference lab: identical model, prompt, seed and policy.
 *   The times represent entire generation calls, not time to first token.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_inference_benchmark.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig modelConfig = {
        260U, 6U, 8U, 12U, 2U, 2U, UINT64_C(19383)
    };
    const UmiAiTrainingStackedBenchmarkConfig benchmark = {
        1U, 5U, 8U, UINT64_C(31), {1.0F, 1U, 1.0F}
    };
    const size_t prompt[] = {69U, 88U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedBenchmarkReport result = {0};
    UmiStatus status = UmiAiTrainingStackedDecoderCreate(&modelConfig, &model);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Cannot create educational model: %d\n", (int)status);
        return 1;
    }
    status = UmiAiTrainingStackedBenchmarkRun(model, prompt,
        sizeof(prompt) / sizeof(prompt[0]), &benchmark, &result);
    UmiAiTrainingStackedDecoderDestroy(model);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "Benchmark rejected: %d\n", (int)status);
        return 1;
    }
    printf("Token-identical pairs: %zu\n", result.completeMatchingPairs);
    printf("Canonical median, ns: %" PRIu64 "\n", result.canonicalMedianNanoseconds);
    printf("Cached median, ns: %" PRIu64 "\n", result.cachedMedianNanoseconds);
    printf("Canonical layer rows: %" PRIu64 "\n", result.canonicalComputedTokenLayerRows);
    printf("Cached full-context layer rows: %" PRIu64 "\n",
        result.cachedComputedFullContextTokenLayerRows);
    printf("Cached incremental projection layer rows: %" PRIu64 "\n",
        result.cachedIncrementalProjectionTokenLayerRows);
    printf("Cached attention query layer rows: %" PRIu64 "\n",
        result.cachedAttentionQueryTokenLayerRows);
    printf("Cached rollovers: %" PRIu64 "\n", result.cachedWindowRebuilds);
    puts("Educational CPU laboratory only; no deployment-level speedup claim.");
    return 0;
}
