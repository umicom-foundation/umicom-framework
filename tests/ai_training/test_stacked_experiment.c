/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_experiment.c
 *
 * PURPOSE:
 *   Check deterministic training epochs and held-out evaluation results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_test_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    UmiAiTrainingStackedDecoder *a = NULL, *b = NULL;
    const size_t inputs[3][4] = {
        {1U, 5U, 6U, 7U}, {1U, 6U, 7U, 8U}, {1U, 5U, 8U, 9U}
    };
    const size_t targets[3][4] = {
        {0U, 6U, 7U, 2U}, {0U, 7U, 8U, 2U}, {0U, 8U, 9U, 2U}
    };
    const uint8_t mask[4] = {0U, 1U, 1U, 1U};
    UmiAiTrainingStackedExample examples[3] = {0};
    UmiAiTrainingStackedExperimentReport reportA = {0}, reportB = {0};
    const UmiAiTrainingStackedExperimentPlan plan = {3U, 0.025F, 555U};
    for (size_t i = 0U; i < 3U; ++i) {
        examples[i].inputIds = inputs[i];
        examples[i].targetIds = targets[i];
        examples[i].lossMask = mask;
        examples[i].tokenCount = 4U;
        examples[i].partition = i < 2U
            ? UMI_AI_TRAINING_PARTITION_TRAIN : UMI_AI_TRAINING_PARTITION_EVALUATE;
        TestStackedSourceDigest(examples[i].originalSourceSha256,
                                (uint8_t)(i * 3U + 1U));
    }
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &a) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &b) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedExperimentValidate(a, examples,
           3U, &plan) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedExperimentRun(a, examples,
           3U, &plan, &reportA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedExperimentRun(b, examples,
           3U, &plan, &reportB) == UMI_STATUS_OK);
    CHECK(reportA.trainingExamples == 2U && reportA.evaluationExamples == 1U);
    CHECK(reportA.epochsCompleted == 3U && reportA.completedTrainingSteps == 6U);
    CHECK(reportA.supervisedTrainingTokens == 18U);
    CHECK(isfinite(reportA.heldoutLossBefore) &&
          isfinite(reportA.heldoutLossAfter));
    CHECK(memcmp(&reportA, &reportB, sizeof(reportA)) == 0);
    UmiAiTrainingStackedDecoderDestroy(b);
    UmiAiTrainingStackedDecoderDestroy(a);
    puts("real stacked model deterministic train/evaluation separation passed");
    return 0;
}
