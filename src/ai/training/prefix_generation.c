/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/prefix_generation.c
 *
 * PURPOSE:
 *   Compose the existing decoder, KV cache, seeded top-p sampler and byte
 *   tokeniser into a semantically explicit growing-prefix generation mode.
 *
 * ARCHITECTURE:
 *   Only orchestration and safe ownership live here. All attention maths,
 *   allocation sizing, positional rebasing, RNG and sampling are delegated
 *   to their established Framework owners. This implementation does NOT
 *   modify the legacy full-context BOS-left-padded generation policy.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/prefix_generation.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/kv_cache.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiTrainingPrefixGenerationSession {
    UmiAiTrainingKvCache *cache;
    UmiAiTrainingDecoderConfig config;
    UmiAiTrainingNucleusOptions policy;
    UmiAiTrainingRandomState randomState;
    float logits[UMI_AI_DECODER_MAX_VOCABULARY];
    size_t promptTokens;
    size_t generatedTokens;
    size_t pendingToken;
    bool hasPendingToken;
    bool terminal;
};

/* This small boundary check reflects the public nucleus policy accepted by
 * the existing sampler. The sampler itself remains authoritative for random
 * selection and numerical probability calculations. */
static UmiStatus ValidateOptions(
    const UmiAiTrainingDecoderConfig *config,
    const UmiAiTrainingNucleusOptions *options)
{
    if (config == NULL || options == NULL ||
        config->contextLength == 0U ||
        config->contextLength > UMI_AI_DECODER_MAX_CONTEXT ||
        config->vocabularySize <= UMI_AI_BYTE_OFFSET ||
        config->vocabularySize > UMI_AI_DECODER_MAX_VOCABULARY ||
        options->topK == 0U ||
        options->topK > (size_t)config->vocabularySize - UMI_AI_BYTE_OFFSET ||
        !isfinite(options->temperature) || options->temperature <= 0.0F ||
        !isfinite(options->topP) || options->topP <= 0.0F ||
        options->topP > 1.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPrefixGenerationWorkEstimateBuild(
    const UmiAiTrainingDecoderConfig *config,
    size_t promptTokenCount, size_t generatedTokenCount,
    UmiAiTrainingPrefixGenerationWorkEstimate *outEstimate)
{
    UmiAiTrainingPrefixGenerationWorkEstimate candidate = {0};
    size_t reservedBytes = 0U;
    size_t appended, growing, rebuilt;
    UmiStatus status;

    if (config == NULL || outEstimate == NULL ||
        config->contextLength == 0U ||
        config->contextLength > UMI_AI_DECODER_MAX_CONTEXT ||
        promptTokenCount == 0U ||
        promptTokenCount > (size_t)config->contextLength ||
        generatedTokenCount == 0U ||
        generatedTokenCount > UMI_AI_GENERATION_MAX_NEW_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* The existing memory owner is the single validator for decoder shape
     * and cache heap allocation; no layout formula is copied into here. */
    status = UmiAiTrainingKvCacheAllocationForConfig(config, &reservedBytes);
    if (status != UMI_STATUS_OK) return status;

    appended = generatedTokenCount - 1U;  /* Last token is pending. */
    growing = (size_t)config->contextLength - promptTokenCount;
    if (growing > appended) growing = appended;
    rebuilt = appended - growing;

    candidate.promptTokens = promptTokenCount;
    candidate.generatedTokens = generatedTokenCount;
    candidate.contextTokens = (size_t)config->contextLength;
    candidate.cacheHeapBytes = reservedBytes;
    candidate.canonicalComputedRows =
        (uint64_t)config->contextLength * (uint64_t)generatedTokenCount;
    candidate.cachedComputedRows = (uint64_t)promptTokenCount +
        (uint64_t)growing + (uint64_t)rebuilt * (uint64_t)config->contextLength;
    candidate.cachedWindowRebuilds = (uint64_t)rebuilt;
    if (candidate.cachedComputedRows > candidate.canonicalComputedRows)
        return UMI_STATUS_INVALID_STATE;
    candidate.computedRowsAvoided = candidate.canonicalComputedRows -
                                   candidate.cachedComputedRows;
    *outEstimate = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPrefixGenerationSessionCreate(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingPrefixGenerationSession **outSession)
{
    UmiAiTrainingPrefixGenerationSession *candidate;
    UmiAiTrainingDecoderConfig config = {0};
    uint64_t steps = 0U;
    UmiStatus status;

    if (model == NULL || promptTokenIds == NULL || outSession == NULL ||
        promptTokenCount == 0U ||
        promptTokenCount > UMI_AI_DECODER_MAX_CONTEXT)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(model, &config, &steps);
    if (status != UMI_STATUS_OK) return status;
    (void)steps;
    if (promptTokenCount > (size_t)config.contextLength)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = ValidateOptions(&config, policy);
    if (status != UMI_STATUS_OK) return status;
    for (size_t index = 0U; index < promptTokenCount; ++index)
        if (promptTokenIds[index] >= (size_t)config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;

    candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->config = config;
    candidate->policy = *policy;
    candidate->promptTokens = promptTokenCount;

    status = UmiAiTrainingRandomSeed(&candidate->randomState, seed);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingKvCacheCreate(model, &candidate->cache);
    if (status != UMI_STATUS_OK) goto failed;

    /* Important: do NOT prepend BOS as the historical generator does. The
     * actual input prefix occupies learned absolute positions 0..P-1. */
    status = UmiAiTrainingKvCachePrefill(candidate->cache,
        promptTokenIds, promptTokenCount, candidate->logits,
        (size_t)config.vocabularySize);
    if (status != UMI_STATUS_OK) goto failed;

    *outSession = candidate;
    return UMI_STATUS_OK;
failed:
    UmiAiTrainingKvCacheDestroy(candidate->cache);
    free(candidate);
    return status;
}

void UmiAiTrainingPrefixGenerationSessionDestroy(
    UmiAiTrainingPrefixGenerationSession *session)
{
    if (session == NULL) return;
    UmiAiTrainingKvCacheDestroy(session->cache);
    free(session);
}

UmiStatus UmiAiTrainingPrefixGenerationSessionNext(
    UmiAiTrainingPrefixGenerationSession *session, size_t *outTokenId)
{
    UmiAiTrainingRandomState proposedRandom;
    float proposedLogits[UMI_AI_DECODER_MAX_VOCABULARY];
    const float *currentLogits;
    size_t tokenCount = 0U;
    size_t candidateToken = 0U;
    uint64_t rows = 0U, rebuilds = 0U;
    UmiStatus status;

    if (session == NULL || outTokenId == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->terminal) return UMI_STATUS_INVALID_STATE;
    if (session->generatedTokens >= UMI_AI_GENERATION_MAX_NEW_TOKENS)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Inspect first to reject a stale cache after decoder training, without
     * consuming RNG state, output storage, or the pending sample. */
    status = UmiAiTrainingKvCacheInspect(session->cache,
        &tokenCount, &rows, &rebuilds);
    if (status != UMI_STATUS_OK) return status;
    (void)tokenCount;
    (void)rows;
    (void)rebuilds;

    currentLogits = session->logits;
    if (session->hasPendingToken) {
        status = UmiAiTrainingKvCacheAppend(session->cache,
            session->pendingToken, proposedLogits,
            (size_t)session->config.vocabularySize);
        if (status != UMI_STATUS_OK) return status;
        currentLogits = proposedLogits;
    }

    proposedRandom = session->randomState;
    status = UmiAiTrainingSampleLogitsNucleus(
        currentLogits, (size_t)session->config.vocabularySize,
        UMI_AI_BYTE_OFFSET, (size_t)session->config.vocabularySize,
        &session->policy, &proposedRandom, &candidateToken, NULL);
    if (status != UMI_STATUS_OK) {
        /* Append may have succeeded. Do not advance this cursor again after
         * an uncommitted sampling failure; Destroy remains safe. */
        session->terminal = true;
        return status;
    }

    if (currentLogits == proposedLogits)
        memcpy(session->logits, proposedLogits,
               (size_t)session->config.vocabularySize * sizeof(float));
    session->randomState = proposedRandom;
    session->pendingToken = candidateToken;
    session->hasPendingToken = true;
    ++session->generatedTokens;
    *outTokenId = candidateToken;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPrefixGenerationSessionNextByte(
    UmiAiTrainingPrefixGenerationSession *session, uint8_t *outByte)
{
    uint32_t encoded;
    size_t token = 0U, written = 0U;
    uint8_t candidate = 0U;
    UmiStatus status;

    if (session == NULL || outByte == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->config.vocabularySize != UMI_AI_BYTE_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingPrefixGenerationSessionNext(session, &token);
    if (status != UMI_STATUS_OK) return status;
    encoded = (uint32_t)token;
    status = UmiAiByteDecode(&encoded, 1U, &candidate, 1U, &written);
    if (status != UMI_STATUS_OK || written != 1U) {
        session->terminal = true;
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_STATE;
    }
    *outByte = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingPrefixGenerationSessionInspect(
    const UmiAiTrainingPrefixGenerationSession *session,
    size_t *outGeneratedTokens,
    UmiAiTrainingGenerationCacheStats *outCacheStats)
{
    UmiAiTrainingGenerationCacheStats stagedStats = {0};
    UmiStatus status;

    if (session == NULL || outGeneratedTokens == NULL || outCacheStats == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->terminal) return UMI_STATUS_INVALID_STATE;
    status = UmiAiTrainingKvCacheInspect(session->cache,
        &stagedStats.retainedTokens, &stagedStats.computedRows,
        &stagedStats.windowRebuilds);
    if (status != UMI_STATUS_OK) return status;

    /* Both outputs become visible only after the cache has been checked. */
    *outGeneratedTokens = session->generatedTokens;
    *outCacheStats = stagedStats;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderGeneratePrefixNucleus(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount,
    UmiAiTrainingGenerationCacheStats *outStats)
{
    UmiAiTrainingPrefixGenerationSession *session = NULL;
    UmiAiTrainingPrefixGenerationWorkEstimate expected = {0};
    UmiAiTrainingGenerationCacheStats observed = {0};
    UmiAiTrainingDecoderConfig config = {0};
    uint64_t steps = 0U;
    size_t produced = 0U;
    size_t generated[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    UmiStatus status;

    if (model == NULL || outTokenIds == NULL || outCount == NULL ||
        promptTokenIds == NULL || generatedTokenCount == 0U ||
        generatedTokenCount > UMI_AI_GENERATION_MAX_NEW_TOKENS ||
        (void *)outTokenIds == (void *)outCount ||
        (outStats != NULL &&
         ((void *)outStats == (void *)outTokenIds ||
          (void *)outStats == (void *)outCount)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < generatedTokenCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    status = UmiAiTrainingDecoderInspect(model, &config, &steps);
    if (status != UMI_STATUS_OK) return status;
    (void)steps;
    status = UmiAiTrainingPrefixGenerationWorkEstimateBuild(
        &config, promptTokenCount, generatedTokenCount, &expected);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingPrefixGenerationSessionCreate(model,
        promptTokenIds, promptTokenCount, policy, seed, &session);
    if (status != UMI_STATUS_OK) return status;

    for (size_t index = 0U; index < generatedTokenCount; ++index) {
        status = UmiAiTrainingPrefixGenerationSessionNext(session,
            &generated[index]);
        if (status != UMI_STATUS_OK) goto cleanup;
    }
    status = UmiAiTrainingPrefixGenerationSessionInspect(session,
        &produced, &observed);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (produced != generatedTokenCount ||
        observed.computedRows != expected.cachedComputedRows ||
        observed.windowRebuilds != expected.cachedWindowRebuilds ||
        observed.retainedTokens !=
            ((promptTokenCount + generatedTokenCount - 1U < expected.contextTokens)
                 ? promptTokenCount + generatedTokenCount - 1U
                 : expected.contextTokens)) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }

    /* Failed calls may not publish partial results. */
    memcpy(outTokenIds, generated, generatedTokenCount * sizeof(size_t));
    *outCount = generatedTokenCount;
    if (outStats != NULL) *outStats = observed;
    status = UMI_STATUS_OK;
cleanup:
    UmiAiTrainingPrefixGenerationSessionDestroy(session);
    return status;
}
