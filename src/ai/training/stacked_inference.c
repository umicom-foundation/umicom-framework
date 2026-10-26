/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_inference.c
 * PURPOSE: Reuse canonical stacked logits, nucleus sampler and byte tokeniser.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_inference.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiTrainingStackedGenerationSession {
    UmiAiTrainingStackedDecoder *model;
    UmiAiTrainingStackedModelIdentity pinned;
    UmiAiTrainingNucleusOptions policy;
    UmiAiTrainingRandomState randomState;
    size_t context[UMI_AI_STACKED_MAX_CONTEXT];
    size_t generatedTokens;
};

static UmiStatus SessionValid(
    const UmiAiTrainingStackedGenerationSession *session)
{
    if (session == NULL || session->model == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiTrainingStackedModelIdentityVerify(
        session->model, &session->pinned);
}

void UmiAiTrainingStackedGenerationSessionDestroy(
    UmiAiTrainingStackedGenerationSession *session)
{
    free(session);
}

UmiStatus UmiAiTrainingStackedGenerationSessionCreate(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingStackedGenerationSession **outSession)
{
    UmiAiTrainingStackedGenerationSession *created = NULL;
    UmiAiTrainingStackedDecoderInfo modelInfo = {0};
    UmiAiTrainingRandomState trialRandom = {0};
    float equalLogits[UMI_AI_STACKED_MAX_VOCABULARY] = {0};
    size_t trialToken = 0U;
    size_t contextLength, vocabulary;
    UmiStatus status;
    if (model == NULL || policy == NULL || outSession == NULL ||
        *outSession != NULL ||
        promptTokenCount > UMI_AI_STACKED_INFERENCE_MAX_PROMPT_TOKENS ||
        (promptTokenCount != 0U && promptTokenIds == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedDecoderInspect(model, &modelInfo);
    if (status != UMI_STATUS_OK) return status;
    contextLength = (size_t)modelInfo.config.contextLength;
    vocabulary = (size_t)modelInfo.config.vocabularySize;
    if (contextLength == 0U || contextLength > UMI_AI_STACKED_MAX_CONTEXT ||
        vocabulary < UMI_AI_BYTE_OFFSET + 1U ||
        vocabulary > UMI_AI_STACKED_MAX_VOCABULARY)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < promptTokenCount; ++i)
        if (promptTokenIds[i] >= vocabulary)
            return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingRandomSeed(&trialRandom, seed);
    if (status != UMI_STATUS_OK) return status;
    /* Let the EXISTING sampler validate policy, allowed interval and any
     * special handling of deterministic top-K=1; discard the trial RNG. */
    status = UmiAiTrainingSampleLogitsNucleus(
        equalLogits, vocabulary, UMI_AI_BYTE_OFFSET, vocabulary,
        policy, &trialRandom, &trialToken, NULL);
    if (status != UMI_STATUS_OK) return status;
    created = calloc(1U, sizeof(*created));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    created->model = model;
    created->policy = *policy;
    status = UmiAiTrainingRandomSeed(&created->randomState, seed);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingStackedModelIdentityCapture(model, &created->pinned);
    if (status != UMI_STATUS_OK) goto failure;
    for (size_t i = 0U; i < contextLength; ++i)
        created->context[i] = UMI_AI_BYTE_BOS;
    const size_t retained = promptTokenCount < contextLength
                            ? promptTokenCount : contextLength;
    if (retained != 0U) {
        memcpy(&created->context[contextLength - retained],
               &promptTokenIds[promptTokenCount - retained],
               retained * sizeof(*promptTokenIds));
    }
    *outSession = created;
    return UMI_STATUS_OK;
failure:
    UmiAiTrainingStackedGenerationSessionDestroy(created);
    return status;
}

UmiStatus UmiAiTrainingStackedGenerationSessionNext(
    UmiAiTrainingStackedGenerationSession *session, size_t *outTokenId)
{
    UmiAiTrainingRandomState candidateRandom;
    float *allLogits = NULL;
    size_t candidateToken = 0U;
    size_t context, vocabulary, valueCount;
    UmiStatus status;
    if (session == NULL || outTokenId == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->generatedTokens >= UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = SessionValid(session);
    if (status != UMI_STATUS_OK) return status;
    context = (size_t)session->pinned.config.contextLength;
    vocabulary = (size_t)session->pinned.config.vocabularySize;
    valueCount = context * vocabulary; /* Both factors already bounded. */
    allLogits = malloc(valueCount * sizeof(*allLogits));
    if (allLogits == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingStackedDecoderLogits(
        session->model, session->context, context, allLogits, valueCount);
    candidateRandom = session->randomState;
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingSampleLogitsNucleus(
            &allLogits[(context - 1U) * vocabulary], vocabulary,
            UMI_AI_BYTE_OFFSET, vocabulary, &session->policy,
            &candidateRandom, &candidateToken, NULL);
    free(allLogits);
    if (status != UMI_STATUS_OK) return status;
    /* Model inference is supposed to be read-only. Check that invariant
     * before publishing a token or changing the session's RNG/context. */
    status = SessionValid(session);
    if (status != UMI_STATUS_OK) return status;
    if (context > 1U)
        memmove(session->context, session->context + 1U,
                (context - 1U) * sizeof(*session->context));
    session->context[context - 1U] = candidateToken;
    session->randomState = candidateRandom;
    ++session->generatedTokens;
    *outTokenId = candidateToken;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedGenerationSessionNextByte(
    UmiAiTrainingStackedGenerationSession *session, uint8_t *outByte)
{
    uint8_t candidate = 0U;
    uint32_t encoded;
    size_t token = 0U, written = 0U;
    UmiStatus status;
    if (session == NULL || outByte == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->pinned.config.vocabularySize != UMI_AI_BYTE_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedGenerationSessionNext(session, &token);
    if (status != UMI_STATUS_OK) return status;
    encoded = (uint32_t)token;
    status = UmiAiByteDecode(&encoded, 1U, &candidate, 1U, &written);
    if (status != UMI_STATUS_OK || written != 1U)
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    *outByte = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedGenerationSessionInspect(
    const UmiAiTrainingStackedGenerationSession *session,
    UmiAiTrainingStackedGenerationStats *outStats)
{
    UmiAiTrainingStackedGenerationStats candidate = {0};
    UmiStatus status;
    if (session == NULL || outStats == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = SessionValid(session);
    if (status != UMI_STATUS_OK) return status;
    candidate.generatedTokens = session->generatedTokens;
    candidate.contextTokens = (size_t)session->pinned.config.contextLength;
    candidate.transformerLayers = (size_t)session->pinned.config.transformerLayers;
    candidate.computedFullContextTokenRows =
        (uint64_t)candidate.generatedTokens * (uint64_t)candidate.contextTokens;
    candidate.computedTransformerTokenLayerRows =
        candidate.computedFullContextTokenRows * (uint64_t)candidate.transformerLayers;
    candidate.modelIdentity = session->pinned;
    *outStats = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedGenerateNucleus(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokens, size_t capacity, size_t *outCount,
    UmiAiTrainingStackedGenerationStats *outStats)
{
    UmiAiTrainingStackedGenerationSession *session = NULL;
    UmiAiTrainingStackedGenerationStats candidateStats = {0};
    size_t staged[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    UmiStatus status;
    if (outTokens == NULL || outCount == NULL ||
        generatedTokenCount == 0U ||
        generatedTokenCount > UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < generatedTokenCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingStackedGenerationSessionCreate(
        model, promptIds, promptCount, policy, seed, &session);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < generatedTokenCount; ++i) {
        status = UmiAiTrainingStackedGenerationSessionNext(session, &staged[i]);
        if (status != UMI_STATUS_OK) break;
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedGenerationSessionInspect(
            session, &candidateStats);
    UmiAiTrainingStackedGenerationSessionDestroy(session);
    if (status != UMI_STATUS_OK) return status;
    memcpy(outTokens, staged, generatedTokenCount * sizeof(*staged));
    *outCount = generatedTokenCount;
    if (outStats != NULL) *outStats = candidateStats;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedGenerateBytesNucleus(
    UmiAiTrainingStackedDecoder *model,
    const uint8_t *promptBytes, size_t promptByteCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedByteCount, uint64_t seed,
    uint8_t *outBytes, size_t capacity, size_t *outCount)
{
    UmiAiTrainingStackedDecoderInfo info = {0};
    uint32_t encoded[UMI_AI_STACKED_INFERENCE_MAX_PROMPT_TOKENS] = {0};
    size_t prompt[UMI_AI_STACKED_INFERENCE_MAX_PROMPT_TOKENS] = {0};
    size_t generated[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    uint32_t generatedIds[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    uint8_t bytes[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    size_t inputCount = 0U, count = 0U, decoded = 0U;
    UmiStatus status;
    if (model == NULL || outBytes == NULL || outCount == NULL ||
        promptByteCount > UMI_AI_STACKED_INFERENCE_MAX_PROMPT_TOKENS ||
        (promptByteCount != 0U && promptBytes == NULL) ||
        generatedByteCount == 0U ||
        generatedByteCount > UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < generatedByteCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingStackedDecoderInspect(model, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.config.vocabularySize != UMI_AI_BYTE_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiByteEncode(promptBytes, promptByteCount,
                             encoded, promptByteCount, &inputCount);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < inputCount; ++i) prompt[i] = (size_t)encoded[i];
    status = UmiAiTrainingStackedGenerateNucleus(model, prompt, inputCount,
        policy, generatedByteCount, seed, generated,
        UMI_AI_STACKED_MAX_GENERATED_TOKENS, &count, NULL);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < count; ++i) generatedIds[i] = (uint32_t)generated[i];
    status = UmiAiByteDecode(generatedIds, count, bytes, sizeof(bytes), &decoded);
    if (status != UMI_STATUS_OK || decoded != generatedByteCount)
        return status == UMI_STATUS_OK ? UMI_STATUS_INVALID_STATE : status;
    memcpy(outBytes, bytes, decoded);
    *outCount = decoded;
    return UMI_STATUS_OK;
}
