/*-----------------------------------------------------------------------------
 * Umicom Framework | tests/ai_training/test_training_dataset_manifest.c
 * Purpose: Qualify immutable source inventory, file-level splits and seeded,
 *          shape-checked next-token minibatches without running a model.
 * Author: Sammy Hegab | Umicom Foundation | MIT
 *---------------------------------------------------------------------------
 * File: tests/ai_training/test_training_dataset_manifest.c
 * LICENCE: MIT
 */
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/initialisation.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error CMake must supply the absolute authorised fixture manifest path.
#endif
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; \
} } while (0)

/* This fixture-only check independently inspects actual bytes and proves
 * selected windows come from the declared files, not the other split. */
static int FixtureContainsWindow(const char *relativePath,
                                 const size_t *inputs,
                                 const size_t *targets, size_t context)
{
    char filename[4096];
    uint8_t contents[2048];
    const char *manifest = UMI_AI_TEST_MANIFEST_PATH;
    const char *slash = strrchr(manifest, '/');
    const char *backslash = strrchr(manifest, '\\');
    FILE *file;
    size_t count;
    int matched = 0;
    if (backslash != NULL && (slash == NULL || backslash > slash)) slash = backslash;
    if (slash == NULL) return 0;
    int needed = snprintf(filename, sizeof(filename), "%.*s%s",
                          (int)(slash - manifest + 1), manifest, relativePath);
    if (needed < 0 || (size_t)needed >= sizeof(filename)) return 0;
    file = fopen(filename, "rb");
    if (file == NULL) return 0;
    count = fread(contents, 1U, sizeof(contents), file);
    if (ferror(file) != 0 || fclose(file) != 0 || count <= context) return 0;
    for (size_t start = 0U; start + context < count; ++start) {
        size_t t;
        for (t = 0U; t < context; ++t) {
            if (inputs[t] != (size_t)contents[start + t] + 4U ||
                targets[t] != (size_t)contents[start + t + 1U] + 4U) break;
        }
        if (t == context) { matched = 1; break; }
    }
    return matched;
}

int main(void)
{
    UmiAiTrainingDataset *dataset = NULL;
    UmiAiTrainingDatasetSummary summary = {0};
    UmiAiTrainingDocumentInfo d0, d1, d2;
    size_t inputs[16], targets[16], equalInputs[16], equalTargets[16];
    size_t unchanged[16];
    uint64_t index = 77U;
    UmiAiTrainingRandomState randomA = {0}, randomB = {0};
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
                                          &dataset) == UMI_STATUS_OK);
    CHECK(dataset != NULL);
    CHECK(UmiAiTrainingDatasetInspect(dataset, &summary) == UMI_STATUS_OK);
    CHECK(summary.documentCount == 3U);
    CHECK(summary.trainingDocuments == 2U && summary.evaluationDocuments == 1U);
    CHECK(summary.trainingBytes > 200U && summary.evaluationBytes > 100U);
    CHECK(UmiAiTrainingDatasetDocument(dataset, 0U, &d0) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetDocument(dataset, 1U, &d1) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetDocument(dataset, 2U, &d2) == UMI_STATUS_OK);
    CHECK(d0.partition == UMI_AI_TRAINING_PARTITION_TRAIN);
    CHECK(d1.partition == UMI_AI_TRAINING_PARTITION_TRAIN);
    CHECK(d2.partition == UMI_AI_TRAINING_PARTITION_EVALUATE);
    CHECK(strcmp(d2.relativePath, "eval/payment.c") == 0);
    CHECK(d0.fingerprint != d1.fingerprint && d1.fingerprint != d2.fingerprint);
    CHECK(UmiAiTrainingDatasetBatch(dataset,
        UMI_AI_TRAINING_PARTITION_TRAIN, 8U, 2U, UINT64_C(55), 4U,
        inputs, targets, 16U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetBatch(dataset,
        UMI_AI_TRAINING_PARTITION_TRAIN, 8U, 2U, UINT64_C(55), 4U,
        equalInputs, equalTargets, 16U) == UMI_STATUS_OK);
    CHECK(memcmp(inputs, equalInputs, sizeof(inputs)) == 0);
    CHECK(memcmp(targets, equalTargets, sizeof(targets)) == 0);
    for (size_t sequence = 0U; sequence < 2U; ++sequence) {
        size_t offset = sequence * 8U;
        CHECK(FixtureContainsWindow("train/account.c", inputs + offset,
                 targets + offset, 8U) ||
              FixtureContainsWindow("train/notes.c", inputs + offset,
                 targets + offset, 8U));
    }
    for (size_t i = 0U; i < 16U; ++i) {
        CHECK(inputs[i] >= 4U && inputs[i] < 260U);
        CHECK(targets[i] >= 4U && targets[i] < 260U);
    }
    memset(unchanged, 0xA5, sizeof(unchanged));
    memcpy(inputs, unchanged, sizeof(inputs));
    CHECK(UmiAiTrainingDatasetBatch(dataset,
        UMI_AI_TRAINING_PARTITION_TRAIN, 8U, 2U, 55U, 4U,
        inputs, targets, 15U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(inputs, unchanged, sizeof(inputs)) == 0);
    CHECK(UmiAiTrainingDatasetBatch(dataset,
        UMI_AI_TRAINING_PARTITION_TRAIN, 8U, 2U, 55U, 4U,
        inputs, inputs, 16U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDatasetBatch(dataset,
        UMI_AI_TRAINING_PARTITION_EVALUATE, 8U, 2U, 55U, 4U,
        inputs, targets, 16U) == UMI_STATUS_OK);
    for (size_t sequence = 0U; sequence < 2U; ++sequence) {
        size_t offset = sequence * 8U;
        CHECK(FixtureContainsWindow("eval/payment.c", inputs + offset,
                                    targets + offset, 8U));
    }
    CHECK(UmiAiTrainingRandomSeed(&randomA, 123U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRandomSeed(&randomB, 123U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingRandomIndex(&randomA, 0U, &index) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(index == 77U && randomA.state == randomB.state);
    for (size_t i = 0U; i < 64U; ++i) {
        uint64_t left = 0U, right = 0U;
        CHECK(UmiAiTrainingRandomIndex(&randomA, 3U, &left) == UMI_STATUS_OK);
        CHECK(UmiAiTrainingRandomIndex(&randomB, 3U, &right) == UMI_STATUS_OK);
        CHECK(left == right && left < 3U);
    }
    UmiAiTrainingDatasetDestroy(dataset);
    puts("PASS: fixed file splits, independent samples, provenance and RNG");
    return 0;
}
