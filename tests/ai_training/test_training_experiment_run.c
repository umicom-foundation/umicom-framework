/*-----------------------------------------------------------------------------
 * Umicom Framework | test_training_experiment_run.c
 * Author: Sammy Hegab, Umicom Foundation | MIT
 * PURPOSE: Exact-seed experiments, held-out metrics, checkpoint equivalence.
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment.h"
#include "umicom/ai/training/experiment_plan.h"
#include "umicom/ai/training/checkpoint.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH required by CMake
#endif
#ifndef UMI_AI_TEST_EXPERIMENT_PLAN_PATH
#error UMI_AI_TEST_EXPERIMENT_PLAN_PATH required by CMake
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "CHECK failed at %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

typedef struct ObserverState {
    uint64_t steps[6];
    size_t events;
} ObserverState;

static UmiStatus Observe(const UmiAiTrainingExperimentProgress *progress,
                         void *userData)
{
    ObserverState *state = userData;
    if (state == NULL || progress == NULL || state->events >= 6U)
        return UMI_STATUS_INVALID_STATE;
    state->steps[state->events++] = progress->completedSteps;
    if (!isfinite(progress->heldOutCrossEntropy) ||
        !isfinite(progress->heldOutPerplexity) ||
        progress->heldOutPerplexity < 1.0)
        return UMI_STATUS_INVALID_STATE;
    if (progress->completedSteps == 0U && progress->hasTrainingLoss != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

static int CheckSameCheckpoints(UmiAiTrainingDecoder *a,
                                UmiAiTrainingDecoder *b)
{
    size_t sizeA = 0U, sizeB = 0U, writtenA = 0U, writtenB = 0U;
    uint8_t *bytesA, *bytesB;
    CHECK(UmiAiTrainingDecoderCheckpointSize(a, &sizeA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointSize(b, &sizeB) == UMI_STATUS_OK);
    CHECK(sizeA == sizeB && sizeA > 0U);
    bytesA = malloc(sizeA);
    bytesB = malloc(sizeB);
    CHECK(bytesA != NULL && bytesB != NULL);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(a, bytesA, sizeA,
                                                 &writtenA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDecoderCheckpointEncode(b, bytesB, sizeB,
                                                 &writtenB) == UMI_STATUS_OK);
    CHECK(writtenA == writtenB && writtenA == sizeA);
    CHECK(memcmp(bytesA, bytesB, writtenA) == 0);
    UmiAiTrainingDecoder *reloaded = NULL;
    CHECK(UmiAiTrainingDecoderCheckpointDecode(bytesA, sizeA,
                                                &reloaded) == UMI_STATUS_OK);
    UmiAiTrainingDecoderConfig config;
    uint64_t steps = 0U;
    CHECK(UmiAiTrainingDecoderInspect(reloaded, &config, &steps) == UMI_STATUS_OK);
    CHECK(steps == 4U && config.vocabularySize == 260U);
    UmiAiTrainingDecoderDestroy(reloaded);
    free(bytesB);
    free(bytesA);
    return 0;
}

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *a = NULL, *b = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingExperimentResult reportA, reportB;
    ObserverState observerA = {0}, observerB = {0};
    char manifestPath[1024];
    CHECK(UmiAiTrainingExperimentPlanLoadFile(
        UMI_AI_TEST_EXPERIMENT_PLAN_PATH, &config, manifestPath,
        sizeof(manifestPath)) == UMI_STATUS_OK);
    CHECK(strcmp(manifestPath, UMI_AI_TEST_MANIFEST_PATH) == 0);
    CHECK(config.decoder.vocabularySize == 260U);
    CHECK(UmiAiTrainingDatasetLoadManifest(manifestPath, &dataset) == UMI_STATUS_OK);
    config.steps = 4U;
    config.evalInterval = 2U;
    config.evalBatches = 3U;
    CHECK(UmiAiTrainingExperimentRun(dataset, &config,
           Observe, &observerA, &a, &reportA) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentRun(dataset, &config,
           Observe, &observerB, &b, &reportB) == UMI_STATUS_OK);
    CHECK(observerA.events == 3U && observerB.events == 3U);
    CHECK(observerA.steps[0] == 0U && observerA.steps[1] == 2U &&
          observerA.steps[2] == 4U);
    CHECK(reportA.completedSteps == 4U && reportA.evaluationCount == 3U);
    CHECK(reportA.dataset.trainingDocuments == 2U);
    CHECK(reportA.dataset.evaluationDocuments == 1U);
    CHECK(isfinite(reportA.baselineHeldOutLoss) &&
          isfinite(reportA.finalHeldOutLoss) &&
          reportA.bestObservedHeldOutLoss <= reportA.baselineHeldOutLoss &&
          reportA.bestObservedHeldOutLoss <= reportA.finalHeldOutLoss);
    CHECK(reportA.completedSteps == reportB.completedSteps);
    CHECK(reportA.evaluationCount == reportB.evaluationCount);
    CHECK(reportA.baselineHeldOutLoss == reportB.baselineHeldOutLoss);
    CHECK(reportA.finalHeldOutLoss == reportB.finalHeldOutLoss);
    CHECK(reportA.bestObservedHeldOutLoss == reportB.bestObservedHeldOutLoss);
    CHECK(reportA.bestObservedStep == reportB.bestObservedStep);
    CHECK(reportA.lastTrainingLoss == reportB.lastTrainingLoss);
    CHECK(CheckSameCheckpoints(a, b) == 0);
    /* Resume must produce identical parameter bits to uninterrupted training.
     * Training batch numbers use ABSOLUTE step numbers, not local run counts. */
    UmiAiTrainingDecoder *partial = NULL, *resumed = NULL;
    UmiAiTrainingExperimentResult reportPartial, reportResumed;
    UmiAiTrainingExperimentConfig firstTwo = config;
    firstTwo.steps = 2U;
    CHECK(UmiAiTrainingExperimentRun(dataset, &firstTwo, NULL, NULL,
                                      &partial, &reportPartial) == UMI_STATUS_OK);
    CHECK(reportPartial.completedSteps == 2U);
    CHECK(UmiAiTrainingExperimentResume(dataset, &config, partial, NULL,
                    NULL, &resumed, &reportResumed) == UMI_STATUS_OK);
    CHECK(reportResumed.completedSteps == 4U);
    CHECK(CheckSameCheckpoints(a, resumed) == 0);
    UmiAiTrainingDecoderConfig partialConfig;
    uint64_t partialSteps = 0U;
    CHECK(UmiAiTrainingDecoderInspect(partial, &partialConfig,
                                      &partialSteps) == UMI_STATUS_OK);
    CHECK(partialSteps == 2U); /* borrowed checkpoint was not changed */
    UmiAiTrainingExperimentConfig mismatch = config;
    mismatch.decoder.initialSeed += UINT64_C(1);
    UmiAiTrainingDecoder *rejected = NULL;
    UmiAiTrainingExperimentResult rejectedReport;
    memset(&rejectedReport, 0xA5, sizeof(rejectedReport));
    UmiAiTrainingExperimentResult unchangedReport;
    memcpy(&unchangedReport, &rejectedReport, sizeof(rejectedReport));
    CHECK(UmiAiTrainingExperimentResume(dataset, &mismatch, partial,
            NULL, NULL, &rejected, &rejectedReport) == UMI_STATUS_INVALID_STATE);
    CHECK(rejected == NULL);
    CHECK(memcmp(&rejectedReport, &unchangedReport, sizeof(rejectedReport)) == 0);
    UmiAiTrainingDecoderDestroy(partial);
    UmiAiTrainingDecoderDestroy(resumed);
    UmiAiTrainingDecoderDestroy(a);
    UmiAiTrainingDecoderDestroy(b);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("reproducible native experiment, sampled holdout and checkpoints: PASS");
    return 0;
}
