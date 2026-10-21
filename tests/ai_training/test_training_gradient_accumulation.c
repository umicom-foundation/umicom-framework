/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_gradient_accumulation.c
 * PURPOSE:
 *   Confirm that several rewound microbatch graphs are equivalent to one
 *   larger mean-loss SGD step, including a shorter final microbatch.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
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
    UmiAiTrainingDecoder *reference = NULL;
    UmiAiTrainingDecoder *micro = NULL;
    UmiAiTrainingDecoder *single = NULL;
    UmiAiTrainingDecoder *restored = NULL;
    float logitsReference[2080], logitsMicro[2080], logitsSingle[2080];
    float referenceLoss = 0.0F, microLoss = 0.0F, singleLoss = 0.0F;
    size_t inputs[40], targets[40];
    uint8_t checkpoint[UMI_AI_DECODER_MAX_CHECKPOINT_BYTES];
    size_t written = 0U;
    uint64_t steps;
    UmiAiTrainingDecoderConfig examined;
    UmiAiTrainingTapeMemory directMemory, microMemory;

    for (size_t i = 0U; i < 40U; ++i) {
        inputs[i] = 4U + (i * 7U) % 40U;
        targets[i] = 4U + ((i + 1U) * 7U) % 40U;
    }
    CHECK(UmiAiTrainingDecoderCreate(&config, &reference) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config, &micro) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCreate(&config, &single) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainStep(reference, inputs, targets, 40U,
                                        0.025F, &referenceLoss) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAccumulated(micro, inputs, targets, 40U,
                                               2U, 0.025F, &microLoss)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderTrainAccumulated(single, inputs, targets, 40U,
                                               1U, 0.025F, &singleLoss)
          == UMI_STATUS_OK);
    CHECK(fabsf(referenceLoss - microLoss) < 1e-5F);
    CHECK(fabsf(referenceLoss - singleLoss) < 1e-5F);
    CHECK(UmiAiTrainingDecoderInspect(micro, &examined, &steps) == UMI_STATUS_OK);
    CHECK(steps == 1U);
    CHECK(UmiAiTrainingDecoderMemoryInspect(reference, &directMemory)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderMemoryInspect(micro, &microMemory)
          == UMI_STATUS_OK);
    CHECK(microMemory.peakBytes < directMemory.peakBytes);
    CHECK(microMemory.currentBytes == directMemory.currentBytes);
    CHECK(UmiAiTrainingDecoderLogits(reference, inputs, 8U, logitsReference,
                                     2080U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(micro, inputs, 8U, logitsMicro,
                                     2080U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderLogits(single, inputs, 8U, logitsSingle,
                                     2080U) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 2080U; ++i) {
        CHECK(fabsf(logitsReference[i] - logitsMicro[i]) < 3e-5F);
        CHECK(fabsf(logitsReference[i] - logitsSingle[i]) < 3e-5F);
    }
    CHECK(UmiAiTrainingDecoderCheckpointEncode(micro, checkpoint,
          sizeof(checkpoint), &written) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointDecode(checkpoint, written, &restored)
          == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderInspect(restored, &examined, &steps) == UMI_STATUS_OK);
    CHECK(steps == 1U && examined.vocabularySize == config.vocabularySize);
    UmiAiTrainingDecoderDestroy(restored);
    UmiAiTrainingDecoderDestroy(single);
    UmiAiTrainingDecoderDestroy(micro);
    UmiAiTrainingDecoderDestroy(reference);
    puts("gradient accumulation: numerical parity, one step and lower graph peak");
    return 0;
}
