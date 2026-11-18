/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_selection.c
 *
 * PURPOSE:
 *   Reuse record validation and serialization to publish a complete selected dataset only after both scans succeed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_selection.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>

typedef struct SelectionContext {
    const unsigned char *membership;
    size_t membershipCount;
    unsigned char side;
    char *row;
    char *staging;
    size_t stagingCapacity;
    size_t selectedRows;
    size_t outputBytes;
} SelectionContext;

static UmiStatus SelectRecord(void *opaque, const UmiAiTrainingPair *pair, size_t line)
{
    SelectionContext *context = opaque;
    if (line == 0U || line > context->membershipCount) return UMI_STATUS_INVALID_ARGUMENT;
    if (context->membership[line - 1U] != context->side) return UMI_STATUS_OK;
    size_t written = 0U;
    char *destination = context->staging == NULL ? context->row : context->staging + context->outputBytes;
    size_t capacity = context->staging == NULL ? UMI_AI_TRAINING_PAIR_LINE_BYTES + 2U :
        context->stagingCapacity - context->outputBytes;
    UmiStatus status = UmiAiTrainingPairWrite(pair, destination, capacity, &written);
    if (status != UMI_STATUS_OK) return status;
    if (written > UMI_AI_TRAINING_JSONL_BYTES - context->outputBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    context->outputBytes += written;
    ++context->selectedRows;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingJsonlSelectionWrite(const void *bytes, size_t length,
    const unsigned char *membership, size_t membershipCount, unsigned char side,
    const UmiCancellationToken *cancel, char *output, size_t capacity,
    UmiAiTrainingJsonlSelectionSummary *outSummary, UmiAiTrainingJsonlReport *outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = (UmiAiTrainingJsonlReport){0};
    if (bytes == NULL || length == 0U || membership == NULL || membershipCount == 0U ||
        membershipCount > UMI_AI_TRAINING_JSONL_ROWS || side > 1U || output == NULL || outSummary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_AI_TRAINING_JSONL_BYTES || capacity == 0U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < membershipCount; ++i)
        if (membership[i] > 1U) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;

    const size_t rowCapacity = UMI_AI_TRAINING_PAIR_LINE_BYTES + 2U;
    char *row = malloc(rowCapacity), *staging = NULL;
    size_t stagedCapacity = 0U;
    UmiStatus status = UMI_STATUS_OUT_OF_MEMORY;
    if (row == NULL) goto finished;
    SelectionContext context = {.membership = membership, .membershipCount = membershipCount,
        .side = side, .row = row};
    /* A measurement scan validates unselected records too. It never touches
     * application output and avoids allocating an entire maximum-size corpus. */
    status = UmiAiTrainingJsonlVisit(bytes, length, cancel, SelectRecord, &context, outReport);
    if (status != UMI_STATUS_OK) goto finished;
    if (outReport->acceptedRows != membershipCount) { status = UMI_STATUS_INVALID_ARGUMENT; goto finished; }
    if (context.outputBytes >= capacity) { status = UMI_STATUS_CAPACITY_EXCEEDED; goto finished; }
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto finished; }
    UmiAiTrainingJsonlSelectionSummary summary = {membershipCount, context.selectedRows, context.outputBytes};
    stagedCapacity = context.outputBytes + 1U;
    staging = malloc(stagedCapacity);
    if (staging == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto finished; }
    staging[0] = '\0';
    context.staging = staging;
    context.stagingCapacity = stagedCapacity;
    context.selectedRows = 0U;
    context.outputBytes = 0U;
    status = UmiAiTrainingJsonlVisit(bytes, length, cancel, SelectRecord, &context, outReport);
    if (status != UMI_STATUS_OK) goto finished;
    if (outReport->acceptedRows != summary.inputRows || context.outputBytes != summary.outputBytes ||
        context.selectedRows != summary.selectedRows) { status = UMI_STATUS_INVALID_STATE; goto finished; }
    if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; goto finished; }
    /* Publication follows every fallible operation. Parser or writer allocation
     * failure during the second scan cannot leave a partially exported dataset. */
    memcpy(output, staging, stagedCapacity);
    *outSummary = summary;
finished:
    if (staging != NULL) umi_secret_clear(staging, stagedCapacity);
    if (row != NULL) umi_secret_clear(row, rowCapacity);
    free(staging);
    free(row);
    return status;
}
