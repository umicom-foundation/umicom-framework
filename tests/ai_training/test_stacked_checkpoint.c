/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_checkpoint.c
 *
 * PURPOSE:
 *   Check that an owned checkpoint restores independent model state and logits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | independent stacked-model snapshot restoration | Sammy Hegab | MIT */
#include "stacked_test_fixture.h"
int main(void)
{
    UmiAiTrainingStackedDecoder *model = NULL, *loaded = NULL;
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    UmiAiTrainingStackedCheckpointInfo info = {0};
    UmiAiTrainingStackedDecoderInfo restored = {0};
    size_t inputs[4], targets[4], required = 0U, written = 0U;
    uint8_t mask[4], *bytes;
    float first[4U * 16U], second[4U * 16U];
    UmiAiTrainingStackedStep step = {0};
    TestStackedWindow(inputs, targets, mask);
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderTrainSelected(model, inputs, targets,
          mask, 4U, 0.03F, &step) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderLogits(model, inputs, 4U,
          first, 64U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedCheckpointSize(model, &required) == UMI_STATUS_OK);
    CHECK(required > 100U);
    bytes = malloc(required);
    CHECK(bytes != NULL);
    CHECK(UmiAiTrainingStackedCheckpointExport(model, bytes, required,
          &written) == UMI_STATUS_OK);
    CHECK(written == required);
    CHECK(UmiAiTrainingStackedCheckpointInspect(bytes, written,
          &info) == UMI_STATUS_OK);
    CHECK(info.config.transformerLayers == 2U);
    CHECK(info.completedTrainingSteps == 1U);
    CHECK(info.parameterTensors == 27U);
    CHECK(UmiAiTrainingStackedCheckpointDecode(bytes, written,
          &loaded) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderInspect(loaded, &restored) == UMI_STATUS_OK);
    CHECK(restored.completedTrainingSteps == 1U);
    CHECK(UmiAiTrainingStackedDecoderLogits(loaded, inputs, 4U,
          second, 64U) == UMI_STATUS_OK);
    CHECK(memcmp(first, second, sizeof(first)) == 0);
    UmiAiTrainingStackedDecoderDestroy(loaded);
    UmiAiTrainingStackedDecoderDestroy(model);
    free(bytes);
    puts("portable stacked model restored exact logits and training step");
    return 0;
}
