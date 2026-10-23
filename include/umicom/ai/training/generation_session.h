/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/generation_session.h
 *
 * PURPOSE:
 *   Pull one generated token at a time from the existing, qualified KV
 *   cache and nucleus sampler. Applications can display partial output and
 *   cancel without rebuilding the model or re-seeding between requests.
 *
 * OWNERSHIP AND SAFETY:
 *   Framework owns this streaming implementation. The caller owns the decoder
 *   and retains it, unchanged, until the session is destroyed. A session owns
 *   its own cache and sampling state. Neither it nor its decoder may be used
 *   concurrently. Destroying a session discards generation state only.
 *
 *   Creation and rejected public inputs leave caller outputs untouched.
 *   Next samples one complete token, publishing it only on success. A rare
 *   internal numerical failure after a successful cache advance makes the
 *   session terminal: destroy it instead of retrying a partially advanced
 *   transaction. Ordinary invalid calls and capacity rejections do not
 *   consume tokens or change the cursor.
 *
 *   Like the qualified cached generator, the first prefill is full-length
 *   BOS-left-padded. Learned absolute positions require complete key/value
 *   rebuilding whenever a full context rolls. This interface improves
 *   responsiveness and lifecycle control, NOT proven inference throughput.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_GENERATION_SESSION_H
#define UMICOM_AI_TRAINING_GENERATION_SESSION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/generation_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingGenerationSession UmiAiTrainingGenerationSession;

/* Create a new, independent session against the current decoder weights.
 * prompt IDs follow GenerateCachedNucleus; the supplied prompt is copied
 * into the initial cache, not borrowed after this call. The policy is copied.
 * On failure, *outSession is unchanged. Use Destroy after successful Create. */
UmiStatus UmiAiTrainingGenerationSessionCreate(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingGenerationSession **outSession);

void UmiAiTrainingGenerationSessionDestroy(
    UmiAiTrainingGenerationSession *session);

/* Pull exactly one token ID, using the existing nucleus sampler and RNG.
 * At most UMI_AI_GENERATION_MAX_NEW_TOKENS tokens may be returned by this
 * educational session. A capacity refusal does not mutate *outTokenId.
 * The first four reserved IDs are never emitted. There is no automatic EOS. */
UmiStatus UmiAiTrainingGenerationSessionNext(
    UmiAiTrainingGenerationSession *session, size_t *outTokenId);

/* Convenience pull for the 260-token reversible byte vocabulary. Output is
 * ONE raw byte, not a NUL-terminated C string; invalid UTF-8 is possible.
 * The byte mapping is delegated to the Framework byte tokenizer. */
UmiStatus UmiAiTrainingGenerationSessionNextByte(
    UmiAiTrainingGenerationSession *session, uint8_t *outByte);

/* Inspect successful token count and real KV cache work counters. These
 * measurements exclude the as-yet-unappended last generated token, as does
 * the existing one-shot cached generator. All outputs stay unchanged on
 * failure (including a decoder that has been trained since Create). */
UmiStatus UmiAiTrainingGenerationSessionInspect(
    const UmiAiTrainingGenerationSession *session,
    size_t *outGeneratedTokens,
    UmiAiTrainingGenerationCacheStats *outCacheStats);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_GENERATION_SESSION_H */
