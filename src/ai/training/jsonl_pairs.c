/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_pairs.c
 *
 * PURPOSE:
 *   Reuse the shared JSON parser to decode strict teaching pairs and scan bounded JSON Lines input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_pairs.h"
#include "umicom/language_runtime/json_tree.h"
#include "umicom/security/secrets.h"
#include <string.h>

static UmiStatus PairField(const UmiJsonTree *tree, const char *name,
    char *output, size_t capacity, int allowEmpty)
{
    int node = -1;
    UmiStatus status = UmiJsonTreeMember(tree, 0, name, &node);
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_PARSE_ERROR;
    if (status != UMI_STATUS_OK) return status;
    if (UmiJsonTreeKind(tree, node) != UMI_LANGUAGE_RUNTIME_JSON_STRING)
        return UMI_STATUS_PARSE_ERROR;
    status = UmiJsonTreeText(tree, node, output, capacity);
    if (status == UMI_STATUS_OK && !allowEmpty && output[0] == '\0')
        return UMI_STATUS_PARSE_ERROR;
    return status;
}

UmiStatus UmiAiTrainingPairRead(const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiAiTrainingPair *outPair)
{
    if (bytes == NULL || length == 0U || outPair == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_AI_TRAINING_PAIR_LINE_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;

    const UmiJsonTreeLimits limits = {UMI_AI_TRAINING_PAIR_LINE_BYTES, 32U, 4U};
    UmiJsonTree *tree = NULL;
    UmiAiTrainingPair candidate = {0};
    UmiStatus status = UmiJsonTreeCreate(bytes, length, &limits, cancel, &tree);
    if (status == UMI_STATUS_OK &&
        UmiJsonTreeKind(tree, 0) != UMI_LANGUAGE_RUNTIME_JSON_OBJECT)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK) status = UmiJsonTreeValidateUniqueKeys(tree, 0, cancel);
    if (status == UMI_STATUS_OK && UmiJsonTreeCount(tree, 0) != 3U)
        status = UMI_STATUS_PARSE_ERROR;
    if (status == UMI_STATUS_OK)
        status = PairField(tree, "source_id", candidate.sourceId, sizeof(candidate.sourceId), 0);
    if (status == UMI_STATUS_OK)
        status = PairField(tree, "prompt", candidate.prompt, sizeof(candidate.prompt), 1);
    if (status == UMI_STATUS_OK)
        status = PairField(tree, "completion", candidate.completion, sizeof(candidate.completion), 0);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    /* Publish one complete owned value. Clear temporary decoded text as well
     * as the parser's owned JSON; caller copies have their own lifetime. */
    if (status == UMI_STATUS_OK) *outPair = candidate;
    umi_secret_clear(&candidate, sizeof(candidate));
    UmiJsonTreeDestroy(tree);
    return status;
}

UmiStatus UmiAiTrainingJsonlVisit(const void *bytes, size_t length,
    const UmiCancellationToken *cancel, UmiAiTrainingPairVisitor visitor,
    void *context, UmiAiTrainingJsonlReport *outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = (UmiAiTrainingJsonlReport){0};
    if (bytes == NULL || length == 0U || visitor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (length > UMI_AI_TRAINING_JSONL_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *input = bytes;
    size_t cursor = 0U;
    UmiAiTrainingPair pair;
    while (cursor < length) {
        outReport->stage = UMI_AI_TRAINING_JSONL_RECORD;
        outReport->failedLine = outReport->acceptedRows + 1U;
        if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
        if (outReport->acceptedRows == UMI_AI_TRAINING_JSONL_ROWS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        size_t end = cursor;
        /* Stop scanning an oversized line promptly; the whole file's byte cap
         * is separate from the per-record parsing and decoded-text caps. */
        while (end < length && input[end] != '\n') {
/* Keep the earlier cap check for review. Count the record separately from its line delimiter so an exact-limit JSON object has the same admission result with LF and CRLF. The former implementation is retained for owner review. */
#if 0
            if (end - cursor == UMI_AI_TRAINING_PAIR_LINE_BYTES)
                return UMI_STATUS_CAPACITY_EXCEEDED;
#endif
            if (end - cursor == UMI_AI_TRAINING_PAIR_LINE_BYTES) {
                /* CR belongs to a CRLF delimiter, or the accepted final CR,
                 * not the JSON record. Admit only that single boundary byte;
                 * a second CR or any further JSON byte still exceeds the cap. */
                if (input[end] == '\r' &&
                    (length - end == 1U || input[end + 1U] == '\n')) {
                    ++end;
                    break;
                }
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            ++end;
        }
        size_t recordBytes = end - cursor;
        if (recordBytes != 0U && input[end - 1U] == '\r') --recordBytes;
        if (recordBytes == 0U) return UMI_STATUS_PARSE_ERROR;
        UmiStatus status = UmiAiTrainingPairRead(input + cursor, recordBytes, cancel, &pair);
        if (status == UMI_STATUS_OK) {
            outReport->stage = UMI_AI_TRAINING_JSONL_VISITOR;
            status = visitor(context, &pair, outReport->failedLine);
        }
        umi_secret_clear(&pair, sizeof(pair));
        if (status != UMI_STATUS_OK) return status;
        cursor = end < length ? end + 1U : end;
        ++outReport->acceptedRows;
        outReport->consumedBytes = cursor;
    }
    outReport->failedLine = 0U;
    outReport->stage = UMI_AI_TRAINING_JSONL_COMPLETE;
    return UMI_STATUS_OK;
}
