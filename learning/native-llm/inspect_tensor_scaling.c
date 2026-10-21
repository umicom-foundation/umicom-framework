/*-----------------------------------------------------------------------------
 * Umicom Framework / Native LLM learning laboratory
 * File: learning/native-llm/inspect_tensor_scaling.c
 *
 * PURPOSE:
 *   Demonstrate a larger multi-sequence training tensor and inspect the
 *   existing Framework capacity and persistent tape allocation contracts.
 *   This executable is a thin consumer, not a second training implementation.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/capacity_plan.h"
#include "umicom/ai/training/decoder.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    UmiAiTrainingDecoderConfig config = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(20261021)
    };
    UmiAiTrainingCapacityPlan plan;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingTapeMemory original, trained;
    size_t input[32], target[32];
    float loss = -1.0F;
    UmiStatus status;

    status = UmiAiTrainingCapacityPlanCompute(&config, 4U, &plan);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "requested training shape rejected: %d\n", (int)status);
        return 1;
    }
    printf("Umicom native multi-sequence training workspace\n");
    printf("Tensor ceiling: %zu scalar FP32 elements\n",
           plan.maximumTensorElements);
    printf("Sequences: 4, tokens: %zu, logits: %zu elements (%zu bytes)\n",
           plan.tokensPerBatch, plan.logitsElements, plan.logitsBytes);
    printf("Theoretical maximum for this tensor shape: %zu sequences\n",
           plan.maximumBatchSequences);
    puts("The dataset API separately enforces its own maximum sequence count.");

    status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "decoder create failed: %d\n", (int)status);
        return 1;
    }
    status = UmiAiTrainingDecoderMemoryInspect(model, &original);
    if (status != UMI_STATUS_OK) goto failed;
    for (size_t i = 0U; i < 32U; ++i) {
        input[i] = 4U + (i % 26U);
        target[i] = 4U + ((i + 1U) % 26U);
    }
    status = UmiAiTrainingDecoderTrainStep(model, input, target,
                                           32U, 0.025F, &loss);
    if (status != UMI_STATUS_OK || !isfinite(loss)) goto failed;
    status = UmiAiTrainingDecoderMemoryInspect(model, &trained);
    if (status != UMI_STATUS_OK) goto failed;
    printf("Training cross-entropy: %.6f\n", (double)loss);
    printf("Persistent bytes: %zu before, %zu after rewind\n",
           original.currentBytes, trained.currentBytes);
    printf("Committed graph high-water mark: %zu bytes\n",
           trained.peakBytes);
    puts("This is a small educational decoder, not an expert coding model.");
    UmiAiTrainingDecoderDestroy(model);
    return 0;
failed:
    fprintf(stderr, "multi-sequence training demonstration failed: %d\n",
            (int)status);
    UmiAiTrainingDecoderDestroy(model);
    return 1;
}
