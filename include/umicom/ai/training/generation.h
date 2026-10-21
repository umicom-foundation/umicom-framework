/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/generation.h
 *
 * PURPOSE:
 *   Provide bounded autoregressive generation from the existing native
 *   decoder and deterministic sampling over reviewed token ranges.
 *
 * ARCHITECTURE AND OWNERSHIP:
 *   The Framework retains ownership of tokenisation, inference and seeded
 *   random sampling. No external model runtime or scripting language is used.
 *   The caller serialises access to its decoder and owns all input and output
 *   memory. Failed calls do not modify output buffers or output counts.
 *   Decoder weights and completed optimiser steps are never modified.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_GENERATION_H
#define UMICOM_AI_TRAINING_GENERATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/initialisation.h"

#ifdef __cplusplus
extern "C" {
#endif

/* These are laboratory resource limits, not limits of the future AI runtime. */
#define UMI_AI_GENERATION_MAX_PROMPT_TOKENS 4096U
#define UMI_AI_GENERATION_MAX_NEW_TOKENS 256U

typedef struct UmiAiTrainingSamplingOptions {
    float temperature;
    size_t topK;
} UmiAiTrainingSamplingOptions;

/* Select from the half-open range [firstAllowed, afterLastAllowed).
 * Every eligible logit must be finite. TopK=1 chooses the highest logit,
 * breaking ties by lowest ID; this choice consumes no RNG state.
 * For TopK>1, probabilities are proportional to exp(logit/temperature)
 * over the largest TopK scores. Equal scores prefer the lower token ID.
 * Random selection reuses the Framework's caller-owned seeded RNG.
 * Invalid input preserves both RNG state and *outTokenId.
 * This is an educational non-cryptographic sampler. */
UmiStatus UmiAiTrainingSampleLogits(
    const float *logits, size_t logitCount,
    size_t firstAllowed, size_t afterLastAllowed,
    const UmiAiTrainingSamplingOptions *options,
    UmiAiTrainingRandomState *randomState,
    size_t *outTokenId);

/* Generate exactly newTokenCount tokens, using the decoder's fixed context
 * length and a rolling last-context window. Short prompts are left-padded
 * with the existing byte-token BOS ID; an empty prompt is permitted.
 * The first four reserved byte-token IDs are never emitted. All prompt IDs
 * must be within the decoder's vocabulary. The model must have at least one
 * non-reserved token (vocabularySize >= 5).
 *
 * The implementation invokes the canonical decoder logits contract for
 * every position and copies results only after all steps succeed. It does not
 * stop automatically at EOS because the elementary byte corpus does not yet
 * train a reliable EOS convention. This API does not stream partial output.
 * Output capacity counts size_t token IDs, not bytes. */
UmiStatus UmiAiTrainingDecoderGenerate(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingSamplingOptions *options,
    size_t newTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount);

/* Convenience adapter for the canonical 260-ID reversible byte vocabulary.
 * Any UTF-8 prompt is treated as raw bytes; output may NOT be valid UTF-8.
 * Binary output, including NUL, is preserved exactly. All buffers belong to
 * the caller. The model must have vocabularySize=260. A zero-byte prompt is
 * allowed and is represented only by the Framework BOS padding policy.
 * Return output length separately: do not assume a C string terminator. */
UmiStatus UmiAiTrainingDecoderGenerateBytes(
    UmiAiTrainingDecoder *model,
    const uint8_t *promptBytes, size_t promptLength,
    const UmiAiTrainingSamplingOptions *options,
    size_t newByteCount, uint64_t seed,
    uint8_t *outBytes, size_t capacity, size_t *outCount);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_GENERATION_H */
