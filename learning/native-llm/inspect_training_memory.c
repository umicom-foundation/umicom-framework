/*-----------------------------------------------------------------------------
 * Umicom Framework — Native language-model learning laboratory
 * File: learning/native-llm/inspect_training_memory.c
 * PURPOSE:
 *   Show capacity planning, committed training graph allocations, and the
 *   difference between persistent model weights and a temporary graph peak.
 *   All neural mathematics and memory limits remain Framework-owned.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/capacity_plan.h"
#include "umicom/ai/training/decoder.h"

#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    UmiAiTrainingDecoderConfig config = {
        260U, 8U, 8U, 16U, 2U, UINT64_C(20261021)
    };
    UmiAiTrainingCapacityPlan plan;
    UmiAiTrainingTapeMemory initial, trained;
    UmiAiTrainingDecoder *model = NULL;
    size_t inputs[]  = {105U, 114U, 120U, 36U, 101U, 103U, 103U, 115U};
    size_t targets[] = {114U, 120U, 36U, 101U, 103U, 103U, 115U, 109U};
    float loss = 0.0F;
    UmiStatus status;

    status = UmiAiTrainingCapacityPlanCompute(&config, 1U, &plan);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "capacity plan rejected (%d)\n", (int)status);
        return 1;
    }
    printf("Umicom native decoder capacity and training memory\n");
    printf("Parameter tensors: %zu; scalar parameters: %zu\n",
           plan.parameterTensors, plan.parameterElements);
    printf("Parameter values: %zu bytes; parameter gradients: %zu bytes\n",
           plan.parameterValueBytes, plan.parameterGradientBytes);
    printf("Batched logits: %zu bytes; attention probabilities: %zu bytes\n",
           plan.logitsBytes, plan.attentionProbabilityBytes);
    printf("Maximum sequences for this configuration: %zu\n",
           plan.maximumBatchSequences);
    puts("A capacity plan excludes intermediate graph nodes and temporary scratch.");

    status = UmiAiTrainingDecoderCreate(&config, &model);
    if (status != UMI_STATUS_OK) return 1;
    status = UmiAiTrainingDecoderMemoryInspect(model, &initial);
    if (status != UMI_STATUS_OK) goto failed;
    printf("Committed training graph before step: %zu bytes\n",
           initial.currentBytes);
    status = UmiAiTrainingDecoderTrainStep(model, inputs, targets, 8U,
                                           0.01F, &loss);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderMemoryInspect(model, &trained);
    if (status != UMI_STATUS_OK) goto failed;
    printf("One optimisation step loss: %.6f\n", (double)loss);
    printf("Committed graph after rewind: %zu bytes\n", trained.currentBytes);
    printf("Highest committed graph allocation: %zu bytes\n", trained.peakBytes);
    if (trained.currentBytes != initial.currentBytes ||
        trained.peakBytes <= initial.peakBytes) goto failed;
    UmiAiTrainingDecoderDestroy(model);
    return 0;
failed:
    fprintf(stderr, "memory inspection failed (%d)\n", (int)status);
    UmiAiTrainingDecoderDestroy(model);
    return 1;
}
