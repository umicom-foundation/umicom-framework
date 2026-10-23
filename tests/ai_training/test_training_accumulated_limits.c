/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_accumulated_limits.c
 * PURPOSE:
 *   Exercise effective batches larger than one tensor and guarantee that
 *   rejected inputs, memory budgets and step lifetimes preserve a model.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/checkpoint.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    const UmiAiTrainingDecoderConfig config = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(20261021)
    };
    UmiAiTrainingDecoder *model = NULL;
    size_t inputs[80], targets[80];
    uint8_t before[UMI_AI_DECODER_MAX_CHECKPOINT_BYTES];
    uint8_t after[UMI_AI_DECODER_MAX_CHECKPOINT_BYTES];
    size_t nBefore = 0U, nAfter = 0U;
    UmiAiTrainingTapeMemory memory;
    UmiAiTrainingDecoderConfig examined;
    uint64_t steps = 0U;
    float loss = -99.0F;

    for (size_t i = 0U; i < 80U; ++i) {
        inputs[i] = 4U + (i * 13U) % 100U;
        targets[i] = 4U + ((i + 1U) * 13U) % 100U;
    }
    CHECK(UmiAiTrainingDecoderCreate(&config, &model) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, before, sizeof(before),
                                                &nBefore) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(model, inputs, targets, 80U, 0.025F,
                                       &loss) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss == -99.0F);
    CHECK(UmiAiTrainingDecoderTrainAccumulated(model, inputs, targets, 80U,
                                               0U, 0.025F, &loss)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAccumulated(model, inputs, targets, 79U,
                                               2U, 0.025F, &loss)
          == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDecoderTrainAccumulated(model, inputs, targets, 80U,
                                               2U, 0.0F, &loss)
          == UMI_STATUS_INVALID_ARGUMENT);
    targets[79] = 260U;
    CHECK(UmiAiTrainingDecoderTrainAccumulated(model, inputs, targets, 80U,
                                               2U, 0.025F, &loss)
          == UMI_STATUS_INVALID_ARGUMENT);
    targets[79] = 4U + (80U * 13U) % 100U;
    CHECK(loss == -99.0F);

    CHECK(UmiAiTrainingDecoderMemoryInspect(model, &memory) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderSetMemoryBudget(model, memory.currentBytes + 1U)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAccumulated(model, inputs, targets, 80U,
                                               2U, 0.025F, &loss)
          == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(loss == -99.0F);
    CHECK(UmiAiTrainingDecoderSetMemoryBudget(model, 0U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(model, after, sizeof(after),
                                                &nAfter) == UMI_STATUS_OK);
    CHECK(nBefore == nAfter && memcmp(before, after, nBefore) == 0);
    CHECK(UmiAiTrainingDecoderInspect(model, &examined, &steps) == UMI_STATUS_OK);
    CHECK(steps == 0U);
    /* 80 positions cannot fit one logits tensor (80 * 260 > 16384), but
     * 10 independent sequences can be reduced from 2-sequence microbatches. */
    CHECK(UmiAiTrainingDecoderTrainAccumulated(model, inputs, targets, 80U,
                                               2U, 0.025F, &loss)
          == UMI_STATUS_OK);
    CHECK(loss > 0.0F);
    CHECK(UmiAiTrainingDecoderInspect(model, &examined, &steps) == UMI_STATUS_OK);
    CHECK(steps == 1U);
    UmiAiTrainingDecoderDestroy(model);
    puts("effective batch beyond tensor limit and failure atomicity: PASS");
    return 0;
}
