/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_training_dataset_source_sha256.c
 *
 * PURPOSE:
 *   Compare cryptographic identities of immutable, ordered dataset snapshots
 *   and ensure neither existing metadata nor the sampler is mutated.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifndef UMI_AI_TEST_MANIFEST_PATH
#error CMake must supply UMI_AI_TEST_MANIFEST_PATH
#endif
#ifndef UMI_AI_TEST_REORDERED_MANIFEST_PATH
#error CMake must supply UMI_AI_TEST_REORDERED_MANIFEST_PATH
#endif
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    return 1; } } while (0)

/* A disposable, same-process fixture establishes a property impossible to
 * demonstrate with only static repository files: the digest belongs to the
 * imported snapshot, not a subsequently modified source file on disk. */
static int WriteText(const char *path, const char *text)
{
    FILE *file = fopen(path, "wb");
    size_t length;
    int closed;
    if (file == NULL) return 0;
    length = strlen(text);
    if (fwrite(text, 1U, length, file) != length) {
        (void)fclose(file);
        return 0;
    }
    closed = fclose(file);
    return closed == 0;
}

static int DocumentMatches(const UmiAiTrainingDataset *dataset, size_t index, const char *expected)
{
    /* Independent known vectors include the exact terminal LF written above. */
    unsigned char digest[UMI_AI_DATASET_DOCUMENT_SHA256_BYTES];
    char hex[UMI_SHA256_HEX_CAPACITY];
    CHECK(UmiAiTrainingDatasetDocumentSha256(dataset, index, digest) == UMI_STATUS_OK);
    UmiSha256Hex(digest, hex);
    CHECK(strcmp(hex, expected) == 0);
    return 0;
}

int main(void)
{
#if defined(_WIN32)
    const unsigned long pid = (unsigned long)GetCurrentProcessId();
#else
    const unsigned long pid = (unsigned long)getpid();
#endif
    UmiAiTrainingDataset *beforeDiskChange = NULL, *afterDiskChange = NULL;
    char manifestPath[96], sourceTrain[96], sourceEval[96], manifestText[320];
    unsigned char snapshotBefore[UMI_AI_DATASET_SOURCE_SHA256_BYTES];
    unsigned char snapshotRecheck[UMI_AI_DATASET_SOURCE_SHA256_BYTES];
    unsigned char snapshotAfter[UMI_AI_DATASET_SOURCE_SHA256_BYTES];
    int written;
    UmiAiTrainingDataset *original = NULL, *repeat = NULL, *reordered = NULL;
    UmiAiTrainingDatasetSummary before = {0}, after = {0};
    unsigned char first[UMI_AI_DATASET_SOURCE_SHA256_BYTES];
    unsigned char second[UMI_AI_DATASET_SOURCE_SHA256_BYTES];
    unsigned char changed[UMI_AI_DATASET_SOURCE_SHA256_BYTES];
    size_t tokensA[8] = {0}, tokensB[8] = {0};
    size_t targetA[8] = {0}, targetB[8] = {0};

    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
          &original) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_MANIFEST_PATH,
          &repeat) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetLoadManifest(UMI_AI_TEST_REORDERED_MANIFEST_PATH,
          &reordered) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetInspect(original, &before) == UMI_STATUS_OK);
    CHECK(before.trainingDocuments > 0U && before.evaluationDocuments > 0U);

    CHECK(UmiAiTrainingDatasetSourceSha256(original, first) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetSourceSha256(repeat, second) == UMI_STATUS_OK);
    CHECK(memcmp(first, second, sizeof(first)) == 0);
    CHECK(UmiAiTrainingDatasetSourceSha256(reordered, changed) == UMI_STATUS_OK);
    CHECK(memcmp(first, changed, sizeof(first)) != 0);

    CHECK(UmiAiTrainingDatasetBatch(original, UMI_AI_TRAINING_PARTITION_TRAIN,
          4U, 2U, 777U, 2U, tokensA, targetA, 8U) == UMI_STATUS_OK);
    CHECK(UmiAiTrainingDatasetBatch(original, UMI_AI_TRAINING_PARTITION_TRAIN,
          4U, 2U, 777U, 2U, tokensB, targetB, 8U) == UMI_STATUS_OK);
    CHECK(memcmp(tokensA, tokensB, sizeof(tokensA)) == 0);
    CHECK(memcmp(targetA, targetB, sizeof(targetA)) == 0);
    CHECK(UmiAiTrainingDatasetInspect(original, &after) == UMI_STATUS_OK);
    CHECK(memcmp(&before, &after, sizeof(before)) == 0);
    CHECK(UmiAiTrainingDatasetSourceSha256(original, second) == UMI_STATUS_OK);
    CHECK(memcmp(first, second, sizeof(first)) == 0);

    memset(second, 0xCC, sizeof(second));
    CHECK(UmiAiTrainingDatasetSourceSha256(NULL, second) ==
          UMI_STATUS_INVALID_ARGUMENT);
    for (size_t i = 0U; i < sizeof(second); ++i) CHECK(second[i] == 0xCCU);
    CHECK(UmiAiTrainingDatasetSourceSha256(original, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
    /* Use safe relative leaf filenames that satisfy the manifest grammar.
     * The training and evaluation source files intentionally contain different
     * bytes, and we change the training source without changing its length. */
    written = snprintf(sourceTrain, sizeof(sourceTrain),
                       "umicom-train-%lu.c", pid);
    CHECK(written > 0 && (size_t)written < sizeof(sourceTrain));
    written = snprintf(sourceEval, sizeof(sourceEval),
                       "umicom-eval-%lu.c", pid);
    CHECK(written > 0 && (size_t)written < sizeof(sourceEval));
    written = snprintf(manifestPath, sizeof(manifestPath),
                       "umicom-dataset-%lu.txt", pid);
    CHECK(written > 0 && (size_t)written < sizeof(manifestPath));
    written = snprintf(manifestText, sizeof(manifestText),
                       "UMICOM-AI-DATASET-1\ntrain %s\neval %s\n",
                       sourceTrain, sourceEval);
    CHECK(written > 0 && (size_t)written < sizeof(manifestText));
    CHECK(WriteText(sourceTrain, "int Account(void){return 100;}\n"));
    CHECK(WriteText(sourceEval, "double Audit(void){return 0.5;}\n"));
    CHECK(WriteText(manifestPath, manifestText));
    CHECK(UmiAiTrainingDatasetLoadManifest(manifestPath,
          &beforeDiskChange) == UMI_STATUS_OK);
    static const char originalTrain[] = "b1f498a4f6e721dd858c84c5356f83990dc54c5ca95794481b53b8112c6b6b4d";
    static const char updatedTrain[] = "e7be1726d95b05675eeaa6f2b6b38936e40c52f6c128577966b460ea602de419";
    static const char unchangedEval[] = "ac103045e51ad0c30a31f54a6d4d40f88354a1edab02cd65680e5e85b0394b63";
    CHECK(DocumentMatches(beforeDiskChange, 0U, originalTrain) == 0);
    CHECK(DocumentMatches(beforeDiskChange, 1U, unchangedEval) == 0);
    CHECK(UmiAiTrainingDatasetSourceSha256(beforeDiskChange,
          snapshotBefore) == UMI_STATUS_OK);
    CHECK(WriteText(sourceTrain, "int Account(void){return 999;}\n"));
    CHECK(UmiAiTrainingDatasetSourceSha256(beforeDiskChange,
          snapshotRecheck) == UMI_STATUS_OK);
    CHECK(memcmp(snapshotBefore, snapshotRecheck,
                 sizeof(snapshotBefore)) == 0);
    CHECK(UmiAiTrainingDatasetLoadManifest(manifestPath,
          &afterDiskChange) == UMI_STATUS_OK);
    /* Reusing the old owner reports old bytes. A deliberate new load reports
     * the edited bytes only for the training document that was changed. */
    CHECK(DocumentMatches(beforeDiskChange, 0U, originalTrain) == 0);
    CHECK(DocumentMatches(afterDiskChange, 0U, updatedTrain) == 0);
    CHECK(DocumentMatches(afterDiskChange, 1U, unchangedEval) == 0);
    unsigned char rejected[UMI_AI_DATASET_DOCUMENT_SHA256_BYTES];
    memset(rejected, 0xA5, sizeof rejected);
    CHECK(UmiAiTrainingDatasetDocumentSha256(afterDiskChange, 2U, rejected) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDatasetDocumentSha256(afterDiskChange, SIZE_MAX, rejected) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDatasetDocumentSha256(NULL, 0U, rejected) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiTrainingDatasetDocumentSha256(afterDiskChange, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    for (size_t i = 0U; i < sizeof rejected; ++i) CHECK(rejected[i] == 0xA5U);
    CHECK(UmiAiTrainingDatasetSourceSha256(afterDiskChange,
          snapshotAfter) == UMI_STATUS_OK);
    CHECK(memcmp(snapshotBefore, snapshotAfter,
                 sizeof(snapshotBefore)) != 0);
    UmiAiTrainingDatasetDestroy(afterDiskChange);
    UmiAiTrainingDatasetDestroy(beforeDiskChange);
    CHECK(remove(manifestPath) == 0);
    CHECK(remove(sourceEval) == 0);
    CHECK(remove(sourceTrain) == 0);

    UmiAiTrainingDatasetDestroy(reordered);
    UmiAiTrainingDatasetDestroy(repeat);
    UmiAiTrainingDatasetDestroy(original);
    puts("dataset source SHA-256: ordered snapshot, sampler, failure atomicity PASS");
    puts("document SHA-256: two immutable revisions, unchanged evaluation and refusal PASS");
    return 0;
}
