/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/jsonl_overlap.h
 *
 * PURPOSE:
 *   Audit exact decoded prompt/completion pairs across reviewed dataset partitions within a caller staging budget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_JSONL_OVERLAP_H
#define UMICOM_AI_TRAINING_JSONL_OVERLAP_H
#include "umicom/ai/training/jsonl_pairs.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingJsonlOverlapSummary {
    size_t rows;
    size_t trainingRows;
    size_t evaluationRows;
    size_t distinctPairs;
    size_t duplicateRows; /* rows - distinctPairs, including within-side repeats. */
    size_t sharedPairs; /* Distinct pairs occurring on BOTH sides. */
    size_t trainingRowsInSharedPairs;
    size_t evaluationRowsInSharedPairs;
    size_t firstTrainingLine; /* Original one-based witness; zero when none. */
    size_t firstEvaluationLine;
    size_t stagingBytes; /* Owned text + row descriptors, excluding allocator/parser overhead. */
} UmiAiTrainingJsonlOverlapSummary;

/* Validate all records, then compare decoded prompt/completion pairs exactly.
 * source_id does NOT participate: different provenance names can contain the
 * same pair. JSON escape variants match after decoding; case, whitespace and
 * Unicode normalization variants remain distinct. Pair boundaries matter.
 * No hashes, near-duplicate/semantic comparison or automatic deletion.
 *
 * Membership is one reviewed byte per row: 0 training, 1 evaluation. Its count
 * must match exactly. Either side may be empty; this audit does not establish
 * split quality or replace JsonlPartition's source-group isolation. Witnesses
 * select the lowest original training line in any shared pair, then the lowest
 * evaluation line for that same pair. Report counts include every repeated row.
 *
 * First scan measures decoded text and validates all records. Allocate row
 * descriptors and terminated text only after their summed size fits the caller's
 * maxStagingBytes. At most JSONL_ROWS rows and JSONL_BYTES input bytes are read.
 * The budget excludes parser temporaries, stack, allocator overhead and the
 * caller's input/membership/output. A zero budget is invalid. Exact required
 * stagingBytes is returned only on success and can differ by pointer width.
 * Temporary text and descriptors are cleared before release; nothing is retained.
 *
 * Summary remains unchanged on every failure. A nonnull report is initialized
 * on entry and describes the latest scan: COMPLETE can accompany a later budget,
 * membership-count, allocation or cancellation error. Cancellation is checked
 * between records, around bounded sorting, during grouping and before publication;
 * qsort itself is not interruptible. Input, membership, report and summary must
 * be valid disjoint objects and remain unchanged by other threads until return.
 * This performs no file, model, network, tokenizer or optimizer operation.
 */
UmiStatus UmiAiTrainingJsonlAuditOverlap(const void *bytes, size_t length,
    const unsigned char *membership, size_t membershipCount,
    size_t maxStagingBytes, const UmiCancellationToken *cancel,
    UmiAiTrainingJsonlOverlapSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
