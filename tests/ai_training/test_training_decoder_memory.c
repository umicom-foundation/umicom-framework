/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_decoder_memory.c
 * PURPOSE: Verify model budget refusal does not mutate weights, steps or
 *          caller outputs; successful training releases temporary tape nodes.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingDecoderConfig cfg = {12U, 3U, 4U, 8U, 2U, 42U};
    UmiAiTrainingDecoder *decoder = NULL;
    UmiAiTrainingTapeMemory base = {0}, after = {0};
    size_t source[] = {1, 2, 3}, target[] = {2, 3, 4};
    float output[36], unchanged[36], loss = -99.0F;
    uint64_t steps = 100U;
    uint8_t checkpointBefore[131072], checkpointAfter[131072];
    size_t countBefore = 0U, countAfter = 0U;

    CHECK(UmiAiTrainingDecoderCreate(&cfg, &decoder) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderMemoryInspect(decoder, &base) == UMI_STATUS_OK);
    CHECK(base.tensorCount == 15U && base.currentBytes == base.peakBytes);
    CHECK(base.tensorValuesBytes > 0U &&
          base.tensorValuesBytes == base.tensorGradientBytes);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(decoder, checkpointBefore,
          sizeof(checkpointBefore), &countBefore) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderSetMemoryBudget(decoder, base.currentBytes - 1U)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiAiTrainingDecoderSetMemoryBudget(decoder, base.currentBytes + 1U)
          == UMI_STATUS_OK);
    for (size_t i = 0U; i < 36U; ++i) output[i] = 123.0F;
    memcpy(unchanged, output, sizeof(output));
    CHECK(UmiAiTrainingDecoderLogits(decoder, source, 3U, output, 36U)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(output, unchanged, sizeof(output)) == 0);
    CHECK(UmiAiTrainingDecoderTrainStep(decoder, source, target, 3U,
          0.01F, &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss == -99.0F);
    CHECK(UmiAiTrainingDecoderInspect(decoder, &cfg, &steps) == UMI_STATUS_OK);
    CHECK(steps == 0U);
    CHECK(UmiAiTrainingDecoderMemoryInspect(decoder, &after) == UMI_STATUS_OK);
    CHECK(after.tensorCount == base.tensorCount &&
          after.currentBytes == base.currentBytes &&
          after.peakBytes == base.peakBytes &&
          after.budgetBytes == base.currentBytes + 1U);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(decoder, checkpointAfter,
          sizeof(checkpointAfter), &countAfter) == UMI_STATUS_OK);
    CHECK(countAfter == countBefore &&
          memcmp(checkpointBefore, checkpointAfter, countBefore) == 0);

    CHECK(UmiAiTrainingDecoderSetMemoryBudget(decoder, 0U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(decoder, source, target, 3U,
          0.01F, &loss) == UMI_STATUS_OK);
    CHECK(loss > 0.0F);
    CHECK(UmiAiTrainingDecoderInspect(decoder, &cfg, &steps) == UMI_STATUS_OK);
    CHECK(steps == 1U);
    CHECK(UmiAiTrainingDecoderMemoryInspect(decoder, &after) == UMI_STATUS_OK);
    CHECK(after.tensorCount == base.tensorCount &&
          after.currentBytes == base.currentBytes &&
          after.peakBytes > base.peakBytes);
    UmiAiTrainingDecoderDestroy(decoder);
    puts("Decoder memory budget, weights and step rollback: PASS");
    return 0;
}
