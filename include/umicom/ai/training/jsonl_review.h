/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/jsonl_review.h
 *
 * PURPOSE:
 *   Compose source-group splitting and exact-pair overlap into one atomic dataset review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_JSONL_REVIEW_H
#define UMICOM_AI_TRAINING_JSONL_REVIEW_H
#include "umicom/ai/training/jsonl_partition.h"
#include "umicom/ai/training/jsonl_overlap.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiTrainingJsonlReviewOptions {
    size_t evaluationGroupCount;
    uint64_t seed;
    size_t maxStagingBytes;
} UmiAiTrainingJsonlReviewOptions;
typedef struct UmiAiTrainingJsonlReviewSummary {
    UmiAiTrainingJsonlPartitionSummary partition;
    UmiAiTrainingJsonlOverlapSummary overlap;
} UmiAiTrainingJsonlReviewSummary;

/* Use JsonlPartition and JsonlAuditOverlap on the SAME borrowed bytes. Outputs
 * are published only after BOTH operations succeed. A successful review can
 * report shared pairs; success means inspection completed, not data approval.
 * No records are deleted, selected or changed and no training is started.
 *
 * evaluationGroupCount is 1..JSONL_ROWS-1 and must leave at least one actual
 * source group for training. maxStagingBytes is positive and applies ONLY to
 * overlap-owned text/descriptors. It excludes input, parser, splitter, stack,
 * allocator overhead and caller output. The composition has a fixed JSONL_ROWS
 * byte membership candidate, cleared before return. Other owners retain their
 * published bounds and clear their temporary allocations before releasing them.
 *
 * Membership/summary stay byte-for-byte unchanged on any failure. A nonnull
 * report is initialized and describes the last attempted parser pass; COMPLETE
 * can accompany a later budget/capacity/group/cancellation error. Only rows
 * membership bytes are written on success, as 0 training or 1 evaluation.
 * Options, input, token and all outputs must be valid, disjoint storage and
 * remain unchanged by other threads until return. Cancellation is cooperative;
 * synchronous parsing/sorting cannot be interrupted inside every operation.
 */
UmiStatus UmiAiTrainingJsonlReview(const void *bytes,size_t length,
    const UmiAiTrainingJsonlReviewOptions *options,const UmiCancellationToken *cancel,
    unsigned char *outEvaluation,size_t capacity,UmiAiTrainingJsonlReviewSummary *outSummary,
    UmiAiTrainingJsonlReport *outReport);
#ifdef __cplusplus
}
#endif
#endif
