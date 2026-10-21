/*-----------------------------------------------------------------------------
 * Umicom Framework | learning/native-llm/train_adamw_experiment.c
 *
 * PURPOSE:
 *   Read a reviewed local source-training plan and run Framework-owned AdamW
 *   optimisation with deterministic absolute-step schedule. An optional
 *   combined checkpoint is read or explicitly published to a NEW path only.
 *   This command neither executes source files nor contacts network services.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/experiment_plan.h"
#include "umicom/ai/training/optimizer_checkpoint.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#define UMI_ADAMW_EXPERIMENT_PATH_MAX 1024U

static int ReadUnsigned(const char *text, uint64_t *outNumber)
{
    uint64_t value = 0U;
    if (text == NULL || text[0] == '\0' || outNumber == NULL) return 0;
    for (const char *cursor = text; *cursor != '\0'; ++cursor) {
        uint64_t digit;
        if (*cursor < '0' || *cursor > '9') return 0;
        digit = (uint64_t)(unsigned)(*cursor - '0');
        if (value > (UINT64_MAX - digit) / UINT64_C(10)) return 0;
        value = value * UINT64_C(10) + digit;
    }
    if (value == 0U) return 0;
    *outNumber = value;
    return 1;
}

static UmiStatus Progress(const UmiAiTrainingExperimentProgress *step,
                          void *context)
{
    (void)context;
    if (step == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    printf("step %" PRIu64 " | held-out CE %.6f | perplexity %.6f\n",
           step->completedSteps, step->heldOutCrossEntropy,
           step->heldOutPerplexity);
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiAiTrainingExperimentConfig plan;
    UmiAiTrainingAdamWConfig optimiserConfig;
    UmiAiTrainingLearningRateSchedule schedule;
    UmiAiTrainingExperimentResult report;
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL, *checkpointModel = NULL;
    UmiAiTrainingAdamW *optimizer = NULL, *checkpointOptimizer = NULL;
    char manifest[UMI_ADAMW_EXPERIMENT_PATH_MAX];
    uint64_t scheduleSteps = 0U;
    UmiStatus status;
    if (argc < 3 || argc > 5 || !ReadUnsigned(argv[2], &scheduleSteps) ||
        scheduleSteps > (uint64_t)UMI_AI_EXPERIMENT_MAX_STEPS) {
        fprintf(stderr, "usage: umicom-native-llm-adamw-experiment "
                "<reviewed-plan> <schedule-total-steps> "
                "[NEW-combined-checkpoint] [existing-combined-checkpoint]\n");
        return 2;
    }
    status = UmiAiTrainingExperimentPlanLoadFile(
        argv[1], &plan, manifest, sizeof(manifest));
    if (status != UMI_STATUS_OK) goto fail;
    status = UmiAiTrainingAdamWDefaults(&optimiserConfig);
    if (status != UMI_STATUS_OK) goto fail;
    schedule = (UmiAiTrainingLearningRateSchedule){
        UMI_AI_TRAINING_SCHEDULE_COSINE, plan.learningRate,
        plan.learningRate * 0.1F,
        scheduleSteps > UINT64_C(4) ? UINT64_C(2) : UINT64_C(0),
        scheduleSteps};
    status = UmiAiTrainingExperimentAdamWValidate(
        &plan, &optimiserConfig, &schedule);
    if (status != UMI_STATUS_OK) goto fail;
    status = UmiAiTrainingDatasetLoadManifest(manifest, &dataset);
    if (status != UMI_STATUS_OK) goto fail;
    if (argc == 5) {
        status = UmiAiTrainingAdamWCheckpointLoadFile(
            argv[4], &checkpointModel, &checkpointOptimizer);
        if (status != UMI_STATUS_OK) goto fail;
        status = UmiAiTrainingExperimentResumeAdamW(dataset, &plan,
            &optimiserConfig, &schedule, checkpointModel, checkpointOptimizer,
            Progress, NULL, &model, &optimizer, &report);
    } else {
        status = UmiAiTrainingExperimentRunAdamW(dataset, &plan,
            &optimiserConfig, &schedule, Progress, NULL,
            &model, &optimizer, &report);
    }
    if (status != UMI_STATUS_OK) goto fail;
    printf("AdamW completed %" PRIu64 " steps; "
           "sampled held-out CE %.6f -> %.6f\n",
           report.completedSteps, report.baselineHeldOutLoss,
           report.finalHeldOutLoss);
    puts("Reproducibility requires the same approved source bytes, "
         "sampling seeds, optimiser settings and learning-rate schedule.");
    if (argc >= 4) {
        status = UmiAiTrainingAdamWCheckpointSaveNewFile(
            model, optimizer, argv[3]);
        if (status != UMI_STATUS_OK) goto fail;
        printf("New combined AdamW checkpoint saved: %s\n", argv[3]);
    }
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingAdamWDestroy(checkpointOptimizer);
    UmiAiTrainingDecoderDestroy(checkpointModel);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;
fail:
    fprintf(stderr, "reviewed AdamW experiment rejected (status=%d)\n",
            (int)status);
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingAdamWDestroy(checkpointOptimizer);
    UmiAiTrainingDecoderDestroy(checkpointModel);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
