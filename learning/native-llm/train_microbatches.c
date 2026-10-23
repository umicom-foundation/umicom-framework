/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM Learning Laboratory
 * File: learning/native-llm/train_microbatches.c
 *
 * PURPOSE:
 *   Demonstrate one effective optimiser update on reviewed C23 source data
 *   using multiple bounded decoder graphs. The authoritative tokenizer,
 *   source sampler, backpropagation and optimiser live in Umicom Framework.
 *
 * SAFETY:
 *   This laboratory only reads the approved manifest and its source files.
 *   It does not write checkpoints, execute source code or publish any model.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/gradient_accumulation.h"

#include <inttypes.h>
#include <stdio.h>

#define SOURCE_CONTEXT 8U
#define SOURCE_SAMPLING_SEQUENCES 4U
#define SOURCE_EFFECTIVE_SEQUENCES 8U
#define SOURCE_MICROBATCH_SEQUENCES 2U

int main(int argc, char **argv)
{
    const UmiAiTrainingDecoderConfig configuration = {
        260U, SOURCE_CONTEXT, 8U, 16U, 2U, UINT64_C(20261021)
    };
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingTapeMemory before, after;
    UmiAiTrainingDecoderConfig verified;
    uint64_t completedSteps = 0U;
    size_t input[SOURCE_EFFECTIVE_SEQUENCES * SOURCE_CONTEXT];
    size_t target[SOURCE_EFFECTIVE_SEQUENCES * SOURCE_CONTEXT];
    float trainingLoss = 0.0F;
    UmiStatus status;

    if (argc != 2) {
        fprintf(stderr, "usage: umicom-native-llm-microbatch <reviewed-source-manifest>\n");
        return 2;
    }
    status = UmiAiTrainingDatasetLoadManifest(argv[1], &dataset);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderCreate(&configuration, &model);
    if (status != UMI_STATUS_OK) goto failed;

    /* The source sampler's bounded API currently accepts four sequences at a
     * time. Join two independently seeded draws into ONE effective batch. */
    for (size_t sample = 0U; sample < SOURCE_EFFECTIVE_SEQUENCES /
                                      SOURCE_SAMPLING_SEQUENCES; ++sample) {
        size_t offset = sample * SOURCE_SAMPLING_SEQUENCES * SOURCE_CONTEXT;
        status = UmiAiTrainingDatasetBatch(dataset,
            UMI_AI_TRAINING_PARTITION_TRAIN, SOURCE_CONTEXT,
            SOURCE_SAMPLING_SEQUENCES, UINT64_C(4477), (uint64_t)sample,
            &input[offset], &target[offset],
            SOURCE_SAMPLING_SEQUENCES * SOURCE_CONTEXT);
        if (status != UMI_STATUS_OK) goto failed;
    }
    status = UmiAiTrainingDecoderMemoryInspect(model, &before);
    if (status != UMI_STATUS_OK) goto failed;
    /* Eight contexts would require 64*260 float logits, exceeding the
     * established per-tensor capacity. The new path rewinds every graph. */
    status = UmiAiTrainingDecoderTrainAccumulated(model,
          input, target, SOURCE_EFFECTIVE_SEQUENCES * SOURCE_CONTEXT,
          SOURCE_MICROBATCH_SEQUENCES, 0.025F, &trainingLoss);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderMemoryInspect(model, &after);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDecoderInspect(model, &verified, &completedSteps);
    if (status != UMI_STATUS_OK) goto failed;

    printf("Umicom native source-code gradient accumulation\n");
    printf("Effective sequences: %u, tokens: %u; microbatch sequences: %u\n",
           SOURCE_EFFECTIVE_SEQUENCES,
           SOURCE_EFFECTIVE_SEQUENCES * SOURCE_CONTEXT,
           SOURCE_MICROBATCH_SEQUENCES);
    printf("Mean training cross-entropy: %.6f\n", (double)trainingLoss);
    printf("Optimiser steps: %" PRIu64 "\n", completedSteps);
    printf("Retained graph bytes: %zu -> %zu; peak: %zu\n",
           before.currentBytes, after.currentBytes, after.peakBytes);
    puts("This is a teaching experiment, not evidence of coding expertise.");
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;
failed:
    fprintf(stderr, "source-code microbatch training rejected (status=%d)\n",
            (int)status);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
