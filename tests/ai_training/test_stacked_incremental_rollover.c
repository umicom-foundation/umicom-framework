/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_incremental_rollover.c
 *
 * PURPOSE:
 *   Learned-position rollover must rebuild every layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_incremental_fixture.h"

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig cfg = IncrementalConfig(2U, 4U);
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[4] = {20U, 21U, 22U, 23U};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedKvIncrementalSession *session = NULL;
    UmiAiTrainingStackedKvIncrementalStats report = {0};
    VERIFY(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionCreate(model,
        prompt, 4U, &policy, UINT64_C(202), &session) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 7U; ++i) {
        size_t sampled = 0U;
        VERIFY(UmiAiTrainingStackedKvIncrementalSessionNext(session,
            &sampled) == UMI_STATUS_OK);
        VERIFY(sampled >= 4U && sampled < 260U);
    }
    VERIFY(UmiAiTrainingStackedKvIncrementalSessionInspect(session,
        &report) == UMI_STATUS_OK);
    VERIFY(report.generatedTokens == 7U);
    VERIFY(report.observedFullWindowRebuilds == 6U);
    VERIFY(report.observedIncrementalAppends == 0U);
    VERIFY(report.observedFullContextTokenLayerRows == (uint64_t)(4U * 2U * 7U));
    VERIFY(report.observedIncrementalProjectionTokenLayerRows == 0U);
    UmiAiTrainingStackedKvIncrementalSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("Rollover: full canonical rebuilds, no stale K/V reuse");
    return 0;
}
