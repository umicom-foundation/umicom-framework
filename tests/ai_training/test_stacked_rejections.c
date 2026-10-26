/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_rejections.c
 *
 * PURPOSE:
 *   Check selected-loss failures and capacity refusals preserve prior state.
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
    size_t inputs[4], targets[4];
    uint8_t mask[4], missing[4] = {0};
    float floatSentinel = -419.0F;
    UmiAiTrainingStackedStep step = {.completedTrainingSteps = 12345U};
    UmiAiTrainingStackedDecoderInfo info = {0};
    TestStackedWindow(inputs, targets, mask);
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderEvaluateSelected(model, inputs, targets,
           missing, 4U, &floatSentinel) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(floatSentinel == -419.0F);
    CHECK(UmiAiTrainingStackedDecoderLogits(model, inputs, 4U,
          &floatSentinel, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(floatSentinel == -419.0F);
    inputs[1] = 99U;
    CHECK(UmiAiTrainingStackedDecoderTrainSelected(model, inputs, targets,
           mask, 4U, 0.02F, &step) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step.completedTrainingSteps == 12345U);
    inputs[1] = 5U;
    CHECK(UmiAiTrainingStackedDecoderTrainSelected(model, inputs, targets,
           mask, 4U, NAN, &step) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(step.completedTrainingSteps == 12345U);
    mask[2] = 2U;
    CHECK(UmiAiTrainingStackedDecoderEvaluateSelected(model, inputs, targets,
           mask, 4U, &floatSentinel) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(floatSentinel == -419.0F);
    CHECK(UmiAiTrainingStackedDecoderInspect(model, &info) == UMI_STATUS_OK);
    CHECK(info.completedTrainingSteps == 0U);
    UmiAiTrainingStackedDecoderDestroy(model);
    puts("stacked model invalid masks, IDs and output capacities passed");
    return 0;
}
