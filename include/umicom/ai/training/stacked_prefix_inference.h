/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_prefix_inference.h
 * PURPOSE:
 *   Provide an explicitly unpadded, growing-prefix inference policy for the
 *   canonical stacked Transformer. Establish the numerical reference needed
 *   before implementing an optimised multi-layer key/value cache.
 *
 * ARCHITECTURE:
 *   The original stacked decoder owns all Transformer mathematics. The
 *   established Framework nucleus sampler and byte tokeniser own sampling
 *   and byte conversion. The established checkpoint identity protects the
 *   borrowed model. This component changes only prompt placement and session
 *   orchestration; it does NOT implement or advertise a KV-cache speedup.
 *
 *   Every sampled step invokes a full context-length, multi-layer forward.
 *   The growing prefix is positioned at indices 0..retained-1 and future
 *   context slots are padded on the RIGHT. Causality permits using the logit
 *   row at retained-1. Once full, the oldest token is removed and all learned
 *   absolute positions are reassigned, requiring full recomputation.
 *   This policy is not interchangeable with the historical BOS-LEFT-PADDED
 *   stacked generator even when sampling policy and random seed agree.
 *
 * OWNERSHIP:
 *   Sessions own their RNG/context and borrow one unchanged decoder. Neither
 *   side may be accessed concurrently. A session's final emitted token is
 *   pending until the next successful pull; Inspect excludes this pending
 *   token from retained-context and window-rebuild counts. Every rejection
 *   preserves caller outputs. Destroy is explicit cancellation.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_PREFIX_INFERENCE_H
#define UMICOM_AI_TRAINING_STACKED_PREFIX_INFERENCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/stacked_inference.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedPrefixSession
    UmiAiTrainingStackedPrefixSession;

typedef struct UmiAiTrainingStackedPrefixStats {
    size_t initialPromptTokens;
    size_t generatedTokens;
    size_t retainedTokens;
    size_t contextTokens;
    size_t transformerLayers;
    uint64_t computedFullContextTokenRows;
    uint64_t computedTransformerTokenLayerRows;
    uint64_t fullWindowRebuilds;
    UmiAiTrainingStackedModelIdentity modelIdentity;
} UmiAiTrainingStackedPrefixStats;

/* Admit a nonempty prefix of 1..contextLength token IDs. No implicit BOS
 * left-padding occurs. Validate every token ID and the nucleus policy using
 * the already qualified Framework sampler. *outSession must start NULL.
 * On failure *outSession remains unchanged; on success caller owns session. */
UmiStatus UmiAiTrainingStackedPrefixSessionCreate(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingStackedPrefixSession **outSession);

void UmiAiTrainingStackedPrefixSessionDestroy(
    UmiAiTrainingStackedPrefixSession *session);

/* Return one token ID in [4,vocabularySize), using the existing sampler.
 * At most 64 pulls succeed. The borrowed checkpoint is verified both before
 * and after the full-context reference call. No model parameters are updated.
 * A successful pull advances the private cursor; failed calls do not.
 * No automatic EOS stopping is implied by the educational byte vocabulary. */
UmiStatus UmiAiTrainingStackedPrefixSessionNext(
    UmiAiTrainingStackedPrefixSession *session, size_t *outTokenId);

/* Convenience for EXACTLY 260 byte-vocabulary IDs. The result is one raw
 * byte and may not be a complete UTF-8 character. A wrong vocabulary or
 * invalid output pointer must not advance the generation session. */
UmiStatus UmiAiTrainingStackedPrefixSessionNextByte(
    UmiAiTrainingStackedPrefixSession *session, uint8_t *outByte);

/* Inspect actual FULL-context evaluation work. These counters MUST NOT be
 * confused with the conditional savings estimated by StackedPrefixCachePlan.
 * The session does not possess any retained key/value tensor allocations.
 * A failed inspection leaves *outStats unchanged. */
UmiStatus UmiAiTrainingStackedPrefixSessionInspect(
    const UmiAiTrainingStackedPrefixSession *session,
    UmiAiTrainingStackedPrefixStats *outStats);

/* Stage all generated outputs and return them only after complete success.
 * The count is 1..64. outStats may be NULL. Buffers must not overlap.
 * Rejected or incomplete generation never publishes partial token output. */
UmiStatus UmiAiTrainingStackedPrefixGenerateNucleus(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount,
    UmiAiTrainingStackedPrefixStats *outStats);

/* Same policy for nonempty 260-vocabulary raw-byte prompts and outputs. The
 * canonical byte tokenizer performs all ID conversions; input and result
 * remain binary, without implicit string termination or UTF-8 repair. */
UmiStatus UmiAiTrainingStackedPrefixGenerateBytesNucleus(
    UmiAiTrainingStackedDecoder *model,
    const uint8_t *promptBytes, size_t promptByteCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedByteCount, uint64_t seed,
    uint8_t *outBytes, size_t capacity, size_t *outCount);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_PREFIX_INFERENCE_H */
