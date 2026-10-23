/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_adamw_experiment_resume.c
 * PURPOSE: Prove that scheduled AdamW exact-resume preserves model AND moments.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_adamw_experiment_resume.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be specified by CMake
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

static int IdenticalCheckpoint(const UmiAiTrainingDecoder *left,
                               const UmiAiTrainingAdamW *leftOptimizer,
                               const UmiAiTrainingDecoder *right,
                               const UmiAiTrainingAdamW *rightOptimizer)
{
    size_t leftSize = 0U, rightSize = 0U, leftWritten = 0U, rightWritten = 0U;
    uint8_t *leftBytes, *rightBytes;
    CHECK(UmiAiTrainingAdamWCheckpointSize(left, leftOptimizer, &leftSize) ==
          UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointSize(right, rightOptimizer, &rightSize) ==
          UMI_STATUS_OK);
    CHECK(leftSize == rightSize && leftSize != 0U);
    leftBytes = malloc(leftSize);
    rightBytes = malloc(rightSize);
    CHECK(leftBytes != NULL && rightBytes != NULL);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(left, leftOptimizer,
          leftBytes, leftSize, &leftWritten) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWCheckpointEncode(right, rightOptimizer,
          rightBytes, rightSize, &rightWritten) == UMI_STATUS_OK);
    CHECK(leftWritten == leftSize && rightWritten == rightSize);
    CHECK(memcmp(leftBytes, rightBytes, leftSize) == 0);
    free(leftBytes);
    free(rightBytes);
    return 0;
}

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *full = NULL, *partial = NULL, *resumed = NULL;
    UmiAiTrainingAdamW *fullOptimizer = NULL, *partialOptimizer = NULL;
    UmiAiTrainingAdamW *resumedOptimizer = NULL;
    UmiAiTrainingExperimentConfig config, first;
    UmiAiTrainingAdamWConfig options;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingExperimentResult fullReport, partialReport, resumedReport;
    uint64_t steps = 0U;
    size_t bytes = 0U;

    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWDefaults(&options) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                           &dataset) == UMI_STATUS_OK);
    config.steps = 8U;
    config.evalInterval = 2U;
    config.evalBatches = 2U;
    config.batchSequences = 2U;
    config.learningRate = 0.018F;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_COSINE, 0.018F, 0.002F, 2U, 8U};
    CHECK(UmiAiTrainingExperimentAdamWValidate(&config, &options, &schedule) ==
          UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentRunAdamW(dataset, &config, &options,
          &schedule, NULL, NULL, &full, &fullOptimizer, &fullReport) ==
          UMI_STATUS_OK);
    CHECK(fullReport.completedSteps == 8U && fullReport.evaluationCount == 5U);
    CHECK(isfinite(fullReport.finalHeldOutLoss));

    /* This intermediate run MUST use the original eight-step schedule;
     * changing schedule.totalSteps when resuming would alter learning rates. */
    first = config;
    first.steps = 3U;
    first.evalInterval = 1U;
    CHECK(UmiAiTrainingExperimentRunAdamW(dataset, &first, &options,
          &schedule, NULL, NULL, &partial, &partialOptimizer, &partialReport) ==
          UMI_STATUS_OK);
    CHECK(partialReport.completedSteps == 3U);
    CHECK(UmiAiTrainingExperimentResumeAdamW(dataset, &config, &options,
          &schedule, partial, partialOptimizer, NULL, NULL,
          &resumed, &resumedOptimizer, &resumedReport) == UMI_STATUS_OK);
    CHECK(resumedReport.completedSteps == 8U);
    CHECK(IdenticalCheckpoint(full, fullOptimizer, resumed, resumedOptimizer) == 0);

    /* A checkpoint supplied by the caller is borrowed and never advanced. */
    CHECK(UmiAiTrainingAdamWInspect(partialOptimizer, &steps, &bytes) ==
          UMI_STATUS_OK);
    CHECK(steps == 3U && bytes > 0U);
    UmiAiTrainingDecoderConfig observed;
    CHECK(UmiAiTrainingDecoderInspect(partial, &observed, &steps) == UMI_STATUS_OK);
    CHECK(steps == 3U);

    UmiAiTrainingAdamWDestroy(resumedOptimizer);
    UmiAiTrainingAdamWDestroy(partialOptimizer);
    UmiAiTrainingAdamWDestroy(fullOptimizer);
    UmiAiTrainingDecoderDestroy(resumed);
    UmiAiTrainingDecoderDestroy(partial);
    UmiAiTrainingDecoderDestroy(full);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("scheduled AdamW run/resume, model+moment checkpoint identity: PASS");
    return 0;
}
