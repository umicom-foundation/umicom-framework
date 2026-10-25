/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_decoder_selected.c
 * PURPOSE: Check that assistant-response loss masks train only the two adapter
 *          matrices while the canonical decoder checkpoint stays unchanged.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder_logit_adapter.h"
#include "umicom/ai/training/supervised_window.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "decoder selected:%d: %s\n", __LINE__, #x); \
    return 1; } } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig configuration = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(12345)
    };
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingDecoderLogitAdapter *adapter = NULL;
    UmiAiTrainingSupervisedWindowInfo window = {0};
    UmiAiTrainingDecoderLogitStep report = {0};
    UmiAiTrainingDecoderLogitInfo before = {0}, after = {0};
    size_t inputs[8] = {0}, targets[8] = {0};
    uint8_t mask[8] = {0}, all[8] = {1,1,1,1,1,1,1,1};
    size_t checkpointBytes = 0U, actual = 0U;
    uint8_t *saved = NULL, *current = NULL;
    float oldLoss = 0.0F, allLoss = 0.0F, maskedLoss = 0.0F;

    const uint8_t prompt[] = {'H','i'};
    const uint8_t completion[] = {' ','O','K'};
    CHECK(UmiAiTrainingSupervisedByteWindowBuild(prompt, 2U, completion, 3U,
        8U, inputs, 8U, targets, 8U, mask, 8U, &window) == UMI_STATUS_OK);
    CHECK(window.supervisedTargets == 4U);
    CHECK(UmiAiTrainingDecoderCreate(&configuration, &decoder) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogitAdapterCreate(decoder, 2U, 99U,
                                                  &adapter) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogitAdapterInspect(adapter, &before) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(decoder, &checkpointBytes) == UMI_STATUS_OK);
    saved = malloc(checkpointBytes); current = malloc(checkpointBytes);
    CHECK(saved != NULL && current != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(decoder, saved,
                checkpointBytes, &actual) == UMI_STATUS_OK);
    CHECK(actual == checkpointBytes);

    CHECK(UmiAiTrainingDecoderLogitAdapterEvaluateSelected(adapter, inputs,
        targets, 8U, mask, 8U, &oldLoss) == UMI_STATUS_OK);
    CHECK(isfinite(oldLoss) && oldLoss >= 0.0F);
    CHECK(UmiAiTrainingDecoderLogitAdapterEvaluate(adapter, inputs,
        targets, 8U, &allLoss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogitAdapterEvaluateSelected(adapter, inputs,
        targets, 8U, all, 8U, &maskedLoss) == UMI_STATUS_OK);
    CHECK(fabsf(allLoss - maskedLoss) < 0.00001F);

    const uint8_t badMask[8] = {0,0,0,0,0,0,0,0};
    report.crossEntropyBeforeUpdate = 999.0F;
    CHECK(UmiAiTrainingDecoderLogitAdapterTrainSelected(adapter, inputs, targets,
        8U, badMask, 8U, 0.025F, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(report.crossEntropyBeforeUpdate == 999.0F);
    CHECK(UmiAiTrainingDecoderLogitAdapterTrainSelected(adapter, inputs, targets,
        8U, mask, 7U, 0.025F, &report) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(report.crossEntropyBeforeUpdate == 999.0F);

    CHECK(UmiAiTrainingDecoderLogitAdapterTrainSelected(adapter, inputs, targets,
        8U, mask, 8U, 0.025F, &report) == UMI_STATUS_OK);
    CHECK(report.completedAdapterSteps == 1U);
    CHECK(report.tokenCount == 8U && isfinite(report.crossEntropyBeforeUpdate));
    CHECK(UmiAiTrainingDecoderLogitAdapterInspect(adapter, &after) == UMI_STATUS_OK);
    CHECK(after.completedAdapterSteps == before.completedAdapterSteps + 1U);
    CHECK(after.frozenDecoderSteps == before.frozenDecoderSteps);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(decoder, current,
                checkpointBytes, &actual) == UMI_STATUS_OK);
    CHECK(actual == checkpointBytes &&
          memcmp(saved, current, checkpointBytes) == 0);

    free(current);
    free(saved);
    UmiAiTrainingDecoderLogitAdapterDestroy(adapter);
    UmiAiTrainingDecoderDestroy(decoder);
    puts("Response-only decoder logit adaptation preserves frozen checkpoint");
    return 0;
}
