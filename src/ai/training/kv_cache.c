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

struct UmiAiTrainingKvCache {
    UmiAiTrainingDecoder *decoder; /* borrowed; owner guarantees lifetime */
    UmiAiTrainingDecoderConfig config;
    uint64_t decoderSteps;
    size_t lengths[UMI_DECODER_PARAMETER_COUNT];
    float parameters[UMI_DECODER_PARAMETER_COUNT][UMI_AI_TRAINING_MAX_ELEMENTS];
    UmiAiKvState state;
};

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

/* The training-tape Matmul uses double accumulation and float32 outputs.
 * Input and output arrays cannot alias in this helper. */
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

void UmiAiTrainingKvCacheDestroy(UmiAiTrainingKvCache *cache)
{
    free(cache);
}

UmiStatus UmiAiTrainingKvCacheReset(UmiAiTrainingKvCache *cache)
{
    UmiStatus status = CacheIsCurrent(cache);
    if (status != UMI_STATUS_OK) return status;
    memset(&cache->state, 0, sizeof(cache->state));
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
    cache->state = proposed;
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
    proposed = cache->state;
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
    cache->state = proposed;
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
    *outTokenCount = cache->state.tokenCount;
    *outComputedRows = cache->state.computedRows;
    *outWindowRebuilds = cache->state.windowRebuilds;
    return UMI_STATUS_OK;
}
