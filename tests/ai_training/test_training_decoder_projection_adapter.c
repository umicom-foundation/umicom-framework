/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_decoder_projection_adapter.c
 * PURPOSE:
 *   Qualify actual native decoder projection-level adaptation, response-only
 *   cross-entropy, frozen checkpoint identity and failure-atomic output.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder_projection_adapter.h"
#include "umicom/ai/training/supervised_window.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "decoder projection assertion line %d: %s\n", \
                __LINE__, #condition); \
        return 1; \
    } \
} while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        260U, 4U, 8U, 16U, 2U, UINT64_C(20261025)
    };
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingDecoderProjectionAdapter *adapter = NULL;
    UmiAiTrainingDecoderProjectionAdapter *rejected = NULL;
    UmiAiTrainingDecoderProjectionAdapterInfo info = {0};
    UmiAiTrainingDecoderProjectionStep report = {0};
    UmiAiTrainingSupervisedWindowInfo windowInfo = {0};
    size_t inputs[4] = {0}, targets[4] = {0};
    uint8_t mask[4] = {0};
    float originalLogits[4U * 260U] = {0};
    float adaptedLogits[4U * 260U] = {0};
    float initialLoss = 0.0F, finalLoss = 0.0F;
    float sentinel = 12345.0F;
    uint8_t *checkpointBefore = NULL, *checkpointAfter = NULL;
    size_t bytes = 0U, usedBefore = 0U, usedAfter = 0U;

    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &bytes) == UMI_STATUS_OK);
    CHECK(bytes > 0U);
    checkpointBefore = malloc(bytes);
    checkpointAfter = malloc(bytes);
    CHECK(checkpointBefore != NULL && checkpointAfter != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, checkpointBefore,
          bytes, &usedBefore) == UMI_STATUS_OK);
    CHECK(usedBefore == bytes);

    const uint8_t prompt[] = {'Q'};
    const uint8_t completion[] = {'A'};
    CHECK(UmiAiTrainingSupervisedByteWindowBuild(
          prompt, sizeof(prompt), completion, sizeof(completion), 4U,
          inputs, 4U, targets, 4U, mask, 4U, &windowInfo) == UMI_STATUS_OK);
    CHECK(mask[0] == 0U && mask[1] == 1U && mask[2] == 1U && mask[3] == 0U);
    CHECK(windowInfo.supervisedTargets == 2U);

    CHECK(UmiAiTrainingDecoderProjectionAdapterCreate(model,
          UINT32_C(0x80), 2U, UINT64_C(41), &rejected) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(rejected == NULL);

    /* Include attention and feed-forward projection roles; two trainable
     * matrices per selected role and no original decoder parameter updates. */
    const uint32_t choices = UMI_AI_DECODER_ADAPT_QUERY |
        UMI_AI_DECODER_ADAPT_KEY |
        UMI_AI_DECODER_ADAPT_ATTENTION_OUTPUT |
        UMI_AI_DECODER_ADAPT_FEED_FORWARD_DOWN;
    CHECK(UmiAiTrainingDecoderProjectionAdapterCreate(
          model, choices, 2U, UINT64_C(41), &adapter) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionAdapterInspect(adapter, &info) ==
          UMI_STATUS_OK);
    CHECK(info.selectedProjections == choices);
    CHECK(info.trainableTensors == 8U && info.trainableElements > 0U);
    CHECK(strlen(info.frozenCheckpointSha256) == 64U);
    CHECK(info.completedAdapterSteps == 0U);

    CHECK(UmiAiTrainingDecoderLogits(model, inputs, 4U,
          originalLogits, 4U * 260U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderProjectionAdapterLogits(
          adapter, inputs, 4U, adaptedLogits, 4U * 260U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 4U * 260U; ++i)
        CHECK(fabsf(originalLogits[i] - adaptedLogits[i]) <= 0.00010F);

    CHECK(UmiAiTrainingDecoderProjectionAdapterLogits(
          adapter, inputs, 4U, &sentinel, 0U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(sentinel == 12345.0F);

    CHECK(UmiAiTrainingDecoderProjectionAdapterEvaluate(
          adapter, inputs, targets, mask, 4U, &initialLoss) == UMI_STATUS_OK);
    CHECK(isfinite(initialLoss) && initialLoss >= 0.0F);
    uint8_t emptyMask[4] = {0};
    CHECK(UmiAiTrainingDecoderProjectionAdapterEvaluate(
          adapter, inputs, targets, emptyMask, 4U, &sentinel) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(sentinel == 12345.0F);

    report.selectedCrossEntropyBeforeUpdate = 12345.0F;
    CHECK(UmiAiTrainingDecoderProjectionAdapterTrainStep(
          adapter, inputs, targets, mask, 4U, NAN, &report) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(report.selectedCrossEntropyBeforeUpdate == 12345.0F);

    for (size_t iteration = 0U; iteration < 32U; ++iteration) {
        CHECK(UmiAiTrainingDecoderProjectionAdapterTrainStep(
              adapter, inputs, targets, mask, 4U, 0.08F, &report) ==
              UMI_STATUS_OK);
        CHECK(report.supervisedTokenCount == 2U);
        CHECK(isfinite(report.selectedCrossEntropyBeforeUpdate));
    }
    CHECK(report.completedAdapterSteps == 32U);
    CHECK(UmiAiTrainingDecoderProjectionAdapterEvaluate(
          adapter, inputs, targets, mask, 4U, &finalLoss) == UMI_STATUS_OK);
    CHECK(isfinite(finalLoss) && finalLoss >= 0.0F);
    CHECK(finalLoss <= initialLoss + 0.01F);

    CHECK(UmiAiTrainingDecoderProjectionAdapterInspect(adapter, &info) ==
          UMI_STATUS_OK);
    CHECK(info.completedAdapterSteps == 32U);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, checkpointAfter,
          bytes, &usedAfter) == UMI_STATUS_OK);
    CHECK(usedAfter == usedBefore &&
          memcmp(checkpointBefore, checkpointAfter, bytes) == 0);

    /* The original decoder is borrowed. An independent decoder update makes
     * its snapshot stale; the adapter refuses further use without changing
     * caller output or claiming that its frozen base is still current. */
    float updatedLoss = 0.0F;
    CHECK(UmiAiTrainingDecoderTrainStep(model, inputs, targets, 4U,
                                       0.001F, &updatedLoss) == UMI_STATUS_OK);
    sentinel = 12345.0F;
    CHECK(UmiAiTrainingDecoderProjectionAdapterEvaluate(
          adapter, inputs, targets, mask, 4U, &sentinel) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(sentinel == 12345.0F);

    UmiAiTrainingDecoderProjectionAdapterDestroy(adapter);
    UmiAiTrainingDecoderDestroy(model);
    free(checkpointAfter);
    free(checkpointBefore);
    printf("Native projection adaptation selected loss %.6f -> %.6f; "
           "frozen checkpoint preserved\n",
           (double)initialLoss, (double)finalLoss);
    return 0;
}
