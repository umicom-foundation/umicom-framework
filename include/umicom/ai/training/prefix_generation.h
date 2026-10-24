/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/prefix_generation.h
 *
 * PURPOSE:
 *   Expose a bounded, opt-in, one-token-at-a-time autoregressive decoder
 *   session that reuses existing K/V rows while an unpadded prefix grows.
 *
 * SEMANTICS — READ BEFORE USING:
 *   The historical UmiAiTrainingGenerationSession uses a FULL BOS-left-padded
 *   context. This NEW interface instead starts at position zero with the
 *   caller's NONEMPTY, unpadded prefix. Consequently its outputs are NOT
 *   interchangeable with historical GenerateNucleus checkpoint evaluations.
 *   It is an explicit alternative inference policy, not a change to weights,
 *   existing decoding APIs, learned positional embeddings or checkpoints.
 *
 *   Until the context fills, every pending generated token is appended using
 *   the Framework-owned KV cache and only one new decoder token-row is
 *   computed. Once full, the original KV owner performs the necessary full
 *   window rebuild because retained learned absolute positions have moved.
 *
 *   The independent canonical reference for this policy is the ORIGINAL
 *   decoder evaluated on a right-padded context. Its logit row at the FINAL
 *   retained prefix position is compared, never the trailing padding row.
 *   Future padding is causally invisible to earlier token positions.
 *
 * OWNERSHIP AND SAFETY:
 *   Model is caller-owned and must outlive each session. A session owns its
 *   own cache and RNG stream. Access must be serialised with model training
 *   and all other decoder operations. Creation, inspection, rejected pulls
 *   and one-shot errors preserve caller-owned outputs. An internal sampling
 *   failure after successful cache advancement makes the session terminal.
 *   Destroy is always permitted and is the cancellation operation.
 *   No scripts, alternate neural kernels or external inference engines.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_PREFIX_GENERATION_H
#define UMICOM_AI_TRAINING_PREFIX_GENERATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/generation_session.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingPrefixGenerationSession
    UmiAiTrainingPrefixGenerationSession;

typedef struct UmiAiTrainingPrefixGenerationWorkEstimate {
    size_t promptTokens;
    size_t generatedTokens;
    size_t contextTokens;
    size_t cacheHeapBytes;
    uint64_t canonicalComputedRows;
    uint64_t cachedComputedRows;
    uint64_t cachedWindowRebuilds;
    uint64_t computedRowsAvoided;
} UmiAiTrainingPrefixGenerationWorkEstimate;

/* Forecast the actual cache-owner token-row work when a prompt of 1..context
 * tokens generates 1..256 samples. The final sample is still pending, so
 * there are generatedTokenCount-1 cache append operations. Canonical rows
 * count one full context of decoder evaluation PER sampled token (the
 * reference right-pads an incomplete prefix only for causal comparison).
 * A learned-absolute-position rollover costs contextLength rows, never one.
 * Shape validation and heap reservation reuse the existing KV allocation
 * planner. On failure, *outEstimate remains byte-for-byte unchanged. */
UmiStatus UmiAiTrainingPrefixGenerationWorkEstimateBuild(
    const UmiAiTrainingDecoderConfig *config,
    size_t promptTokenCount, size_t generatedTokenCount,
    UmiAiTrainingPrefixGenerationWorkEstimate *outEstimate);

/* Create a session with an explicit NONEMPTY, unpadded prefix. Neither the
 * prompt nor the caller's policy is retained by reference. 1..contextLength
 * IDs are accepted; unlike the legacy BOS-padded API, empty or oversize
 * prompts are intentionally refused to prevent semantic ambiguity.
 * Every input ID must be inside the decoder vocabulary, including reserved
 * IDs when explicitly supplied by the caller. Sampling emits IDs >= 4.
 * Returns the new opaque handle only after complete success. */
UmiStatus UmiAiTrainingPrefixGenerationSessionCreate(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingPrefixGenerationSession **outSession);

void UmiAiTrainingPrefixGenerationSessionDestroy(
    UmiAiTrainingPrefixGenerationSession *session);

/* Produce exactly one sample using the original Framework nucleus sampler.
 * At most UMI_AI_GENERATION_MAX_NEW_TOKENS calls may succeed. The previous
 * sample remains pending until the next successful pull; Inspect therefore
 * does NOT include it in retained tokens or row counts. Rejected calls do
 * not consume the sampling RNG or change *outTokenId. */
UmiStatus UmiAiTrainingPrefixGenerationSessionNext(
    UmiAiTrainingPrefixGenerationSession *session, size_t *outTokenId);

/* Convenience mapping for the canonical 260-token byte vocabulary only.
 * The output is one RAW byte, not a null-terminated UTF-8 string. */
UmiStatus UmiAiTrainingPrefixGenerationSessionNextByte(
    UmiAiTrainingPrefixGenerationSession *session, uint8_t *outByte);

/* Return generated count and the actual Framework KV inspection counters.
 * Rejected inspections preserve BOTH output objects. Stale weights and
 * terminal sessions are rejected, matching the original session contract. */
UmiStatus UmiAiTrainingPrefixGenerationSessionInspect(
    const UmiAiTrainingPrefixGenerationSession *session,
    size_t *outGeneratedTokens,
    UmiAiTrainingGenerationCacheStats *outCacheStats);

/* One-shot convenience composition of Create, Next, Inspect and Destroy.
 * It publishes all outputs only if the complete generation succeeded and
 * measured cache workload agrees with WorkEstimateBuild. No new sampler or
 * decoder mathematics are implemented here. The token buffer capacity is
 * measured in size_t entries. Output objects must not overlap one another. */
UmiStatus UmiAiTrainingDecoderGeneratePrefixNucleus(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount,
    UmiAiTrainingGenerationCacheStats *outStats);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_PREFIX_GENERATION_H */
