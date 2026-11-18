/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_summary.c
 *
 * PURPOSE:
 *   Compose strict JSONL admission with bounded, cleared source-group statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_summary.h"
#include "umicom/security/secrets.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct SourceGroup {
    char id[UMI_AI_TRAINING_PAIR_SOURCE_BYTES + 1U];
} SourceGroup;
typedef struct SummaryContext {
    UmiAiTrainingJsonlSummary summary;
    SourceGroup *groups;
    size_t capacity;
} SummaryContext;

static UmiStatus SummarizePair(void *opaque,
    const UmiAiTrainingPair *pair, size_t line)
{
    SummaryContext *context = opaque;
    UmiAiTrainingJsonlSummary *summary = &context->summary;
    if (summary->rows >= context->capacity || line != summary->rows + 1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* PairRead already bounded and terminated these owned fields. Use the
     * validated representation, never rescan raw JSON or infer token counts. */
    size_t prompt = strlen(pair->prompt), completion = strlen(pair->completion);
    if (summary->promptBytes > SIZE_MAX - prompt ||
        summary->completionBytes > SIZE_MAX - completion)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(context->groups[summary->rows].id, pair->sourceId);
    ++summary->rows;
    if (prompt == 0U) ++summary->emptyPrompts;
    summary->promptBytes += prompt;
    summary->completionBytes += completion;
    if (prompt > summary->longestPromptBytes) summary->longestPromptBytes = prompt;
    if (completion > summary->longestCompletionBytes) summary->longestCompletionBytes = completion;
    return UMI_STATUS_OK;
}
static int CompareSourceGroup(const void *left, const void *right)
{
    const SourceGroup *a = left, *b = right;
    return strcmp(a->id, b->id);
}
UmiStatus UmiAiTrainingJsonlSummarize(const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiAiTrainingJsonlSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = (UmiAiTrainingJsonlReport){0};
    if (bytes == NULL || length == 0U || outSummary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_AI_TRAINING_JSONL_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    /* Every accepted row consumes at least one input byte. This conservative
     * bound keeps even tiny malformed inputs from allocating the full row cap.
     * Exact names avoid a hash collision silently merging unrelated sources. */
    size_t capacity = length < UMI_AI_TRAINING_JSONL_ROWS ? length : UMI_AI_TRAINING_JSONL_ROWS;
    if (capacity > SIZE_MAX / sizeof(SourceGroup)) return UMI_STATUS_CAPACITY_EXCEEDED;
    SourceGroup *groups = malloc(capacity * sizeof(*groups));
    if (groups == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    SummaryContext context = {.groups = groups, .capacity = capacity};
    UmiStatus status = UmiAiTrainingJsonlVisit(bytes, length, cancel,
        SummarizePair, &context, outReport);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) {
        qsort(groups, context.summary.rows, sizeof(*groups), CompareSourceGroup);
        size_t run = 0U;
        for (size_t i = 0U; i < context.summary.rows; ++i) {
            if (i == 0U || strcmp(groups[i - 1U].id, groups[i].id) != 0) {
                ++context.summary.sourceGroups;
                run = 0U;
            }
            ++run;
            if (run > context.summary.largestSourceGroupRows)
                context.summary.largestSourceGroupRows = run;
        }
        if (umi_cancellation_token_is_requested(cancel)) status = UMI_STATUS_CANCELLED;
    }
    if (status == UMI_STATUS_OK) *outSummary = context.summary;
    /* The temporary group inventory is private to this call. Keep neither
     * source identifiers nor borrowed parser fields after it returns. */
    umi_secret_clear(groups, capacity * sizeof(*groups));
    free(groups);
    return status;
}
