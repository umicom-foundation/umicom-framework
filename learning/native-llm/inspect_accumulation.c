/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/inspect_accumulation.c
 *
 * PURPOSE:
 *   Compare one effective training batch with bounded microbatches through the canonical decoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/capacity_plan.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int usage(const char *program, int result)
{
    FILE *stream = result == 0 ? stdout : stderr;
    fprintf(stream, "Usage: %s [1|2|5]\nCompare a 40-token training step with 1, 2 or 5 sequences per microbatch.\n", program);
    return result;
}

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig config = {260U, 8U, 8U, 16U, 2U, UINT64_C(4477)};
    UmiAiTrainingDecoder *reference = NULL;
    UmiAiTrainingDecoder *micro = NULL;
    UmiAiTrainingDecoderConfig inspected;
    UmiAiTrainingCapacityPlan plan;
    UmiAiTrainingTapeMemory referenceMemory, microMemory;
    size_t input[40], target[40], sequences = 2U;
    float referenceLogits[2080], microLogits[2080];
    float referenceLoss = 0.0F, microLoss = 0.0F, largestDifference = 0.0F;
    uint64_t referenceSteps = 0U, microSteps = 0U;
    UmiStatus status = UMI_STATUS_OK;
    int result = 1;
    if (argc == 2 && strcmp(argv[1], "--help") == 0) return usage(argv[0], 0);
    if (argc > 2) return usage(argv[0], 2);
    if (argc == 2) {
        if (strcmp(argv[1], "1") == 0) sequences = 1U;
        else if (strcmp(argv[1], "2") == 0) sequences = 2U;
        else if (strcmp(argv[1], "5") == 0) sequences = 5U;
        else return usage(argv[0], 2);
    }
    for (size_t i = 0U; i < 40U; ++i) {
        input[i] = 4U + (i * 7U) % 40U;
        target[i] = 4U + ((i + 1U) * 7U) % 40U;
    }
    /* Both models start from the same seed. Training mathematics and graph
     * ownership remain in Framework; this client only compares observations. */
#define STEP(expression) do { status = (expression); if (status != UMI_STATUS_OK) goto finish; } while (0)
    STEP(UmiAiTrainingCapacityPlanCompute(&config, sequences, &plan));
    STEP(UmiAiTrainingDecoderCreate(&config, &reference));
    STEP(UmiAiTrainingDecoderCreate(&config, &micro));
    STEP(UmiAiTrainingDecoderTrainStep(reference, input, target, 40U, 0.025F, &referenceLoss));
    STEP(UmiAiTrainingDecoderTrainAccumulated(micro, input, target, 40U, sequences, 0.025F, &microLoss));
    STEP(UmiAiTrainingDecoderMemoryInspect(reference, &referenceMemory));
    STEP(UmiAiTrainingDecoderMemoryInspect(micro, &microMemory));
    STEP(UmiAiTrainingDecoderInspect(reference, &inspected, &referenceSteps));
    STEP(UmiAiTrainingDecoderInspect(micro, &inspected, &microSteps));
    STEP(UmiAiTrainingDecoderLogits(reference, input, 8U, referenceLogits, 2080U));
    STEP(UmiAiTrainingDecoderLogits(micro, input, 8U, microLogits, 2080U));
    for (size_t i = 0U; i < 2080U; ++i) {
        float difference = fabsf(referenceLogits[i] - microLogits[i]);
        if (!isfinite(difference)) goto finish;
        if (difference > largestDifference) largestDifference = difference;
    }
    if (!isfinite(referenceLoss) || !isfinite(microLoss) ||
        fabsf(referenceLoss - microLoss) >= 1e-5F || largestDifference >= 3e-5F ||
        referenceSteps != 1U || microSteps != 1U) goto finish;
    printf("Effective tokens: 40\nSequences per microbatch: %zu\n", sequences);
    printf("Microbatch token capacity: %zu\n", plan.tokensPerBatch);
    printf("Reference loss: %.7f\nAccumulated loss: %.7f\n", (double)referenceLoss, (double)microLoss);
    printf("Largest logit difference: %.9f\n", (double)largestDifference);
    printf("Tape peak bytes: reference=%zu accumulated=%zu\n", referenceMemory.peakBytes, microMemory.peakBytes);
    puts("Optimiser steps: 1 each\nComparison: within tolerance\nScope: local microbatches; no worker network or model-quality claim.");
    result = 0;
finish:
    if (result != 0) fprintf(stderr, "Accumulation comparison failed (status %d).\n", (int)status);
    UmiAiTrainingDecoderDestroy(micro);
    UmiAiTrainingDecoderDestroy(reference);
    return result;
#undef STEP
}
