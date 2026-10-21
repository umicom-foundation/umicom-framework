/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_scheduled_experiment.c
 * PURPOSE: Verify absolute-step schedule and exact resumed SGD parameters.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment.h"
#include "umicom/ai/training/experiment_accumulation.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/optimizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH is required
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static int CheckSameCheckpoint(UmiAiTrainingDecoder *a, UmiAiTrainingDecoder *b)
{
    size_t bytesA = 0U, bytesB = 0U, usedA = 0U, usedB = 0U;
    uint8_t *left, *right;
    CHECK(UmiAiTrainingDecoderCheckpointSize(a, &bytesA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(b, &bytesB) == UMI_STATUS_OK);
    CHECK(bytesA == bytesB && bytesA > 0U);
    left = malloc(bytesA); right = malloc(bytesB);
    CHECK(left != NULL && right != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(a, left, bytesA, &usedA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(b, right, bytesB, &usedB) == UMI_STATUS_OK);
    CHECK(usedA == bytesA && usedB == bytesB && memcmp(left, right, bytesA) == 0);
    free(left); free(right);
    return 0;
}

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *full = NULL, *partial = NULL, *resumed = NULL;
    UmiAiTrainingExperimentConfig config, shorter;
    UmiAiTrainingExperimentAccumulation batch = {8U, 2U};
    UmiAiTrainingExperimentResult resultA, resultB, resultC, unchanged;
    UmiAiTrainingLearningRateSchedule schedule = {
        UMI_AI_TRAINING_SCHEDULE_COSINE, 0.025F, 0.005F, 1U, 4U};
    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    config.steps = 4U;
    config.evalInterval = 2U;
    config.evalBatches = 2U;
    shorter = config;
    shorter.steps = 2U;
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                           &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentRunAccumulatedScheduled(dataset, &config,
        &batch, &schedule, NULL, NULL, &full, &resultA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentRunAccumulatedScheduled(dataset, &shorter,
        &batch, &schedule, NULL, NULL, &partial, &resultB) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentResumeAccumulatedScheduled(dataset, &config,
        &batch, &schedule, partial, NULL, NULL, &resumed, &resultC) == UMI_STATUS_OK);
    CHECK(resultA.completedSteps == 4U && resultB.completedSteps == 2U &&
          resultC.completedSteps == 4U);
    CHECK(CheckSameCheckpoint(full, resumed) == 0);
    UmiAiTrainingDecoderDestroy(full);
    UmiAiTrainingDecoderDestroy(partial);
    UmiAiTrainingDecoderDestroy(resumed);
    full = partial = resumed = NULL;
    CHECK(UmiAiTrainingExperimentRunScheduled(dataset, &config, &schedule,
        NULL, NULL, &full, &resultA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentRunScheduled(dataset, &shorter, &schedule,
        NULL, NULL, &partial, &resultB) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentResumeScheduled(dataset, &config,
        &schedule, partial, NULL, NULL, &resumed, &resultC) == UMI_STATUS_OK);
    CHECK(CheckSameCheckpoint(full, resumed) == 0);
    unchanged = resultA;
    UmiAiTrainingDecoder *rejected = NULL;
    schedule.totalSteps = 3U;
    CHECK(UmiAiTrainingExperimentRunScheduled(dataset, &config, &schedule,
        NULL, NULL, &rejected, &resultA) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(rejected == NULL && memcmp(&unchanged, &resultA, sizeof(resultA)) == 0);
    UmiAiTrainingDecoderDestroy(full);
    UmiAiTrainingDecoderDestroy(partial);
    UmiAiTrainingDecoderDestroy(resumed);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("scheduled direct and effective-batch SGD resume: PASS");
    return 0;
}
