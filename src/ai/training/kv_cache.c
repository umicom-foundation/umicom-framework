/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/kv_cache.c
 *
 * PURPOSE:
 *   A bounded inference-only key/value cache for the canonical native decoder.
 *
 * MATHEMATICAL CONTRACT:
 *   Every operator matches the canonical Framework decoder: row-wise pre-LN,
 *   Q/K/V projections, multi-head causal scaled dot-product attention, output
 *   projection, two residual paths, ReLU MLP and logits projection. The same
 *   double-intermediate/float32-storage boundaries are used as in the tape.
 *   This is a qualified inference kernel, NOT a separate training or model
 *   definition. Future accelerated backends must pass the same parity tests.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_cache.h"
#include "umicom/ai/training/tape.h"
#include "umicom/ai/training/matrix_kernel.h"
#include "decoder_private.h"

#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* The mutable state is small enough to stage transactionally on the stack.
 * We never duplicate the large immutable parameter snapshot per token. */
typedef struct UmiAiKvState {
    size_t tokenCount;
    size_t tokens[UMI_AI_DECODER_MAX_CONTEXT];
    float keys[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_CHANNELS];
    float values[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_CHANNELS];
    float lastLogits[UMI_AI_DECODER_MAX_VOCABULARY];
    uint64_t computedRows;
    uint64_t windowRebuilds;
} UmiAiKvState;

/* The previous cache reserved maximum tensor capacity for every parameter.
 * It was correct but its fixed-size weight matrix dominated small models.
 * Retain that layout for review; it is not part of the public C ABI. */
#if 0
struct UmiAiTrainingKvCache {
    UmiAiTrainingDecoder *decoder; /* borrowed; owner guarantees lifetime */
    UmiAiTrainingDecoderConfig config;
    uint64_t decoderSteps;
    size_t lengths[UMI_DECODER_PARAMETER_COUNT];
    float parameters[UMI_DECODER_PARAMETER_COUNT][UMI_AI_TRAINING_MAX_ELEMENTS];
    UmiAiKvState state;
};
#endif

/* One shape-sized allocation owns exact copied parameter values, key/value
 * rows and one last-token logit row. All float regions live in packed[].
 * The small fixed token-ID array is bounded by the public context limit.
 * Per-call UmiAiKvState remains a local staging buffer so rejected operations
 * cannot mutate any persistent state. No duplicate training owner is created. */
struct UmiAiTrainingKvCache {
    UmiAiTrainingDecoder *decoder; /* borrowed; owner guarantees lifetime */
    UmiAiTrainingDecoderConfig config;
    uint64_t decoderSteps;
    size_t allocationBytes;
    size_t lengths[UMI_DECODER_PARAMETER_COUNT];
    float *parameters[UMI_DECODER_PARAMETER_COUNT];
    size_t tokenCount;
    size_t tokens[UMI_AI_DECODER_MAX_CONTEXT];
    uint64_t computedRows;
    uint64_t windowRebuilds;
    float *keys;
    float *values;
    float *lastLogits;
    float packed[];
};

/* All sizes are derived from the canonical decoder schema and checked before
 * allocation. A result is committed only after the entire layout is valid. */
static UmiStatus LayoutForConfig(
    const UmiAiTrainingDecoderConfig *config,
    size_t *outFloatCount, size_t *outBytes,
    size_t outLengths[UMI_DECODER_PARAMETER_COUNT])
{
    size_t lengths[UMI_DECODER_PARAMETER_COUNT] = {0};
    size_t floats = 0U;
    size_t keyValueElements;
    if (config == NULL || outFloatCount == NULL || outBytes == NULL ||
        UmiAiDecoderValidateConfig(config) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t parameter = 0U;
         parameter < (size_t)UMI_DECODER_PARAMETER_COUNT; ++parameter) {
        size_t rows = 0U, columns = 0U;
        UmiAiDecoderParameterShape(config, parameter, &rows, &columns);
        if (rows == 0U || columns == 0U ||
            columns > UMI_AI_TRAINING_MAX_ELEMENTS ||
            rows > UMI_AI_TRAINING_MAX_ELEMENTS / columns)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        const size_t elements = rows * columns;
        if (floats > SIZE_MAX - elements)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        lengths[parameter] = elements;
        floats += elements;
    }
    if ((size_t)config->channels > SIZE_MAX / (size_t)config->contextLength)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    keyValueElements = (size_t)config->channels *
                       (size_t)config->contextLength;
    if (keyValueElements > (SIZE_MAX - floats) / 2U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    floats += 2U * keyValueElements;
    if (floats > SIZE_MAX - (size_t)config->vocabularySize)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    floats += (size_t)config->vocabularySize;
    if (floats > (SIZE_MAX - sizeof(UmiAiTrainingKvCache)) / sizeof(float))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *outFloatCount = floats;
    *outBytes = sizeof(UmiAiTrainingKvCache) + floats * sizeof(float);
    if (outLengths != NULL)
        memcpy(outLengths, lengths, sizeof(lengths));
    return UMI_STATUS_OK;
}

/* Copy the compact persistent cache into an isolated stack transaction. The
 * decoder is single-threaded: callers must not mutate a cache concurrently. */
static void StageState(const UmiAiTrainingKvCache *cache,
                       UmiAiKvState *outState)
{
    const size_t entries = cache->tokenCount * (size_t)cache->config.channels;
    memset(outState, 0, sizeof(*outState));
    outState->tokenCount = cache->tokenCount;
    outState->computedRows = cache->computedRows;
    outState->windowRebuilds = cache->windowRebuilds;
    memcpy(outState->tokens, cache->tokens,
           cache->tokenCount * sizeof(*cache->tokens));
    memcpy(outState->keys, cache->keys, entries * sizeof(float));
    memcpy(outState->values, cache->values, entries * sizeof(float));
    if (cache->tokenCount > 0U)
        memcpy(outState->lastLogits, cache->lastLogits,
               (size_t)cache->config.vocabularySize * sizeof(float));
}

/* Publish the complete state only after every staged projection succeeds.
 * A failed call leaves the token history, numerical outputs and counters
 * untouched. Unused capacity does not contain observable cache entries. */
static void CommitState(UmiAiTrainingKvCache *cache,
                        const UmiAiKvState *state)
{
    const size_t entries = state->tokenCount * (size_t)cache->config.channels;
    cache->tokenCount = state->tokenCount;
    cache->computedRows = state->computedRows;
    cache->windowRebuilds = state->windowRebuilds;
    memcpy(cache->tokens, state->tokens,
           state->tokenCount * sizeof(*cache->tokens));
    memcpy(cache->keys, state->keys, entries * sizeof(float));
    memcpy(cache->values, state->values, entries * sizeof(float));
    if (state->tokenCount > 0U)
        memcpy(cache->lastLogits, state->lastLogits,
               (size_t)cache->config.vocabularySize * sizeof(float));
}


static bool CanStore(double value)
{
    return isfinite(value) && value >= -(double)FLT_MAX &&
           value <= (double)FLT_MAX;
}

static UmiStatus CacheIsCurrent(const UmiAiTrainingKvCache *cache)
{
    UmiAiTrainingDecoderConfig config;
    uint64_t steps;
    UmiStatus status;
    if (cache == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(cache->decoder, &config, &steps);
    if (status != UMI_STATUS_OK) return status;
    /* Parameter shapes and seeds are fixed for the life of a decoder. Only
     * completedSteps can change after construction. An updated decoder must
     * not reuse the copied pre-update projection matrices. */
    if (steps != cache->decoderSteps ||
        config.vocabularySize != cache->config.vocabularySize ||
        config.contextLength != cache->config.contextLength ||
        config.channels != cache->config.channels ||
        config.hiddenChannels != cache->config.hiddenChannels ||
        config.attentionHeads != cache->config.attentionHeads ||
        config.initialSeed != cache->config.initialSeed) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

static const float *Weights(const UmiAiTrainingKvCache *cache,
                            enum UmiAiDecoderParameterId id)
{
    return cache->parameters[(size_t)id];
}

/* The shared training/inference kernel accumulates in double and stores FP32.
 * Caller inputs and outputs are borrowed; staging makes publication atomic. */
static UmiStatus MultiplyRow(const float *row, size_t inputWidth,
                             const float *weights, size_t outputWidth,
                             float *out)
{
    /* The inference kernel and canonical tape now share one numerical
     * implementation, avoiding duplicated dot-product code and permitting
     * future qualified acceleration without changing the decoder ABI.
     * The local staging buffer preserves failure-atomic output publication. */
    UmiStatus status = UmiAiTrainingMatrixMultiply(
        row, 1U, inputWidth, weights, outputWidth,
        UMI_AI_MATRIX_AUTO, out, outputWidth);
    return status == UMI_STATUS_OK ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

#if 0
/* Historical scalar projection, superseded by the shared Framework-owned
 * UmiAiTrainingMatrixMultiply kernel. Retained for numerical review. */
static UmiStatus MultiplyRow(const float *row, size_t inputWidth,
                             const float *weights, size_t outputWidth,
                             float *out)
{
    for (size_t col = 0U; col < outputWidth; ++col) {
        double sum = 0.0;
        for (size_t inner = 0U; inner < inputWidth; ++inner) {
            sum += (double)row[inner] *
                   (double)weights[inner * outputWidth + col];
        }
        if (!CanStore(sum)) return UMI_STATUS_INVALID_STATE;
        out[col] = (float)sum;
    }
    return UMI_STATUS_OK;
}

#endif

static UmiStatus AddRow(const float *left, const float *right,
                        size_t count, float *out)
{
    for (size_t col = 0U; col < count; ++col) {
        double value = (double)left[col] + (double)right[col];
        if (!CanStore(value)) return UMI_STATUS_INVALID_STATE;
        out[col] = (float)value;
    }
    return UMI_STATUS_OK;
}

/* Match the tape's two-pass mean/variance calculation and its 1e-4 epsilon.
 * Gamma and beta are copied from the canonical trained model. */
static UmiStatus NormaliseRow(const float *input, size_t width,
                              const float *gamma, const float *beta,
                              float *out)
{
    double mean = 0.0;
    double variance = 0.0;
    for (size_t i = 0U; i < width; ++i) mean += (double)input[i];
    mean /= (double)width;
    for (size_t i = 0U; i < width; ++i) {
        double delta = (double)input[i] - mean;
        variance += delta * delta;
    }
    variance /= (double)width;
    double inverseStd = 1.0 / sqrt(variance + 1e-4);
    for (size_t i = 0U; i < width; ++i) {
        double value = ((double)input[i] - mean) * inverseStd *
                        (double)gamma[i] + (double)beta[i];
        if (!CanStore(value)) return UMI_STATUS_INVALID_STATE;
        out[i] = (float)value;
    }
    return UMI_STATUS_OK;
}

/* Evaluate exactly one token; earlier K/V projections are reused unchanged.
 * Writing only to a staged UmiAiKvState keeps failed operations atomic. */
static UmiStatus AdvanceRow(const UmiAiTrainingKvCache *cache,
                            UmiAiKvState *state, size_t tokenId)
{
    const size_t time = state->tokenCount;
    const size_t channels = (size_t)cache->config.channels;
    const size_t hidden = (size_t)cache->config.hiddenChannels;
    const size_t vocab = (size_t)cache->config.vocabularySize;
    const size_t heads = (size_t)cache->config.attentionHeads;
    const size_t headWidth = channels / heads;
    float input[UMI_AI_DECODER_MAX_CHANNELS] = {0};
    float norm[UMI_AI_DECODER_MAX_CHANNELS] = {0};
    float query[UMI_AI_DECODER_MAX_CHANNELS] = {0};
    float attended[UMI_AI_DECODER_MAX_CHANNELS] = {0};
    float projection[UMI_AI_DECODER_MAX_CHANNELS] = {0};
    float residual[UMI_AI_DECODER_MAX_CHANNELS] = {0};
    float feedNorm[UMI_AI_DECODER_MAX_CHANNELS] = {0};
    float expanded[UMI_AI_DECODER_MAX_HIDDEN_CHANNELS] = {0};
    float contracted[UMI_AI_DECODER_MAX_CHANNELS] = {0};
    float output[UMI_AI_DECODER_MAX_CHANNELS] = {0};
    double scores[UMI_AI_DECODER_MAX_CONTEXT];
    float *key = &state->keys[time * channels];
    float *value = &state->values[time * channels];
    UmiStatus status;

    if (time >= (size_t)cache->config.contextLength || tokenId >= vocab ||
        state->computedRows == UINT64_MAX) return UMI_STATUS_INVALID_ARGUMENT;

    const float *tokenEmb = Weights(cache, UMI_DECODER_TOKEN_EMBEDDING);
    const float *positionEmb = Weights(cache, UMI_DECODER_POSITION_EMBEDDING);
    for (size_t i = 0U; i < channels; ++i) {
        double v = (double)tokenEmb[tokenId * channels + i] +
                   (double)positionEmb[time * channels + i];
        if (!CanStore(v)) return UMI_STATUS_INVALID_STATE;
        input[i] = (float)v;
    }
    status = NormaliseRow(input, channels,
                          Weights(cache, UMI_DECODER_NORM1_SCALE),
                          Weights(cache, UMI_DECODER_NORM1_BIAS), norm);
    if (status != UMI_STATUS_OK) return status;
    status = MultiplyRow(norm, channels, Weights(cache, UMI_DECODER_QUERY),
                         channels, query);
    if (status != UMI_STATUS_OK) return status;
    status = MultiplyRow(norm, channels, Weights(cache, UMI_DECODER_KEY),
                         channels, key);
    if (status != UMI_STATUS_OK) return status;
    status = MultiplyRow(norm, channels, Weights(cache, UMI_DECODER_VALUE),
                         channels, value);
    if (status != UMI_STATUS_OK) return status;

    /* Use the exact causal softmax normalisation and float-probability
     * materialisation of the training tape. Each attention head occupies a
     * contiguous channel interval; no future K/V rows are referenced. */
    for (size_t head = 0U; head < heads; ++head) {
        const size_t offset = head * headWidth;
        double maximum = -INFINITY;
        double denominator = 0.0;
        const double scale = sqrt((double)headWidth);
        for (size_t past = 0U; past <= time; ++past) {
            double dot = 0.0;
            for (size_t ch = 0U; ch < headWidth; ++ch) {
                dot += (double)query[offset + ch] *
                       (double)state->keys[past * channels + offset + ch];
            }
            scores[past] = dot / scale;
            if (!isfinite(scores[past])) return UMI_STATUS_INVALID_STATE;
            if (scores[past] > maximum) maximum = scores[past];
        }
        for (size_t past = 0U; past <= time; ++past)
            denominator += exp(scores[past] - maximum);
        if (!isfinite(denominator) || denominator <= 0.0)
            return UMI_STATUS_INVALID_STATE;
        for (size_t ch = 0U; ch < headWidth; ++ch) {
            double result = 0.0;
            for (size_t past = 0U; past <= time; ++past) {
                float probability = (float)(exp(scores[past] - maximum) /
                                            denominator);
                result += (double)probability *
                          (double)state->values[past * channels + offset + ch];
            }
            if (!CanStore(result)) return UMI_STATUS_INVALID_STATE;
            attended[offset + ch] = (float)result;
        }
    }
    status = MultiplyRow(attended, channels,
                         Weights(cache, UMI_DECODER_PROJECTION), channels,
                         projection);
    if (status != UMI_STATUS_OK) return status;
    status = AddRow(input, projection, channels, residual);
    if (status != UMI_STATUS_OK) return status;

    status = NormaliseRow(residual, channels,
                          Weights(cache, UMI_DECODER_NORM2_SCALE),
                          Weights(cache, UMI_DECODER_NORM2_BIAS), feedNorm);
    if (status != UMI_STATUS_OK) return status;
    status = MultiplyRow(feedNorm, channels, Weights(cache, UMI_DECODER_UP),
                         hidden, expanded);
    if (status != UMI_STATUS_OK) return status;
    status = AddRow(expanded, Weights(cache, UMI_DECODER_UP_BIAS),
                    hidden, expanded);
    if (status != UMI_STATUS_OK) return status;
    for (size_t ch = 0U; ch < hidden; ++ch)
        expanded[ch] = expanded[ch] > 0.0F ? expanded[ch] : 0.0F;
    status = MultiplyRow(expanded, hidden, Weights(cache, UMI_DECODER_DOWN),
                         channels, contracted);
    if (status != UMI_STATUS_OK) return status;
    status = AddRow(contracted, Weights(cache, UMI_DECODER_DOWN_BIAS),
                    channels, contracted);
    if (status != UMI_STATUS_OK) return status;
    status = AddRow(residual, contracted, channels, output);
    if (status != UMI_STATUS_OK) return status;
    status = MultiplyRow(output, channels, Weights(cache, UMI_DECODER_LOGITS),
                         vocab, state->lastLogits);
    if (status != UMI_STATUS_OK) return status;

    state->tokens[time] = tokenId;
    ++state->tokenCount;
    ++state->computedRows;
    return UMI_STATUS_OK;
}

/* Superseded fixed-reservation creation path, retained for review.
 * The live implementation below sizes one allocation using actual parameter
 * shapes, and retains the same failure-atomic public ownership contract. */
#if 0
UmiStatus UmiAiTrainingKvCacheCreate(UmiAiTrainingDecoder *decoder,
                                     UmiAiTrainingKvCache **outCache)
{
    UmiAiTrainingKvCache *candidate;
    UmiStatus status;
    if (decoder == NULL || outCache == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->decoder = decoder;
    status = UmiAiTrainingDecoderInspect(decoder, &candidate->config,
                                         &candidate->decoderSteps);
    if (status != UMI_STATUS_OK) goto failed;
    if (UmiAiDecoderValidateConfig(&candidate->config) != UMI_STATUS_OK) {
        status = UMI_STATUS_INVALID_STATE;
        goto failed;
    }
    for (size_t p = 0U; p < (size_t)UMI_DECODER_PARAMETER_COUNT; ++p) {
        size_t rows = 0U, columns = 0U;
        UmiAiDecoderParameterShape(&candidate->config, p, &rows, &columns);
        if (columns == 0U || rows > UMI_AI_TRAINING_MAX_ELEMENTS / columns) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto failed;
        }
        candidate->lengths[p] = rows * columns;
        status = UmiAiTrainingTensorValues(decoder->parameters[p],
            candidate->parameters[p], candidate->lengths[p]);
        if (status != UMI_STATUS_OK) goto failed;
    }
    *outCache = candidate;
    return UMI_STATUS_OK;
failed:
    free(candidate);
    return status;
}
#endif

UmiStatus UmiAiTrainingKvCacheAllocationForConfig(
    const UmiAiTrainingDecoderConfig *config, size_t *outBytes)
{
    size_t floatCount = 0U;
    size_t allocated = 0U;
    UmiStatus status;
    if (outBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = LayoutForConfig(config, &floatCount, &allocated, NULL);
    if (status != UMI_STATUS_OK) return status;
    *outBytes = allocated;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingKvCacheCreate(UmiAiTrainingDecoder *decoder,
                                     UmiAiTrainingKvCache **outCache)
{
    UmiAiTrainingDecoderConfig config;
    UmiAiTrainingKvCache *candidate;
    uint64_t steps;
    size_t floatCount = 0U;
    size_t allocated = 0U;
    size_t lengths[UMI_DECODER_PARAMETER_COUNT];
    size_t offset = 0U;
    size_t keyValueElements;
    UmiStatus status;
    if (decoder == NULL || outCache == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(decoder, &config, &steps);
    if (status != UMI_STATUS_OK) return status;
    status = LayoutForConfig(&config, &floatCount, &allocated, lengths);
    if (status != UMI_STATUS_OK) return status;
    candidate = calloc(1U, allocated);
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->decoder = decoder;
    candidate->config = config;
    candidate->decoderSteps = steps;
    candidate->allocationBytes = allocated;
    for (size_t p = 0U; p < (size_t)UMI_DECODER_PARAMETER_COUNT; ++p) {
        candidate->lengths[p] = lengths[p];
        candidate->parameters[p] = &candidate->packed[offset];
        offset += lengths[p];
        status = UmiAiTrainingTensorValues(decoder->parameters[p],
                         candidate->parameters[p], lengths[p]);
        if (status != UMI_STATUS_OK) {
            free(candidate);
            return status;
        }
    }
    keyValueElements = (size_t)config.contextLength * (size_t)config.channels;
    candidate->keys = &candidate->packed[offset];
    offset += keyValueElements;
    candidate->values = &candidate->packed[offset];
    offset += keyValueElements;
    candidate->lastLogits = &candidate->packed[offset];
    offset += (size_t)config.vocabularySize;
    /* The checked layout is authoritative; never silently leave unused or
     * missing payload behind after publishing the owned cache. */
    if (offset != floatCount) {
        free(candidate);
        return UMI_STATUS_INTERNAL_ERROR;
    }
    *outCache = candidate;
    return UMI_STATUS_OK;
}

/* The memory inspector reports the exact requested allocation for this
 * instance; it excludes allocator metadata and temporary stack staging. */
UmiStatus UmiAiTrainingKvCacheAllocatedBytes(
    const UmiAiTrainingKvCache *cache, size_t *outBytes)
{
    if (cache == NULL || outBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outBytes = cache->allocationBytes;
    return UMI_STATUS_OK;
}

void UmiAiTrainingKvCacheDestroy(UmiAiTrainingKvCache *cache)
{
    free(cache);
}

UmiStatus UmiAiTrainingKvCacheReset(UmiAiTrainingKvCache *cache)
{
    UmiStatus status = CacheIsCurrent(cache);
    if (status != UMI_STATUS_OK) return status;
    /* Clearing only the compact mutable state keeps model parameters intact.
     * Previously the complete fixed-size state was cleared as one structure. */
#if 0
    memset(&cache->state, 0, sizeof(cache->state));
#endif
    cache->tokenCount = 0U;
    cache->computedRows = 0U;
    cache->windowRebuilds = 0U;
    memset(cache->tokens, 0, sizeof(cache->tokens));
    const size_t keyValueElements = (size_t)cache->config.contextLength *
                                    (size_t)cache->config.channels;
    memset(cache->keys, 0, keyValueElements * sizeof(float));
    memset(cache->values, 0, keyValueElements * sizeof(float));
    memset(cache->lastLogits, 0,
           (size_t)cache->config.vocabularySize * sizeof(float));
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingKvCachePrefill(UmiAiTrainingKvCache *cache,
                                     const size_t *tokenIds, size_t tokenCount,
                                     float *outLogits, size_t capacity)
{
    UmiAiKvState proposed = {0};
    UmiStatus status = CacheIsCurrent(cache);
    if (status != UMI_STATUS_OK) return status;
    if (tokenIds == NULL || outLogits == NULL || tokenCount == 0U ||
        tokenCount > (size_t)cache->config.contextLength)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < (size_t)cache->config.vocabularySize)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < tokenCount; ++i)
        if (tokenIds[i] >= (size_t)cache->config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < tokenCount; ++i) {
        status = AdvanceRow(cache, &proposed, tokenIds[i]);
        if (status != UMI_STATUS_OK) return status;
    }
    /* The former direct structure assignment is retained for comparison.
     * The packed-state owner now copies only valid token/key/value extents. */
#if 0
    cache->state = proposed;
#endif
    CommitState(cache, &proposed);
    memcpy(outLogits, proposed.lastLogits,
           (size_t)cache->config.vocabularySize * sizeof(*outLogits));
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingKvCacheAppend(UmiAiTrainingKvCache *cache,
                                    size_t tokenId,
                                    float *outLogits, size_t capacity)
{
    UmiAiKvState proposed;
    UmiStatus status = CacheIsCurrent(cache);
    if (status != UMI_STATUS_OK) return status;
    if (outLogits == NULL || tokenId >= (size_t)cache->config.vocabularySize)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < (size_t)cache->config.vocabularySize)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Snapshot the compact published state before attempting an append. */
#if 0
    proposed = cache->state;
#endif
    StageState(cache, &proposed);
    if (proposed.tokenCount < (size_t)cache->config.contextLength) {
        status = AdvanceRow(cache, &proposed, tokenId);
        if (status != UMI_STATUS_OK) return status;
    } else {
        /* Absolute positional embeddings assign a different position to
         * every surviving token after eviction. Old K/V must therefore be
         * invalidated and recomputed, not shifted and reused. */
        size_t context[UMI_AI_DECODER_MAX_CONTEXT];
        size_t length = proposed.tokenCount;
        uint64_t computed = proposed.computedRows;
        uint64_t rebuilt = proposed.windowRebuilds;
        if (rebuilt == UINT64_MAX ||
            computed > UINT64_MAX - (uint64_t)length)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t i = 1U; i < length; ++i)
            context[i - 1U] = proposed.tokens[i];
        context[length - 1U] = tokenId;
        memset(&proposed, 0, sizeof(proposed));
        proposed.computedRows = computed;
        proposed.windowRebuilds = rebuilt + UINT64_C(1);
        for (size_t i = 0U; i < length; ++i) {
            status = AdvanceRow(cache, &proposed, context[i]);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    /* The former direct structure assignment is retained for comparison.
     * The packed-state owner now copies only valid token/key/value extents. */
#if 0
    cache->state = proposed;
#endif
    CommitState(cache, &proposed);
    memcpy(outLogits, proposed.lastLogits,
           (size_t)cache->config.vocabularySize * sizeof(*outLogits));
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingKvCacheInspect(const UmiAiTrainingKvCache *cache,
                                     size_t *outTokenCount,
                                     uint64_t *outComputedRows,
                                     uint64_t *outWindowRebuilds)
{
    UmiStatus status = CacheIsCurrent(cache);
    if (status != UMI_STATUS_OK) return status;
    if (outTokenCount == NULL || outComputedRows == NULL ||
        outWindowRebuilds == NULL) return UMI_STATUS_INVALID_ARGUMENT;
#if 0
    *outTokenCount = cache->state.tokenCount;
    *outComputedRows = cache->state.computedRows;
    *outWindowRebuilds = cache->state.windowRebuilds;
#endif
    *outTokenCount = cache->tokenCount;
    *outComputedRows = cache->computedRows;
    *outWindowRebuilds = cache->windowRebuilds;
    return UMI_STATUS_OK;
}

/* Historical fixed-allocation report. Retained for review: the revised
 * implementation reports a conservative shape-independent upper bound.
 * Existing callers can still use this as an admission limit. Callers needing
 * actual memory must use AllocationForConfig or AllocatedBytes instead. */
#if 0
/* Use the real private structure rather than assuming a compact K/V-only
 * allocation. Parameter reservations dominate this educational cache's heap
 * use and are part of capacity planning until we add variable-sized storage. */
UmiStatus UmiAiTrainingKvCacheAllocationSize(size_t *outBytes)
{
    if (outBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outBytes = sizeof(UmiAiTrainingKvCache);
    return UMI_STATUS_OK;
}
#endif

UmiStatus UmiAiTrainingKvCacheAllocationSize(size_t *outBytes)
{
    if (outBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* The maximum tensor and sequence dimensions are compile-time bounded.
     * This is a conservative upper bound, not a live model allocation. */
    const size_t maximumFloatElements =
        (size_t)UMI_DECODER_PARAMETER_COUNT * UMI_AI_TRAINING_MAX_ELEMENTS +
        2U * (size_t)UMI_AI_DECODER_MAX_CONTEXT *
             (size_t)UMI_AI_DECODER_MAX_CHANNELS +
        (size_t)UMI_AI_DECODER_MAX_VOCABULARY;
    *outBytes = sizeof(UmiAiTrainingKvCache) +
                maximumFloatElements * sizeof(float);
    return UMI_STATUS_OK;
}
