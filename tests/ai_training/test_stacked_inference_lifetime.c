/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_inference_lifetime.c
 *
 * PURPOSE:
 *   Stale-model session lifetime
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_inference_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedGenerationSession *session = NULL;
    const UmiAiTrainingNucleusOptions policy = {1.0F, 1U, 1.0F};
    const size_t prompt[] = {109U, 114U};
    const size_t inputs[] = {1U, 109U, 114U, 120U};
    const size_t targets[] = {0U, 0U, 23U, 24U};
    const uint8_t mask[] = {0U, 0U, 1U, 1U};
    UmiAiTrainingStackedStep step = {0};
    size_t sampled = 12345U;
    UmiAiTrainingStackedGenerationStats stats = {0};
    CHECK(MakeModel(UINT64_C(9), &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedGenerationSessionCreate(model, prompt, 2U,
        &policy, 5U, &session) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderTrainSelected(model, inputs,
        targets, mask, 4U, 0.001F, &step) == UMI_STATUS_OK);
    CHECK(step.completedTrainingSteps == 1U);
    CHECK(UmiAiTrainingStackedGenerationSessionNext(session, &sampled) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(sampled == 12345U);
    memset(&stats, 0x55, sizeof(stats));
    UmiAiTrainingStackedGenerationStats unchanged = stats;
    CHECK(UmiAiTrainingStackedGenerationSessionInspect(session, &stats) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&stats, &unchanged, sizeof(stats)) == 0);
    UmiAiTrainingStackedGenerationSessionDestroy(session);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked inference: training invalidates pinned generation sessions");
    return 0;
}
