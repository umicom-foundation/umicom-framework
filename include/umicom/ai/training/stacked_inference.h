/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_inference.h
 * PURPOSE: Bounded, seeded streaming and one-shot stacked Transformer inference.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_INFERENCE_H
#define UMICOM_AI_TRAINING_STACKED_INFERENCE_H

#include "umicom/ai/training/generation.h"
#include "umicom/ai/training/stacked_model_identity.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_STACKED_INFERENCE_MAX_PROMPT_TOKENS 4096U

typedef struct UmiAiTrainingStackedGenerationSession
    UmiAiTrainingStackedGenerationSession;

typedef struct UmiAiTrainingStackedGenerationStats {
    size_t generatedTokens;
    size_t contextTokens;
    size_t transformerLayers;
    uint64_t computedFullContextTokenRows;
    uint64_t computedTransformerTokenLayerRows;
    UmiAiTrainingStackedModelIdentity modelIdentity;
} UmiAiTrainingStackedGenerationStats;

/* Explicitly use the same BOS-LEFT-PADDED, full fixed context on EVERY pull
 * as the historical stacked greedy generator. Context eviction shifts tokens
 * left and recomputes every Transformer layer. There is NO KV cache and NO
 * claim of a speedup; counts represent requested layer-token rows, not FLOPs.
 *
 * The established Framework top-K/top-p sampler and deterministic RNG own
 * sampling. Eligible outputs are token IDs [4,vocabularySize): the reserved
 * PAD/BOS/EOS/UNK identifiers are NEVER emitted. Generation never auto-stops
 * at EOS. Any returned byte can be invalid UTF-8: treat output as binary.
 *
 * Prompt IDs are validated in full, even those outside the retained context.
 * 0..4096 prompt IDs and a model with vocabulary >=5 are accepted. Session
 * borrows an unchanged model, copies its context, policy and RNG, and pins
 * the canonical SHA-256 of its checkpoint. Training or replacing the model
 * before Destroy invalidates subsequent calls. Callers serialise model use.
 * Initialise *outSession=NULL. Rejection does not modify it. */
UmiStatus UmiAiTrainingStackedGenerationSessionCreate(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingStackedGenerationSession **outSession);

/* Destroy is the explicit cancellation operation. It does not update or
 * destroy the borrowed model and never emits a half-finished generated token. */
void UmiAiTrainingStackedGenerationSessionDestroy(
    UmiAiTrainingStackedGenerationSession *session);

/* Next publishes ONE complete sampled token or leaves *outTokenId untouched.
 * Maximum 64 successful pulls per session. No tape tensors survive a call.
 * Invalid public calls do not consume RNG or advance the context. */
UmiStatus UmiAiTrainingStackedGenerationSessionNext(
    UmiAiTrainingStackedGenerationSession *session, size_t *outTokenId);

/* Only valid for the Framework's exact 260-ID byte vocabulary. The result is
 * one raw byte, not a character or null-terminated string. */
UmiStatus UmiAiTrainingStackedGenerationSessionNextByte(
    UmiAiTrainingStackedGenerationSession *session, uint8_t *outByte);

/* Inspect read-only workload and the pinned model identity. Hash verification
 * is repeated; stale models are refused. Failure does not change outStats. */
UmiStatus UmiAiTrainingStackedGenerationSessionInspect(
    const UmiAiTrainingStackedGenerationSession *session,
    UmiAiTrainingStackedGenerationStats *outStats);

/* Atomic one-shot composition of Create, Next, Inspect and Destroy. Output
 * capacity is measured in size_t tokens; count must be 1..64. Caller output
 * objects must not overlap. Failed requests leave ALL outputs untouched. */
UmiStatus UmiAiTrainingStackedGenerateNucleus(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokens, size_t capacity, size_t *outCount,
    UmiAiTrainingStackedGenerationStats *outStats);

/* Convenience binary byte boundary for vocabulary 260: tokenize and decode
 * through the existing Framework byte tokenizer. Embedded NUL and invalid
 * UTF-8 are not discarded or reformatted. No parser or code execution. */
UmiStatus UmiAiTrainingStackedGenerateBytesNucleus(
    UmiAiTrainingStackedDecoder *model,
    const uint8_t *promptBytes, size_t promptByteCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedByteCount, uint64_t seed,
    uint8_t *outBytes, size_t capacity, size_t *outCount);

#ifdef __cplusplus
}
#endif
#endif
