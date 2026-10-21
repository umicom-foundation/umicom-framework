/*-----------------------------------------------------------------------------
 * Umicom Framework — Native LLM educational laboratory
 * File: learning/native-llm/train_experiment.c
 *
 * PURPOSE:
 *   Thin native user interface for reviewed, reproducible Framework-owned
 *   source-model experiments. No duplicate training or parsing algorithms.
 *   Checkpoints are written only on an explicit request to a new path.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment.h"
#include "umicom/ai/training/experiment_plan.h"
#include "umicom/ai/training/checkpoint.h"

#include <inttypes.h>
#include <stdio.h>

#define UMI_EXPERIMENT_CLI_MANIFEST_CAPACITY 1024U

static UmiStatus ShowProgress(
    const UmiAiTrainingExperimentProgress *progress, void *userData)
{
    (void)userData;
    printf("step %4" PRIu64 " | held-out CE %.6f | sampled perplexity %.4f",
           progress->completedSteps, progress->heldOutCrossEntropy,
           progress->heldOutPerplexity);
    if (progress->hasTrainingLoss) {
        printf(" | last train-batch CE %.6f", (double)progress->lastTrainingLoss);
    }
    putchar('\n');
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingDecoder *startingCheckpoint = NULL;
    UmiAiTrainingExperimentConfig config;
    UmiAiTrainingExperimentResult report;
    UmiStatus status;
    char manifestPath[UMI_EXPERIMENT_CLI_MANIFEST_CAPACITY];
    if (argc != 2 && argc != 3 && argc != 4) {
        fprintf(stderr, "usage: umicom-native-llm-experiment <reviewed-plan> [new-checkpoint-path] [existing-checkpoint-to-resume]\n");
        return 2;
    }
    status = UmiAiTrainingExperimentPlanLoadFile(
        argv[1], &config, manifestPath, sizeof(manifestPath));
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "experiment plan rejected (status=%d)\n", (int)status);
        return 1;
    }
    status = UmiAiTrainingDatasetLoadManifest(manifestPath, &dataset);
    if (status != UMI_STATUS_OK) {
        fprintf(stderr, "source manifest rejected (status=%d): %s\n",
                (int)status, manifestPath);
        return 1;
    }
    UmiAiTrainingDatasetSummary datasetSummary;
    status = UmiAiTrainingDatasetInspect(dataset, &datasetSummary);
    if (status != UMI_STATUS_OK) goto error;
    printf("Umicom Native AI — reproducible decoder experiment\n"
           "Dataset manifest: %s\n"
           "Train: %zu files, %zu bytes; evaluate: %zu files, %zu bytes\n"
           "Seeded from-scratch decoder; steps=%u, interval=%u, "
           "evaluation samples=%u\n",
           manifestPath, datasetSummary.trainingDocuments,
           datasetSummary.trainingBytes, datasetSummary.evaluationDocuments,
           datasetSummary.evaluationBytes, (unsigned)config.steps,
           (unsigned)config.evalInterval, (unsigned)config.evalBatches);
    for (size_t index = 0U; index < datasetSummary.documentCount; ++index) {
        UmiAiTrainingDocumentInfo item;
        status = UmiAiTrainingDatasetDocument(dataset, index, &item);
        if (status != UMI_STATUS_OK) goto error;
        printf("  %-5s %-34s bytes=%zu fnv64=%016" PRIx64 "\n",
               item.partition == UMI_AI_TRAINING_PARTITION_TRAIN ?
                   "train" : "eval", item.relativePath,
               item.byteCount, item.fingerprint);
    }
    if (argc == 4) {
        status = UmiAiTrainingDecoderCheckpointLoadFile(argv[3],
                                                       &startingCheckpoint);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "resume checkpoint rejected (status=%d)\n", (int)status);
            goto error;
        }
        puts("Resume requires the SAME reviewed corpus and training settings;");
        puts("the checkpoint format does not bind the original dataset hash.");
        status = UmiAiTrainingExperimentResume(dataset, &config,
                    startingCheckpoint, ShowProgress, NULL, &model, &report);
    } else {
        status = UmiAiTrainingExperimentRun(dataset, &config, ShowProgress,
                                            NULL, &model, &report);
    }
    if (status != UMI_STATUS_OK) goto error;
    printf("Completed %" PRIu64 " steps, %u evaluations\n"
           "Held-out CE: %.6f -> %.6f; best observed %.6f at step %" PRIu64 "\n",
           report.completedSteps, report.evaluationCount,
           report.baselineHeldOutLoss, report.finalHeldOutLoss,
           report.bestObservedHeldOutLoss, report.bestObservedStep);
    puts("These are sampled held-out measurements, not general coding"
         " proficiency. No evaluation document was used for training.");
    if (argc >= 3) {
        status = UmiAiTrainingDecoderCheckpointSaveNewFile(model, argv[2]);
        if (status != UMI_STATUS_OK) {
            fprintf(stderr, "refused to save checkpoint (status=%d)\n",
                    (int)status);
            goto error;
        }
        printf("New checkpoint saved: %s\n", argv[2]);
    }
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDecoderDestroy(startingCheckpoint);
    UmiAiTrainingDatasetDestroy(dataset);
    return 0;
error:
    fprintf(stderr, "experiment failed (status=%d)\n", (int)status);
    UmiAiTrainingDecoderDestroy(model);
    UmiAiTrainingDecoderDestroy(startingCheckpoint);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
