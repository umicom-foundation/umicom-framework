/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/generation_cache.c
 *
 * PURPOSE:
 *   Connect the qualified inference-only KV cache to the existing top-p
 *   sampler, preserving the canonical full-context, BOS-padding semantics.
 *
 * ENGINEERING:
 *   No decoder or sampling mathematics is reimplemented. A fresh cache owns
 *   only its inference snapshot, and all generated values are staged before
 *   caller output is published. Every intermediate error destroys the cache
 *   and leaves outputs unchanged. Rolling full windows deliberately rebuild
 *   absolute-position keys/values; see the public header for limitations.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_cache.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/kv_cache.h"

#include <math.h>
#include <string.h>

static UmiStatus CheckGenerationPolicy(
    const UmiAiTrainingDecoderConfig *config,
    const UmiAiTrainingNucleusOptions *policy)
{
    if (config == NULL || policy == NULL ||
        config->contextLength == 0U ||
        config->contextLength > UMI_AI_DECODER_MAX_CONTEXT ||
        config->vocabularySize <= UMI_AI_BYTE_OFFSET ||
        config->vocabularySize > UMI_AI_DECODER_MAX_VOCABULARY ||
        policy->topK == 0U ||
        policy->topK > (size_t)config->vocabularySize - UMI_AI_BYTE_OFFSET ||
        !isfinite(policy->temperature) || policy->temperature <= 0.0F ||
        !isfinite(policy->topP) || policy->topP <= 0.0F ||
        policy->topP > 1.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderGenerateCachedNucleus(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t newTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount,
    UmiAiTrainingGenerationCacheStats *outStats)
{
    UmiAiTrainingDecoderConfig config;
    UmiAiTrainingKvCache *cache = NULL;
    UmiAiTrainingRandomState randomState;
    UmiAiTrainingGenerationCacheStats stagedStats = {0};
    uint64_t completedSteps = 0U;
    size_t generated[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t context[UMI_AI_DECODER_MAX_CONTEXT];
    float logits[UMI_AI_DECODER_MAX_VOCABULARY];
    size_t retained = 0U;
    UmiStatus status;

    if (model == NULL || outTokenIds == NULL || outCount == NULL ||
        (promptTokenCount > 0U && promptTokenIds == NULL) ||
        promptTokenCount > UMI_AI_GENERATION_MAX_PROMPT_TOKENS ||
        newTokenCount == 0U || newTokenCount > UMI_AI_GENERATION_MAX_NEW_TOKENS ||
        (outStats != NULL &&
            ((void *)outStats == (void *)outCount ||
             (void *)outStats == (void *)outTokenIds)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < newTokenCount) return UMI_STATUS_CAPACITY_EXCEEDED;

    status = UmiAiTrainingDecoderInspect(model, &config, &completedSteps);
    if (status != UMI_STATUS_OK) return status;
    (void)completedSteps;
    status = CheckGenerationPolicy(&config, policy);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < promptTokenCount; ++i)
        if (promptTokenIds[i] >= (size_t)config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingRandomSeed(&randomState, seed);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingKvCacheCreate(model, &cache);
    if (status != UMI_STATUS_OK) return status;

    /* Match the ORIGINAL generator's full-length, BOS-left-padded context.
     * Prefilling only the unpadded prefix would change absolute positions. */
    const size_t contextCount = (size_t)config.contextLength;
    const size_t available = promptTokenCount < contextCount ?
                             promptTokenCount : contextCount;
    const size_t padding = contextCount - available;
    for (size_t i = 0U; i < padding; ++i)
        context[i] = UMI_AI_BYTE_BOS;
    for (size_t i = 0U; i < available; ++i)
        context[padding + i] = promptTokenIds[promptTokenCount - available + i];

    status = UmiAiTrainingKvCachePrefill(cache, context, contextCount,
                                        logits, (size_t)config.vocabularySize);
    for (size_t step = 0U; status == UMI_STATUS_OK && step < newTokenCount; ++step) {
        if (step > 0U)
            status = UmiAiTrainingKvCacheAppend(cache, generated[step - 1U],
                                                logits, (size_t)config.vocabularySize);
        if (status != UMI_STATUS_OK) break;
        status = UmiAiTrainingSampleLogitsNucleus(
            logits, (size_t)config.vocabularySize,
            UMI_AI_BYTE_OFFSET, (size_t)config.vocabularySize,
            policy, &randomState, &generated[step], NULL);
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingKvCacheInspect(cache, &retained,
                       &stagedStats.computedRows, &stagedStats.windowRebuilds);
    stagedStats.retainedTokens = retained;
    UmiAiTrainingKvCacheDestroy(cache);
    if (status != UMI_STATUS_OK) return status;

    /* Publish only after the complete sample and cache inspection succeed. */
    memcpy(outTokenIds, generated, newTokenCount * sizeof(*outTokenIds));
    *outCount = newTokenCount;
    if (outStats != NULL) *outStats = stagedStats;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderGenerateCachedBytesNucleus(
    UmiAiTrainingDecoder *model,
    const uint8_t *promptBytes, size_t promptLength,
    const UmiAiTrainingNucleusOptions *policy,
    size_t newByteCount, uint64_t seed,
    uint8_t *outBytes, size_t capacity, size_t *outCount,
    UmiAiTrainingGenerationCacheStats *outStats)
{
    UmiAiTrainingDecoderConfig config;
    UmiAiTrainingGenerationCacheStats stagedStats;
    uint64_t completedSteps = 0U;
    uint32_t encoded[UMI_AI_GENERATION_MAX_PROMPT_TOKENS];
    size_t promptIds[UMI_AI_GENERATION_MAX_PROMPT_TOKENS];
    size_t generated[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    uint32_t generatedIds[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t count = 0U;
    UmiStatus status;

    if (model == NULL || outBytes == NULL || outCount == NULL ||
        (promptLength > 0U && promptBytes == NULL) ||
        promptLength > UMI_AI_GENERATION_MAX_PROMPT_TOKENS ||
        newByteCount == 0U || newByteCount > UMI_AI_GENERATION_MAX_NEW_TOKENS ||
        (outStats != NULL && ((void *)outStats == (void *)outCount ||
                              (void *)outStats == (void *)outBytes)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < newByteCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingDecoderInspect(model, &config, &completedSteps);
    if (status != UMI_STATUS_OK) return status;
    (void)completedSteps;
    if (config.vocabularySize != UMI_AI_BYTE_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;

    if (promptLength > 0U) {
        status = UmiAiByteEncode(promptBytes, promptLength, encoded,
                                UMI_AI_GENERATION_MAX_PROMPT_TOKENS, &count);
        if (status != UMI_STATUS_OK) return status;
        if (count != promptLength) return UMI_STATUS_INVALID_STATE;
        for (size_t i = 0U; i < count; ++i)
            promptIds[i] = (size_t)encoded[i];
    }
    status = UmiAiTrainingDecoderGenerateCachedNucleus(model,
        promptLength > 0U ? promptIds : NULL, promptLength, policy,
        newByteCount, seed, generated, newByteCount, &count, &stagedStats);
    if (status != UMI_STATUS_OK) return status;
    if (count != newByteCount) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < count; ++i)
        generatedIds[i] = (uint32_t)generated[i];
    status = UmiAiByteDecode(generatedIds, count, outBytes, capacity, &count);
    if (status != UMI_STATUS_OK) return status;
    *outCount = count;
    if (outStats != NULL) *outStats = stagedStats;
    return UMI_STATUS_OK;
}
