/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_overlap.c
 *
 * PURPOSE:
 *   Measure, own and compare decoded training pairs before atomically publishing overlap observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_overlap.h"
#include "umicom/security/secrets.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct OverlapRow {
    char *prompt;
    char *completion;
    size_t line;
    unsigned char side;
} OverlapRow;
typedef struct OverlapMeasure { size_t rows, textBytes; } OverlapMeasure;
typedef struct OverlapCopy {
    OverlapRow *rows;
    char *text;
    const unsigned char *membership;
    size_t capacity, textCapacity, count, used;
} OverlapCopy;

static UmiStatus MeasurePair(void *opaque, const UmiAiTrainingPair *pair, size_t line)
{
    OverlapMeasure *measure = opaque;
    if (measure->rows >= UMI_AI_TRAINING_JSONL_ROWS || line != measure->rows + 1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t prompt = strlen(pair->prompt) + 1U;
    const size_t completion = strlen(pair->completion) + 1U;
    /* PairRead supplies bounded terminated fields. Subtraction still protects
     * accumulation if those public limits are expanded in a future profile. */
    if (prompt > SIZE_MAX - completion || prompt + completion > SIZE_MAX - measure->textBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    measure->textBytes += prompt + completion;
    ++measure->rows;
    return UMI_STATUS_OK;
}
static UmiStatus CopyPair(void *opaque, const UmiAiTrainingPair *pair, size_t line)
{
    OverlapCopy *copy = opaque;
    if (copy->count >= copy->capacity || line != copy->count + 1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t prompt = strlen(pair->prompt) + 1U;
    const size_t completion = strlen(pair->completion) + 1U;
    if (copy->used > copy->textCapacity || prompt > copy->textCapacity - copy->used ||
        completion > copy->textCapacity - copy->used - prompt)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    OverlapRow *row = &copy->rows[copy->count];
    row->prompt = copy->text + copy->used;
    memcpy(row->prompt, pair->prompt, prompt);
    copy->used += prompt;
    row->completion = copy->text + copy->used;
    memcpy(row->completion, pair->completion, completion);
    copy->used += completion;
    row->line = line;
    row->side = copy->membership[copy->count++];
    return UMI_STATUS_OK;
}
static int ComparePair(const OverlapRow *a, const OverlapRow *b)
{
    const int prompt = strcmp(a->prompt, b->prompt);
    return prompt != 0 ? prompt : strcmp(a->completion, b->completion);
}
static int CompareRow(const void *left, const void *right)
{
    const OverlapRow *a = left, *b = right;
    const int pair = ComparePair(a, b);
    if (pair != 0) return pair;
    return (a->line > b->line) - (a->line < b->line);
}
UmiStatus UmiAiTrainingJsonlAuditOverlap(const void *bytes, size_t length,
    const unsigned char *membership, size_t membershipCount,
    size_t maxStagingBytes, const UmiCancellationToken *cancel,
    UmiAiTrainingJsonlOverlapSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = (UmiAiTrainingJsonlReport){0};
    if (bytes == NULL || length == 0U || membership == NULL || membershipCount == 0U ||
        maxStagingBytes == 0U || outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_AI_TRAINING_JSONL_BYTES || membershipCount > UMI_AI_TRAINING_JSONL_ROWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    for (size_t i = 0U; i < membershipCount; ++i)
        if (membership[i] > 1U) return UMI_STATUS_INVALID_ARGUMENT;
    OverlapMeasure measure = {0};
    UmiStatus status = UmiAiTrainingJsonlVisit(bytes, length, cancel, MeasurePair, &measure, outReport);
    if (status != UMI_STATUS_OK) return status;
    if (measure.rows != membershipCount) return UMI_STATUS_INVALID_ARGUMENT;
    if (measure.rows > SIZE_MAX / sizeof(OverlapRow)) return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t rowBytes = measure.rows * sizeof(OverlapRow);
    if (rowBytes > maxStagingBytes || measure.textBytes > maxStagingBytes - rowBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    /* Two owned payloads replace retained visitor pointers. Text is measured
     * first so a short dataset never reserves the maximum corpus-sized arena. */
    OverlapRow *rows = malloc(rowBytes);
    char *text = malloc(measure.textBytes);
    status = UMI_STATUS_OUT_OF_MEMORY;
    if (rows == NULL || text == NULL) goto finished;
    OverlapCopy copy = {.rows = rows, .text = text, .membership = membership,
        .capacity = measure.rows, .textCapacity = measure.textBytes};
    status = UmiAiTrainingJsonlVisit(bytes, length, cancel, CopyPair, &copy, outReport);
    if (status != UMI_STATUS_OK) goto finished;
    if (copy.count != measure.rows || copy.used != measure.textBytes) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto finished;
    }
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto finished; }
    qsort(rows, measure.rows, sizeof(*rows), CompareRow);
    UmiAiTrainingJsonlOverlapSummary summary = {.rows = measure.rows,
        .stagingBytes = rowBytes + measure.textBytes};
    /* Exact string comparison keeps pair boundaries and avoids claiming that
     * a digest collision is proof of equal training content. Sorting is private;
     * original input order and membership remain unchanged. */
    for (size_t begin = 0U; begin < measure.rows;) {
        size_t end = begin, training = 0U, evaluation = 0U, firstTraining = 0U, firstEvaluation = 0U;
        while (end < measure.rows && ComparePair(&rows[begin], &rows[end]) == 0) {
            if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto finished; }
            if (rows[end].side == 0U) {
                ++training;
                if (firstTraining == 0U) firstTraining = rows[end].line;
            } else {
                ++evaluation;
                if (firstEvaluation == 0U) firstEvaluation = rows[end].line;
            }
            ++end;
        }
        ++summary.distinctPairs;
        summary.trainingRows += training;
        summary.evaluationRows += evaluation;
        if (training != 0U && evaluation != 0U) {
            ++summary.sharedPairs;
            summary.trainingRowsInSharedPairs += training;
            summary.evaluationRowsInSharedPairs += evaluation;
            if (summary.firstTrainingLine == 0U || firstTraining < summary.firstTrainingLine) {
                summary.firstTrainingLine = firstTraining;
                summary.firstEvaluationLine = firstEvaluation;
            }
        }
        begin = end;
    }
    summary.duplicateRows = summary.rows - summary.distinctPairs;
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto finished; }
    *outSummary = summary;
    status = UMI_STATUS_OK;
finished:
    if (text != NULL) umi_secret_clear(text, measure.textBytes);
    if (rows != NULL) umi_secret_clear(rows, rowBytes);
    free(text);
    free(rows);
    return status;
}
