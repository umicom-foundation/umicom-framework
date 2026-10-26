/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_training.c
 *
 * PURPOSE:
 *   Check selected-target training and resulting model weight changes.
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
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    UmiAiTrainingStackedStep step = {0};
    UmiAiTrainingStackedDecoderInfo info = {0};
    size_t inputs[4], targets[4];
    uint8_t mask[4];
    float before = 0.0F, after = 0.0F;
    TestStackedWindow(inputs, targets, mask);
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderEvaluateSelected(
          model, inputs, targets, mask, 4U, &before) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 45U; ++i) {
        CHECK(UmiAiTrainingStackedDecoderTrainSelected(
              model, inputs, targets, mask, 4U, 0.025F, &step) == UMI_STATUS_OK);
        CHECK(step.supervisedTokens == 3U);
        CHECK(isfinite(step.selectedCrossEntropyBeforeUpdate));
    }
    CHECK(UmiAiTrainingStackedDecoderEvaluateSelected(
          model, inputs, targets, mask, 4U, &after) == UMI_STATUS_OK);
    CHECK(isfinite(before) && isfinite(after));
    CHECK(after < before);
    CHECK(UmiAiTrainingStackedDecoderInspect(model, &info) == UMI_STATUS_OK);
    CHECK(info.completedTrainingSteps == 45U);
    UmiAiTrainingStackedDecoderDestroy(model);
    printf("real stacked decoder cross-entropy %.6f -> %.6f\n",
            (double)before, (double)after);
    return 0;
}
