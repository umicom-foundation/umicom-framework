/*-----------------------------------------------------------------------------
 * Umicom Framework | Native LLM learning laboratory
 * PURPOSE: Train on an approved source manifest using one AdamW optimiser
 * update for several bounded microbatch graphs. A thin C23 consumer of the
 * canonical Framework dataset, experiment, decoder and optimiser services.
 * Never executes source files or changes checkpoint files.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/gradient_accumulation.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingAdamW *optimizer = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingAdamWConfig options;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingExperimentResult result;
    uint64_t steps = 0U;
    size_t stateBytes = 0U;
    UmiStatus status;
    if (argc != 2) {
        fprintf(stderr, "usage: umicom-native-llm-adamw-microbatch <reviewed-source-manifest>\n");
        return 2;
    }
    status = UmiAiTrainingExperimentDefaults(&config);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingAdamWDefaults(&options);
    if (status != UMI_STATUS_OK) goto failure;
    config.steps = 3U;
    config.evalInterval = 1U;
    config.evalBatches = 1U;
    config.batchSequences = 4U;
    config.learningRate = 0.014F;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_CONSTANT, 0.014F, 0.014F, 0U, 3U
    };
    status = UmiAiTrainingDatasetLoadManifest(argv[1], &dataset);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingExperimentRunAdamWAccumulated(
        dataset, &config, &options, &schedule, 2U,
        NULL, NULL, &model, &optimizer, &result);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingAdamWInspect(optimizer, &steps, &stateBytes);
    if (status != UMI_STATUS_OK) goto failure;
    if (steps != (uint64_t)config.steps ||
        !isfinite(result.finalHeldOutLoss)) {
        status = UMI_STATUS_INVALID_STATE;
        goto failure;
    }
    printf("Umicom native AdamW effective-batch training\n");
    printf("Effective sequences per step: %u; microbatch sequences: 2\n",
           config.batchSequences);
    printf("Completed optimiser steps: %" PRIu64 "\n", steps);
    printf("Final held-out cross-entropy: %.6f\n", result.finalHeldOutLoss);
    printf("AdamW state payload: %zu bytes\n", stateBytes);
    printf("Source data is reviewed; no source file is executed.\n");
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;
failure:
    fprintf(stderr, "AdamW microbatch experiment failed (status=%d)\n", (int)status);
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
