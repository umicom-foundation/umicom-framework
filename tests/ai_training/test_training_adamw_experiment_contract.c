/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_adamw_experiment_contract.c
 * PURPOSE: Reject incompatible schedules, optimiser options and resume state.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_adamw_experiment_contract.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <stdio.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be specified by CMake
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingAdamWConfig options;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingDecoder *starting = NULL, *rejected = NULL;
    UmiAiTrainingAdamW *startingOptimizer = NULL, *rejectedOptimizer = NULL;
    UmiAiTrainingExperimentResult before, result;
    size_t checkpointBytes = 0U;
    uint64_t optimiserSteps = UINT64_MAX;
    size_t stateBytes = 0U;

    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWDefaults(&options) == UMI_STATUS_OK);
    config.steps = 2U;
    config.evalInterval = 1U;
    config.evalBatches = 1U;
    config.learningRate = 0.02F;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_LINEAR, 0.02F, 0.001F, 0U, 4U};
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                           &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentAdamWValidate(&config, &options, &schedule) ==
          UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentRunAdamW(dataset, &config, &options,
          &schedule, NULL, NULL, &starting, &startingOptimizer, &result) ==
          UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSize(starting, startingOptimizer,
                                            &checkpointBytes) == UMI_STATUS_OK);
    CHECK(checkpointBytes > 0U);
    config.steps = 4U;
    memset(&result, 0x5a, sizeof(result));
    before = result;
    options.beta1 = 0.8F;
    CHECK(UmiAiTrainingExperimentResumeAdamW(dataset, &config, &options,
          &schedule, starting, startingOptimizer, NULL, NULL,
          &rejected, &rejectedOptimizer, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(rejected == NULL && rejectedOptimizer == NULL);
    CHECK(memcmp(&before, &result, sizeof(result)) == 0);
    options.beta1 = 0.9F;
    CHECK(UmiAiTrainingExperimentResumeAdamW(dataset, &config, &options,
          &schedule, starting, startingOptimizer, NULL, NULL,
          &rejected, &rejectedOptimizer, &result) == UMI_STATUS_OK);
    UmiAiTrainingAdamWDestroy(rejectedOptimizer);
    UmiAiTrainingDecoderDestroy(rejected);
    rejected = NULL;
    rejectedOptimizer = NULL;

    CHECK(UmiAiTrainingAdamWInspect(startingOptimizer, &optimiserSteps,
                                     &stateBytes) == UMI_STATUS_OK);
    CHECK(optimiserSteps == 2U && stateBytes > 0U);
    CHECK(UmiAiTrainingExperimentResumeAdamW(dataset, &config, &options,
          &schedule, starting, NULL, NULL, NULL,
          &rejected, &rejectedOptimizer, &result) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(rejected == NULL && rejectedOptimizer == NULL);
    schedule.totalSteps = 3U;
    CHECK(UmiAiTrainingExperimentAdamWValidate(&config, &options, &schedule) ==
          UMI_STATUS_INVALID_ARGUMENT);
    schedule.totalSteps = 4U;
    schedule.minimumLearningRate = 0.03F;
    CHECK(UmiAiTrainingExperimentAdamWValidate(&config, &options, &schedule) ==
          UMI_STATUS_INVALID_ARGUMENT);
    schedule.minimumLearningRate = 0.001F;
    schedule.peakLearningRate = 0.025F;
    CHECK(UmiAiTrainingExperimentAdamWValidate(&config, &options, &schedule) ==
          UMI_STATUS_INVALID_ARGUMENT);
    schedule.peakLearningRate = 0.02F;
    options.epsilon = 0.0F;
    CHECK(UmiAiTrainingExperimentAdamWValidate(&config, &options, &schedule) ==
          UMI_STATUS_INVALID_ARGUMENT);

    UmiAiTrainingAdamWDestroy(startingOptimizer);
    UmiAiTrainingDecoderDestroy(starting);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("AdamW experiment schedule, identity and atomic rejection: PASS");
    return 0;
}
