/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/dataset_split.h
 *
 * PURPOSE:
 *   Produce deterministic, non-overlapping train/evaluation sequence IDs.
 *   Sampling keeps whole sequences intact; actual data remain caller-owned.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
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

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_DATASET_SPLIT_H */
