/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/dataset_split.h
 *
 * PURPOSE:
 *   Produce deterministic, non-overlapping train/evaluation sequence IDs.
 *   Sampling keeps whole sequences intact; actual data remain caller-owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_DATASET_SPLIT_H
#define UMICOM_AI_TRAINING_DATASET_SPLIT_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Outputs a complete permutation of 0..sequenceCount-1. The first
 * (sequenceCount - evaluationCount) IDs belong to TRAINING; the rest are
 * EVALUATION. One seed gives the same partition on all supported CPUs.
 * This is not a leakage detector: the corpus author must avoid duplicates
 * and near-duplicates between splits. Zero-sized groups are disallowed.
 * Invalid requests do not change outOrder. */
UmiStatus UmiAiTrainingSplitSequences(size_t sequenceCount,
                                     size_t evaluationCount,
                                     uint64_t seed,
                                     size_t *outOrder,
                                     size_t capacity);


/* Group-aware splitting prevents rows from the SAME caller-assigned source
 * identity crossing partitions. Repeated IDs are allowed, including 0 and
 * UINT64_MAX. Rows may represent document chunks, video frames or related
 * examples. This does not discover near-duplicates or establish data rights.
 *
 * The same set of group IDs, seed and evaluationGroupCount has the same group
 * membership regardless of row order or repeated-row counts. Sort unique IDs,
 * shuffle with Framework's integer RNG, then map membership back to rows.
 * outEvaluation[i] is 0 for training or 1 for evaluation. Select at least one
 * group for each partition; a group ratio is NOT a guaranteed row ratio.
 *
 * At most 4096 rows are accepted. All output bytes remain unchanged on failure.
 * Arrays are caller-owned; only rowCount output bytes are written. No corpus
 * or model is read. Temporary storage is bounded and released before return.
 * This seeded split is neither stratified nor a chronological holdout.
 */
#define UMI_AI_TRAINING_MAX_GROUP_ROWS 4096U
UmiStatus UmiAiTrainingSplitGroups(const uint64_t *groupIds,
    size_t rowCount, size_t evaluationGroupCount, uint64_t seed,
    unsigned char *outEvaluation, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_DATASET_SPLIT_H */
