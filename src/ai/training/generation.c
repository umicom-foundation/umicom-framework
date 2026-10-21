/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/generation.c
 *
 * PURPOSE:
 *   Implement deterministic native next-token sampling and bounded decoder
 *   inference while reusing the canonical training tape, tokeniser and RNG.
 *
 * DESIGN:
 *   Deterministic tie ordering, finite-logit validation, stable exponentials,
 *   checked capacities, no global random state and no partial caller writes.
 *   Generated token history remains local and is published on complete success.
 *   Model weights are not changed. This is CPU education, not a production
 *   multilingual coding assistant or a security-grade random generator.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef struct UmiAiTrainingCandidate {
    double logit;
    size_t tokenId;
} UmiAiTrainingCandidate;

UmiStatus UmiAiTrainingSampleLogits(
    const float *logits, size_t logitCount,
    size_t firstAllowed, size_t afterLastAllowed,
    const UmiAiTrainingSamplingOptions *options,
    UmiAiTrainingRandomState *randomState,
    size_t *outTokenId)
{
    UmiAiTrainingCandidate selected[UMI_AI_DECODER_MAX_VOCABULARY];
    double weights[UMI_AI_DECODER_MAX_VOCABULARY];
    size_t selectedCount = 0U;
    size_t topK;

    if (logits == NULL || options == NULL || outTokenId == NULL ||
        logitCount == 0U || logitCount > UMI_AI_DECODER_MAX_VOCABULARY ||
        firstAllowed >= afterLastAllowed || afterLastAllowed > logitCount ||
        options->topK == 0U ||
        options->topK > afterLastAllowed - firstAllowed ||
        !isfinite(options->temperature) || options->temperature <= 0.0F ||
        (options->topK > 1U && randomState == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    topK = options->topK;
    /* Find the best K scores by descending logit and ascending token ID.
     * No unstable library sorting or floating-point equality tolerance is
     * used: deterministic ties are part of the public sampling contract. */
    for (size_t tokenId = firstAllowed; tokenId < afterLastAllowed; ++tokenId) {
        double candidate = (double)logits[tokenId];
        size_t position = 0U;
        if (!isfinite(candidate)) return UMI_STATUS_INVALID_ARGUMENT;
        while (position < selectedCount &&
               (selected[position].logit > candidate ||
                (selected[position].logit == candidate &&
                 selected[position].tokenId < tokenId))) {
            ++position;
        }
        if (position >= topK) continue;
        if (selectedCount < topK) ++selectedCount;
        for (size_t shift = selectedCount - 1U; shift > position; --shift) {
            selected[shift] = selected[shift - 1U];
        }
        selected[position].logit = candidate;
        selected[position].tokenId = tokenId;
    }
    if (selectedCount != topK) return UMI_STATUS_INVALID_STATE;
    if (topK == 1U) {
        *outTokenId = selected[0].tokenId;
        return UMI_STATUS_OK;
    }

    /* Subtract the maximum before exp() to avoid positive exponent overflow.
     * A very low temperature can underflow less likely terms to zero, which
     * correctly approaches greedy selection and leaves a positive sum. */
    double total = 0.0;
    for (size_t index = 0U; index < topK; ++index) {
        double exponent = (selected[index].logit - selected[0].logit) /
                          (double)options->temperature;
        weights[index] = exp(exponent);
        total += weights[index];
    }
    if (!isfinite(total) || total <= 0.0) return UMI_STATUS_INVALID_STATE;

    /* All validation and numerical work precedes the RNG draw. */
    UmiAiTrainingRandomState proposed = *randomState;
    float unit = 0.0F;
    UmiStatus status = UmiAiTrainingRandomUniform(
        &proposed, 0.0F, 1.0F, &unit);
    if (status != UMI_STATUS_OK) return status;
    double threshold = (double)unit * total;
    double cumulative = 0.0;
    size_t chosen = selected[topK - 1U].tokenId;
    for (size_t index = 0U; index < topK; ++index) {
        cumulative += weights[index];
        if (threshold < cumulative) {
            chosen = selected[index].tokenId;
            break;
        }
    }
    *randomState = proposed;
    *outTokenId = chosen;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderGenerate(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingSamplingOptions *options,
    size_t newTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount)
{
    UmiAiTrainingDecoderConfig config;
    uint64_t completedSteps = 0U;
    size_t generated[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t context[UMI_AI_DECODER_MAX_CONTEXT];
    float logits[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_VOCABULARY];
    UmiAiTrainingRandomState randomState;
    UmiStatus status;

    if (model == NULL || options == NULL || outTokenIds == NULL ||
        outCount == NULL ||
        (promptTokenCount > 0U && promptTokenIds == NULL) ||
        promptTokenCount > UMI_AI_GENERATION_MAX_PROMPT_TOKENS ||
        newTokenCount == 0U ||
        newTokenCount > UMI_AI_GENERATION_MAX_NEW_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < newTokenCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingDecoderInspect(model, &config, &completedSteps);
    if (status != UMI_STATUS_OK) return status;
    (void)completedSteps;
    if (config.contextLength == 0U ||
        config.contextLength > UMI_AI_DECODER_MAX_CONTEXT ||
        config.vocabularySize <= UMI_AI_BYTE_OFFSET ||
        config.vocabularySize > UMI_AI_DECODER_MAX_VOCABULARY ||
        options->topK == 0U ||
        options->topK > (size_t)config.vocabularySize - UMI_AI_BYTE_OFFSET ||
        !isfinite(options->temperature) || options->temperature <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < promptTokenCount; ++index) {
        if (promptTokenIds[index] >= (size_t)config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = UmiAiTrainingRandomSeed(&randomState, seed);
    if (status != UMI_STATUS_OK) return status;

    for (size_t step = 0U; step < newTokenCount; ++step) {
        const size_t historyLength = promptTokenCount + step;
        const size_t available = historyLength < (size_t)config.contextLength ?
                                 historyLength : (size_t)config.contextLength;
        const size_t missing = (size_t)config.contextLength - available;
        for (size_t index = 0U; index < missing; ++index)
            context[index] = UMI_AI_BYTE_BOS;
        for (size_t index = 0U; index < available; ++index) {
            size_t historyIndex = historyLength - available + index;
            context[missing + index] = historyIndex < promptTokenCount ?
                promptTokenIds[historyIndex] :
                generated[historyIndex - promptTokenCount];
        }
        status = UmiAiTrainingDecoderLogits(model, context,
                    (size_t)config.contextLength, logits,
                    (size_t)config.contextLength * (size_t)config.vocabularySize);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingSampleLogits(
            &logits[((size_t)config.contextLength - 1U) *
                    (size_t)config.vocabularySize],
            (size_t)config.vocabularySize,
            UMI_AI_BYTE_OFFSET, (size_t)config.vocabularySize,
            options, &randomState, &generated[step]);
        if (status != UMI_STATUS_OK) return status;
    }
    memcpy(outTokenIds, generated, newTokenCount * sizeof(*outTokenIds));
    *outCount = newTokenCount;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderGenerateBytes(
    UmiAiTrainingDecoder *model,
    const uint8_t *promptBytes, size_t promptLength,
    const UmiAiTrainingSamplingOptions *options,
    size_t newByteCount, uint64_t seed,
    uint8_t *outBytes, size_t capacity, size_t *outCount)
{
    UmiAiTrainingDecoderConfig config;
    uint64_t completedSteps;
    uint32_t sourceIds[UMI_AI_GENERATION_MAX_PROMPT_TOKENS];
    size_t promptIds[UMI_AI_GENERATION_MAX_PROMPT_TOKENS];
    size_t generated[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    uint32_t generatedIds[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t produced = 0U;
    UmiStatus status;

    if (model == NULL || outBytes == NULL || outCount == NULL ||
        (promptLength > 0U && promptBytes == NULL) ||
        promptLength > UMI_AI_GENERATION_MAX_PROMPT_TOKENS ||
        newByteCount == 0U || newByteCount > UMI_AI_GENERATION_MAX_NEW_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < newByteCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingDecoderInspect(model, &config, &completedSteps);
    if (status != UMI_STATUS_OK) return status;
    (void)completedSteps;
    if (config.vocabularySize != UMI_AI_BYTE_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (promptLength > 0U) {
        status = UmiAiByteEncode(promptBytes, promptLength, sourceIds,
                                UMI_AI_GENERATION_MAX_PROMPT_TOKENS, &produced);
        if (status != UMI_STATUS_OK || produced != promptLength)
            return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
        for (size_t index = 0U; index < produced; ++index)
            promptIds[index] = (size_t)sourceIds[index];
    }
    status = UmiAiTrainingDecoderGenerate(model,
        promptLength > 0U ? promptIds : NULL, promptLength, options,
        newByteCount, seed, generated, newByteCount, &produced);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < produced; ++index)
        generatedIds[index] = (uint32_t)generated[index];
    /* The tokenizer validates the whole sequence before writing to the
     * caller. Reserved IDs can never reach this adapter from our sampler. */
    return UmiAiByteDecode(generatedIds, produced, outBytes, capacity, outCount);
}
