/*-----------------------------------------------------------------------------
 * Umicom Framework — Native language-model learning laboratory
 * File: learning/native-llm/train_scheduled.c
 *
 * PURPOSE:
 *   Run reviewed source-code experiments with deterministic cosine learning-
 *   rate scheduling, either direct SGD or Framework-owned microbatch SGD.
 *   The training horizon is explicit so short runs can resume against the
 *   same unchanged schedule without relying on terminal working directory.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment.h"
#include "umicom/ai/training/experiment_accumulation.h"
#include "umicom/ai/training/experiment_plan.h"
#include "umicom/ai/training/optimizer.h"
#include "umicom/ai/training/checkpoint.h"

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int ParseStep(const char *text, uint64_t *outValue)
{
    char *end = NULL;
    unsigned long long value;
    if (text == NULL || outValue == NULL || text[0] == '\0' ||
        text[0] == '-' || text[0] == '+') return 0;
    errno = 0;
    value = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0') return 0;
    *outValue = (uint64_t)value;
    return 1;
}

static int ParseRate(const char *text, float *outValue)
{
    char *end = NULL;
    float value;
    if (text == NULL || outValue == NULL || text[0] == '\0') return 0;
    errno = 0;
    value = strtof(text, &end);
    if (errno != 0 || end == text || *end != '\0' || !isfinite(value) ||
        value <= 0.0F) return 0;
    *outValue = value;
    return 1;
}

static UmiStatus ShowProgress(
    const UmiAiTrainingExperimentProgress *progress, void *userData)
{
    (void)userData;
    printf("step %" PRIu64 " | held-out CE %.6f | perplexity %.6f\n",
        progress->completedSteps, progress->heldOutCrossEntropy,
        progress->heldOutPerplexity);
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL, *checkpoint = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingExperimentAccumulation accumulation = {0};
    UmiAiTrainingLearningRateSchedule schedule = {0};
    UmiAiTrainingExperimentResult result;
    char manifest[1024];
    UmiStatus status;
    if (argc != 5 && argc != 6 && argc != 7) {
        fprintf(stderr, "usage: umicom-native-llm-scheduled <reviewed-plan> "
            "<planned-total-steps> <warmup-steps> <minimum-rate> "
            "[new-checkpoint] [existing-checkpoint-to-resume]\n");
        return 2;
    }
    if (!ParseStep(argv[2], &schedule.totalSteps) ||
        !ParseStep(argv[3], &schedule.warmupSteps) ||
        !ParseRate(argv[4], &schedule.minimumLearningRate)) {
        fputs("invalid learning-rate schedule inputs\n", stderr);
        return 2;
    }
    status = UmiAiTrainingExperimentPlanLoadAccumulated(argv[1],
        &config, &accumulation, manifest, sizeof(manifest));
    if (status != UMI_STATUS_OK) goto failed;
    schedule.kind = UMI_AI_TRAINING_SCHEDULE_COSINE;
    schedule.peakLearningRate = config.learningRate;
    if (UmiAiTrainingLearningRateValidate(&schedule) != UMI_STATUS_OK ||
        (uint64_t)config.steps > schedule.totalSteps) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto failed;
    }
    status = UmiAiTrainingDatasetLoadManifest(manifest, &dataset);
    if (status != UMI_STATUS_OK) goto failed;
    if (argc == 7) {
        status = UmiAiTrainingDecoderCheckpointLoadFile(argv[6], &checkpoint);
        if (status != UMI_STATUS_OK) goto failed;
    }
    printf("Umicom scheduled C23 training: %u run steps; "
           "planned horizon=%" PRIu64 "; warmup=%" PRIu64 "\n",
           (unsigned)config.steps, schedule.totalSteps, schedule.warmupSteps);
    if (checkpoint != NULL) {
        puts("Resume requires the exact original schedule, source corpus, "
             "seeds and optimiser configuration; the checkpoint does not bind them.");
    }
    if (accumulation.effectiveSequences != 0U) {
        status = checkpoint == NULL ?
            UmiAiTrainingExperimentRunAccumulatedScheduled(dataset, &config,
                &accumulation, &schedule, ShowProgress, NULL, &model, &result) :
            UmiAiTrainingExperimentResumeAccumulatedScheduled(dataset, &config,
                &accumulation, &schedule, checkpoint, ShowProgress, NULL,
                &model, &result);
    } else {
        status = checkpoint == NULL ?
            UmiAiTrainingExperimentRunScheduled(dataset, &config, &schedule,
                ShowProgress, NULL, &model, &result) :
            UmiAiTrainingExperimentResumeScheduled(dataset, &config, &schedule,
                checkpoint, ShowProgress, NULL, &model, &result);
    }
    if (status != UMI_STATUS_OK) goto failed;
    printf("completed steps=%" PRIu64 "; final held-out CE %.6f\n",
           result.completedSteps, result.finalHeldOutLoss);
    if (argc >= 6) {
        status = UmiAiTrainingDecoderCheckpointSaveNewFile(model, argv[5]);
        if (status != UMI_STATUS_OK) goto failed;
        printf("new checkpoint: %s\n", argv[5]);
    }
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDecoderDestroy(checkpoint);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;
failed:
    fprintf(stderr, "scheduled native experiment rejected (status=%d)\n", (int)status);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDecoderDestroy(checkpoint);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
