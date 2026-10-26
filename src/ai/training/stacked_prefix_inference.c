/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_prefix_inference.c
 * PURPOSE: Stream an unpadded prefix through the established stacked decoder
 *          without introducing another attention, sampler or checkpoint codec.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_prefix_inference.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiTrainingStackedPrefixSession {
    UmiAiTrainingStackedDecoder *model;
    UmiAiTrainingStackedModelIdentity pinned;
    UmiAiTrainingNucleusOptions policy;
    UmiAiTrainingRandomState random;
    size_t retained[UMI_AI_STACKED_MAX_CONTEXT];
    size_t retainedCount;
    size_t initialPromptCount;
    size_t generatedCount;
    size_t pendingToken;
    uint64_t evictions;
    int hasPending;
};

static UmiStatus PrefixCheckSession(
    const UmiAiTrainingStackedPrefixSession *session)
{
    if (session == NULL || session->model == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiTrainingStackedModelIdentityVerify(
        session->model, &session->pinned);
}

void UmiAiTrainingStackedPrefixSessionDestroy(
    UmiAiTrainingStackedPrefixSession *session)
{
    free(session);
}

UmiStatus UmiAiTrainingStackedPrefixSessionCreate(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingStackedPrefixSession **outSession)
{
    UmiAiTrainingStackedPrefixSession *candidate = NULL;
    UmiAiTrainingStackedDecoderInfo info = {0};
    UmiAiTrainingRandomState testRandom = {0};
    float neutral[UMI_AI_STACKED_MAX_VOCABULARY] = {0};
    size_t testToken = 0U;
    UmiStatus status;
    if (model == NULL || promptTokenIds == NULL || policy == NULL ||
        outSession == NULL || *outSession != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedDecoderInspect(model, &info);
    if (status != UMI_STATUS_OK) return status;
    size_t context = (size_t)info.config.contextLength;
    size_t vocabulary = (size_t)info.config.vocabularySize;
    if (context == 0U || context > UMI_AI_STACKED_MAX_CONTEXT ||
        promptTokenCount == 0U || promptTokenCount > context ||
        vocabulary <= UMI_AI_BYTE_OFFSET ||
        vocabulary > UMI_AI_STACKED_MAX_VOCABULARY ||
        info.config.transformerLayers == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < promptTokenCount; ++index)
        if (promptTokenIds[index] >= vocabulary)
            return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingRandomSeed(&testRandom, seed);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingSampleLogitsNucleus(
        neutral, vocabulary, UMI_AI_BYTE_OFFSET, vocabulary,
        policy, &testRandom, &testToken, NULL);
    if (status != UMI_STATUS_OK) return status;

    candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->model = model;
    candidate->policy = *policy;
    candidate->retainedCount = promptTokenCount;
    candidate->initialPromptCount = promptTokenCount;
    memcpy(candidate->retained, promptTokenIds,
           promptTokenCount * sizeof(*promptTokenIds));
    status = UmiAiTrainingRandomSeed(&candidate->random, seed);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedModelIdentityCapture(model,
                                                          &candidate->pinned);
    if (status == UMI_STATUS_OK)
        status = PrefixCheckSession(candidate);
    if (status != UMI_STATUS_OK) {
        UmiAiTrainingStackedPrefixSessionDestroy(candidate);
        return status;
    }
    *outSession = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedPrefixSessionNext(
    UmiAiTrainingStackedPrefixSession *session, size_t *outTokenId)
{
    size_t candidateHistory[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    size_t fullContext[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    UmiAiTrainingRandomState candidateRandom;
    float *allLogits = NULL;
    size_t proposedToken = 0U;
    size_t retained, context, vocabulary;
    uint64_t evictions;
    UmiStatus status;
    if (session == NULL || outTokenId == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->generatedCount >= UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = PrefixCheckSession(session);
    if (status != UMI_STATUS_OK) return status;
    context = (size_t)session->pinned.config.contextLength;
    vocabulary = (size_t)session->pinned.config.vocabularySize;
    retained = session->retainedCount;
    evictions = session->evictions;
    if (context == 0U || context > UMI_AI_STACKED_MAX_CONTEXT ||
        retained == 0U || retained > context ||
        vocabulary == 0U || vocabulary > UMI_AI_STACKED_MAX_VOCABULARY)
        return UMI_STATUS_INVALID_STATE;
    memcpy(candidateHistory, session->retained,
           retained * sizeof(candidateHistory[0]));

    /* The previous output becomes context ONLY when the next sample is
     * requested. After the last emitted token there is no wasted append. */
    if (session->hasPending) {
        if (retained < context)
            candidateHistory[retained++] = session->pendingToken;
        else {
            memmove(candidateHistory, candidateHistory + 1U,
                    (context - 1U) * sizeof(candidateHistory[0]));
            candidateHistory[context - 1U] = session->pendingToken;
            ++evictions;
        }
    }

    memcpy(fullContext, candidateHistory,
           retained * sizeof(fullContext[0]));
    for (size_t position = retained; position < context; ++position)
        fullContext[position] = UMI_AI_BYTE_BOS;

    allLogits = malloc(context * vocabulary * sizeof(*allLogits));
    if (allLogits == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingStackedDecoderLogits(session->model,
                  fullContext, context, allLogits, context * vocabulary);
    candidateRandom = session->random;
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingSampleLogitsNucleus(
            &allLogits[(retained - 1U) * vocabulary], vocabulary,
            UMI_AI_BYTE_OFFSET, vocabulary, &session->policy,
            &candidateRandom, &proposedToken, NULL);
    free(allLogits);
    if (status != UMI_STATUS_OK) return status;
    if (proposedToken < UMI_AI_BYTE_OFFSET || proposedToken >= vocabulary)
        return UMI_STATUS_INVALID_STATE;
    status = PrefixCheckSession(session);
    if (status != UMI_STATUS_OK) return status;

    memcpy(session->retained, candidateHistory,
           retained * sizeof(candidateHistory[0]));
    session->retainedCount = retained;
    session->evictions = evictions;
    session->random = candidateRandom;
    session->pendingToken = proposedToken;
    session->hasPending = 1;
    ++session->generatedCount;
    *outTokenId = proposedToken;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedPrefixSessionNextByte(
    UmiAiTrainingStackedPrefixSession *session, uint8_t *outByte)
{
    size_t token = 0U;
    UmiStatus status;
    if (session == NULL || outByte == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->pinned.config.vocabularySize != UMI_AI_BYTE_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedPrefixSessionNext(session, &token);
    if (status != UMI_STATUS_OK) return status;
    /* The established tokenizer remains the only byte/ID mapping owner. */
    uint32_t encoded = (uint32_t)token;
    uint8_t candidate = 0U;
    size_t written = 0U;
    status = UmiAiByteDecode(&encoded, 1U, &candidate, 1U, &written);
    if (status != UMI_STATUS_OK || written != 1U)
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    *outByte = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedPrefixSessionInspect(
    const UmiAiTrainingStackedPrefixSession *session,
    UmiAiTrainingStackedPrefixStats *outStats)
{
    UmiAiTrainingStackedPrefixStats candidate = {0};
    UmiStatus status;
    if (outStats == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = PrefixCheckSession(session);
    if (status != UMI_STATUS_OK) return status;
    size_t context = (size_t)session->pinned.config.contextLength;
    size_t layers = (size_t)session->pinned.config.transformerLayers;
    candidate.initialPromptTokens = session->initialPromptCount;
    candidate.generatedTokens = session->generatedCount;
    candidate.retainedTokens = session->retainedCount;
    candidate.contextTokens = context;
    candidate.transformerLayers = layers;
    candidate.computedFullContextTokenRows = (uint64_t)context *
                                                (uint64_t)session->generatedCount;
    candidate.computedTransformerTokenLayerRows =
        candidate.computedFullContextTokenRows * (uint64_t)layers;
    candidate.fullWindowRebuilds = session->evictions;
    candidate.modelIdentity = session->pinned;
    *outStats = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedPrefixGenerateNucleus(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount,
    UmiAiTrainingStackedPrefixStats *outStats)
{
    UmiAiTrainingStackedPrefixSession *session = NULL;
    UmiAiTrainingStackedPrefixStats stagedStats = {0};
    size_t stagedTokens[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    UmiStatus status;
    if (model == NULL || outTokenIds == NULL || outCount == NULL ||
        generatedTokenCount == 0U ||
        generatedTokenCount > UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < generatedTokenCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingStackedPrefixSessionCreate(
        model, promptTokenIds, promptTokenCount, policy, seed, &session);
    if (status != UMI_STATUS_OK) return status;
    for (size_t step = 0U; step < generatedTokenCount; ++step) {
        status = UmiAiTrainingStackedPrefixSessionNext(
            session, &stagedTokens[step]);
        if (status != UMI_STATUS_OK) break;
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedPrefixSessionInspect(session,
                                                           &stagedStats);
    UmiAiTrainingStackedPrefixSessionDestroy(session);
    if (status != UMI_STATUS_OK) return status;
    if (stagedStats.generatedTokens != generatedTokenCount ||
        stagedStats.computedFullContextTokenRows !=
            (uint64_t)stagedStats.contextTokens * generatedTokenCount)
        return UMI_STATUS_INVALID_STATE;
    memcpy(outTokenIds, stagedTokens,
           generatedTokenCount * sizeof(stagedTokens[0]));
    *outCount = generatedTokenCount;
    if (outStats != NULL) *outStats = stagedStats;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedPrefixGenerateBytesNucleus(
    UmiAiTrainingStackedDecoder *model,
    const uint8_t *promptBytes, size_t promptByteCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedByteCount, uint64_t seed,
    uint8_t *outBytes, size_t capacity, size_t *outCount)
{
    UmiAiTrainingStackedDecoderInfo info = {0};
    uint32_t encoded[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    uint32_t ids[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    size_t prompt[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    size_t generated[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    uint8_t staged[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    size_t count = 0U, written = 0U;
    UmiStatus status;
    if (model == NULL || promptBytes == NULL || outBytes == NULL ||
        outCount == NULL || promptByteCount == 0U ||
        generatedByteCount == 0U ||
        generatedByteCount > UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < generatedByteCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingStackedDecoderInspect(model, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.config.vocabularySize != UMI_AI_BYTE_VOCABULARY ||
        promptByteCount > (size_t)info.config.contextLength ||
        promptByteCount > UMI_AI_STACKED_MAX_CONTEXT)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiByteEncode(promptBytes, promptByteCount,
        encoded, UMI_AI_STACKED_MAX_CONTEXT, &count);
    if (status != UMI_STATUS_OK || count != promptByteCount)
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    for (size_t index = 0U; index < count; ++index)
        prompt[index] = (size_t)encoded[index];
    status = UmiAiTrainingStackedPrefixGenerateNucleus(
        model, prompt, count, policy, generatedByteCount, seed,
        generated, UMI_AI_STACKED_MAX_GENERATED_TOKENS, &written, NULL);
    if (status != UMI_STATUS_OK || written != generatedByteCount)
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    for (size_t index = 0U; index < written; ++index)
        ids[index] = (uint32_t)generated[index];
    status = UmiAiByteDecode(ids, written, staged,
                              sizeof(staged), &count);
    if (status != UMI_STATUS_OK || count != generatedByteCount)
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    memcpy(outBytes, staged, count);
    *outCount = count;
    return UMI_STATUS_OK;
}
