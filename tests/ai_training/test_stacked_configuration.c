/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_configuration.c
 *
 * PURPOSE:
 *   Check model configuration and scaling estimates against decoder metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | model configuration and planning checks | Sammy Hegab | MIT */
#include "stacked_test_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    UmiAiTrainingModelScalingEstimate estimate = {0};
    UmiAiTrainingStackedDecoder *model = NULL;
    UmiAiTrainingStackedDecoderInfo info = {0};
    CHECK(UmiAiTrainingStackedDecoderPlan(&cfg, &estimate) == UMI_STATUS_OK);
    CHECK(estimate.totalParameters > 0U);
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderInspect(model, &info) == UMI_STATUS_OK);
    CHECK(info.config.transformerLayers == 2U);
    CHECK(info.trainableParameterTensors == 27U);
    CHECK((uint64_t)info.trainableParameterElements == estimate.totalParameters);
    CHECK(info.completedTrainingSteps == 0U);
    UmiAiTrainingStackedDecoderDestroy(model);
    model = NULL;
    cfg.transformerLayers = 0U;
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(model == NULL);
    cfg = TestStackedConfig();
    cfg.transformerLayers = UMI_AI_STACKED_MAX_LAYERS + 1U;
    CHECK(UmiAiTrainingStackedDecoderPlan(&cfg, &estimate) == UMI_STATUS_INVALID_ARGUMENT);
    cfg = TestStackedConfig();
    cfg.attentionHeads = 3U;
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_INVALID_ARGUMENT);
    cfg = TestStackedConfig();
    cfg.vocabularySize = 0U;
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_INVALID_ARGUMENT);
    puts("stacked configuration, independent scaling plan and shape rejection passed");
    return 0;
}
