/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_dataset_manifest_rejections.c
 * Purpose: Deny bad/duplicate manifests without partial output publication.
 * Author: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_dataset_manifest_rejections.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/dataset_manifest.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

static int WriteFile(const char *filename, const char *contents)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL) return 0;
    size_t length = strlen(contents);
    int successful = fwrite(contents, 1U, length, file) == length;
    if (fclose(file) != 0) successful = 0;
    return successful;
}

int main(void)
{
    const char *manifest = "umicom-batch06-reject-manifest.tmp";
    const char *a = "umicom-batch06-reject-a.tmp";
    const char *b = "umicom-batch06-reject-b.tmp";
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDataset *oldValue = (UmiAiTrainingDataset *)0x1; /* sentinel: never dereferenced */
    CHECK(WriteFile(a, "valid training alpha contents"));
    CHECK(WriteFile(b, "distinct evaluation beta contents"));
    CHECK(WriteFile(manifest,
        "UMICOM-AI-DATASET-1\ntrain ../outside.c\neval umicom-batch06-reject-b.tmp\n"));
    dataset = oldValue;
    CHECK(UmiAiTrainingDatasetLoadManifest(manifest, &dataset) == UMI_STATUS_PARSE_ERROR);
    CHECK(dataset == oldValue);
    CHECK(WriteFile(manifest,
        "UMICOM-AI-DATASET-1\ntrain umicom-batch06-reject-a.tmp\neval umicom-batch06-reject-a.tmp\n"));
    CHECK(UmiAiTrainingDatasetLoadManifest(manifest, &dataset) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(dataset == oldValue);
    CHECK(WriteFile(manifest,
        "UMICOM-AI-DATASET-1\ntrain umicom-batch06-reject-a.tmp\n"));
    CHECK(UmiAiTrainingDatasetLoadManifest(manifest, &dataset) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(dataset == oldValue);
    CHECK(WriteFile(manifest,
        "WRONG-HEADER\ntrain umicom-batch06-reject-a.tmp\neval umicom-batch06-reject-b.tmp\n"));
    CHECK(UmiAiTrainingDatasetLoadManifest(manifest, &dataset) == UMI_STATUS_PARSE_ERROR);
    CHECK(dataset == oldValue);
    CHECK(WriteFile(b, "valid training alpha contents"));
    CHECK(WriteFile(manifest,
        "UMICOM-AI-DATASET-1\ntrain umicom-batch06-reject-a.tmp\neval umicom-batch06-reject-b.tmp\n"));
    CHECK(UmiAiTrainingDatasetLoadManifest(manifest, &dataset) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(dataset == oldValue);
    CHECK(remove(manifest) == 0);
    CHECK(remove(a) == 0);
    CHECK(remove(b) == 0);
    puts("PASS: manifest syntax, traversal, duplicates and atomic rejection");
    return 0;
}
