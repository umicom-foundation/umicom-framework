/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_inference_qualification.c
 *
 * PURPOSE:
 *   Audit every growing-prefix logit, newly produced K/V projection and
 *   retained K/V row; require canonical full rebuild on position eviction.
 *   This is an additive C23 qualification service, not a second decoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_inference_qualification.h"
#include "umicom/ai/training/stacked_kv_step.h"
#include "umicom/ai/training/stacked_kv_store.h"
#include "umicom/ai/training/stacked_projection_trace.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#define QUALIFY_KV_CAPACITY \
    (UMI_AI_STACKED_MAX_LAYERS * UMI_AI_STACKED_MAX_CONTEXT * \
     UMI_AI_STACKED_MAX_CHANNELS)
#define QUALIFY_LOGIT_CAPACITY \
    (UMI_AI_STACKED_MAX_CONTEXT * UMI_AI_STACKED_MAX_VOCABULARY)
#define QUALIFY_STEP_KV_CAPACITY \
    (UMI_AI_STACKED_MAX_LAYERS * UMI_AI_STACKED_MAX_CHANNELS)

/* Input vectors are borrowed. Reject a caller report that aliases
 * inputs rather than overwriting any input on successful publication. */
static int QualificationOverlaps(const void *left, size_t leftBytes,
                                 const void *right, size_t rightBytes)
{
    const uintptr_t a = (uintptr_t)left;
    const uintptr_t b = (uintptr_t)right;
    if (a > UINTPTR_MAX - leftBytes || b > UINTPTR_MAX - rightBytes)
        return 1;
    return a < b + rightBytes && b < a + leftBytes;
}

/* All comparison counters count actual inspected FP32 elements, never
 * inferred floating-point operations or wall-clock savings. */
static UmiStatus QualificationCompare(
    const float *reference, const float *candidate, size_t count,
    const UmiAiTrainingInferenceParityTolerance *tolerance,
    size_t *cumulative, double *maximum)
{
    UmiAiTrainingInferenceParityReport comparison = {0};
    UmiStatus status = UmiAiTrainingInferenceParityCompare(
        reference, candidate, count, tolerance, &comparison);
    if (status != UMI_STATUS_OK) return status;
    if (!comparison.withinTolerance) return UMI_STATUS_INVALID_STATE;
    if (*cumulative > SIZE_MAX - count) return UMI_STATUS_CAPACITY_EXCEEDED;
    *cumulative += count;
    if (comparison.maximumAbsoluteDifference > *maximum)
        *maximum = comparison.maximumAbsoluteDifference;
    return UMI_STATUS_OK;
}

/* Compare source-trace logits to an independent non-observing canonical
 * model forward. Both existing APIs retain ownership of their graph nodes. */
static UmiStatus QualificationTrace(
    UmiAiTrainingStackedDecoder *model, const size_t *history,
    size_t retained, size_t context, size_t vocabulary,
    size_t keyValueCount,
    const UmiAiTrainingInferenceParityTolerance *tolerance,
    UmiAiTrainingStackedQualificationReport *report,
    float *keys, float *values, float *tracedLogits,
    float *canonicalLogits)
{
    size_t padded[UMI_AI_STACKED_MAX_CONTEXT];
    for (size_t i = 0U; i < context; ++i)
        padded[i] = i < retained ? history[i] : UMI_AI_BYTE_BOS;
    UmiStatus status = UmiAiTrainingStackedDecoderTraceProjections(
        model, padded, context, keys, keyValueCount, values, keyValueCount,
        tracedLogits, context * vocabulary);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedDecoderLogits(
        model, padded, context, canonicalLogits, context * vocabulary);
    if (status != UMI_STATUS_OK) return status;
    return QualificationCompare(canonicalLogits, tracedLogits,
        retained * vocabulary, tolerance,
        &report->independentCanonicalLogitsCompared,
        &report->maximumLogitDifference);
}

/* Every stored row is compared with independently traced, full-context K/V.
 * Data is in [layer][context position][channel] in the canonical trace. */
static UmiStatus QualificationCheckStore(
    const UmiAiTrainingStackedKvStore *store,
    const float *keys, const float *values,
    size_t layers, size_t context, size_t channels, size_t retained,
    const UmiAiTrainingInferenceParityTolerance *tolerance,
    UmiAiTrainingStackedQualificationReport *report)
{
    float cachedKeys[UMI_AI_STACKED_MAX_CHANNELS];
    float cachedValues[UMI_AI_STACKED_MAX_CHANNELS];
    UmiStatus status;
    for (size_t layer = 0U; layer < layers; ++layer) {
        for (size_t row = 0U; row < retained; ++row) {
            const size_t offset = (layer * context + row) * channels;
            status = UmiAiTrainingStackedKvStoreRead(store, layer, row,
                cachedKeys, channels, cachedValues, channels);
            if (status != UMI_STATUS_OK) return status;
            status = QualificationCompare(keys + offset, cachedKeys,
                channels, tolerance, &report->retainedKeyValueScalarsCompared,
                &report->maximumKeyValueDifference);
            if (status != UMI_STATUS_OK) return status;
            status = QualificationCompare(values + offset, cachedValues,
                channels, tolerance, &report->retainedKeyValueScalarsCompared,
                &report->maximumKeyValueDifference);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedInferenceQualify(
    UmiAiTrainingStackedDecoder *model,
    const size_t *prefixTokenIds, size_t prefixTokenCount,
    const size_t *continuationTokenIds, size_t continuationTokenCount,
    const UmiAiTrainingInferenceParityTolerance *tolerance,
    UmiAiTrainingStackedQualificationReport *outReport)
{
    UmiAiTrainingStackedQualificationReport staged = {0};
    UmiAiTrainingStackedKvStoreConfig storeConfig = {0};
    UmiAiTrainingStackedKvStore *store = NULL;
    UmiAiTrainingStackedKvStepReport stepReport = {0};
    UmiStatus status;
    size_t history[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    float keys[QUALIFY_KV_CAPACITY] = {0};
    float values[QUALIFY_KV_CAPACITY] = {0};
    float compactKeys[QUALIFY_KV_CAPACITY] = {0};
    float compactValues[QUALIFY_KV_CAPACITY] = {0};
    float newKeys[QUALIFY_STEP_KV_CAPACITY] = {0};
    float newValues[QUALIFY_STEP_KV_CAPACITY] = {0};
    float candidateLogits[UMI_AI_STACKED_MAX_VOCABULARY] = {0};
    float tracedLogits[QUALIFY_LOGIT_CAPACITY] = {0};
    float previousLogits[QUALIFY_LOGIT_CAPACITY] = {0};
    float canonicalLogits[QUALIFY_LOGIT_CAPACITY] = {0};

    if (model == NULL || prefixTokenIds == NULL ||
        continuationTokenIds == NULL || tolerance == NULL ||
        outReport == NULL || prefixTokenCount == 0U ||
        prefixTokenCount > UMI_AI_STACKED_MAX_CONTEXT ||
        continuationTokenCount == 0U ||
        continuationTokenCount > UMI_AI_STACKED_MAX_GENERATED_TOKENS ||
        !isfinite(tolerance->absoluteTolerance) ||
        !isfinite(tolerance->relativeTolerance) ||
        tolerance->absoluteTolerance < 0.0F ||
        tolerance->relativeTolerance < 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (QualificationOverlaps(outReport, sizeof(*outReport),
            prefixTokenIds, prefixTokenCount * sizeof(size_t)) ||
        QualificationOverlaps(outReport, sizeof(*outReport),
            continuationTokenIds, continuationTokenCount * sizeof(size_t)) ||
        QualificationOverlaps(outReport, sizeof(*outReport),
            tolerance, sizeof(*tolerance)))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedModelIdentityCapture(model,
        &staged.checkpoint);
    if (status != UMI_STATUS_OK) return status;
    const size_t context = (size_t)staged.checkpoint.config.contextLength;
    const size_t layers = (size_t)staged.checkpoint.config.transformerLayers;
    const size_t channels = (size_t)staged.checkpoint.config.channels;
    const size_t vocabulary = (size_t)staged.checkpoint.config.vocabularySize;
    if (context == 0U || context > UMI_AI_STACKED_MAX_CONTEXT ||
        layers == 0U || layers > UMI_AI_STACKED_MAX_LAYERS ||
        channels == 0U || channels > UMI_AI_STACKED_MAX_CHANNELS ||
        vocabulary <= UMI_AI_BYTE_BOS ||
        vocabulary > UMI_AI_STACKED_MAX_VOCABULARY ||
        prefixTokenCount > context)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < prefixTokenCount; ++i)
        if (prefixTokenIds[i] >= vocabulary)
            return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < continuationTokenCount; ++i)
        if (continuationTokenIds[i] >= vocabulary)
            return UMI_STATUS_INVALID_ARGUMENT;

    staged.initialPromptTokens = prefixTokenCount;
    staged.continuationTokens = continuationTokenCount;
    staged.transformerLayers = layers;
    staged.contextLength = context;
    memcpy(storeConfig.checkpointSha256,
        staged.checkpoint.checkpointSha256,
        UMI_AI_STACKED_KV_IDENTITY_BYTES);
    storeConfig.decoder = staged.checkpoint.config;
    status = UmiAiTrainingStackedKvStoreCreate(&storeConfig, &store);
    if (status != UMI_STATUS_OK) return status;

    memcpy(history, prefixTokenIds, prefixTokenCount * sizeof(size_t));
    size_t retained = prefixTokenCount;
    const size_t fullKvCount = layers * context * channels;
    const size_t newKvCount = layers * channels;
    status = QualificationTrace(model, history, retained, context, vocabulary,
        fullKvCount, tolerance, &staged, keys, values,
        tracedLogits, canonicalLogits);
    if (status != UMI_STATUS_OK) goto release;
    for (size_t layer = 0U; layer < layers; ++layer) {
        const size_t traceStart = layer * context * channels;
        const size_t compactStart = layer * retained * channels;
        memcpy(compactKeys + compactStart, keys + traceStart,
            retained * channels * sizeof(float));
        memcpy(compactValues + compactStart, values + traceStart,
            retained * channels * sizeof(float));
    }
    status = UmiAiTrainingStackedKvStorePrefill(store, history, retained,
        compactKeys, layers * retained * channels,
        compactValues, layers * retained * channels);
    if (status != UMI_STATUS_OK) goto release;
    status = QualificationCheckStore(store, keys, values, layers, context,
        channels, retained, tolerance, &staged);
    if (status != UMI_STATUS_OK) goto release;
    memcpy(previousLogits, tracedLogits, context * vocabulary * sizeof(float));

    for (size_t index = 0U; index < continuationTokenCount; ++index) {
        status = UmiAiTrainingStackedModelIdentityVerify(
            model, &staged.checkpoint);
        if (status != UMI_STATUS_OK) goto release;
        const size_t tokenId = continuationTokenIds[index];
        if (retained < context) {
            const size_t previousCount = retained;
            status = UmiAiTrainingStackedKvStepPropose(model, store, tokenId,
                newKeys, newKvCount, newValues, newKvCount,
                candidateLogits, vocabulary, &stepReport);
            if (status != UMI_STATUS_OK) goto release;
            if (stepReport.retainedBefore != previousCount ||
                stepReport.retainedAfter != previousCount + 1U ||
                stepReport.transformerLayers != layers ||
                stepReport.newlyProjectedTokenLayerRows != (uint64_t)layers ||
                stepReport.attentionQueryLayerRowsExecuted != (uint64_t)layers) {
                status = UMI_STATUS_INVALID_STATE;
                goto release;
            }
            history[retained] = tokenId;
            ++retained;
            status = QualificationTrace(model, history, retained, context,
                vocabulary, fullKvCount, tolerance, &staged,
                keys, values, tracedLogits, canonicalLogits);
            if (status != UMI_STATUS_OK) goto release;
            status = QualificationCompare(previousLogits, tracedLogits,
                previousCount * vocabulary, tolerance,
                &staged.unchangedPrefixLogitsCompared,
                &staged.maximumLogitDifference);
            if (status != UMI_STATUS_OK) goto release;
            status = QualificationCompare(canonicalLogits +
                previousCount * vocabulary, candidateLogits,
                vocabulary, tolerance, &staged.incrementalLogitsCompared,
                &staged.maximumLogitDifference);
            if (status != UMI_STATUS_OK) goto release;
            for (size_t layer = 0U; layer < layers; ++layer) {
                const size_t fullOffset = (layer * context + previousCount) *
                    channels;
                const size_t newOffset = layer * channels;
                status = QualificationCompare(keys + fullOffset,
                    newKeys + newOffset, channels, tolerance,
                    &staged.incrementalKeyValueScalarsCompared,
                    &staged.maximumKeyValueDifference);
                if (status != UMI_STATUS_OK) goto release;
                status = QualificationCompare(values + fullOffset,
                    newValues + newOffset, channels, tolerance,
                    &staged.incrementalKeyValueScalarsCompared,
                    &staged.maximumKeyValueDifference);
                if (status != UMI_STATUS_OK) goto release;
            }
            status = UmiAiTrainingStackedKvStoreAppend(store, tokenId,
                newKeys, newKvCount, newValues, newKvCount);
            if (status != UMI_STATUS_OK) goto release;
            ++staged.growingPrefixSteps;
        } else {
            /* Learned absolute position zero now belongs to a different
             * token. Historical upper-layer K/V cannot simply shift. */
            memmove(history, history + 1U,
                (context - 1U) * sizeof(size_t));
            history[context - 1U] = tokenId;
            status = QualificationTrace(model, history, context, context,
                vocabulary, fullKvCount, tolerance, &staged,
                keys, values, tracedLogits, canonicalLogits);
            if (status != UMI_STATUS_OK) goto release;
            status = UmiAiTrainingStackedKvStoreReplaceRolled(store,
                history, context, keys, fullKvCount, values, fullKvCount);
            if (status != UMI_STATUS_OK) goto release;
            ++staged.fullWindowRebuilds;
        }
        status = QualificationCheckStore(store, keys, values, layers,
            context, channels, retained, tolerance, &staged);
        if (status != UMI_STATUS_OK) goto release;
        status = UmiAiTrainingStackedKvStoreCheckIdentity(store,
            staged.checkpoint.checkpointSha256);
        if (status != UMI_STATUS_OK) goto release;
        memcpy(previousLogits, tracedLogits,
            context * vocabulary * sizeof(float));
    }
    status = UmiAiTrainingStackedModelIdentityVerify(
        model, &staged.checkpoint);
    if (status != UMI_STATUS_OK) goto release;
    *outReport = staged;
release:
    UmiAiTrainingStackedKvStoreDestroy(store);
    return status;
}
