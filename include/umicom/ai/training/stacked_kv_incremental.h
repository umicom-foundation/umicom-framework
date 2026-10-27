/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_kv_incremental.h
 * PURPOSE: Opt-in, checkpoint-bound incremental native Transformer inference.
 *
 * DESIGN:
 *   Initial prefill and learned-absolute-position rollover use the ORIGINAL
 *   stacked decoder's full-context projection trace. While a prefix grows
 *   without eviction, the tape-owned KvStepPropose reuses genuinely retained
 *   keys/values to calculate the next token's Q/K/V and output logits.
 *   The existing Framework nucleus sampler decides the output token.
 *   The existing KV store owns all row data. No new tokenizer, sampler,
 *   checkpoint format or script is introduced.
 *
 *   The previous approach calculated placeholder earlier query rows through
 *   the canonical tape attention operator. That implementation remains in a
 *   disabled historical block. The current growing-prefix inference path
 *   invokes Framework's forward-only single-query attention kernel, yielding
 *   one actual query row per layer, and continues to use the canonical tape
 *   for the rest of the Transformer block.
 *
 * CAUTION:
 *   A reduction in executed query rows does NOT imply faster whole-model
 *   wall-clock inference. Rollover always falls back to a complete model
 *   forward. This is a bounded opt-in research path rather than a production
 *   LLM runtime. The older full-context and prefix generators are unchanged.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_KV_INCREMENTAL_H
#define UMICOM_AI_TRAINING_STACKED_KV_INCREMENTAL_H

#include "umicom/ai/training/stacked_kv_step.h"
#include "umicom/ai/training/stacked_inference.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedKvIncrementalSession
    UmiAiTrainingStackedKvIncrementalSession;

typedef struct UmiAiTrainingStackedKvIncrementalStats {
    size_t initialPromptTokens;
    size_t generatedTokens;
    size_t retainedTokens;
    size_t transformerLayers;
    size_t contextTokens;
    uint64_t referenceFullContextTokenLayerRows;
    uint64_t observedFullContextTokenLayerRows;
    uint64_t observedIncrementalProjectionTokenLayerRows;
    /* Includes full-context rows from prefill/rollover plus actual one-query
     * rows from growing-prefix steps. Not a latency or FLOP counter. */
    uint64_t observedAttentionQueryTokenLayerRows;
    uint64_t observedFullWindowRebuilds;
    uint64_t observedIncrementalAppends;
    size_t reservedKvBytes;
    UmiAiTrainingStackedModelIdentity modelIdentity;
} UmiAiTrainingStackedKvIncrementalStats;

/* Create using one NONEMPTY unpadded prefix of 1..contextLength tokens.
 * Canonical full-context tracing prefills the store and selects the final
 * nonpadding logit row. Every input token is validated. The model is borrowed
 * and must remain alive and unchanged until Destroy. Initialise the caller's
 * *outSession to NULL. Failure leaves *outSession unchanged. */
UmiStatus UmiAiTrainingStackedKvIncrementalSessionCreate(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingStackedKvIncrementalSession **outSession);

void UmiAiTrainingStackedKvIncrementalSessionDestroy(
    UmiAiTrainingStackedKvIncrementalSession *session);

/* Return ONE sampled token. The newest emitted ID is pending: it is appended
 * to the KV store only before the NEXT sample, matching the canonical prefix
 * session. At most 64 pulls succeed. No implicit EOS stop. The sampling
 * state and caller output are unchanged when a request is rejected. Model
 * identity is checked before and after each successful numerical operation.
 * Destroy is explicit cancellation; sessions are not thread-safe. */
UmiStatus UmiAiTrainingStackedKvIncrementalSessionNext(
    UmiAiTrainingStackedKvIncrementalSession *session,
    size_t *outTokenId);

/* Binary-byte convenience only when vocabularySize==260. The returned byte
 * may be NUL or an incomplete UTF-8 sequence. No C-string assumptions. */
UmiStatus UmiAiTrainingStackedKvIncrementalSessionNextByte(
    UmiAiTrainingStackedKvIncrementalSession *session,
    uint8_t *outByte);

/* Distinguish actual full-context forwards, single-row projections and
 * attention query-row work. referenceFullContextTokenLayerRows is the cost
 * of the original reference strategy for the SAME completed pull count; it
 * is not observed speed or measured elapsed time. Output is unchanged on
 * failure, including model-identity mismatch. */
UmiStatus UmiAiTrainingStackedKvIncrementalSessionInspect(
    const UmiAiTrainingStackedKvIncrementalSession *session,
    UmiAiTrainingStackedKvIncrementalStats *outStats);

/* One-shot composition. Tokens and optional stats are published only after
 * every requested sample has been produced. Output regions must not overlap.
 * Caller owns all arrays; capacity counts size_t token IDs. */
UmiStatus UmiAiTrainingStackedKvIncrementalGenerate(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokens, size_t capacity, size_t *outCount,
    UmiAiTrainingStackedKvIncrementalStats *outStats);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_KV_INCREMENTAL_H */
