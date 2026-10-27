/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_incremental_session_parity.c
 *
 * PURPOSE:
 *   Compare actual C23 stacked incremental sampling with the unchanged canonical
 *   growing-prefix sampler, including seeded nucleus policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_incremental_fixture.h"

static int Compare(uint32_t layers, size_t prefixCount,
                   UmiAiTrainingNucleusOptions policy, uint64_t seed)
{
    const UmiAiTrainingStackedDecoderConfig config =
        IncrementalConfig(layers, 5U);
    const size_t prompt[5] = {66U, 78U, 90U, 88U, 84U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvIncrementalSession *session = NULL;
    size_t observed[8] = {0};
    size_t reference[8] = {0};
    size_t referenceCount = 0U;
    UmiAiTrainingStackedKvIncrementalStats stats = {0};
    VERIFY(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionCreate(model,
        prompt, prefixCount, &policy, seed, &session) == UMI_STATUS_OK);
    for (size_t step = 0U; step < 8U; ++step)
        VERIFY(UmiAiTrainingStackedKvIncrementalSessionNext(session,
                &observed[step]) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedPrefixGenerateNucleus(model,
        prompt, prefixCount, &policy, 8U, seed,
        reference, 8U, &referenceCount, NULL) == UMI_STATUS_OK);
    VERIFY(referenceCount == 8U);
    VERIFY(memcmp(observed, reference, sizeof(reference)) == 0);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionInspect(session,
        &stats) == UMI_STATUS_OK);
    VERIFY(stats.generatedTokens == 8U && stats.transformerLayers == layers);
    UmiAiTrainingStackedKvIncrementalSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    return 0;
}

int main(void)
{
    const UmiAiTrainingNucleusOptions greedy = {1.0F, 1U, 1.0F};
    const UmiAiTrainingNucleusOptions sampled = {0.85F, 5U, 0.9F};
    VERIFY(Compare(1U, 1U, greedy, UINT64_C(109)) == 0);
    VERIFY(Compare(2U, 3U, sampled, UINT64_C(118)) == 0);
    VERIFY(Compare(3U, 5U, greedy, UINT64_C(127)) == 0);
    puts("Stacked KV incremental: exact sampled-token parity across rollover");
    return 0;
}
