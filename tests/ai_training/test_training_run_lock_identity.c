/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_run_lock_identity.c
 * PURPOSE: Prove exact policy/dataset/checkpoint identity, a permitted new
 *          stop step, strict record grammar and atomic failed outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * LICENCE: MIT
 */
#include "umicom/ai/training/training_run_lock.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be provided by CMake
#endif
#ifndef UMI_AI_TEST_REORDERED_MANIFEST_PATH
#error UMI_AI_TEST_REORDERED_MANIFEST_PATH must be provided by CMake
#endif

#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; \
} } while (0)

int main(void)
{
    const uint8_t checkpoint[] = {0x4DU, 0x13U, 0x91U, 0x00U, 0xFFU};
    uint8_t changedBytes[sizeof(checkpoint)];
    UmiAiTrainingDataset *dataset = NULL, *reordered = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingAdamWConfig adamw;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingGradientPolicy clipping = {1.0F, 2U, 1U};
    UmiAiTrainingRunLockInputs inputs;
    char record[UMI_AI_TRAINING_RUN_LOCK_RECORD_CAPACITY];
    char tiny[2] = {'X', '\0'};
    size_t count = 0U, sentinel = 9876U;

    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWDefaults(&adamw) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                           &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(
        UMI_AI_TEST_REORDERED_MANIFEST_PATH, &reordered) == UMI_STATUS_OK);
    config.steps = 4U;
    config.evalInterval = 2U;
    config.evalBatches = 1U;
    config.batchSequences = 2U;
    config.learningRate = 0.016F;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_COSINE, 0.016F, 0.003F, 2U, 8U};
    inputs = (UmiAiTrainingRunLockInputs){
        dataset, &config, &adamw, &schedule, &clipping};
    CHECK(UmiAiTrainingRunLockFormat(checkpoint, sizeof(checkpoint),
          &inputs, record, sizeof(record), &count) == UMI_STATUS_OK);
    CHECK(count == strlen(record));
    CHECK(strstr(record, "UMICOM-AI-ADAMW-RUN-LOCK-1\n") == record);
    CHECK(strstr(record, "checkpoint_sha256=") != NULL);
    CHECK(strstr(record, "dataset_snapshot_sha256=") != NULL);
    CHECK(strstr(record, "run_settings_sha256=") != NULL);
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_OK);

    /* A scheduled intermediate checkpoint continues to a LATER end step;
     * the original eight-step cosine schedule has NOT changed. */
    config.steps = 8U;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_OK);
    config.trainSeed++;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);
    config.trainSeed--;
    config.evalSeed++;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);
    config.evalSeed--;
    config.evalInterval++;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);
    config.evalInterval--;
    schedule.totalSteps++;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);
    schedule.totalSteps--;
    schedule.warmupSteps++;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);
    schedule.warmupSteps--;
    const float recordedDecay = adamw.weightDecay;
    adamw.weightDecay += 0.01F;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);
    adamw.weightDecay = recordedDecay;
    clipping.microbatchSequences = 2U;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);
    clipping.microbatchSequences = 1U;
    clipping.maximumGradientNorm = 0.5F;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);
    clipping.maximumGradientNorm = 1.0F;
    inputs.dataset = reordered;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);
    inputs.dataset = dataset;
    memcpy(changedBytes, checkpoint, sizeof(checkpoint));
    changedBytes[1] ^= 1U;
    CHECK(UmiAiTrainingRunLockVerify(changedBytes, sizeof(changedBytes),
          &inputs, record, count) == UMI_STATUS_PARSE_ERROR);

    /* Neither record nor reported length may be mutated by rejection. */
    CHECK(UmiAiTrainingRunLockFormat(checkpoint, sizeof(checkpoint),
          &inputs, tiny, sizeof(tiny), &sentinel) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(tiny[0] == 'X' && sentinel == 9876U);
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count + 1U) == UMI_STATUS_PARSE_ERROR);
    clipping.maximumGradientNorm = -1.0F;
    CHECK(UmiAiTrainingRunLockVerify(checkpoint, sizeof(checkpoint),
          &inputs, record, count) == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiTrainingDatasetDestroy(reordered);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("AdamW run lock: settings, dataset, checkpoint and output atomicity PASS");
    return 0;
}
