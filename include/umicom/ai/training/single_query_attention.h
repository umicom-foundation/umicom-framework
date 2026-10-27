/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/single_query_attention.h
 *
 * PURPOSE:
 *   Compute the last query of a decoder attention window against actual
 *   retained keys and values, without fabricating unused earlier query rows.
 *   This bounded, forward-only C23 numerical primitive has no model, cache,
 *   optimiser, storage or application ownership.
 *
 * LAYOUT AND NUMERICAL CONTRACT:
 *   Query is [channels]; keys and values are [validRows][channels], each
 *   row-major. Each head owns a contiguous width of channels. Every supplied
 *   row is causally visible to the newest query; the caller excludes padding
 *   and later positions. Scores are accumulated in double precision, then
 *   softmax probabilities are rounded to float before weighted-value sums,
 *   matching the existing Framework multi-head tape operator's arithmetic
 *   convention. Exact bit parity is NOT guaranteed across libm implementations.
 *
 * OWNERSHIP:
 *   All pointers and capacities are caller-owned, sizes count float elements.
 *   Input buffers must remain stable for the call. Output is written only on
 *   success and must not overlap an input region or the optional report.
 *   No model state is modified; the caller handles checkpoint identity,
 *   upper-layer processing, training-tape ownership and context rollover.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SINGLE_QUERY_ATTENTION_H
#define UMICOM_AI_TRAINING_SINGLE_QUERY_ATTENTION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingSingleQueryAttentionReport {
    size_t validRows;
    size_t channels;
    size_t headCount;
    size_t headWidth;
    size_t temporaryBytes;       /* Requested scratch payload; excludes allocator overhead. */
    uint64_t scoredKeyRows;      /* Number of (head, key row) score evaluations. */
    uint64_t dotProductTerms;    /* Number of scalar Q*K terms accumulated. */
    uint64_t weightedValueTerms; /* Number of scalar probability*V terms. */
} UmiAiTrainingSingleQueryAttentionReport;

/*
 * Precondition: 1 <= validRows, 1 <= headCount <= channels, and channels is
 * divisible by headCount. The total KV element count must fit the Framework
 * training tape's bounded educational tensor limit. Empty/short inputs,
 * nonfinite inputs, invalid layout and output/input overlaps are rejected.
 * Aliasing between input arrays is permitted because they are read-only.
 * A rejected call preserves *outRow and *outReport byte for byte.
 * This function is inference-only; it does not install a differentiable tape
 * node and must not be substituted for the training-time attention operator.
 */
UmiStatus UmiAiTrainingSingleQueryAttentionForward(
    const float *query, size_t queryCapacity,
    const float *keys, size_t keyCapacity,
    const float *values, size_t valueCapacity,
    size_t validRows, size_t channels, size_t headCount,
    float *outRow, size_t outCapacity,
    UmiAiTrainingSingleQueryAttentionReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_SINGLE_QUERY_ATTENTION_H */
