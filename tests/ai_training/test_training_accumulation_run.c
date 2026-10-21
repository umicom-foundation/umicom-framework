/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_accumulation_run.c
 * PURPOSE: Check deterministic source draws, held-out evaluation, and exact
 *          checkpoint equivalence after an interrupted effective-batch run.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_accumulation.h"
#include "umicom/ai/training/experiment_plan.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UMI_AI_TEST_ACCUMULATED_PLAN_PATH
#error UMI_AI_TEST_ACCUMULATED_PLAN_PATH required by CMake
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

typedef struct ObserverState {
    uint64_t steps[8];
    size_t events;
} ObserverState;

static UmiStatus Observe(const UmiAiTrainingExperimentProgress *progress,
                         void *context)
{
    ObserverState *state = context;
    if (progress == NULL || state == NULL || state->events >= 8U ||
        !isfinite(progress->heldOutCrossEntropy) ||
        !isfinite(progress->heldOutPerplexity))
        return UMI_STATUS_INVALID_STATE;
    state->steps[state->events++] = progress->completedSteps;
    return UMI_STATUS_OK;
}

static int CompareModels(const UmiAiTrainingDecoder *left,
                         const UmiAiTrainingDecoder *right)
{
    size_t leftSize = 0U, rightSize = 0U;
    size_t leftWritten = 0U, rightWritten = 0U;
    uint8_t *leftBytes = NULL, *rightBytes = NULL;
    CHECK(UmiAiTrainingDecoderCheckpointSize(left, &leftSize) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(right, &rightSize) == UMI_STATUS_OK);
    CHECK(leftSize > 0U && leftSize == rightSize);
    leftBytes = malloc(leftSize);
    rightBytes = malloc(rightSize);
    CHECK(leftBytes != NULL && rightBytes != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(left, leftBytes, leftSize,
                                                &leftWritten) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(right, rightBytes, rightSize,
                                                &rightWritten) == UMI_STATUS_OK);
    CHECK(leftWritten == leftSize && rightWritten == rightSize);
    CHECK(memcmp(leftBytes, rightBytes, leftSize) == 0);
    free(rightBytes);
    free(leftBytes);
    return 0;
}

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *full = NULL, *duplicate = NULL;
    UmiAiTrainingDecoder *partial = NULL, *continued = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingExperimentConfig halfway;
    UmiAiTrainingExperimentAccumulation accumulation;
    UmiAiTrainingExperimentResult report, repeated, partialReport, continuedReport;
    ObserverState observer = {0};
    char manifestPath[1024];
    CHECK(UmiAiTrainingExperimentPlanLoadAccumulated(
        UMI_AI_TEST_ACCUMULATED_PLAN_PATH, &config,
        &accumulation, manifestPath, sizeof(manifestPath)) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(manifestPath, &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentRunAccumulated(dataset, &config, &accumulation,
             Observe, &observer, &full, &report) == UMI_STATUS_OK);
    CHECK(full != NULL && report.completedSteps == 4U);
    CHECK(observer.events == 3U && observer.steps[0] == 0U &&
          observer.steps[1] == 2U && observer.steps[2] == 4U);
    CHECK(report.dataset.trainingDocuments == 2U &&
          report.dataset.evaluationDocuments == 1U);
    CHECK(isfinite(report.baselineHeldOutLoss) &&
          isfinite(report.finalHeldOutLoss) && isfinite(report.lastTrainingLoss));
    CHECK(UmiAiTrainingExperimentRunAccumulated(dataset, &config, &accumulation,
             NULL, NULL, &duplicate, &repeated) == UMI_STATUS_OK);
    CHECK(CompareModels(full, duplicate) == 0);
    CHECK(report.finalHeldOutLoss == repeated.finalHeldOutLoss &&
          report.lastTrainingLoss == repeated.lastTrainingLoss);

    halfway = config;
    halfway.steps = 2U;
    CHECK(UmiAiTrainingExperimentRunAccumulated(dataset, &halfway,
             &accumulation, NULL, NULL,
             &partial, &partialReport) == UMI_STATUS_OK);
    CHECK(partialReport.completedSteps == 2U);
    CHECK(UmiAiTrainingExperimentResumeAccumulated(dataset, &config,
             &accumulation, partial, NULL, NULL,
             &continued, &continuedReport) == UMI_STATUS_OK);
    CHECK(continuedReport.completedSteps == 4U);
    CHECK(CompareModels(full, continued) == 0);
    UmiAiTrainingDecoderConfig inspectConfig;
    uint64_t inspectSteps = UINT64_MAX;
    CHECK(UmiAiTrainingDecoderInspect(partial, &inspectConfig,
                                       &inspectSteps) == UMI_STATUS_OK);
    CHECK(inspectSteps == 2U);

    UmiAiTrainingDecoderDestroy(continued);
    UmiAiTrainingDecoderDestroy(partial);
    UmiAiTrainingDecoderDestroy(duplicate);
    UmiAiTrainingDecoderDestroy(full);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("Effective-batch deterministic training and exact resume: PASS");
    return 0;
}
