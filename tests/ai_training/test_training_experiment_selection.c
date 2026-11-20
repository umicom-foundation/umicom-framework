/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_experiment_selection.c
 *
 * PURPOSE:
 *   Verify selected experiment weights, early stopping and atomic cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/experiment.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Experiment checkpoint selection, line %d: %s\n", __LINE__, #condition); \
    goto finish; } } while (0)

typedef struct Observation {
    uint32_t count;
    uint64_t last;
    uint64_t cancelAt;
} Observation;

static UmiStatus Observe(const UmiAiTrainingExperimentProgress *progress, void *context)
{
    Observation *observed = context;
    ++observed->count;
    observed->last = progress->completedSteps;
    return observed->cancelAt == progress->completedSteps ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}

static int EqualCheckpoints(const UmiAiTrainingDecoder *first, const UmiAiTrainingDecoder *second)
{
    size_t size = 0U, secondSize = 0U, firstWritten = 0U, secondWritten = 0U;
    if (UmiAiTrainingDecoderCheckpointSize(first, &size) != UMI_STATUS_OK ||
        UmiAiTrainingDecoderCheckpointSize(second, &secondSize) != UMI_STATUS_OK ||
        size == 0U || size != secondSize) return 0;
    uint8_t *left = malloc(size), *right = malloc(size);
    const int same = left != NULL && right != NULL &&
        UmiAiTrainingDecoderCheckpointEncode(first, left, size, &firstWritten) == UMI_STATUS_OK &&
        UmiAiTrainingDecoderCheckpointEncode(second, right, size, &secondWritten) == UMI_STATUS_OK &&
        firstWritten == size && secondWritten == size && memcmp(left, right, size) == 0;
    free(right);
    free(left);
    return same;
}

int main(void)
{
    UmiAiTrainingDataset *disjoint = NULL, *related = NULL;
    UmiAiTrainingDecoder *early = NULL, *legacy = NULL, *untrained = NULL, *best = NULL, *threshold = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingExperimentSelectedResult stopped, selected, strict, previous;
    UmiAiTrainingExperimentResult full;
    UmiAiTrainingDecoderConfig shape;
    uint64_t actual = 0U;
    double observedLoss = 0.0, perplexity = 0.0;
    int result = EXIT_FAILURE;
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_DISJOINT_MANIFEST, &disjoint) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_RELATED_MANIFEST, &related) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    config.steps = 8U;
    config.evalInterval = 1U;
    config.evalBatches = 2U;
    const UmiAiTrainingExperimentSelection patience = {0.0, 2U};
    Observation observed = {.cancelAt = UINT64_MAX};

    /* Repeated training bytes have no overlap with the held-out bytes here.
     * This keeps overfitting visible and proves the baseline is selectable. */
    CHECK(UmiAiTrainingExperimentRunSelected(disjoint, &config, &patience,
        Observe, &observed, &early, &stopped) == UMI_STATUS_OK);
    CHECK(stopped.stoppedEarly && stopped.selectedStep == 0U && stopped.run.completedSteps == 2U);
    CHECK(observed.count == 3U && observed.last == 2U && stopped.run.evaluationCount == observed.count);
    CHECK(stopped.selectedHeldOutLoss == stopped.run.baselineHeldOutLoss &&
          stopped.run.finalHeldOutLoss > stopped.selectedHeldOutLoss);
    CHECK(UmiAiTrainingDecoderCreate(&config.decoder, &untrained) == UMI_STATUS_OK);
    CHECK(EqualCheckpoints(early, untrained));
    CHECK(UmiAiTrainingExperimentEvaluate(early, disjoint, &config, &observedLoss, &perplexity) == UMI_STATUS_OK);
    CHECK(observedLoss == stopped.selectedHeldOutLoss && isfinite(perplexity));
    CHECK(UmiAiTrainingExperimentRun(disjoint, &config, NULL, NULL, &legacy, &full) == UMI_STATUS_OK);
    CHECK(full.completedSteps == config.steps && full.finalHeldOutLoss > stopped.selectedHeldOutLoss);
    CHECK(UmiAiTrainingDecoderInspect(legacy, &shape, &actual) == UMI_STATUS_OK && actual == config.steps);

    const UmiAiTrainingExperimentSelection chooseBest = {0.0, 0U};
    CHECK(UmiAiTrainingExperimentRunSelected(related, &config, &chooseBest,
        NULL, NULL, &best, &selected) == UMI_STATUS_OK);
    CHECK(!selected.stoppedEarly && selected.run.completedSteps == config.steps);
    CHECK(selected.selectedStep > 0U && selected.selectedStep == selected.run.bestObservedStep);
    CHECK(selected.selectedHeldOutLoss == selected.run.bestObservedHeldOutLoss &&
          selected.selectedHeldOutLoss < selected.run.baselineHeldOutLoss);
    CHECK(UmiAiTrainingDecoderInspect(best, &shape, &actual) == UMI_STATUS_OK && actual == selected.selectedStep);
    CHECK(UmiAiTrainingExperimentEvaluate(best, related, &config, &observedLoss, &perplexity) == UMI_STATUS_OK);
    CHECK(observedLoss == selected.selectedHeldOutLoss);

    const UmiAiTrainingExperimentSelection requireLargeChange = {100.0, 0U};
    CHECK(UmiAiTrainingExperimentRunSelected(related, &config, &requireLargeChange,
        NULL, NULL, &threshold, &strict) == UMI_STATUS_OK);
    CHECK(!strict.stoppedEarly && strict.run.completedSteps == config.steps && strict.selectedStep == 0U);
    CHECK(strict.run.bestObservedStep == selected.run.bestObservedStep &&
          strict.run.finalHeldOutLoss == selected.run.finalHeldOutLoss && EqualCheckpoints(threshold, untrained));

    memset(&previous, 0x5A, sizeof previous);
    memcpy(&strict, &previous, sizeof previous);
    UmiAiTrainingDecoder *existing = early;
    observed = (Observation){.cancelAt = 2U};
    CHECK(UmiAiTrainingExperimentRunSelected(disjoint, &config, &patience,
        Observe, &observed, &existing, &strict) == UMI_STATUS_CANCELLED);
    CHECK(existing == early && memcmp(&strict, &previous, sizeof previous) == 0 && observed.count == 3U);
    const UmiAiTrainingExperimentSelection invalid = {NAN, 2U};
    CHECK(UmiAiTrainingExperimentRunSelected(disjoint, &config, &invalid,
        Observe, &observed, &existing, &strict) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(existing == early && memcmp(&strict, &previous, sizeof previous) == 0 && observed.count == 3U);
    printf("Disjoint held-out loss baseline %.6f terminal %.6f selected step %llu completed %llu\n",
        stopped.run.baselineHeldOutLoss, stopped.run.finalHeldOutLoss,
        (unsigned long long)stopped.selectedStep, (unsigned long long)stopped.run.completedSteps);
    printf("Related held-out loss baseline %.6f selected %.6f at step %llu\n",
        selected.run.baselineHeldOutLoss, selected.selectedHeldOutLoss,
        (unsigned long long)selected.selectedStep);
    result = EXIT_SUCCESS;
finish:
    UmiAiTrainingDecoderDestroy(threshold);
    UmiAiTrainingDecoderDestroy(best);
    UmiAiTrainingDecoderDestroy(untrained);
    UmiAiTrainingDecoderDestroy(legacy);
    UmiAiTrainingDecoderDestroy(early);
    UmiAiTrainingDatasetDestroy(related);
    UmiAiTrainingDatasetDestroy(disjoint);
    return result;
}
