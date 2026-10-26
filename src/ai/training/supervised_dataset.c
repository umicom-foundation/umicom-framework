/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/supervised_dataset.c
 * PURPOSE: Audit real approved JSONL bytes, parse the canonical producer's
 *          bounded schema, and copy response-only windows into an immutable
 *          training/evaluation dataset. Reuse the native tokenizer and RNG.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_dataset.h"
#include "umicom/ai/training/initialisation.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiTrainingSupervisedDataset {
    UmiAiTrainingSupervisedDatasetInfo info;
    UmiAiTrainingSupervisedExample *items;
};

static void DatasetClear(void *buffer, size_t bytes)
{
    volatile unsigned char *cursor = buffer;
    while (bytes-- != 0U) *cursor++ = 0U;
}

static int DatasetHex(unsigned char value)
{
    if (value >= '0' && value <= '9') return (int)(value - '0');
    if (value >= 'a' && value <= 'f') return (int)(value - 'a') + 10;
    return -1; /* ReviewedExampleJsonl emits lowercase canonical hex only. */
}

/* Parse one of the TWO fields emitted by ReviewedExampleJsonl. No generic
 * JSON parser is claimed: other property orderings, comments, alternate
 * escapes, missing closing quotes and additional records are refused.
 * Individual decoded text buffers are temporary and never retained. */
static UmiStatus DatasetReadQuoted(
    const unsigned char *line, size_t length, size_t *cursor,
    uint8_t *output, size_t outputCapacity, size_t *outCount)
{
    size_t count = 0U, at;
    if (*cursor >= length || line[*cursor] != '"')
        return UMI_STATUS_PARSE_ERROR;
    at = *cursor + 1U;
    for (; at < length; ++at) {
        unsigned char current = line[at];
        if (current == '"') {
            *cursor = at + 1U;
            *outCount = count;
            return count == 0U ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
        }
        if (current == '\\') {
            if (at + 1U >= length) return UMI_STATUS_PARSE_ERROR;
            current = line[++at];
            if (current == '"' || current == '\\') {
                /* Same literal bytes as the approved reviewed-example producer. */
            } else if (current == 'n') current = '\n';
            else if (current == 'r') current = '\r';
            else if (current == 't') current = '\t';
            else if (current == 'u') {
                if (length - at <= 4U || line[at + 1U] != '0' ||
                    line[at + 2U] != '0') return UMI_STATUS_PARSE_ERROR;
                int first = DatasetHex(line[at + 3U]);
                int second = DatasetHex(line[at + 4U]);
                if (first < 0 || second < 0) return UMI_STATUS_PARSE_ERROR;
                unsigned decoded = (unsigned)((first << 4) | second);
                /* The producer writes \u00xx only for controls not handled
                 * by short escapes; it never escapes non-ASCII UTF-8 here. */
                if (decoded >= 0x20U || decoded == 0U ||
                    decoded == (unsigned)'\n' || decoded == (unsigned)'\r' ||
                    decoded == (unsigned)'\t') return UMI_STATUS_PARSE_ERROR;
                current = (unsigned char)decoded;
                at += 4U;
            } else return UMI_STATUS_PARSE_ERROR;
        } else if (current < 0x20U) return UMI_STATUS_PARSE_ERROR;
        if (count == outputCapacity) return UMI_STATUS_CAPACITY_EXCEEDED;
        output[count++] = current;
    }
    return UMI_STATUS_PARSE_ERROR;
}

static UmiStatus DatasetDecodeRecord(
    const char *record, size_t length,
    uint8_t *prompt, size_t *promptCount,
    uint8_t *completion, size_t *completionCount)
{
    static const char start[] = "{\"prompt\":";
    static const char middle[] = ",\"completion\":";
    size_t at = sizeof(start) - 1U;
    UmiStatus status;
    if (record == NULL || length < sizeof(start) + sizeof(middle) + 5U ||
        memcmp(record, start, sizeof(start) - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    const unsigned char *line = (const unsigned char *)record;
    status = DatasetReadQuoted(line, length, &at, prompt,
                               UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES, promptCount);
    if (status != UMI_STATUS_OK) return status;
    if (length - at < sizeof(middle) - 1U ||
        memcmp(record + at, middle, sizeof(middle) - 1U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    at += sizeof(middle) - 1U;
    status = DatasetReadQuoted(line, length, &at, completion,
                               UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES,
                               completionCount);
    if (status != UMI_STATUS_OK) return status;
    return length - at == 2U && line[at] == '}' && line[at + 1U] == '\n'
           ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
}

void UmiAiTrainingSupervisedDatasetDestroy(UmiAiTrainingSupervisedDataset *dataset)
{
    if (dataset == NULL) return;
    if (dataset->items != NULL) {
        DatasetClear(dataset->items, dataset->info.exampleCount * sizeof(*dataset->items));
        free(dataset->items);
    }
    DatasetClear(dataset, sizeof(*dataset));
    free(dataset);
}

UmiStatus UmiAiTrainingSupervisedDatasetCreate(
    const char *const *jsonlRecords, const size_t *jsonlLengths,
    const UmiAiTrainingReviewedExampleEvidence *evidence,
    size_t exampleCount, size_t contextLength,
    UmiAiTrainingSupervisedDataset **outDataset)
{
    UmiAiTrainingReviewedCorpusReport audit = {0};
    UmiAiTrainingSupervisedDataset *candidate = NULL;
    uint8_t *prompt = NULL, *completion = NULL;
    UmiStatus status;
    if (jsonlRecords == NULL || jsonlLengths == NULL || evidence == NULL ||
        outDataset == NULL || *outDataset != NULL ||
        exampleCount < 2U || exampleCount > UMI_AI_SUPERVISED_DATASET_MAX_EXAMPLES ||
        contextLength == 0U || contextLength > UMI_AI_SUPERVISED_DATASET_MAX_CONTEXT)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Audit the WHOLE collection first. A locally well-formed record alone
     * cannot establish source-level train/evaluation isolation. */
    status = UmiAiTrainingReviewedCorpusAudit(jsonlRecords, jsonlLengths,
                                               evidence, exampleCount, &audit);
    if (status != UMI_STATUS_OK) return status;
    candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->items = calloc(exampleCount, sizeof(*candidate->items));
    prompt = malloc(UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES);
    completion = malloc(UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES);
    if (candidate->items == NULL || prompt == NULL || completion == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto fail;
    }
    candidate->info.exampleCount = exampleCount;
    candidate->info.contextLength = contextLength;
    candidate->info.trainingExamples = audit.trainingExamples;
    candidate->info.evaluationExamples = audit.evaluationExamples;
    candidate->info.distinctSources = audit.distinctOriginalSources;
    memcpy(candidate->info.orderedCorpusSha256, audit.orderedDigest,
           sizeof(candidate->info.orderedCorpusSha256));

    for (size_t i = 0U; i < exampleCount; ++i) {
        size_t promptCount = 0U, completionCount = 0U;
        UmiAiTrainingSupervisedWindowInfo windowInfo = {0};
        UmiAiTrainingSupervisedExample *example = &candidate->items[i];
        status = DatasetDecodeRecord(jsonlRecords[i], jsonlLengths[i],
                                     prompt, &promptCount,
                                     completion, &completionCount);
        if (status != UMI_STATUS_OK) goto fail;
        status = UmiAiTrainingSupervisedByteWindowBuild(
            prompt, promptCount, completion, completionCount, contextLength,
            example->inputIds, contextLength,
            example->targetIds, contextLength,
            example->lossMask, contextLength, &windowInfo);
        if (status != UMI_STATUS_OK) goto fail;
        example->partition = evidence[i].admittedSource.partition;
        example->supervisedTokens = windowInfo.supervisedTargets;
        example->promptBytes = windowInfo.promptBytes;
        example->completionBytes = windowInfo.completionBytes;
        if (example->partition == UMI_AI_TRAINING_PARTITION_TRAIN) {
            if (candidate->info.trainingTargets > SIZE_MAX - example->supervisedTokens) {
                status = UMI_STATUS_CAPACITY_EXCEEDED; goto fail;
            }
            candidate->info.trainingTargets += example->supervisedTokens;
        } else {
            if (candidate->info.evaluationTargets > SIZE_MAX - example->supervisedTokens) {
                status = UMI_STATUS_CAPACITY_EXCEEDED; goto fail;
            }
            candidate->info.evaluationTargets += example->supervisedTokens;
        }
        /* Decoded text is no longer needed, even by the immutable dataset. */
        DatasetClear(prompt, UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES);
        DatasetClear(completion, UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES);
    }
    free(prompt);
    free(completion);
    *outDataset = candidate;
    return UMI_STATUS_OK;
fail:
    if (prompt != NULL) {
        DatasetClear(prompt, UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES);
        free(prompt);
    }
    if (completion != NULL) {
        DatasetClear(completion, UMI_AI_REVIEWED_EXAMPLE_MAX_SPAN_BYTES);
        free(completion);
    }
    UmiAiTrainingSupervisedDatasetDestroy(candidate);
    return status;
}

UmiStatus UmiAiTrainingSupervisedDatasetInspect(
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingSupervisedDatasetInfo *outInfo)
{
    if (dataset == NULL || outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outInfo = dataset->info;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSupervisedDatasetGet(
    const UmiAiTrainingSupervisedDataset *dataset, size_t index,
    UmiAiTrainingSupervisedExample *outExample)
{
    if (dataset == NULL || outExample == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= dataset->info.exampleCount)
        return UMI_STATUS_NOT_FOUND;
    *outExample = dataset->items[index];
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSupervisedDatasetOrder(
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingPartition partition, uint64_t seed, uint64_t epoch,
    size_t *outIndices, size_t capacity, size_t *outCount)
{
    size_t candidate[UMI_AI_SUPERVISED_DATASET_MAX_EXAMPLES] = {0};
    size_t count = 0U;
    UmiAiTrainingRandomState random = {0};
    UmiStatus status;
    if (dataset == NULL || outIndices == NULL || outCount == NULL ||
        (partition != UMI_AI_TRAINING_PARTITION_TRAIN &&
         partition != UMI_AI_TRAINING_PARTITION_EVALUATE))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < dataset->info.exampleCount; ++i)
        if (dataset->items[i].partition == partition)
            candidate[count++] = i;
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Unbiased Fisher-Yates uses the existing Framework integer RNG.
     * A local RNG never changes another experiment's random state. */
    status = UmiAiTrainingRandomSeed(&random,
        seed ^ (epoch * UINT64_C(0x9e3779b97f4a7c15)) ^
        (partition == UMI_AI_TRAINING_PARTITION_TRAIN
            ? UINT64_C(0x8cb92baa91df3fa5)
            : UINT64_C(0x15303d997c227a11)));
    if (status != UMI_STATUS_OK) return status;
    for (size_t remaining = count; remaining > 1U; --remaining) {
        uint64_t picked = 0U;
        status = UmiAiTrainingRandomIndex(&random, (uint64_t)remaining, &picked);
        if (status != UMI_STATUS_OK) return status;
        size_t temporary = candidate[remaining - 1U];
        candidate[remaining - 1U] = candidate[(size_t)picked];
        candidate[(size_t)picked] = temporary;
    }
    memcpy(outIndices, candidate, count * sizeof(*candidate));
    *outCount = count;
    return UMI_STATUS_OK;
}
