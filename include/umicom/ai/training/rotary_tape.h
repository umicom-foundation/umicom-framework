/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/rotary_tape.h
 *
 * PURPOSE:
 *   Attach the existing rotary-position mathematics to the canonical native
 *   training tape without modifying tape internals or trained checkpoints.
 *
 * ARCHITECTURE:
 *   This bounded teaching adapter composes existing tape EmbeddingLookup,
 *   Matmul, Add and MultiHeadCausalAttention operations. It derives fixed
 *   per-position rotation matrices from UmiAiTrainingRotaryRotate, so forward
 *   and backward derivatives are owned by the existing tape. A future direct
 *   tape operator can replace this intentionally expensive composition after
 *   numerical qualification; the established tape/decoder are preserved.
 *
 *   This is a TRAINING GRAPH component, not a new checkpoint format, full
 *   decoder, streaming inference engine, or efficient production operator.
 *   Exactly 1..8 token rows and at most 8 projected channels are supported.
 *   No additional scripting language or neural kernel is introduced.
 *
 * LIFETIME AND ERRORS:
 *   Caller owns the tape and all tensor handles; published results are owned
 *   by the tape. On failure, the original graph prefix is retained and the
 *   output handle is unchanged. As documented by TapeRollback, its cleanup
 *   clears pre-existing tape gradients; callers must recompute Backward.
 *   These functions cannot be called concurrently on the same tape.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_ROTARY_TAPE_H
#define UMICOM_AI_TRAINING_ROTARY_TAPE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/tape.h"
#include "umicom/ai/training/rotary_position.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_ROTARY_TAPE_MAX_ROWS 8U
#define UMI_AI_ROTARY_TAPE_MAX_CHANNELS 8U

/* Rotate a [rows, heads * headChannels] tape tensor. rows must be a whole
 * number of independent sequences; each sequence uses its own startPositions
 * (NULL/0 means every sequence starts at absolute position 0).
 * Gradients propagate to projected, including when projected is the result
 * of an earlier trainable projection, not merely a leaf. */
UmiStatus UmiAiTrainingRotaryTapeApply(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *projected,
    const UmiAiTrainingRotaryConfig *rotary,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    UmiAiTrainingTensor **outRotated);

/* Rotate query and key with exactly the same per-position mathematics, then
 * call the established tape-owned multi-head causal attention operator.
 * Values are not rotated. All composed nodes are one rollback transaction.
 * This API does NOT change existing TransformerBlockForward behaviour. */
UmiStatus UmiAiTrainingRotaryTapeAttention(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *queries,
    const UmiAiTrainingTensor *keys,
    const UmiAiTrainingTensor *values,
    const UmiAiTrainingRotaryConfig *rotary,
    size_t sequenceLength, size_t attentionHeads,
    const uint64_t *startPositions, size_t startCount,
    UmiAiTrainingTensor **outAttended);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_ROTARY_TAPE_H */
