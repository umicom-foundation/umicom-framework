/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_stacked_prefix_streaming.c
 * PURPOSE: Compare one-shot and streaming unpadded generation against the
 *          REAL established stacked decoder and nucleus sampler.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_prefix_inference.h"
#include "umicom/ai/training/stacked_prefix_cache_plan.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "prefix streaming line %d: %s\n", __LINE__, #x); return 1; } } while (0)
int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {32U, 4U, 4U, 8U, 2U, 2U, UINT64_C(7081)};
    const size_t prompt[] = {5U, 9U};
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedPrefixSession *session = NULL;
    UmiAiTrainingStackedPrefixStats streaming = {0}, oneShot = {0};
    UmiAiTrainingStackedPrefixCachePlan hypothetical = {0};
    size_t pulled[6] = {0}, produced[6] = {0}, count = 0U;
    CHECK(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedPrefixSessionCreate(model, prompt, 2U,
          &policy, UINT64_C(1234), &session) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedPrefixCachePlanBuild(&config, 2U, 6U,
          &hypothetical) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 6U; ++i) {
        CHECK(UmiAiTrainingStackedPrefixSessionNext(session, &pulled[i]) == UMI_STATUS_OK);
        CHECK(pulled[i] >= 4U && pulled[i] < (size_t)config.vocabularySize);
    }
    CHECK(UmiAiTrainingStackedPrefixSessionInspect(session, &streaming) == UMI_STATUS_OK);
    CHECK(streaming.generatedTokens == 6U && streaming.retainedTokens == 4U);
    CHECK(streaming.fullWindowRebuilds == 3U);
    CHECK(streaming.computedFullContextTokenRows == 24U);
    CHECK(streaming.computedTransformerTokenLayerRows == 48U);
    CHECK(hypothetical.canonicalTokenLayerRows == 48U);
    CHECK(hypothetical.candidateTokenLayerRows <= hypothetical.canonicalTokenLayerRows);
    UmiAiTrainingStackedPrefixSessionDestroy(session);
    CHECK(UmiAiTrainingStackedPrefixGenerateNucleus(model, prompt, 2U,
          &policy, 6U, UINT64_C(1234), produced, 6U, &count,
          &oneShot) == UMI_STATUS_OK);
    CHECK(count == 6U && memcmp(pulled, produced, sizeof(pulled)) == 0);
    CHECK(oneShot.computedTransformerTokenLayerRows ==
          streaming.computedTransformerTokenLayerRows);
    CHECK(oneShot.fullWindowRebuilds == streaming.fullWindowRebuilds);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked prefix: real model streaming and one-shot parity passed");
    return 0;
}
