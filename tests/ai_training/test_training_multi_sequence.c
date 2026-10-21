/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_multi_sequence.c
 * PURPOSE:
 *   Confirm that a reviewed multi-file corpus can train several independent
 *   sequences per optimisation step with repeatable checkpoint evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/capacity_plan.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/experiment.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must designate the reviewed source fixture
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "multi-sequence %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

int main(void)
{
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingCapacityPlan plan;
    UmiAiTrainingExperimentResult firstResult, secondResult;
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *first = NULL, *second = NULL;
    uint8_t *firstBytes = NULL, *secondBytes = NULL;
    size_t byteCount = 0U, countFirst = 0U, countSecond = 0U;
    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    config.batchSequences = 4U;
    config.steps = 2U;
    config.evalInterval = 1U;
    config.evalBatches = 2U;
    CHECK(UmiAiTrainingExperimentValidate(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingCapacityPlanCompute(&config.decoder, 4U, &plan)
          == UMI_STATUS_OK);
    CHECK(plan.logitsElements == 8320U);
    CHECK(plan.maximumBatchSequences >= 4U);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
          &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentRun(dataset, &config, NULL, NULL,
          &first, &firstResult) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentRun(dataset, &config, NULL, NULL,
          &second, &secondResult) == UMI_STATUS_OK);
    CHECK(firstResult.completedSteps == 2U);
    CHECK(secondResult.completedSteps == firstResult.completedSteps);
    CHECK(firstResult.finalHeldOutLoss == secondResult.finalHeldOutLoss);
    CHECK(isfinite(firstResult.finalHeldOutLoss));
    CHECK(UmiAiTrainingDecoderCheckpointSize(first, &byteCount)
          == UMI_STATUS_OK);
    firstBytes = malloc(byteCount);
    secondBytes = malloc(byteCount);
    CHECK(firstBytes != NULL && secondBytes != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(first, firstBytes,
          byteCount, &countFirst) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(second, secondBytes,
          byteCount, &countSecond) == UMI_STATUS_OK);
    CHECK(countFirst == byteCount && countSecond == byteCount);
    CHECK(memcmp(firstBytes, secondBytes, byteCount) == 0);
    free(secondBytes);
    free(firstBytes);
    UmiAiTrainingDecoderDestroy(second);
    UmiAiTrainingDecoderDestroy(first);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("multi-sequence training, fixed hold-out and repeatable weights: PASS");
    return 0;
}
