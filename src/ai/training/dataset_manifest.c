/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/dataset_manifest.c
 *
 * PURPOSE:
 *   Implement file-level dataset boundaries, immutable source snapshots,
 *   recorded provenance and deterministic model-ready next-token batches.
 *
 * ENGINEERING NOTES:
 *   The manifest must be selected/reviewed by its caller. Relative-path
 *   lexical checks prevent obvious traversal strings, but symlinks/junctions
 *   and file replacement are NOT confined by this component. It is not a
 *   sandbox or a loader for arbitrary untrusted manifests. No source is
 *   modified, uploaded or executed. The canonical Corpus and training PRNG
 *   remain the only data/import and sampling implementations.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab / Umicom Foundation
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/corpus.h"
#include "umicom/ai/training/initialisation.h"

#include <stdbool.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_DATASET_MAX_RESOLVED_PATH 4096U
#define UMI_DATASET_MAX_LINE 255U

struct UmiAiTrainingDataset {
    UmiAiCorpus *corpora[UMI_AI_DATASET_MAX_FILES];
    UmiAiTrainingDocumentInfo documents[UMI_AI_DATASET_MAX_FILES];
    UmiAiTrainingDatasetSummary summary;
};


/* The disk path is for a TRUSTED local workspace, not a containment root. */
static UmiStatus JoinManifestDirectory(const char *manifestPath,
                                     const char *relativePath,
                                     char *result, size_t capacity)
{
    const char *slash, *backslash, *last;
    size_t prefixLength, relativeLength;
    if (manifestPath == NULL || relativePath == NULL || result == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    slash = strrchr(manifestPath, '/');
    backslash = strrchr(manifestPath, '\\');
    last = slash;
    if (backslash != NULL && (last == NULL || backslash > last)) last = backslash;
    prefixLength = last == NULL ? 0U : (size_t)(last - manifestPath) + 1U;
    relativeLength = strlen(relativePath);
    if (prefixLength > capacity - 1U ||
        relativeLength > capacity - 1U - prefixLength)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (prefixLength != 0U) memcpy(result, manifestPath, prefixLength);
    memcpy(result + prefixLength, relativePath, relativeLength + 1U);
    return UMI_STATUS_OK;
}

static bool ValidPath(const char *path)
{
    size_t componentLength = 0U;
    size_t size = strlen(path);
    if (size == 0U || size >= UMI_AI_DATASET_MAX_PATH ||
        path[0] == '/' ||
        path[size - 1U] == '/') return false;
    for (size_t i = 0U; i <= size; ++i) {
        unsigned char c = (unsigned char)path[i];
        if (c == '/' || c == '\0') {
            const char *start = path + i - componentLength;
            if (componentLength == 0U ||
                (componentLength == 1U && start[0] == '.') ||
                (componentLength == 2U && start[0] == '.' && start[1] == '.'))
                return false;
            componentLength = 0U;
        } else {
            if (!((c >= 'A' && c <= 'Z') ||
                  (c >= 'a' && c <= 'z') ||
                  (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.'))
                return false;
            ++componentLength;
        }
    }
    return true;
}

static bool SamePath(const char *a, const char *b)
{
    for (;;) {
        unsigned char left = (unsigned char)*a++;
        unsigned char right = (unsigned char)*b++;
        if (tolower(left) != tolower(right)) return false;
        if (left == 0U) return true;
    }
}

void UmiAiTrainingDatasetDestroy(UmiAiTrainingDataset *dataset)
{
    if (dataset == NULL) return;
    for (size_t i = 0U; i < UMI_AI_DATASET_MAX_FILES; ++i)
        UmiAiCorpusDestroy(dataset->corpora[i]);
    free(dataset);
}

static UmiStatus RegisterDocument(UmiAiTrainingDataset *dataset,
                                  const char *manifestPath,
                                  UmiAiTrainingPartition partition,
                                  const char *relativePath)
{
    UmiAiCorpus *newCorpus = NULL;
    char resolved[UMI_DATASET_MAX_RESOLVED_PATH];
    UmiAiTrainingDocumentInfo info = {0};
    size_t index = dataset->summary.documentCount;
    UmiStatus status;
    if (index >= UMI_AI_DATASET_MAX_FILES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (!ValidPath(relativePath)) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < index; ++i) {
        if (SamePath(dataset->documents[i].relativePath, relativePath))
            return UMI_STATUS_ALREADY_EXISTS;
    }
    status = JoinManifestDirectory(manifestPath, relativePath,
                                   resolved, sizeof(resolved));
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiCorpusLoadFile(resolved, &newCorpus);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiCorpusLength(newCorpus, &info.byteCount);
    if (status == UMI_STATUS_OK)
        status = UmiAiCorpusFingerprint(newCorpus, &info.fingerprint);
    if (status != UMI_STATUS_OK) {
        UmiAiCorpusDestroy(newCorpus); return status;
    }
    /* A matching byte count and fingerprint is sufficient to reject an
     * obvious repeated file. This hash is not collision-resistant. */
    for (size_t i = 0U; i < index; ++i) {
        if (dataset->documents[i].byteCount == info.byteCount &&
            dataset->documents[i].fingerprint == info.fingerprint) {
            UmiAiCorpusDestroy(newCorpus);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    info.partition = partition;
    memcpy(info.relativePath, relativePath, strlen(relativePath) + 1U);
    dataset->corpora[index] = newCorpus;
    dataset->documents[index] = info;
    dataset->summary.documentCount = index + 1U;
    if (partition == UMI_AI_TRAINING_PARTITION_TRAIN) {
        dataset->summary.trainingDocuments++;
        dataset->summary.trainingBytes += info.byteCount;
    } else {
        dataset->summary.evaluationDocuments++;
        dataset->summary.evaluationBytes += info.byteCount;
    }
    return UMI_STATUS_OK;
}

/* Parse into the dataset in one transaction: only the pointer to the fully
 * validated dataset is returned to the caller. Only LF/CRLF and ASCII field
 * syntax are supported, so no locale or script interpreter is required. */
static UmiStatus ParseManifest(UmiAiTrainingDataset *dataset,
                              const char *manifestPath,
                              char *contents, size_t byteCount)
{
    size_t lineStart = 0U;
    bool seenHeader = false;
    while (lineStart < byteCount) {
        size_t lineEnd = lineStart;
        size_t lineLength;
        char *line;
        char *value;
        UmiAiTrainingPartition partition;
        while (lineEnd < byteCount && contents[lineEnd] != '\n') ++lineEnd;
        lineLength = lineEnd - lineStart;
        if (lineLength > 0U && contents[lineStart + lineLength - 1U] == '\r')
            --lineLength;
        if (lineLength > UMI_DATASET_MAX_LINE) return UMI_STATUS_CAPACITY_EXCEEDED;
        contents[lineStart + lineLength] = '\0';
        line = contents + lineStart;
        lineStart = lineEnd + (lineEnd < byteCount ? 1U : 0U);
        if (lineLength == 0U || line[0] == '#') continue;
        if (!seenHeader) {
            if (strcmp(line, "UMICOM-AI-DATASET-1") != 0)
                return UMI_STATUS_PARSE_ERROR;
            seenHeader = true;
            continue;
        }
        if (strncmp(line, "train", 5U) == 0 &&
            (line[5] == ' ' || line[5] == '\t')) {
            partition = UMI_AI_TRAINING_PARTITION_TRAIN;
            value = line + 6;
        } else if (strncmp(line, "eval", 4U) == 0 &&
                   (line[4] == ' ' || line[4] == '\t')) {
            partition = UMI_AI_TRAINING_PARTITION_EVALUATE;
            value = line + 5;
        } else return UMI_STATUS_PARSE_ERROR;
        while (*value == ' ' || *value == '\t') ++value;
        if (!ValidPath(value)) return UMI_STATUS_PARSE_ERROR;
        UmiStatus status = RegisterDocument(dataset, manifestPath,
                                            partition, value);
        if (status != UMI_STATUS_OK) return status;
    }
    if (!seenHeader || dataset->summary.trainingDocuments == 0U ||
        dataset->summary.evaluationDocuments == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDatasetLoadManifest(
    const char *manifestPath, UmiAiTrainingDataset **outDataset)
{
    FILE *file;
    char *buffer;
    UmiAiTrainingDataset *created;
    size_t actual;
    UmiStatus status;
    if (manifestPath == NULL || manifestPath[0] == '\0' ||
        outDataset == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    file = fopen(manifestPath, "rb");
    if (file == NULL) return UMI_STATUS_IO_ERROR;
    buffer = malloc(UMI_AI_DATASET_MAX_MANIFEST_BYTES + 2U);
    if (buffer == NULL) { fclose(file); return UMI_STATUS_OUT_OF_MEMORY; }
    actual = fread(buffer, 1U, UMI_AI_DATASET_MAX_MANIFEST_BYTES + 1U, file);
    int readError = ferror(file);
    int closeError = fclose(file);
    if (readError != 0 || closeError != 0) {
        free(buffer); return UMI_STATUS_IO_ERROR;
    }
    if (actual == 0U || actual > UMI_AI_DATASET_MAX_MANIFEST_BYTES) {
        free(buffer);
        return actual == 0U ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* No truncated NUL sequence may masquerade as a valid line. */
    if (memchr(buffer, '\0', actual) != NULL) {
        free(buffer); return UMI_STATUS_PARSE_ERROR;
    }
    buffer[actual] = '\0';
    created = calloc(1U, sizeof(*created));
    if (created == NULL) { free(buffer); return UMI_STATUS_OUT_OF_MEMORY; }
    status = ParseManifest(created, manifestPath, buffer, actual);
    free(buffer);
    if (status != UMI_STATUS_OK) {
        UmiAiTrainingDatasetDestroy(created);
        return status;
    }
    *outDataset = created;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDatasetInspect(
    const UmiAiTrainingDataset *dataset,
    UmiAiTrainingDatasetSummary *outSummary)
{
    if (dataset == NULL || outSummary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outSummary = dataset->summary;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDatasetDocument(
    const UmiAiTrainingDataset *dataset, size_t index,
    UmiAiTrainingDocumentInfo *outInfo)
{
    if (dataset == NULL || outInfo == NULL ||
        index >= dataset->summary.documentCount)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outInfo = dataset->documents[index];
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDatasetBatch(
    const UmiAiTrainingDataset *dataset,
    UmiAiTrainingPartition partition,
    size_t contextLength, size_t sequenceCount,
    uint64_t seed, uint64_t batchNumber,
    size_t *outInputs, size_t *outTargets, size_t capacity)
{
    size_t possible = 0U;
    size_t count;
    size_t localInputs[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    size_t localTargets[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    UmiAiTrainingRandomState random;
    UmiStatus status;
    if (dataset == NULL || outInputs == NULL || outTargets == NULL ||
        outInputs == outTargets ||
        (partition != UMI_AI_TRAINING_PARTITION_TRAIN &&
         partition != UMI_AI_TRAINING_PARTITION_EVALUATE) ||
        contextLength == 0U || contextLength > UMI_AI_DATASET_MAX_CONTEXT ||
        sequenceCount == 0U || sequenceCount > UMI_AI_DATASET_MAX_SEQUENCES)
        return UMI_STATUS_INVALID_ARGUMENT;
    count = sequenceCount * contextLength;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Disjoint buffers are a public contract. Check address intervals on
     * our supported pointer-width platforms before publishing any result. */
    uintptr_t left = (uintptr_t)(void *)outInputs;
    uintptr_t right = (uintptr_t)(void *)outTargets;
    uintptr_t bytes = (uintptr_t)(count * sizeof(size_t));
    if (left > UINTPTR_MAX - bytes || right > UINTPTR_MAX - bytes ||
        (left < right + bytes && right < left + bytes))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < dataset->summary.documentCount; ++i) {
        const UmiAiTrainingDocumentInfo *doc = &dataset->documents[i];
        if (doc->partition == partition && doc->byteCount > contextLength)
            possible += doc->byteCount - contextLength;
    }
    if (possible == 0U) return UMI_STATUS_INVALID_STATE;
    /* Fixed partition salts prohibit accidentally sharing one selection stream
     * between training and evaluation at the same seed and batch number. */
    status = UmiAiTrainingRandomSeed(&random, seed ^
        (batchNumber * UINT64_C(0x9e3779b97f4a7c15)) ^
        (partition == UMI_AI_TRAINING_PARTITION_TRAIN ?
         UINT64_C(0x62d93c89cb71a4e5) : UINT64_C(0xc6bc279692b5c323)));
    if (status != UMI_STATUS_OK) return status;
    for (size_t seq = 0U; seq < sequenceCount; ++seq) {
        uint64_t position = 0U;
        uint32_t sourceIds[UMI_AI_DATASET_MAX_CONTEXT];
        uint32_t targetIds[UMI_AI_DATASET_MAX_CONTEXT];
        const UmiAiCorpus *chosen = NULL;
        size_t start = 0U;
        status = UmiAiTrainingRandomIndex(&random, (uint64_t)possible,
                                          &position);
        if (status != UMI_STATUS_OK) return status;
        for (size_t i = 0U; i < dataset->summary.documentCount; ++i) {
            const UmiAiTrainingDocumentInfo *doc = &dataset->documents[i];
            size_t windows = doc->partition == partition &&
                doc->byteCount > contextLength ?
                doc->byteCount - contextLength : 0U;
            if (position < (uint64_t)windows) {
                chosen = dataset->corpora[i];
                start = (size_t)position;
                break;
            }
            position -= (uint64_t)windows;
        }
        if (chosen == NULL) return UMI_STATUS_INTERNAL_ERROR;
        status = UmiAiCorpusWindow(chosen, start, contextLength,
                                   sourceIds, targetIds, contextLength);
        if (status != UMI_STATUS_OK) return status;
        for (size_t t = 0U; t < contextLength; ++t) {
            localInputs[seq * contextLength + t] = (size_t)sourceIds[t];
            localTargets[seq * contextLength + t] = (size_t)targetIds[t];
        }
    }
    memcpy(outInputs, localInputs, count * sizeof(*outInputs));
    memcpy(outTargets, localTargets, count * sizeof(*outTargets));
    return UMI_STATUS_OK;
}
