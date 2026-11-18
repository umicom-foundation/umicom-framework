/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_partition.c
 *
 * PURPOSE:
 *   Validate and rank exact source names before publishing an atomic dataset partition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_partition.h"
#include "umicom/ai/training/dataset_split.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>

_Static_assert(UMI_AI_TRAINING_JSONL_ROWS <= UMI_AI_TRAINING_MAX_GROUP_ROWS,
    "JSONL partitions must fit the shared group splitter");
typedef struct PartitionSource {
    char id[UMI_AI_TRAINING_PAIR_SOURCE_BYTES + 1U];
    size_t row;
} PartitionSource;
typedef struct PartitionContext {
    PartitionSource *sources;
    size_t rows;
    size_t capacity;
} PartitionContext;
static UmiStatus CollectPartitionSource(void *opaque,
    const UmiAiTrainingPair *pair, size_t line)
{
    PartitionContext *context = opaque;
    if (context->rows >= context->capacity || line != context->rows + 1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    PartitionSource *source = &context->sources[context->rows];
    strcpy(source->id, pair->sourceId); /* PairRead bounded and terminated it. */
    source->row = context->rows++;
    return UMI_STATUS_OK;
}
static int ComparePartitionSource(const void *left, const void *right)
{
    const PartitionSource *a = left, *b = right;
    return strcmp(a->id, b->id);
}
UmiStatus UmiAiTrainingJsonlPartition(const void *bytes, size_t length,
    size_t evaluationGroupCount, uint64_t seed,
    const UmiCancellationToken *cancel, unsigned char *outEvaluation,
    size_t capacity, UmiAiTrainingJsonlPartitionSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = (UmiAiTrainingJsonlReport){0};
    if (bytes == NULL || length == 0U || outEvaluation == NULL ||
        outSummary == NULL || evaluationGroupCount == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_AI_TRAINING_JSONL_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    size_t slots = length < UMI_AI_TRAINING_JSONL_ROWS ? length : UMI_AI_TRAINING_JSONL_ROWS;
    if (slots > SIZE_MAX / sizeof(PartitionSource) || slots > SIZE_MAX / sizeof(uint64_t))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    PartitionSource *sources = malloc(slots * sizeof(*sources));
    uint64_t *ids = malloc(slots * sizeof(*ids));
    unsigned char *candidate = malloc(slots);
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (sources == NULL || ids == NULL || candidate == NULL) goto finished;
    PartitionContext context = {.sources = sources, .capacity = slots};
    status = UmiAiTrainingJsonlVisit(bytes, length, cancel,
        CollectPartitionSource, &context, outReport);
    if (status != UMI_STATUS_OK) goto finished;
    if (capacity < context.rows) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto finished; }
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto finished; }
    qsort(sources, context.rows, sizeof(*sources), ComparePartitionSource);
    size_t groups = 0U;
    for (size_t i = 0U; i < context.rows; ++i) {
        if (i == 0U || strcmp(sources[i-1U].id, sources[i].id) != 0) ++groups;
        ids[sources[i].row] = (uint64_t)(groups - 1U);
    }
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto finished; }
    /* The shared splitter owns the shuffle and admission rules. Ranking exact
     * strings supplies collision-free identities while retaining row order. */
    status = UmiAiTrainingSplitGroups(ids, context.rows, evaluationGroupCount,
        seed, candidate, context.rows);
    if (status != UMI_STATUS_OK) goto finished;
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto finished; }
    UmiAiTrainingJsonlPartitionSummary summary = {.rows = context.rows, .sourceGroups = groups};
    for (size_t i = 0U; i < context.rows; ++i) {
        if (candidate[i] == 1U) ++summary.evaluationRows;
        else ++summary.trainingRows;
    }
    /* Publication is last: parsing, sorting, splitting and cancellation cannot
     * expose a partially assigned corpus to a later training operation. */
    memcpy(outEvaluation, candidate, context.rows);
    *outSummary = summary;
finished:
    if (sources != NULL) umi_secret_clear(sources, slots * sizeof(*sources));
    if (ids != NULL) umi_secret_clear(ids, slots * sizeof(*ids));
    if (candidate != NULL) umi_secret_clear(candidate, slots);
    free(candidate);
    free(ids);
    free(sources);
    return status;
}
