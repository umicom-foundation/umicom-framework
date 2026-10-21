/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_adamw_experiment_cancel.c
 * PURPOSE: Observer cancellation cannot publish partly updated native models.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error UMI_AI_TEST_MANIFEST_PATH must be specified by CMake
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; \
} } while (0)

typedef struct CancelState { uint64_t atStep; size_t calls; } CancelState;
static UmiStatus CancelAt(const UmiAiTrainingExperimentProgress *progress,
                          void *userData)
{
    CancelState *state = userData;
    if (progress == NULL || state == NULL ||
        !isfinite(progress->heldOutCrossEntropy))
        return UMI_STATUS_INVALID_STATE;
    state->calls++;
    return progress->completedSteps == state->atStep ?
           UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL, *checkpoint = NULL;
    UmiAiTrainingAdamW *optimizer = NULL, *checkpointOptimizer = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingAdamWConfig options;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingExperimentResult report, unchanged;
    CancelState early = {0U, 0U}, later = {2U, 0U};
    uint64_t steps = 0U;
    size_t bytes = 0U;

    CHECK(UmiAiTrainingExperimentDefaults(&config) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingAdamWDefaults(&options) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                           &dataset) == UMI_STATUS_OK);
    config.steps = 4U;
    config.evalInterval = 1U;
    config.evalBatches = 1U;
    config.learningRate = 0.02F;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_CONSTANT, 0.02F, 0.01F, 0U, 4U};
    memset(&report, 0x56, sizeof(report));
    unchanged = report;
    CHECK(UmiAiTrainingExperimentRunAdamW(dataset, &config, &options,
          &schedule, CancelAt, &early, &model, &optimizer, &report) ==
          UMI_STATUS_CANCELLED);
    CHECK(early.calls == 1U && model == NULL && optimizer == NULL);
    CHECK(memcmp(&report, &unchanged, sizeof(report)) == 0);
    CHECK(UmiAiTrainingExperimentRunAdamW(dataset, &config, &options,
          &schedule, CancelAt, &later, &model, &optimizer, &report) ==
          UMI_STATUS_CANCELLED);
    CHECK(later.calls == 3U && model == NULL && optimizer == NULL);
    CHECK(memcmp(&report, &unchanged, sizeof(report)) == 0);

    /* A legitimate published checkpoint is not modified by subsequent
     * cancelled resume attempts. */
    config.steps = 2U;
    CHECK(UmiAiTrainingExperimentRunAdamW(dataset, &config, &options,
          &schedule, NULL, NULL, &checkpoint, &checkpointOptimizer, &report) ==
          UMI_STATUS_OK);
    config.steps = 4U;
    memset(&report, 0x56, sizeof(report));
    early.atStep = 2U;
    CHECK(UmiAiTrainingExperimentResumeAdamW(dataset, &config, &options,
          &schedule, checkpoint, checkpointOptimizer,
          CancelAt, &early, &model, &optimizer, &report) ==
          UMI_STATUS_CANCELLED);
    CHECK(model == NULL && optimizer == NULL);
    CHECK(memcmp(&report, &unchanged, sizeof(report)) == 0);
    CHECK(UmiAiTrainingAdamWInspect(checkpointOptimizer, &steps, &bytes) ==
          UMI_STATUS_OK);
    CHECK(steps == 2U && bytes > 0U);

    UmiAiTrainingAdamWDestroy(checkpointOptimizer);
    UmiAiTrainingDecoderDestroy(checkpoint);
    UmiAiTrainingDatasetDestroy(dataset);
    puts("AdamW experiment observer cancellation and borrowed checkpoint: PASS");
    return 0;
}
