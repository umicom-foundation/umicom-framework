/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/jsonl_selection.h
 *
 * PURPOSE:
 *   Serialize one reviewed membership side through the existing bounded training-record schema.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_JSONL_SELECTION_H
#define UMICOM_AI_TRAINING_JSONL_SELECTION_H
#include "umicom/ai/training/jsonl_pairs.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingJsonlSelectionSummary {
    size_t inputRows;
    size_t selectedRows;
    size_t outputBytes; /* Includes each LF; excludes the final NUL. */
} UmiAiTrainingJsonlSelectionSummary;

/* Validate every input record and select rows whose membership equals side.
 * side and every membership byte must be 0 or1; membershipCount must exactly
 * match the input row count. Reuse the output of JsonlPartition to preserve its
 * grouping rules. Arbitrary caller membership does NOT prove group isolation.
 * The reviewed membership belongs to these exact source bytes and row order.
 *
 * Selected records retain their input order and decoded field values. The
 * existing PairWrite owner emits source_id/prompt/completion order, LF records
 * and a final NUL. Original whitespace, escape spelling and CRLF are normalized.
 * No selected rows is a successful empty string; callers decide whether that
 * is acceptable for a particular training/evaluation workflow.
 *
 * Input limits are those of JsonlVisit; encoded output is at most JSONL_BYTES.
 * Capacity includes the final NUL. Size/validate first, allocate exactly the
 * selected encoded size plus1, scan again into private staging, then publish.
 * Temporary memory includes one maximum-size row and the exact staged output,
 * in addition to bounded per-record parser/writer memory. Temporary text is
 * cleared before release. There is no file, network or model operation.
 *
 * Output and summary stay unchanged on every failure. A nonnull report is
 * always initialized; it describes the latest scan, so COMPLETE may accompany
 * a later size, count, allocation or cancellation error. All buffers must be
 * disjoint valid objects, unchanged by other threads until return. The source,
 * membership and callbacks are not retained. Cancellation is checked by the
 * parser, between scans and immediately before publication.
 */
UmiStatus UmiAiTrainingJsonlSelectionWrite(const void *bytes, size_t length,
    const unsigned char *membership, size_t membershipCount, unsigned char side,
    const UmiCancellationToken *cancel, char *output, size_t capacity,
    UmiAiTrainingJsonlSelectionSummary *outSummary, UmiAiTrainingJsonlReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
