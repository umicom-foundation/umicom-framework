/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_wide_decoder.c
 * PURPOSE:
 *   Verify a larger parameter matrix can train, encode a compatible decoder
 *   checkpoint and reload without altering numerical inference results.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/capacity_plan.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/decoder.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "wide decoder %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig config = {260U, 8U, 32U, 64U, 4U, 20261021U};
    UmiAiTrainingCapacityPlan plan;
    UmiAiTrainingDecoder *model = NULL, *restored = NULL;
    UmiAiTrainingTapeMemory allocation;
    uint64_t completedSteps = 0U;
    size_t beforeLength = 0U, afterLength = 0U, required = 0U;
    size_t tokens[16], targets[16];
    float loss = -1.0F;
    float *beforeLogits = calloc(16U * 260U, sizeof(float));
    float *afterLogits = calloc(16U * 260U, sizeof(float));
    uint8_t *checkpoint = NULL, *reencoded = NULL;
    CHECK(beforeLogits != NULL && afterLogits != NULL);
    CHECK(UmiAiTrainingCapacityPlanCompute(&config, 2U, &plan)
          == UMI_STATUS_OK);
    CHECK(plan.parameterElements > 16384U);
    CHECK(plan.parameterTensors == 15U);
    CHECK(plan.maximumBatchSequences >= 2U);
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderMemoryInspect(model, &allocation)
          == UMI_STATUS_OK);
    CHECK(allocation.tensorCount == 15U);
    for (size_t i = 0U; i < 16U; ++i) {
        tokens[i] = 4U + (i * 7U) % 200U;
        targets[i] = 4U + (i * 7U + 1U) % 200U;
    }
    CHECK(UmiAiTrainingDecoderTrainStep(model, tokens, targets, 16U,
          0.025F, &loss) == UMI_STATUS_OK);
    CHECK(isfinite(loss) && loss > 0.0F);
    CHECK(UmiAiTrainingDecoderInspect(model, &config, &completedSteps)
          == UMI_STATUS_OK);
    CHECK(completedSteps == 1U);
    CHECK(UmiAiTrainingDecoderLogits(model, tokens, 16U,
          beforeLogits, 16U * 260U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(model, &required)
          == UMI_STATUS_OK);
    CHECK(required <= UMI_AI_DECODER_MAX_CHECKPOINT_BYTES);
    checkpoint = malloc(required);
    reencoded = malloc(required);
    CHECK(checkpoint != NULL && reencoded != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, checkpoint,
          required, &beforeLength) == UMI_STATUS_OK);
    CHECK(beforeLength == required);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(checkpoint, beforeLength,
          &restored) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(restored, tokens, 16U,
          afterLogits, 16U * 260U) == UMI_STATUS_OK);
    CHECK(memcmp(beforeLogits, afterLogits, 16U * 260U * sizeof(float)) == 0);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(restored, reencoded,
          required, &afterLength) == UMI_STATUS_OK);
    CHECK(afterLength == beforeLength);
    CHECK(memcmp(checkpoint, reencoded, required) == 0);
    CHECK(UmiAiTrainingDecoderInspect(restored, &config, &completedSteps)
          == UMI_STATUS_OK);
    CHECK(completedSteps == 1U);

    free(reencoded);
    free(checkpoint);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(model);
    free(afterLogits);
    free(beforeLogits);
    puts("wider trained decoder and unchanged checkpoint format: PASS");
    return 0;
}
