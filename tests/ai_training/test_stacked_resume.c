/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_resume.c
 *
 * PURPOSE:
 *   Check stateless SGD continuation after checkpoint recovery.
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
    UmiAiTrainingStackedDecoder *model = NULL, *resumed = NULL;
    UmiAiTrainingStackedDecoderConfig cfg = TestStackedConfig();
    size_t inputs[4], targets[4], bytesNeeded = 0U, written = 0U;
    uint8_t mask[4], *checkpoint;
    UmiAiTrainingStackedStep step = {0};
    float uninterrupted[64], restored[64];
    TestStackedWindow(inputs, targets, mask);
    CHECK(UmiAiTrainingStackedDecoderCreate(&cfg, &model) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 5U; ++i)
        CHECK(UmiAiTrainingStackedDecoderTrainSelected(model, inputs, targets,
              mask, 4U, 0.02F, &step) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedCheckpointSize(model, &bytesNeeded) == UMI_STATUS_OK);
    checkpoint = malloc(bytesNeeded);
    CHECK(checkpoint != NULL);
    CHECK(UmiAiTrainingStackedCheckpointExport(model, checkpoint,
          bytesNeeded, &written) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedCheckpointDecode(checkpoint, written,
          &resumed) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 8U; ++i) {
        CHECK(UmiAiTrainingStackedDecoderTrainSelected(model, inputs, targets,
              mask, 4U, 0.02F, &step) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingStackedDecoderTrainSelected(resumed, inputs, targets,
              mask, 4U, 0.02F, &step) == UMI_STATUS_OK);
    }
    CHECK(step.completedTrainingSteps == 13U);
    CHECK(UmiAiTrainingStackedDecoderLogits(model, inputs, 4U,
          uninterrupted, 64U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingStackedDecoderLogits(resumed, inputs, 4U,
          restored, 64U) == UMI_STATUS_OK);
    CHECK(memcmp(uninterrupted, restored, sizeof(restored)) == 0);
    UmiAiTrainingStackedDecoderDestroy(resumed);
    UmiAiTrainingStackedDecoderDestroy(model);
    free(checkpoint);
    puts("same-build stacked SGD continuation reproduced exact logits");
    return 0;
}
