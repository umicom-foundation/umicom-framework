/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/sample_order.h
 *
 * PURPOSE:
 *   Define bounded reproducible row order for an explicitly selected training partition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_SAMPLE_ORDER_H
#define UMICOM_AI_TRAINING_SAMPLE_ORDER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_TRAINING_MAX_ORDER_ROWS 65536U

/* Produce a permutation of [0,rowCount): each index appears exactly once.
 * The caller FIRST selects a training partition, then interprets these indices
 * within its stable row order. This function neither splits a dataset nor
 * shuffles source bytes. It does not sample with replacement or balance labels.
 *
 * Require 1..MAX_ORDER_ROWS rows and capacity >= rowCount uint32_t elements.
 * The output is caller-owned; unused tail elements remain unchanged. All
 * output remains unchanged on validation/allocation/RNG failure. Exactly one
 * rowCount*sizeof(uint32_t) private payload is allocated during the call
 * (at most 256 KiB; allocator overhead is separate) and freed before return.
 * No retained pointers, file/network I/O, global RNG or model execution.
 *
 * A descending Fisher-Yates shuffle uses Framework's bounded integer RNG.
 * Same rowCount and seed produce identical integer order on supported hosts.
 * All uint64_t seeds, including zero, are valid; no hidden epoch increment.
 * Reusing a seed repeats an epoch. Different seeds MAY produce the same order;
 * a 64-bit seed cannot address all permutations of a large dataset. This is
 * reproducible training order, not cryptographic randomness or a quality claim.
 * Dataset identity, split membership, row order and seed belong in a resume
 * record. The caller serializes access and maintains valid output storage.
 */
UmiStatus UmiAiTrainingSampleOrder(size_t rowCount, uint64_t seed,
    uint32_t *outOrder, size_t capacity);

#ifdef __cplusplus
}
#endif
#endif
