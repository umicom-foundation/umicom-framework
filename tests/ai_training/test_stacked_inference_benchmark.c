/* Umicom Framework | C23 paired benchmark acceptance | MIT
 * Includes rejection atomicity and first/repeated learned-position rollover.
 * AUTHOR: Sammy Hegab / Umicom Foundation */
#include "stacked_incremental_fixture.h"
#include "umicom/ai/training/stacked_inference_benchmark.h"

static int Exercise(uint32_t layers, size_t promptCount)
{
    const UmiAiTrainingStackedDecoderConfig shape = IncrementalConfig(layers, 5U);
    const size_t prompts[5] = {70U, 71U, 72U, 73U, 74U};
    const UmiAiTrainingStackedBenchmarkConfig plan = {
        1U, 3U, 7U, UINT64_C(101), {1.0F, 1U, 1.0F}
    };
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedBenchmarkReport result = {0};
    VERIFY(UmiAiTrainingStackedDecoderCreate(&shape, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedBenchmarkRun(model, prompts, promptCount,
        &plan, &result) == UMI_STATUS_OK);
    VERIFY(result.completeMatchingPairs == 4U);
    VERIFY(result.generatedTokens == 7U && result.transformerLayers == layers);
    VERIFY(result.canonicalMedianNanoseconds > 0U);
    VERIFY(result.cachedMedianNanoseconds > 0U);
    VERIFY(result.canonicalComputedTokenLayerRows > 0U);
    VERIFY(result.cachedAttentionQueryTokenLayerRows > 0U);
    if (promptCount == 5U) VERIFY(result.cachedWindowRebuilds > 0U);
    UmiAiTrainingStackedBenchmarkReport unchanged;
    memset(&unchanged, 0xa5, sizeof(unchanged));
    result = unchanged;
    UmiAiTrainingStackedBenchmarkConfig invalid = plan;
    invalid.repetitions = 0U;
    VERIFY(UmiAiTrainingStackedBenchmarkRun(model, prompts, promptCount,
        &invalid, &result) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(memcmp(&result, &unchanged, sizeof(result)) == 0);
    invalid = plan;
    invalid.generatedTokens = 65U;
    VERIFY(UmiAiTrainingStackedBenchmarkRun(model, prompts, promptCount,
        &invalid, &result) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(memcmp(&result, &unchanged, sizeof(result)) == 0);
    UmiAiTrainingStackedDecoderDestroy(model);
    return 0;
}

int main(void)
{
    VERIFY(Exercise(1U, 1U) == 0);
    VERIFY(Exercise(2U, 3U) == 0);
    VERIFY(Exercise(3U, 5U) == 0);
    puts("Paired native inference benchmark: bounded, parity checked and atomic");
    return 0;
}
