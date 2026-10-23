/*-----------------------------------------------------------------------------
 * Umicom Framework | test_training_experiment_cancel.c
 * Author: Sammy Hegab, Umicom Foundation | MIT
 * PURPOSE: Explicit cancellation without publishing partial model or result.
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_experiment_cancel.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/experiment.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH required by CMake
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "CHECK failed %s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

typedef struct CancelState { uint32_t events; } CancelState;
static UmiStatus CancelAfterBaseline(
    const UmiAiTrainingExperimentProgress *progress, void *userData)
{
    CancelState *state = userData;
    if (progress == NULL || state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ++state->events;
    return UMI_STATUS_CANCELLED;
}

static UmiStatus CancelAfterTwoSteps(
    const UmiAiTrainingExperimentProgress *progress, void *userData)
{
    CancelState *state = userData;
    if (progress == NULL || state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ++state->events;
    return progress->completedSteps >= 2U ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingExperimentResult report;
    UmiAiTrainingExperimentResult before;
    UmiAiTrainingDecoder *model = NULL;
    CancelState state = {0};
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                            &dataset) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    memset(&report, 0x5A, sizeof(report));
    memcpy(&before, &report, sizeof(before));
    CHECK(UmiAiTrainingExperimentRun(dataset, &config,
        CancelAfterBaseline, &state, &model, &report) == UMI_STATUS_CANCELLED);
    CHECK(state.events == 1U);
    CHECK(model == NULL);
    CHECK(memcmp(&report, &before, sizeof(before)) == 0);
    /* Cancellation after actual SGD work also keeps the incomplete model
     * private. An observer's earlier outputs are not transactional. */
    state.events = 0U;
    config.steps = 4U;
    config.evalInterval = 2U;
    CHECK(UmiAiTrainingExperimentRun(dataset, &config,
        CancelAfterTwoSteps, &state, &model, &report) == UMI_STATUS_CANCELLED);
    CHECK(state.events == 2U);
    CHECK(model == NULL);
    CHECK(memcmp(&report, &before, sizeof(before)) == 0);
    config.steps = 64U;
    config.evalInterval = 16U;
    /* The earlier tensor ceiling rejected two sequences. Four sequences
     * now fit at context eight. Test a larger context which still exceeds
     * the new storage bound without crossing the manifest policy limits. */
#if 0
    config.batchSequences = 2U;
    CHECK(UmiAiTrainingExperimentValidate(&config) == UMI_STATUS_CAPACITY_EXCEEDED);
#endif
    config.batchSequences = 4U;
    config.decoder.contextLength = 32U;
    CHECK(UmiAiTrainingExperimentValidate(&config) == UMI_STATUS_CAPACITY_EXCEEDED);
    config.batchSequences = 1U;
    config.decoder.contextLength = 8U;
    config.evalInterval = 0U;
    CHECK(UmiAiTrainingExperimentValidate(&config) == UMI_STATUS_INVALID_ARGUMENT);
    config.evalInterval = 16U;
    config.learningRate = NAN;
    CHECK(UmiAiTrainingExperimentValidate(&config) == UMI_STATUS_INVALID_ARGUMENT);
    config.learningRate = 0.025F;
    config.decoder.attentionHeads = 3U;
    CHECK(UmiAiTrainingExperimentValidate(&config) == UMI_STATUS_INVALID_ARGUMENT);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("cancellation, invalid plans, capacity bounds: PASS");
    return 0;
}
