/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_kv_capture.c
 * PURPOSE:
 *   Observe authoritative per-layer K/V projections and populate the existing
 *   multi-layer cache owner while retaining complete decoder correctness.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_capture.h"
#include "umicom/ai/training/stacked_projection_trace.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define CAPTURE_PREVIOUS_ROW_TOLERANCE 0.00004F

struct UmiAiTrainingStackedKvCapture {
    UmiAiTrainingStackedDecoder *model;       /* Borrowed, never owned. */
    UmiAiTrainingStackedModelIdentity identity;
    UmiAiTrainingStackedKvStore *store;
    size_t ids[UMI_AI_STACKED_MAX_CONTEXT];
    size_t retained;
    uint64_t referenceEvaluations;
    float maximumPreviousRowDifference;
};

/* No parameter is ever copied into application-owned model storage. This
 * verifies the ORIGINAL Framework checkpoint before any tensor observation. */
static UmiStatus CaptureCurrent(const UmiAiTrainingStackedKvCapture *capture)
{
    if (capture == NULL || capture->model == NULL || capture->store == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiAiTrainingStackedModelIdentityVerify(
        capture->model, &capture->identity);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiTrainingStackedKvStoreCheckIdentity(
        capture->store, capture->identity.checkpointSha256);
}

void UmiAiTrainingStackedKvCaptureDestroy(
    UmiAiTrainingStackedKvCapture *capture)
{
    if (capture == NULL) return;
    UmiAiTrainingStackedKvStoreDestroy(capture->store);
    free(capture);
}

UmiStatus UmiAiTrainingStackedKvCaptureCreate(
    UmiAiTrainingStackedDecoder *model,
    UmiAiTrainingStackedKvCapture **outCapture)
{
    UmiAiTrainingStackedKvCapture *candidate = NULL;
    UmiAiTrainingStackedKvStoreConfig storeConfig = {0};
    UmiStatus status;
    if (model == NULL || outCapture == NULL || *outCapture != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->model = model;
    status = UmiAiTrainingStackedModelIdentityCapture(model, &candidate->identity);
    if (status != UMI_STATUS_OK) goto failure;
    storeConfig.decoder = candidate->identity.config;
    memcpy(storeConfig.checkpointSha256, candidate->identity.checkpointSha256,
           UMI_AI_STACKED_KV_IDENTITY_BYTES);
    status = UmiAiTrainingStackedKvStoreCreate(&storeConfig, &candidate->store);
    if (status != UMI_STATUS_OK) goto failure;
    status = CaptureCurrent(candidate);
    if (status != UMI_STATUS_OK) goto failure;
    *outCapture = candidate;
    return UMI_STATUS_OK;
failure:
    UmiAiTrainingStackedKvCaptureDestroy(candidate);
    return status;
}

/* Full-context trace is intentionally explicit. Future efficient inference
 * needs a separate qualified path, not a misleading cached-work counter. */
typedef struct CaptureObservation {
    float *keys;
    float *values;
    float *logits;
    size_t kvElements;
    size_t logitElements;
} CaptureObservation;

static UmiStatus CaptureReference(
    UmiAiTrainingStackedKvCapture *capture,
    const size_t *ids, size_t count,
    CaptureObservation *outObservation)
{
    const size_t context = (size_t)capture->identity.config.contextLength;
    const size_t channels = (size_t)capture->identity.config.channels;
    const size_t layers = (size_t)capture->identity.config.transformerLayers;
    const size_t vocabulary = (size_t)capture->identity.config.vocabularySize;
    size_t padded[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    float *candidate = NULL;
    CaptureObservation found = {0};
    UmiStatus status;
    if (count == 0U || count > context || ids == NULL ||
        outObservation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < count; ++i)
        if (ids[i] >= vocabulary) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < context; ++i)
        padded[i] = i < count ? ids[i] : UMI_AI_BYTE_BOS;
    found.kvElements = layers * context * channels;
    found.logitElements = context * vocabulary;
    if (found.kvElements > (SIZE_MAX / sizeof(float) -
                            found.logitElements) / 2U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = malloc((2U * found.kvElements + found.logitElements) *
                       sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    found.keys = candidate;
    found.values = candidate + found.kvElements;
    found.logits = candidate + found.kvElements * 2U;
    status = UmiAiTrainingStackedDecoderTraceProjections(
        capture->model, padded, context,
        found.keys, found.kvElements,
        found.values, found.kvElements,
        found.logits, found.logitElements);
    if (status == UMI_STATUS_OK)
        status = CaptureCurrent(capture);
    if (status != UMI_STATUS_OK) {
        free(candidate);
        return status;
    }
    *outObservation = found;
    return UMI_STATUS_OK;
}

static void CaptureObservationFree(CaptureObservation *observation)
{
    if (observation == NULL) return;
    free(observation->keys);
    *observation = (CaptureObservation){0};
}

/* This check is required even though the causal prefix lemma suggests exact
 * reuse. A future malformed projection hook must not silently corrupt state. */
static UmiStatus CaptureComparePrior(
    const UmiAiTrainingStackedKvCapture *capture,
    const CaptureObservation *observation,
    float *outMaximum)
{
    const size_t context = (size_t)capture->identity.config.contextLength;
    const size_t width = (size_t)capture->identity.config.channels;
    const size_t layers = (size_t)capture->identity.config.transformerLayers;
    float maximum = 0.0F;
    float previousKey[UMI_AI_STACKED_MAX_CHANNELS];
    float previousValue[UMI_AI_STACKED_MAX_CHANNELS];
    for (size_t layer = 0U; layer < layers; ++layer) {
        for (size_t position = 0U; position < capture->retained; ++position) {
            UmiStatus status = UmiAiTrainingStackedKvStoreRead(
                capture->store, layer, position,
                previousKey, width, previousValue, width);
            if (status != UMI_STATUS_OK) return status;
            const size_t offset = (layer * context + position) * width;
            for (size_t component = 0U; component < width; ++component) {
                const float kDiff = fabsf(previousKey[component] -
                                        observation->keys[offset + component]);
                const float vDiff = fabsf(previousValue[component] -
                                        observation->values[offset + component]);
                if (!isfinite(kDiff) || !isfinite(vDiff) ||
                    kDiff > CAPTURE_PREVIOUS_ROW_TOLERANCE ||
                    vDiff > CAPTURE_PREVIOUS_ROW_TOLERANCE)
                    return UMI_STATUS_INVALID_STATE;
                if (kDiff > maximum) maximum = kDiff;
                if (vDiff > maximum) maximum = vDiff;
            }
        }
    }
    *outMaximum = maximum;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvCapturePrefill(
    UmiAiTrainingStackedKvCapture *capture,
    const size_t *prefixIds, size_t prefixCount,
    float *outLastLogits, size_t logitCapacity)
{
    CaptureObservation observation = {0};
    float *compactKeys = NULL, *compactValues = NULL;
    UmiAiTrainingStackedKvStoreInfo info = {0};
    UmiStatus status;
    if (capture == NULL || outLastLogits == NULL || prefixIds == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = CaptureCurrent(capture);
    if (status != UMI_STATUS_OK) return status;
    const size_t context = (size_t)capture->identity.config.contextLength;
    const size_t vocabulary = (size_t)capture->identity.config.vocabularySize;
    const size_t channels = (size_t)capture->identity.config.channels;
    const size_t layers = (size_t)capture->identity.config.transformerLayers;
    if (prefixCount == 0U || prefixCount > context ||
        logitCapacity < vocabulary || capture->retained != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedKvStoreInspect(capture->store, &info);
    if (status != UMI_STATUS_OK || info.retainedTokens != 0U)
        return UMI_STATUS_INVALID_STATE;
    if (capture->referenceEvaluations == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = CaptureReference(capture, prefixIds, prefixCount, &observation);
    if (status != UMI_STATUS_OK) return status;
    const size_t count = layers * prefixCount * channels;
    compactKeys = malloc(count * sizeof(float));
    compactValues = malloc(count * sizeof(float));
    if (compactKeys == NULL || compactValues == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto done;
    }
    for (size_t layer = 0U; layer < layers; ++layer) {
        const size_t destination = layer * prefixCount * channels;
        const size_t source = layer * context * channels;
        memcpy(compactKeys + destination, observation.keys + source,
               prefixCount * channels * sizeof(float));
        memcpy(compactValues + destination, observation.values + source,
               prefixCount * channels * sizeof(float));
    }
    status = UmiAiTrainingStackedKvStorePrefill(capture->store,
        prefixIds, prefixCount, compactKeys, count, compactValues, count);
    if (status == UMI_STATUS_OK) {
        memcpy(capture->ids, prefixIds, prefixCount * sizeof(size_t));
        capture->retained = prefixCount;
        ++capture->referenceEvaluations;
        memcpy(outLastLogits, observation.logits +
               (prefixCount - 1U) * vocabulary, vocabulary * sizeof(float));
    }
done:
    free(compactValues);
    free(compactKeys);
    CaptureObservationFree(&observation);
    return status;
}

UmiStatus UmiAiTrainingStackedKvCaptureAppend(
    UmiAiTrainingStackedKvCapture *capture, size_t nextTokenId,
    float *outLastLogits, size_t logitCapacity)
{
    CaptureObservation observation = {0};
    UmiStatus status;
    float maximum = 0.0F;
    if (capture == NULL || outLastLogits == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = CaptureCurrent(capture);
    if (status != UMI_STATUS_OK) return status;
    const size_t context = (size_t)capture->identity.config.contextLength;
    const size_t vocabulary = (size_t)capture->identity.config.vocabularySize;
    const size_t channels = (size_t)capture->identity.config.channels;
    const size_t layers = (size_t)capture->identity.config.transformerLayers;
    if (capture->retained == 0U || nextTokenId >= vocabulary)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (logitCapacity < vocabulary) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (capture->referenceEvaluations == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t nextIds[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    const int rollover = capture->retained == context;
    const size_t nextCount = rollover ? context : capture->retained + 1U;
    if (rollover) {
        for (size_t i = 0U; i + 1U < context; ++i)
            nextIds[i] = capture->ids[i + 1U];
    } else {
        memcpy(nextIds, capture->ids,
               capture->retained * sizeof(size_t));
    }
    nextIds[nextCount - 1U] = nextTokenId;
    status = CaptureReference(capture, nextIds, nextCount, &observation);
    if (status != UMI_STATUS_OK) return status;
    if (!rollover) {
        status = CaptureComparePrior(capture, &observation, &maximum);
        if (status != UMI_STATUS_OK) goto done;
        /* StoreAppend accepts one token's [layer][channel] rows. */
        float lastKeys[UMI_AI_STACKED_MAX_LAYERS * UMI_AI_STACKED_MAX_CHANNELS];
        float lastValues[UMI_AI_STACKED_MAX_LAYERS * UMI_AI_STACKED_MAX_CHANNELS];
        for (size_t layer = 0U; layer < layers; ++layer) {
            const size_t source = (layer * context + nextCount - 1U) * channels;
            memcpy(lastKeys + layer * channels,
                   observation.keys + source, channels * sizeof(float));
            memcpy(lastValues + layer * channels,
                   observation.values + source, channels * sizeof(float));
        }
        status = UmiAiTrainingStackedKvStoreAppend(capture->store,
             nextTokenId, lastKeys, layers * channels,
             lastValues, layers * channels);
    } else {
        status = UmiAiTrainingStackedKvStoreReplaceRolled(capture->store,
             nextIds, nextCount, observation.keys, observation.kvElements,
             observation.values, observation.kvElements);
    }
    if (status == UMI_STATUS_OK) {
        memcpy(capture->ids, nextIds, nextCount * sizeof(size_t));
        capture->retained = nextCount;
        ++capture->referenceEvaluations;
        if (maximum > capture->maximumPreviousRowDifference)
            capture->maximumPreviousRowDifference = maximum;
        memcpy(outLastLogits, observation.logits +
               (nextCount - 1U) * vocabulary, vocabulary * sizeof(float));
    }
done:
    CaptureObservationFree(&observation);
    return status;
}

UmiStatus UmiAiTrainingStackedKvCaptureInspect(
    const UmiAiTrainingStackedKvCapture *capture,
    UmiAiTrainingStackedKvCaptureInfo *outInfo)
{
    UmiAiTrainingStackedKvCaptureInfo result = {0};
    UmiStatus status;
    if (outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = CaptureCurrent(capture);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedKvStoreInspect(capture->store, &result.stored);
    if (status != UMI_STATUS_OK) return status;
    result.completeReferenceEvaluations = capture->referenceEvaluations;
    const uint64_t perEvaluation =
        (uint64_t)capture->identity.config.contextLength *
        (uint64_t)capture->identity.config.transformerLayers;
    if (perEvaluation != 0U && capture->referenceEvaluations >
        UINT64_MAX / perEvaluation) return UMI_STATUS_CAPACITY_EXCEEDED;
    result.actuallyEvaluatedTokenLayerRows =
        perEvaluation * capture->referenceEvaluations;
    result.vocabularySize = (size_t)capture->identity.config.vocabularySize;
    result.maximumPreviousRowDifference = capture->maximumPreviousRowDifference;
    result.modelIdentity = capture->identity;
    *outInfo = result;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvCaptureRead(
    const UmiAiTrainingStackedKvCapture *capture,
    size_t layer, size_t retainedPosition,
    float *outKey, size_t keyCapacity,
    float *outValue, size_t valueCapacity)
{
    UmiStatus status = CaptureCurrent(capture);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiTrainingStackedKvStoreRead(capture->store, layer, retainedPosition,
        outKey, keyCapacity, outValue, valueCapacity);
}

UmiStatus UmiAiTrainingStackedKvCaptureReset(
    UmiAiTrainingStackedKvCapture *capture)
{
    UmiStatus status = CaptureCurrent(capture);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedKvStoreReset(capture->store);
    if (status == UMI_STATUS_OK) {
        memset(capture->ids, 0, sizeof(capture->ids));
        capture->retained = 0U;
        capture->referenceEvaluations = 0U;
        capture->maximumPreviousRowDifference = 0.0F;
    }
    return status;
}
