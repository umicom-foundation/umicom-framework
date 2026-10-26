/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-stacked/demo.c
 *
 * PURPOSE:
 *   Demonstrate stacked-decoder evaluation, training and checkpoint recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/stacked_decoder.h"
#include "umicom/ai/training/stacked_checkpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    const UmiAiTrainingStackedDecoderConfig config = {
        16U, 4U, 8U, 12U, 2U, 2U, UINT64_C(20261026)
    };
    const size_t inputs[4] = {1U, 5U, 6U, 7U};
    const size_t targets[4] = {0U, 6U, 7U, 2U};
    const uint8_t mask[4] = {0U, 1U, 1U, 1U};
    UmiAiTrainingStackedDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingStackedStep step = {0};
    UmiAiTrainingStackedDecoderInfo info = {0};
    float before = 0.0F, after = 0.0F;
    uint8_t *snapshot = NULL;
    size_t required = 0U, written = 0U;
    UmiStatus status = UmiAiTrainingStackedDecoderCreate(&config, &model);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiAiTrainingStackedDecoderEvaluateSelected(
        model, inputs, targets, mask, 4U, &before);
    if (status != UMI_STATUS_OK) goto done;
    for (size_t i = 0U; i < 12U; ++i) {
        status = UmiAiTrainingStackedDecoderTrainSelected(
            model, inputs, targets, mask, 4U, 0.025F, &step);
        if (status != UMI_STATUS_OK) goto done;
    }
    status = UmiAiTrainingStackedDecoderEvaluateSelected(
        model, inputs, targets, mask, 4U, &after);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiAiTrainingStackedDecoderInspect(model, &info);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiAiTrainingStackedCheckpointSize(model, &required);
    if (status != UMI_STATUS_OK) goto done;
    snapshot = malloc(required);
    if (snapshot == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto done; }
    status = UmiAiTrainingStackedCheckpointExport(
        model, snapshot, required, &written);
    if (status != UMI_STATUS_OK) goto done;
    status = UmiAiTrainingStackedCheckpointDecode(
        snapshot, written, &restored);
    if (status != UMI_STATUS_OK) goto done;
    float restoredLoss = 0.0F;
    status = UmiAiTrainingStackedDecoderEvaluateSelected(
        restored, inputs, targets, mask, 4U, &restoredLoss);
    if (status != UMI_STATUS_OK) goto done;
    printf("layers=%u trainable=%zu steps=%llu\n",
        config.transformerLayers, info.trainableParameterElements,
        (unsigned long long)step.completedTrainingSteps);
    printf("held-in-memory teaching loss: %.6f -> %.6f; restored: %.6f\n",
        (double)before, (double)after, (double)restoredLoss);
    printf("portable checkpoint bytes=%zu\n", written);
done:
    if (status != UMI_STATUS_OK)
        fprintf(stderr, "stacked decoder demonstration failed: status=%d\n", (int)status);
    UmiAiTrainingStackedDecoderDestroy(restored);
    UmiAiTrainingStackedDecoderDestroy(model);
    free(snapshot);
    return status == UMI_STATUS_OK ? 0 : 1;
}
