/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/generation_session.c
 *
 * PURPOSE:
 *   Persist a small, caller-controlled C23 inference cursor across one-token
 *   requests using the canonical decoder KV cache and top-p sampler.
 *
 * ARCHITECTURE:
 *   The decoder, KV cache, random generator and byte tokenizer remain their
 *   existing Framework owners. This file provides lifecycle orchestration,
 *   never reimplements model mathematics, random sampling or tokenisation.
 *   A returned token is pending: append it to the cache at the NEXT pull,
 *   matching the existing one-shot cached-generation sequence exactly.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_session.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/kv_cache.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiTrainingGenerationSession {
    UmiAiTrainingKvCache *cache;
    UmiAiTrainingDecoderConfig config;
    UmiAiTrainingNucleusOptions policy;
    UmiAiTrainingRandomState randomState;
    float logits[UMI_AI_DECODER_MAX_VOCABULARY];
    size_t generatedTokens;
    size_t pendingToken;
    bool hasPendingToken;
    bool terminal;
};

static UmiStatus ValidatePolicy(
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

UmiStatus UmiAiTrainingGenerationSessionCreate(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingGenerationSession **outSession)
{
    UmiAiTrainingGenerationSession *candidate;
    UmiAiTrainingDecoderConfig config;
    uint64_t completedSteps = 0U;
    size_t context[UMI_AI_DECODER_MAX_CONTEXT];
    size_t available, padding, contextCount;
    UmiStatus status;

    if (model == NULL || outSession == NULL ||
        (promptTokenCount != 0U && promptTokenIds == NULL) ||
        promptTokenCount > UMI_AI_GENERATION_MAX_PROMPT_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(model, &config, &completedSteps);
    if (status != UMI_STATUS_OK) return status;
    (void)completedSteps;
    status = ValidatePolicy(&config, policy);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < promptTokenCount; ++i) {
        if (promptTokenIds[i] >= (size_t)config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->config = config;
    candidate->policy = *policy;
    status = UmiAiTrainingRandomSeed(&candidate->randomState, seed);
    if (status != UMI_STATUS_OK) goto failed;
    status = UmiAiTrainingKvCacheCreate(model, &candidate->cache);
    if (status != UMI_STATUS_OK) goto failed;

    /* Align the first predicted logit with the one-shot canonical generator.
     * Filling only the prompt length would shift learned position embeddings. */
    contextCount = (size_t)config.contextLength;
    available = promptTokenCount < contextCount ? promptTokenCount : contextCount;
    padding = contextCount - available;
    for (size_t i = 0U; i < padding; ++i)
        context[i] = UMI_AI_BYTE_BOS;
    for (size_t i = 0U; i < available; ++i)
        context[padding + i] = promptTokenIds[promptTokenCount - available + i];
    status = UmiAiTrainingKvCachePrefill(candidate->cache,
        context, contextCount, candidate->logits, (size_t)config.vocabularySize);
    if (status != UMI_STATUS_OK) goto failed;

    *outSession = candidate;
    return UMI_STATUS_OK;
failed:
    UmiAiTrainingKvCacheDestroy(candidate->cache);
    free(candidate);
    return status;
}

void UmiAiTrainingGenerationSessionDestroy(
    UmiAiTrainingGenerationSession *session)
{
    if (session == NULL) return;
    UmiAiTrainingKvCacheDestroy(session->cache);
    free(session);
}

UmiStatus UmiAiTrainingGenerationSessionNext(
    UmiAiTrainingGenerationSession *session, size_t *outTokenId)
{
    UmiAiTrainingRandomState proposedRandom;
    float proposedLogits[UMI_AI_DECODER_MAX_VOCABULARY];
    const float *currentLogits;
    size_t tokenCount = 0U;
    uint64_t computedRows = 0U, windowRebuilds = 0U;
    size_t selected = 0U;
    UmiStatus status;

    if (session == NULL || outTokenId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->terminal) return UMI_STATUS_INVALID_STATE;
    if (session->generatedTokens >= UMI_AI_GENERATION_MAX_NEW_TOKENS)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Inspect checks the cache/decoder lifetime and unchanged optimiser step.
     * A post-training stale cache is rejected BEFORE any mutation. */
    status = UmiAiTrainingKvCacheInspect(session->cache,
        &tokenCount, &computedRows, &windowRebuilds);
    if (status != UMI_STATUS_OK) return status;
    (void)tokenCount;
    (void)computedRows;
    (void)windowRebuilds;

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
        &session->policy, &proposedRandom, &selected, NULL);
    if (status != UMI_STATUS_OK) {
        /* CacheAppend can already have advanced. Refuse further requests
         * rather than letting a caller accidentally skip a position. */
        session->terminal = true;
        return status;
    }
    if (currentLogits == proposedLogits)
        memcpy(session->logits, proposedLogits,
            (size_t)session->config.vocabularySize * sizeof(float));
    session->randomState = proposedRandom;
    session->pendingToken = selected;
    session->hasPendingToken = true;
    ++session->generatedTokens;
    *outTokenId = selected;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingGenerationSessionNextByte(
    UmiAiTrainingGenerationSession *session, uint8_t *outByte)
{
    uint32_t byteToken;
    uint8_t candidate = 0U;
    size_t produced = 0U;
    size_t tokenId = 0U;
    UmiStatus status;
    if (session == NULL || outByte == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (session->config.vocabularySize != UMI_AI_BYTE_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingGenerationSessionNext(session, &tokenId);
    if (status != UMI_STATUS_OK) return status;
    /* A valid sampler output is always within the non-reserved byte range.
     * Decode through the canonical Framework byte-token service. */
    byteToken = (uint32_t)tokenId;
    status = UmiAiByteDecode(&byteToken, 1U, &candidate, 1U, &produced);
    if (status != UMI_STATUS_OK || produced != 1U) {
        session->terminal = true;
        return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_STATE;
    }
    *outByte = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingGenerationSessionInspect(
    const UmiAiTrainingGenerationSession *session,
    size_t *outGeneratedTokens,
    UmiAiTrainingGenerationCacheStats *outCacheStats)
{
    UmiAiTrainingGenerationCacheStats statistics = {0};
    UmiStatus status;
    if (session == NULL || outGeneratedTokens == NULL || outCacheStats == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->terminal) return UMI_STATUS_INVALID_STATE;
    status = UmiAiTrainingKvCacheInspect(session->cache,
        &statistics.retainedTokens, &statistics.computedRows,
        &statistics.windowRebuilds);
    if (status != UMI_STATUS_OK) return status;
    *outGeneratedTokens = session->generatedTokens;
    *outCacheStats = statistics;
    return UMI_STATUS_OK;
}
