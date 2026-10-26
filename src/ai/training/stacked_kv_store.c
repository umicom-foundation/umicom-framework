/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_kv_store.c
 * PURPOSE: Retain validated per-layer K/V rows under an explicit, full-layer
 *          rebuild contract for learned-absolute-position context rollover.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_store.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiTrainingStackedKvStore {
    UmiAiTrainingStackedKvStoreConfig config;
    size_t totalElements;      /* Per K OR per V, full reserved capacity. */
    size_t retained;
    size_t initialPrompt;
    uint64_t appends;
    uint64_t rebuilds;
    uint64_t acceptedRows;
    size_t ids[UMI_AI_STACKED_MAX_CONTEXT];
    float packed[];            /* Keys followed by values; layer-major. */
};

static int StoreValidConfiguration(const UmiAiTrainingStackedKvStoreConfig *c)
{
    if (c == NULL) return 0;
    const UmiAiTrainingStackedDecoderConfig *s = &c->decoder;
    if (s->vocabularySize < 5U || s->vocabularySize > UMI_AI_STACKED_MAX_VOCABULARY ||
        s->contextLength == 0U || s->contextLength > UMI_AI_STACKED_MAX_CONTEXT ||
        s->channels < 2U || s->channels > UMI_AI_STACKED_MAX_CHANNELS ||
        s->hiddenChannels == 0U || s->hiddenChannels > UMI_AI_STACKED_MAX_HIDDEN_CHANNELS ||
        s->attentionHeads == 0U || s->channels % s->attentionHeads != 0U ||
        s->transformerLayers == 0U || s->transformerLayers > UMI_AI_STACKED_MAX_LAYERS)
        return 0;
    unsigned int nonzero = 0U;
    for (size_t i = 0U; i < UMI_AI_STACKED_KV_IDENTITY_BYTES; ++i)
        nonzero |= (unsigned int)c->checkpointSha256[i];
    return nonzero != 0U;
}

static int StoreFinite(const float *v, size_t count)
{
    if (v == NULL) return 0;
    for (size_t i = 0U; i < count; ++i)
        if (!isfinite(v[i])) return 0;
    return 1;
}

static size_t StoreOffset(const UmiAiTrainingStackedKvStore *s,
                          size_t layer, size_t token)
{
    const size_t width = (size_t)s->config.decoder.channels;
    const size_t length = (size_t)s->config.decoder.contextLength;
    return (layer * length + token) * width;
}

static int StoreTokenIdsValid(const UmiAiTrainingStackedKvStore *s,
                              const size_t *ids, size_t count)
{
    if (ids == NULL || count == 0U ||
        count > (size_t)s->config.decoder.contextLength) return 0;
    for (size_t i = 0U; i < count; ++i)
        if (ids[i] >= (size_t)s->config.decoder.vocabularySize) return 0;
    return 1;
}

static int StoreRowsAvailable(const UmiAiTrainingStackedKvStore *s,
                              uint64_t additional)
{
    return additional <= UINT64_MAX - s->acceptedRows;
}

/* This uses uintptr_t only for a conservative disjoint-output check. The
 * returned rows are a pure copy; no output aliasing can safely be published. */
static int StoreOverlaps(const void *a, size_t aBytes,
                         const void *b, size_t bBytes)
{
    uintptr_t first = (uintptr_t)a, second = (uintptr_t)b;
    if (first > UINTPTR_MAX - aBytes || second > UINTPTR_MAX - bBytes)
        return 1;
    return first < second + bBytes && second < first + aBytes;
}

UmiStatus UmiAiTrainingStackedKvStoreCreate(
    const UmiAiTrainingStackedKvStoreConfig *config,
    UmiAiTrainingStackedKvStore **outStore)
{
    if (outStore == NULL || *outStore != NULL ||
        !StoreValidConfiguration(config)) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t layers = (size_t)config->decoder.transformerLayers;
    const size_t context = (size_t)config->decoder.contextLength;
    const size_t channels = (size_t)config->decoder.channels;
    if (layers > SIZE_MAX / context || layers * context > SIZE_MAX / channels)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t elements = layers * context * channels;
    /* Delegate global parameter/cache-shape validation to the original
     * Framework scaling planner. This module owns only row lifetimes. */
    const UmiAiTrainingModelScalingShape shape = {
        (uint64_t)config->decoder.vocabularySize,
        (uint64_t)config->decoder.contextLength,
        (uint64_t)config->decoder.channels,
        (uint64_t)config->decoder.hiddenChannels,
        (uint64_t)config->decoder.attentionHeads,
        (uint64_t)config->decoder.transformerLayers, UINT64_C(1), UINT64_C(0)
    };
    UmiAiTrainingModelScalingEstimate sizing = {0};
    UmiStatus status = UmiAiTrainingModelScalingEstimateBuild(&shape, &sizing);
    if (status != UMI_STATUS_OK) return status;
    if (sizing.kvCacheFp32Bytes !=
        (uint64_t)(elements * 2U * sizeof(float))) return UMI_STATUS_INVALID_STATE;
    if (elements > (SIZE_MAX - sizeof(UmiAiTrainingStackedKvStore)) /
                     (2U * sizeof(float))) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAiTrainingStackedKvStore *created = calloc(1U,
         sizeof(*created) + elements * 2U * sizeof(float));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    created->config = *config;
    created->totalElements = elements;
    *outStore = created;
    return UMI_STATUS_OK;
}

void UmiAiTrainingStackedKvStoreDestroy(UmiAiTrainingStackedKvStore *store)
{
    if (store == NULL) return;
    /* Erase retained vectors as a hygiene measure. No secure memory-wipe
     * or data-remanence guarantee is implied by standard C memset. */
    memset(store->packed, 0, store->totalElements * 2U * sizeof(float));
    free(store);
}

UmiStatus UmiAiTrainingStackedKvStorePrefill(
    UmiAiTrainingStackedKvStore *store,
    const size_t *tokenIds, size_t tokenCount,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount)
{
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t layers = (size_t)store->config.decoder.transformerLayers;
    const size_t channels = (size_t)store->config.decoder.channels;
    if (store->retained != 0U ||
        !StoreTokenIdsValid(store, tokenIds, tokenCount) ||
        keyCount != layers * tokenCount * channels || valueCount != keyCount ||
        !StoreFinite(keys, keyCount) || !StoreFinite(values, valueCount))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!StoreRowsAvailable(store, (uint64_t)(layers * tokenCount)))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    float *allKeys = store->packed;
    float *allValues = store->packed + store->totalElements;
    for (size_t layer = 0U; layer < layers; ++layer)
        for (size_t time = 0U; time < tokenCount; ++time) {
            const size_t destination = StoreOffset(store, layer, time);
            const size_t source = (layer * tokenCount + time) * channels;
            memcpy(allKeys + destination, keys + source, channels * sizeof(float));
            memcpy(allValues + destination, values + source, channels * sizeof(float));
        }
    memcpy(store->ids, tokenIds, tokenCount * sizeof(size_t));
    store->initialPrompt = tokenCount;
    store->retained = tokenCount;
    store->acceptedRows += (uint64_t)(layers * tokenCount);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvStoreAppend(
    UmiAiTrainingStackedKvStore *store, size_t tokenId,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount)
{
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t layers = (size_t)store->config.decoder.transformerLayers;
    const size_t channels = (size_t)store->config.decoder.channels;
    if (store->retained == 0U ||
        store->retained == (size_t)store->config.decoder.contextLength ||
        tokenId >= (size_t)store->config.decoder.vocabularySize ||
        keyCount != layers * channels || valueCount != keyCount ||
        !StoreFinite(keys, keyCount) || !StoreFinite(values, valueCount))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (store->appends == UINT64_MAX || !StoreRowsAvailable(store, (uint64_t)layers))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    float *allKeys = store->packed;
    float *allValues = store->packed + store->totalElements;
    for (size_t layer = 0U; layer < layers; ++layer) {
        const size_t offset = StoreOffset(store, layer, store->retained);
        memcpy(allKeys + offset, keys + layer * channels, channels * sizeof(float));
        memcpy(allValues + offset, values + layer * channels, channels * sizeof(float));
    }
    store->ids[store->retained++] = tokenId;
    ++store->appends;
    store->acceptedRows += (uint64_t)layers;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvStoreReplaceRolled(
    UmiAiTrainingStackedKvStore *store,
    const size_t *shiftedTokenIds, size_t tokenCount,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount)
{
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t length = (size_t)store->config.decoder.contextLength;
    const size_t layers = (size_t)store->config.decoder.transformerLayers;
    if (store->retained != length || tokenCount != length ||
        !StoreTokenIdsValid(store, shiftedTokenIds, tokenCount) ||
        keyCount != store->totalElements || valueCount != keyCount ||
        !StoreFinite(keys, keyCount) || !StoreFinite(values, valueCount))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i + 1U < length; ++i)
        if (shiftedTokenIds[i] != store->ids[i + 1U])
            return UMI_STATUS_INVALID_STATE;
    const uint64_t added = (uint64_t)(layers * length);
    if (store->rebuilds == UINT64_MAX || !StoreRowsAvailable(store, added))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* This is a single full-tensor swap, never a partial ring overwrite.
     * Every layer/position has been validated BEFORE the first byte changes. */
    memcpy(store->packed, keys, keyCount * sizeof(float));
    memcpy(store->packed + store->totalElements, values, valueCount * sizeof(float));
    memcpy(store->ids, shiftedTokenIds, length * sizeof(size_t));
    ++store->rebuilds;
    store->acceptedRows += added;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvStoreRead(
    const UmiAiTrainingStackedKvStore *store,
    size_t layerIndex, size_t tokenIndex,
    float *outKey, size_t keyCapacity,
    float *outValue, size_t valueCapacity)
{
    if (store == NULL || outKey == NULL || outValue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const size_t width = (size_t)store->config.decoder.channels;
    if (layerIndex >= (size_t)store->config.decoder.transformerLayers ||
        tokenIndex >= store->retained)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (keyCapacity < width || valueCapacity < width)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (StoreOverlaps(outKey, width * sizeof(float), outValue,
                      width * sizeof(float)))
        return UMI_STATUS_INVALID_ARGUMENT;
    const size_t offset = StoreOffset(store, layerIndex, tokenIndex);
    memcpy(outKey, store->packed + offset, width * sizeof(float));
    memcpy(outValue, store->packed + store->totalElements + offset,
           width * sizeof(float));
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvStoreCheckIdentity(
    const UmiAiTrainingStackedKvStore *store,
    const uint8_t checkpointSha256[UMI_AI_STACKED_KV_IDENTITY_BYTES])
{
    if (store == NULL || checkpointSha256 == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return memcmp(store->config.checkpointSha256, checkpointSha256,
                  UMI_AI_STACKED_KV_IDENTITY_BYTES) == 0 ?
                  UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

UmiStatus UmiAiTrainingStackedKvStoreInspect(
    const UmiAiTrainingStackedKvStore *store,
    UmiAiTrainingStackedKvStoreInfo *outInfo)
{
    if (store == NULL || outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingStackedKvStoreInfo candidate = {0};
    candidate.transformerLayers = (size_t)store->config.decoder.transformerLayers;
    candidate.contextCapacity = (size_t)store->config.decoder.contextLength;
    candidate.channels = (size_t)store->config.decoder.channels;
    candidate.retainedTokens = store->retained;
    candidate.initialPromptTokens = store->initialPrompt;
    candidate.singleRowAppends = store->appends;
    candidate.fullWindowRebuilds = store->rebuilds;
    candidate.acceptedTokenLayerRows = store->acceptedRows;
    candidate.reservedKvBytes = store->totalElements * 2U * sizeof(float);
    memcpy(candidate.checkpointSha256, store->config.checkpointSha256,
           UMI_AI_STACKED_KV_IDENTITY_BYTES);
    *outInfo = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvStoreReset(
    UmiAiTrainingStackedKvStore *store)
{
    if (store == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(store->packed, 0, store->totalElements * 2U * sizeof(float));
    memset(store->ids, 0, sizeof(store->ids));
    store->retained = store->initialPrompt = 0U;
    store->appends = store->rebuilds = store->acceptedRows = UINT64_C(0);
    return UMI_STATUS_OK;
}
