/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/kv_position_qualification.h
 *
 * PURPOSE:
 *   Verify an incremental KV-cache against the ORIGINAL decoder logits at
 *   the last position of a full context, both before and after one eviction.
 *
 * ENGINEERING CONTRACT:
 *   This qualified learning decoder uses learned absolute token positions.
 *   Growing-prefix reuse is valid while retained token positions do not
 *   change. Once a full context rolls, surviving tokens change positions,
 *   and the existing Framework KV owner must rebuild its representations.
 *   This check provides numerical evidence, not a new positional encoding,
 *   a new decoder or a claim of faster autoregressive generation.
 *
 * OWNERSHIP:
 *   The caller owns the decoder, token IDs and report. This function borrows
 *   them only for the call. The decoder must not be trained, destroyed or used
 *   concurrently. No parameters, checkpoint bytes or persistent files change.
 *   All rejected or unsuccessful calls leave *outQualification unchanged.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_KV_POSITION_QUALIFICATION_H
#define UMICOM_AI_TRAINING_KV_POSITION_QUALIFICATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/kv_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingKvPositionQualification {
    size_t contextTokens;
    size_t vocabularySize;
    size_t comparedLogits;
    size_t cacheHeapBytes;
    uint64_t computedRowsBeforeRollover;
    uint64_t computedRowsAfterRollover;
    uint64_t fullWindowRebuilds;
    float maximumAbsoluteDifferenceBeforeRollover;
    float maximumAbsoluteDifferenceAfterRollover;
} UmiAiTrainingKvPositionQualification;

/* Qualify the SAME decoder at two explicit, fixed-length positions:
 *  (a) Prefill token[0], then Append tokens 1..(contextLength-1), reusing K/V;
 *      compare its final logits with the canonical full-context decoder.
 *  (b) Append one more token, forcing positional rebasing and a whole-window
 *      rebuild; compare against canonical logits for shifted token[1..N-1]
 *      followed by nextTokenId.
 *
 * `tokenCount` must equal decoder.contextLength (1..32). Every token ID,
 * including `nextTokenId`, must be within decoder vocabulary. The absolute
 * tolerance is 0.00004F, consistent with existing Framework KV parity tests.
 * All vocabulary logits at both stages must be finite and within tolerance.
 * The cache must report N rows before rollover, 2N rows after, and exactly
 * one full-window rebuild. A mismatch returns INVALID_STATE, without
 * publishing a misleading qualification report.
 *
 * No expectation of token-row savings is made for the rollover stage.
 * Cache heap size excludes allocator metadata and decoder-owned storage.
 */
UmiStatus UmiAiTrainingKvPositionQualify(
    UmiAiTrainingDecoder *model,
    const size_t *tokenIds, size_t tokenCount, size_t nextTokenId,
    UmiAiTrainingKvPositionQualification *outQualification);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_KV_POSITION_QUALIFICATION_H */
