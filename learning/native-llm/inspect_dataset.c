/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-llm/inspect_dataset.c
 *
 * PURPOSE:
 *   Inspect a reviewed training dataset and reproducible token windows through Framework-owned services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/dataset_manifest.h"
#include <stdio.h>

/* Keep the learning tool a consumer: parsing, split validation, snapshots,
 * hashing and sampling are all provided by the same Framework service used
 * by training. The tool never uploads a document or writes a checkpoint. */
static UmiStatus ShowWindow(const UmiAiTrainingDataset *dataset,
                           UmiAiTrainingPartition partition)
{
    size_t inputs[8], targets[8];
    UmiStatus status = UmiAiTrainingDatasetBatch(dataset, partition, 8U, 1U,
        UINT64_C(4477), UINT64_C(0), inputs, targets, 8U);
    if (status != UMI_STATUS_OK) return status;
    printf("%s input tokens:", partition == UMI_AI_TRAINING_PARTITION_TRAIN ?
           "Training" : "Evaluation");
    for (size_t index = 0U; index < 8U; ++index) printf(" %zu", inputs[index]);
    putchar('\n');
    printf("Next-byte targets:");
    for (size_t index = 0U; index < 8U; ++index) printf(" %zu", targets[index]);
    putchar('\n');
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDatasetSummary summary;
    unsigned char digest[UMI_AI_DATASET_SOURCE_SHA256_BYTES];
    UmiStatus status;
    if (argc != 2) {
        fputs("usage: umicom-native-llm-dataset <reviewed-local-manifest>\n", stderr);
        return 2;
    }
    status = UmiAiTrainingDatasetLoadManifest(argv[1], &dataset);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDatasetInspect(dataset, &summary);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingDatasetSourceSha256(dataset, digest);
    if (status != UMI_STATUS_OK) goto failed;
    printf("Documents: %zu; training: %zu; evaluation: %zu\n",
           summary.documentCount, summary.trainingDocuments,
           summary.evaluationDocuments);
    printf("Training bytes: %zu; evaluation bytes: %zu\n",
           summary.trainingBytes, summary.evaluationBytes);
    printf("Dataset SHA-256: ");
    for (size_t index = 0U; index < sizeof digest; ++index)
        printf("%02x", (unsigned int)digest[index]);
    putchar('\n');
    for (size_t index = 0U; index < summary.documentCount; ++index) {
        UmiAiTrainingDocumentInfo document;
        status = UmiAiTrainingDatasetDocument(dataset, index, &document);
        if (status != UMI_STATUS_OK) goto failed;
        printf("%s %s (%zu bytes)\n",
               document.partition == UMI_AI_TRAINING_PARTITION_TRAIN ?
                   "train" : "eval", document.relativePath, document.byteCount);
    }
    status = ShowWindow(dataset, UMI_AI_TRAINING_PARTITION_TRAIN);
    if (status != UMI_STATUS_OK) goto failed;
    status = ShowWindow(dataset, UMI_AI_TRAINING_PARTITION_EVALUATE);
    if (status != UMI_STATUS_OK) goto failed;
    UmiAiTrainingDatasetDestroy(dataset);
    if (ferror(stdout)) return 1;
    return 0;
failed:
    fprintf(stderr, "Dataset inspection failed (status=%d). Check the manifest and local documents.\n",
            (int)status);
    UmiAiTrainingDatasetDestroy(dataset);
    return 1;
}
