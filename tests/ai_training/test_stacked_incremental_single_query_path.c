/*-----------------------------------------------------------------------------
 * Umicom Framework | Multi-layer single-query session qualification
 * Covers growing prefix, first and repeated rollover, exact sampled IDs and
 * the observed query-row counter. Author: Sammy Hegab | MIT
 *---------------------------------------------------------------------------*/
#include "stacked_incremental_fixture.h"

static int Run(uint32_t layers)
{
    const UmiAiTrainingStackedDecoderConfig config =
        IncrementalConfig(layers, 6U);
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[1] = {69U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvIncrementalSession *session = NULL;
    UmiAiTrainingStackedKvIncrementalStats observed = {0};
    size_t generated[8] = {0};
    size_t canonical[8] = {0};
    size_t count = 0U;
    const uint64_t seed = UINT64_C(9631) + (uint64_t)layers;
    VERIFY(UmiAiTrainingStackedDecoderCreate(&config, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionCreate(model,
        prompt, 1U, &policy, seed, &session) == UMI_STATUS_OK);
    for (size_t index = 0U; index < 8U; ++index)
        VERIFY(UmiAiTrainingStackedKvIncrementalSessionNext(
            session, &generated[index]) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedPrefixGenerateNucleus(model,
        prompt, 1U, &policy, 8U, seed,
        canonical, 8U, &count, NULL) == UMI_STATUS_OK);
    VERIFY(count == 8U && memcmp(generated, canonical, sizeof(generated)) == 0);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionInspect(session,
        &observed) == UMI_STATUS_OK);
    VERIFY(observed.observedIncrementalAppends == 5U);
    VERIFY(observed.observedFullWindowRebuilds == 2U);
    VERIFY(observed.observedIncrementalProjectionTokenLayerRows ==
        UINT64_C(5) * (uint64_t)layers);
    VERIFY(observed.observedFullContextTokenLayerRows ==
        UINT64_C(18) * (uint64_t)layers);
    VERIFY(observed.observedAttentionQueryTokenLayerRows ==
        UINT64_C(23) * (uint64_t)layers);
    VERIFY(observed.referenceFullContextTokenLayerRows ==
        UINT64_C(48) * (uint64_t)layers);
    UmiAiTrainingStackedKvIncrementalSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    return 0;
}

int main(void)
{
    VERIFY(Run(1U) == 0);
    VERIFY(Run(2U) == 0);
    VERIFY(Run(3U) == 0);
    puts("Single-query session matches canonical generation and query counters");
    return 0;
}
