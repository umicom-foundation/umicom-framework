/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_kv_incremental.c
 * PURPOSE: Use the canonical stacked projection trace for prefill/rollover,
 *          retain K/V in the established store, calculate growing-prefix
 *          token rows through the Framework training tape and sample through
 *          the established nucleus sampler. No application-local model maths.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_incremental.h"
#include "umicom/ai/training/stacked_projection_trace.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define KV_INC_LAYER_ELEMENTS \
    (UMI_AI_STACKED_MAX_LAYERS * UMI_AI_STACKED_MAX_CHANNELS)

struct UmiAiTrainingStackedKvIncrementalSession {
    UmiAiTrainingStackedDecoder *model; /* Borrowed. */
    UmiAiTrainingStackedModelIdentity pinned;
    UmiAiTrainingStackedKvStore *store;
    UmiAiTrainingNucleusOptions policy;
    UmiAiTrainingRandomState rng;
    size_t tokenIds[UMI_AI_STACKED_MAX_CONTEXT];
    size_t retained;
    size_t initialPrompt;
    size_t pendingId;
    size_t generated;
    int hasPending;
    int terminal;
    float lastLogits[UMI_AI_STACKED_MAX_VOCABULARY];
    uint64_t fullContextLayerRows;
    uint64_t incrementalProjectionLayerRows;
    uint64_t attentionQueryLayerRows;
};

static UmiStatus IncrementalCurrent(
    const UmiAiTrainingStackedKvIncrementalSession *session)
{
    if (session == NULL || session->model == NULL || session->store == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->terminal) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiAiTrainingStackedModelIdentityVerify(
        session->model, &session->pinned);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiTrainingStackedKvStoreCheckIdentity(
        session->store, session->pinned.checkpointSha256);
}

void UmiAiTrainingStackedKvIncrementalSessionDestroy(
    UmiAiTrainingStackedKvIncrementalSession *session)
{
    if (session == NULL) return;
    UmiAiTrainingStackedKvStoreDestroy(session->store);
    free(session);
}

/* Request actual canonical projected K/V rows; padding lies strictly in the
 * future for a short prefix and cannot affect earlier causal positions. */
static UmiStatus IncrementalTrace(
    UmiAiTrainingStackedKvIncrementalSession *session,
    const size_t *tokenIds, size_t retained,
    float *outKeys, float *outValues, float *outLastLogits)
{
    const size_t length = (size_t)session->pinned.config.contextLength;
    const size_t layers = (size_t)session->pinned.config.transformerLayers;
    const size_t width = (size_t)session->pinned.config.channels;
    const size_t vocab = (size_t)session->pinned.config.vocabularySize;
    size_t fullContext[UMI_AI_STACKED_MAX_CONTEXT];
    const size_t fullKv = length * layers * width;
    const size_t totalFloats = fullKv * 2U + length * vocab;
    float *workspace = malloc(totalFloats * sizeof(float));
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    float *keys = workspace;
    float *values = workspace + fullKv;
    float *logits = workspace + 2U * fullKv;
    for (size_t i = 0U; i < length; ++i)
        fullContext[i] = i < retained ? tokenIds[i] : UMI_AI_BYTE_BOS;
    UmiStatus status = UmiAiTrainingStackedDecoderTraceProjections(
        session->model, fullContext, length,
        keys, fullKv, values, fullKv, logits, length * vocab);
    if (status == UMI_STATUS_OK)
        status = IncrementalCurrent(session);
    if (status == UMI_STATUS_OK) {
        for (size_t layer = 0U; layer < layers; ++layer) {
            const size_t source = layer * length * width;
            const size_t destination = layer * retained * width;
            memcpy(outKeys + destination, keys + source,
                   retained * width * sizeof(float));
            memcpy(outValues + destination, values + source,
                   retained * width * sizeof(float));
        }
        memcpy(outLastLogits, logits + (retained - 1U) * vocab,
               vocab * sizeof(float));
    }
    free(workspace);
    return status;
}

UmiStatus UmiAiTrainingStackedKvIncrementalSessionCreate(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy, uint64_t seed,
    UmiAiTrainingStackedKvIncrementalSession **outSession)
{
    UmiAiTrainingStackedKvIncrementalSession *created = NULL;
    UmiAiTrainingStackedKvStoreConfig storeConfig = {0};
    float trialLogits[UMI_AI_STACKED_MAX_VOCABULARY] = {0};
    UmiAiTrainingRandomState trialRng = {0};
    size_t trialId = 0U;
    float *keys = NULL, *values = NULL;
    UmiStatus status;
    if (model == NULL || promptIds == NULL || policy == NULL ||
        outSession == NULL || *outSession != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    created = calloc(1U, sizeof(*created));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    created->model = model;
    status = UmiAiTrainingStackedModelIdentityCapture(model, &created->pinned);
    if (status != UMI_STATUS_OK) goto failure;
    const size_t context = (size_t)created->pinned.config.contextLength;
    const size_t vocab = (size_t)created->pinned.config.vocabularySize;
    const size_t layers = (size_t)created->pinned.config.transformerLayers;
    const size_t channels = (size_t)created->pinned.config.channels;
    if (promptCount == 0U || promptCount > context ||
        vocab < UMI_AI_BYTE_OFFSET + 1U) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto failure;
    }
    for (size_t i = 0U; i < promptCount; ++i)
        if (promptIds[i] >= vocab) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            goto failure;
        }
    /* The original Framework sampler validates ALL policy bounds. */
    status = UmiAiTrainingRandomSeed(&trialRng, seed);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingSampleLogitsNucleus(
            trialLogits, vocab, UMI_AI_BYTE_OFFSET, vocab,
            policy, &trialRng, &trialId, NULL);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingRandomSeed(&created->rng, seed);
    if (status != UMI_STATUS_OK) goto failure;
    created->policy = *policy;
    storeConfig.decoder = created->pinned.config;
    memcpy(storeConfig.checkpointSha256,
           created->pinned.checkpointSha256, UMI_SHA256_BYTES);
    status = UmiAiTrainingStackedKvStoreCreate(&storeConfig, &created->store);
    if (status != UMI_STATUS_OK) goto failure;

    const size_t kvElements = layers * promptCount * channels;
    keys = malloc(kvElements * sizeof(float));
    values = malloc(kvElements * sizeof(float));
    if (keys == NULL || values == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto failure;
    }
    status = IncrementalTrace(created, promptIds, promptCount,
                              keys, values, created->lastLogits);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingStackedKvStorePrefill(
        created->store, promptIds, promptCount,
        keys, kvElements, values, kvElements);
    if (status != UMI_STATUS_OK) goto failure;
    memcpy(created->tokenIds, promptIds, promptCount * sizeof(size_t));
    created->initialPrompt = created->retained = promptCount;
    created->fullContextLayerRows = (uint64_t)(context * layers);
    created->attentionQueryLayerRows = (uint64_t)(context * layers);
    *outSession = created;
    free(values);
    free(keys);
    return UMI_STATUS_OK;

failure:
    free(values);
    free(keys);
    UmiAiTrainingStackedKvIncrementalSessionDestroy(created);
    return status;
}

UmiStatus UmiAiTrainingStackedKvIncrementalSessionNext(
    UmiAiTrainingStackedKvIncrementalSession *session,
    size_t *outTokenId)
{
    UmiAiTrainingRandomState candidateRng;
    UmiAiTrainingStackedKvStoreInfo previous = {0};
    float candidateLogits[UMI_AI_STACKED_MAX_VOCABULARY] = {0};
    float proposedKeys[KV_INC_LAYER_ELEMENTS] = {0};
    float proposedValues[KV_INC_LAYER_ELEMENTS] = {0};
    float fullKeys[UMI_AI_STACKED_MAX_LAYERS *
                   UMI_AI_STACKED_MAX_CONTEXT * UMI_AI_STACKED_MAX_CHANNELS];
    float fullValues[UMI_AI_STACKED_MAX_LAYERS *
                     UMI_AI_STACKED_MAX_CONTEXT * UMI_AI_STACKED_MAX_CHANNELS];
    size_t nextIds[UMI_AI_STACKED_MAX_CONTEXT];
    size_t chosen = 0U;
    UmiAiTrainingStackedKvStepReport step = {0};
    UmiStatus status;
    if (session == NULL || outTokenId == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->generated >= UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = IncrementalCurrent(session);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedKvStoreInspect(session->store, &previous);
    if (status != UMI_STATUS_OK || previous.retainedTokens != session->retained)
        return UMI_STATUS_INVALID_STATE;
    const size_t context = (size_t)session->pinned.config.contextLength;
    const size_t layers = (size_t)session->pinned.config.transformerLayers;
    const size_t channels = (size_t)session->pinned.config.channels;
    const size_t vocab = (size_t)session->pinned.config.vocabularySize;
    const size_t kvElements = layers * channels;
    const int advance = session->hasPending;
    const int rollover = advance && session->retained == context;
    if (!advance) {
        memcpy(candidateLogits, session->lastLogits,
               vocab * sizeof(float));
    } else if (!rollover) {
        status = UmiAiTrainingStackedKvStepPropose(
            session->model, session->store, session->pendingId,
            proposedKeys, kvElements, proposedValues, kvElements,
            candidateLogits, vocab, &step);
        if (status != UMI_STATUS_OK) return status;
    } else {
        for (size_t i = 0U; i + 1U < context; ++i)
            nextIds[i] = session->tokenIds[i + 1U];
        nextIds[context - 1U] = session->pendingId;
        status = IncrementalTrace(session, nextIds, context,
            fullKeys, fullValues, candidateLogits);
        if (status != UMI_STATUS_OK) return status;
    }
    candidateRng = session->rng;
    status = UmiAiTrainingSampleLogitsNucleus(candidateLogits, vocab,
        UMI_AI_BYTE_OFFSET, vocab, &session->policy,
        &candidateRng, &chosen, NULL);
    if (status != UMI_STATUS_OK) return status;
    status = IncrementalCurrent(session);
    if (status != UMI_STATUS_OK) return status;

    /* Sampling has succeeded; only now commit the pending token and its K/V.
     * Store Append/Replace validate the entire candidate before publication. */
    if (advance && !rollover) {
        status = UmiAiTrainingStackedKvStoreAppend(session->store,
            session->pendingId, proposedKeys, kvElements,
            proposedValues, kvElements);
        if (status != UMI_STATUS_OK) return status;
        session->tokenIds[session->retained++] = session->pendingId;
        session->incrementalProjectionLayerRows += (uint64_t)layers;
        session->attentionQueryLayerRows +=
            step.attentionQueryLayerRowsExecuted;
    } else if (rollover) {
        const size_t count = context * kvElements;
        status = UmiAiTrainingStackedKvStoreReplaceRolled(
            session->store, nextIds, context,
            fullKeys, count, fullValues, count);
        if (status != UMI_STATUS_OK) return status;
        memcpy(session->tokenIds, nextIds, context * sizeof(size_t));
        session->fullContextLayerRows += (uint64_t)(context * layers);
        session->attentionQueryLayerRows += (uint64_t)(context * layers);
    }
    memcpy(session->lastLogits, candidateLogits,
           vocab * sizeof(float));
    session->rng = candidateRng;
    session->pendingId = chosen;
    session->hasPending = 1;
    ++session->generated;
    *outTokenId = chosen;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvIncrementalSessionNextByte(
    UmiAiTrainingStackedKvIncrementalSession *session,
    uint8_t *outByte)
{
    size_t id = 0U;
    uint8_t decoded = 0U;
    uint32_t encoded;
    size_t written = 0U;
    if (session == NULL || outByte == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (session->pinned.config.vocabularySize != UMI_AI_BYTE_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiTrainingStackedKvIncrementalSessionNext(
        session, &id);
    if (status != UMI_STATUS_OK) return status;
    encoded = (uint32_t)id;
    status = UmiAiByteDecode(&encoded, 1U, &decoded, 1U, &written);
    if (status != UMI_STATUS_OK || written != 1U) {
        session->terminal = 1;
        return UMI_STATUS_INVALID_STATE;
    }
    *outByte = decoded;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvIncrementalSessionInspect(
    const UmiAiTrainingStackedKvIncrementalSession *session,
    UmiAiTrainingStackedKvIncrementalStats *outStats)
{
    UmiAiTrainingStackedKvStoreInfo info = {0};
    UmiAiTrainingStackedKvIncrementalStats result = {0};
    if (outStats == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = IncrementalCurrent(session);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedKvStoreInspect(session->store, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.retainedTokens != session->retained)
        return UMI_STATUS_INVALID_STATE;
    const size_t context = (size_t)session->pinned.config.contextLength;
    const size_t layers = (size_t)session->pinned.config.transformerLayers;
    result.initialPromptTokens = session->initialPrompt;
    result.generatedTokens = session->generated;
    result.retainedTokens = session->retained;
    result.transformerLayers = layers;
    result.contextTokens = context;
    result.referenceFullContextTokenLayerRows =
        (uint64_t)(session->generated * context * layers);
    result.observedFullContextTokenLayerRows = session->fullContextLayerRows;
    result.observedIncrementalProjectionTokenLayerRows =
        session->incrementalProjectionLayerRows;
    result.observedAttentionQueryTokenLayerRows =
        session->attentionQueryLayerRows;
    result.observedFullWindowRebuilds = info.fullWindowRebuilds;
    result.observedIncrementalAppends = info.singleRowAppends;
    result.reservedKvBytes = info.reservedKvBytes;
    result.modelIdentity = session->pinned;
    *outStats = result;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvIncrementalGenerate(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    size_t *outTokens, size_t capacity, size_t *outCount,
    UmiAiTrainingStackedKvIncrementalStats *outStats)
{
    UmiAiTrainingStackedKvIncrementalSession *session = NULL;
    size_t produced[UMI_AI_STACKED_MAX_GENERATED_TOKENS];
    UmiAiTrainingStackedKvIncrementalStats report = {0};
    UmiStatus status;
    if (outTokens == NULL || outCount == NULL ||
        generatedTokenCount == 0U ||
        generatedTokenCount > UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < generatedTokenCount)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingStackedKvIncrementalSessionCreate(
        model, promptIds, promptCount, policy, seed, &session);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < generatedTokenCount; ++i) {
        status = UmiAiTrainingStackedKvIncrementalSessionNext(
            session, &produced[i]);
        if (status != UMI_STATUS_OK) break;
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingStackedKvIncrementalSessionInspect(
            session, &report);
    UmiAiTrainingStackedKvIncrementalSessionDestroy(session);
    if (status != UMI_STATUS_OK) return status;
    memcpy(outTokens, produced, generatedTokenCount * sizeof(size_t));
    *outCount = generatedTokenCount;
    if (outStats != NULL) *outStats = report;
    return UMI_STATUS_OK;
}
