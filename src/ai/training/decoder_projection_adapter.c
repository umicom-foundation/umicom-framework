/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/decoder_projection_adapter.c
 *
 * PURPOSE:
 *   Train genuine native Transformer Q/K/V, attention-output and feed-forward
 *   low-rank projections with supervised, assistant-response-selected loss.
 *
 * ARCHITECTURE:
 *   A private tape owns frozen copies of the decoder's fifteen original
 *   parameter tensors and selected trainable low-rank leaves. Both token and
 *   positional embeddings, the canonical Transformer composition, output
 *   projection, selected cross-entropy and SGD come from existing Framework
 *   services. The historical decoder's parameter-prefix rewind is untouched.
 *
 *   The borrowed decoder remains unchanged, checked by exact portable
 *   checkpoint identity. This is NOT an alternative decoder implementation:
 *   only tensor ownership and the existing operations' composition are here.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder_projection_adapter.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/low_rank.h"
#include "umicom/ai/training/selected_loss.h"
#include "umicom/ai/training/transformer.h"
#include "umicom/base/sha256.h"
#include "decoder_private.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ADAPTER_ROLE_COUNT 6U

struct UmiAiTrainingDecoderProjectionAdapter {
    UmiAiTrainingDecoder *borrowedDecoder;
    UmiAiTrainingTape *tape;
    UmiAiTrainingDecoderConfig config;
    UmiAiTrainingTensor *frozen[UMI_DECODER_PARAMETER_COUNT];
    UmiAiTrainingLowRankProjection projections[ADAPTER_ROLE_COUNT];
    UmiAiTrainingTransformerLowRankBindings bindings;
    UmiAiTrainingTensor *trainables[UMI_AI_TRANSFORMER_LOW_RANK_MAX_TRAINABLES];
    size_t trainableCount;
    size_t trainableElements;
    size_t rank;
    size_t retainedNodes;
    uint32_t mask;
    uint64_t borrowedDecoderSteps;
    uint64_t completedAdapterSteps;
    char checkpointSha256[UMI_SHA256_HEX_CAPACITY];
};

/* These are the established decoder's internal parameter identifiers, not
 * a new public ABI or a second checkpoint parameter ordering. */
static const size_t ADAPTER_PARAMETERS[ADAPTER_ROLE_COUNT] = {
    UMI_DECODER_QUERY,
    UMI_DECODER_KEY,
    UMI_DECODER_VALUE,
    UMI_DECODER_PROJECTION,
    UMI_DECODER_UP,
    UMI_DECODER_DOWN
};

static const uint32_t ADAPTER_FLAGS[ADAPTER_ROLE_COUNT] = {
    UMI_AI_DECODER_ADAPT_QUERY,
    UMI_AI_DECODER_ADAPT_KEY,
    UMI_AI_DECODER_ADAPT_VALUE,
    UMI_AI_DECODER_ADAPT_ATTENTION_OUTPUT,
    UMI_AI_DECODER_ADAPT_FEED_FORWARD_UP,
    UMI_AI_DECODER_ADAPT_FEED_FORWARD_DOWN
};

static int AdapterSameConfig(const UmiAiTrainingDecoderConfig *left,
                             const UmiAiTrainingDecoderConfig *right)
{
    return left->vocabularySize == right->vocabularySize &&
           left->contextLength == right->contextLength &&
           left->channels == right->channels &&
           left->hiddenChannels == right->hiddenChannels &&
           left->attentionHeads == right->attentionHeads &&
           left->initialSeed == right->initialSeed;
}

/* Re-encode the canonical decoder instead of hashing an in-memory C struct.
 * This includes actual weights and training steps and does not rely on padding
 * bytes, native endianness or caller-controlled model descriptions. */
static UmiStatus AdapterCheckpointIdentity(
    const UmiAiTrainingDecoder *decoder,
    char outHex[UMI_SHA256_HEX_CAPACITY])
{
    size_t required = 0U, written = 0U;
    uint8_t *checkpoint = NULL;
    char digest[UMI_SHA256_HEX_CAPACITY] = {0};
    UmiStatus status;
    if (decoder == NULL || outHex == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderCheckpointSize(decoder, &required);
    if (status != UMI_STATUS_OK) return status;
    if (required == 0U || required > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    checkpoint = malloc(required);
    if (checkpoint == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingDecoderCheckpointEncode(
        decoder, checkpoint, required, &written);
    if (status == UMI_STATUS_OK && written != required)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = UmiSha256Buffer(checkpoint, written, digest);
    if (status == UMI_STATUS_OK)
        memcpy(outHex, digest, sizeof(digest));
    free(checkpoint);
    return status;
}

static UmiStatus AdapterCheckBorrowedModel(
    const UmiAiTrainingDecoderProjectionAdapter *adapter)
{
    UmiAiTrainingDecoderConfig current = {0};
    uint64_t steps = 0U;
    char digest[UMI_SHA256_HEX_CAPACITY] = {0};
    UmiStatus status;
    if (adapter == NULL || adapter->borrowedDecoder == NULL ||
        adapter->tape == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(adapter->borrowedDecoder,
                                         &current, &steps);
    if (status != UMI_STATUS_OK) return status;
    if (!AdapterSameConfig(&current, &adapter->config) ||
        steps != adapter->borrowedDecoderSteps)
        return UMI_STATUS_INVALID_STATE;
    status = AdapterCheckpointIdentity(adapter->borrowedDecoder, digest);
    if (status != UMI_STATUS_OK) return status;
    return memcmp(digest, adapter->checkpointSha256,
                  sizeof(digest)) == 0
           ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

void UmiAiTrainingDecoderProjectionAdapterDestroy(
    UmiAiTrainingDecoderProjectionAdapter *adapter)
{
    if (adapter == NULL) return;
    UmiAiTrainingTapeDestroy(adapter->tape);
    free(adapter);
}

static UmiStatus AdapterCopyFrozenParameters(
    UmiAiTrainingDecoderProjectionAdapter *adapter)
{
    UmiStatus status;
    for (size_t index = 0U; index < UMI_DECODER_PARAMETER_COUNT; ++index) {
        size_t rows = 0U, columns = 0U;
        float *values = NULL;
        UmiAiDecoderParameterShape(&adapter->config, index, &rows, &columns);
        if (rows == 0U || columns == 0U ||
            rows > UMI_AI_TRAINING_MAX_ELEMENTS / columns)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        size_t count = rows * columns;
        values = malloc(count * sizeof(*values));
        if (values == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        status = UmiAiTrainingTensorValues(
            adapter->borrowedDecoder->parameters[index], values, count);
        if (status == UMI_STATUS_OK)
            status = UmiAiTrainingTensorCreate(adapter->tape,
                       rows, columns, values, count, 0,
                       &adapter->frozen[index]);
        free(values);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

static void AdapterBindProjection(
    UmiAiTrainingDecoderProjectionAdapter *adapter, size_t index)
{
    const UmiAiTrainingLowRankProjection *chosen = &adapter->projections[index];
    switch (index) {
    case 0U: adapter->bindings.query = chosen; break;
    case 1U: adapter->bindings.key = chosen; break;
    case 2U: adapter->bindings.value = chosen; break;
    case 3U: adapter->bindings.attentionOutput = chosen; break;
    case 4U: adapter->bindings.feedForwardUp = chosen; break;
    case 5U: adapter->bindings.feedForwardDown = chosen; break;
    default: break;
    }
}

/* The distinct seeds make the initial down projections reproducible without
 * duplicating the Framework random-number implementation. The up matrices
 * start at zero, preserving the original model's first forward output. */
static UmiStatus AdapterCreateSelectedProjections(
    UmiAiTrainingDecoderProjectionAdapter *adapter, uint64_t initialSeed)
{
    for (size_t role = 0U; role < ADAPTER_ROLE_COUNT; ++role) {
        if ((adapter->mask & ADAPTER_FLAGS[role]) == 0U) continue;
        size_t rows = 0U, columns = 0U;
        float *base = NULL;
        UmiStatus status;
        UmiAiDecoderParameterShape(&adapter->config,
                                   ADAPTER_PARAMETERS[role], &rows, &columns);
        if (rows == 0U || columns == 0U ||
            rows > UMI_AI_TRAINING_MAX_ELEMENTS / columns)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        const size_t count = rows * columns;
        base = malloc(count * sizeof(*base));
        if (base == NULL) return UMI_STATUS_OUT_OF_MEMORY;
        status = UmiAiTrainingTensorValues(
            adapter->frozen[ADAPTER_PARAMETERS[role]], base, count);
        if (status == UMI_STATUS_OK) {
            const uint64_t distinctSeed = initialSeed ^
                (UINT64_C(0x9e3779b97f4a7c15) * (uint64_t)(role + 1U));
            status = UmiAiTrainingLowRankCreate(adapter->tape,
                         base, rows, columns, adapter->rank, distinctSeed,
                         &adapter->projections[role]);
        }
        free(base);
        if (status != UMI_STATUS_OK) return status;
        AdapterBindProjection(adapter, role);
        if (adapter->rank > (SIZE_MAX - adapter->trainableElements) /
                             (rows + columns))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        adapter->trainableElements += adapter->rank * (rows + columns);
    }
    return UmiAiTrainingTransformerLowRankTrainables(&adapter->bindings,
               adapter->trainables,
               UMI_AI_TRANSFORMER_LOW_RANK_MAX_TRAINABLES,
               &adapter->trainableCount);
}

UmiStatus UmiAiTrainingDecoderProjectionAdapterCreate(
    UmiAiTrainingDecoder *decoder,
    uint32_t selectedProjections, size_t rank, uint64_t initialSeed,
    UmiAiTrainingDecoderProjectionAdapter **outAdapter)
{
    UmiAiTrainingDecoderProjectionAdapter *created = NULL;
    UmiAiTrainingDecoderConfig config = {0};
    UmiStatus status;
    uint64_t steps = 0U;

    if (decoder == NULL || outAdapter == NULL || *outAdapter != NULL ||
        selectedProjections == 0U ||
        (selectedProjections & ~UMI_AI_DECODER_ADAPT_ALL) != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(decoder, &config, &steps);
    if (status != UMI_STATUS_OK) return status;
    if (rank == 0U || rank > (size_t)config.channels ||
        rank > (size_t)config.hiddenChannels ||
        config.contextLength == 0U ||
        config.vocabularySize == 0U ||
        (size_t)config.contextLength > UMI_AI_TRAINING_MAX_ELEMENTS /
                                       (size_t)config.vocabularySize)
        return UMI_STATUS_INVALID_ARGUMENT;

    created = calloc(1U, sizeof(*created));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    created->borrowedDecoder = decoder;
    created->config = config;
    created->borrowedDecoderSteps = steps;
    created->rank = rank;
    created->mask = selectedProjections;

    status = AdapterCheckpointIdentity(decoder, created->checkpointSha256);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingTapeCreate(&created->tape);
    if (status != UMI_STATUS_OK) goto failure;
    status = AdapterCopyFrozenParameters(created);
    if (status != UMI_STATUS_OK) goto failure;
    status = AdapterCreateSelectedProjections(created, initialSeed);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingTapeCheckpoint(created->tape, &created->retainedNodes);
    if (status != UMI_STATUS_OK) goto failure;

    /* The parameter copy and the recorded checkpoint must still refer to the
     * same decoder. The caller may not use or train it concurrently. */
    status = AdapterCheckBorrowedModel(created);
    if (status != UMI_STATUS_OK) goto failure;
    *outAdapter = created;
    return UMI_STATUS_OK;

failure:
    UmiAiTrainingDecoderProjectionAdapterDestroy(created);
    return status;
}

UmiStatus UmiAiTrainingDecoderProjectionAdapterInspect(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    UmiAiTrainingDecoderProjectionAdapterInfo *outInfo)
{
    UmiStatus status;
    UmiAiTrainingDecoderProjectionAdapterInfo candidate = {0};
    if (outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = AdapterCheckBorrowedModel(adapter);
    if (status != UMI_STATUS_OK) return status;
    candidate.decoder = adapter->config;
    candidate.selectedProjections = adapter->mask;
    candidate.rank = adapter->rank;
    candidate.trainableTensors = adapter->trainableCount;
    candidate.trainableElements = adapter->trainableElements;
    candidate.retainedTapeNodes = adapter->retainedNodes;
    candidate.frozenDecoderSteps = adapter->borrowedDecoderSteps;
    candidate.completedAdapterSteps = adapter->completedAdapterSteps;
    memcpy(candidate.frozenCheckpointSha256, adapter->checkpointSha256,
           sizeof(candidate.frozenCheckpointSha256));
    *outInfo = candidate;
    return UMI_STATUS_OK;
}

static UmiStatus AdapterValidateContext(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const size_t *inputs, size_t count)
{
    const size_t context = (size_t)adapter->config.contextLength;
    if (inputs == NULL || count != context ||
        context == 0U || context > UMI_AI_DECODER_MAX_CONTEXT ||
        context > UMI_AI_TRAINING_MAX_ELEMENTS /
                  (size_t)adapter->config.vocabularySize)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < count; ++index)
        if (inputs[index] >= (size_t)adapter->config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* Only Framework-owned operators perform neural mathematics. We compose the
 * SAME token/position embeddings and pre-LN Transformer shapes as decoder.c.
 * Unlike the canonical opaque decoder, this independent tape retains adapter
 * leaves through successive training steps, avoiding parameter-prefix loss. */
static UmiStatus AdapterForward(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const size_t *inputs, size_t count,
    UmiAiTrainingTensor **outLogits)
{
    UmiAiTrainingTensor *token = NULL, *position = NULL, *combined = NULL;
    UmiAiTrainingTensor *hidden = NULL, *logits = NULL;
    size_t positionIds[UMI_AI_DECODER_MAX_CONTEXT] = {0};
    UmiStatus status;
    for (size_t index = 0U; index < count; ++index)
        positionIds[index] = index;

    const UmiAiTrainingTransformerBlockParameters parameters = {
        adapter->frozen[UMI_DECODER_NORM1_SCALE],
        adapter->frozen[UMI_DECODER_NORM1_BIAS],
        adapter->frozen[UMI_DECODER_QUERY],
        adapter->frozen[UMI_DECODER_KEY],
        adapter->frozen[UMI_DECODER_VALUE],
        adapter->frozen[UMI_DECODER_PROJECTION],
        adapter->frozen[UMI_DECODER_NORM2_SCALE],
        adapter->frozen[UMI_DECODER_NORM2_BIAS],
        adapter->frozen[UMI_DECODER_UP],
        adapter->frozen[UMI_DECODER_UP_BIAS],
        adapter->frozen[UMI_DECODER_DOWN],
        adapter->frozen[UMI_DECODER_DOWN_BIAS]
    };
    const UmiAiTrainingTransformerBlockConfig blockConfig = {
        (size_t)adapter->config.contextLength,
        (size_t)adapter->config.attentionHeads,
        1e-4F
    };
#define ADAPTER_TRY(expression) do { \
    status = (expression); \
    if (status != UMI_STATUS_OK) return status; \
} while (0)
    ADAPTER_TRY(UmiAiTrainingEmbeddingLookup(adapter->tape,
        adapter->frozen[UMI_DECODER_TOKEN_EMBEDDING],
        inputs, count, &token));
    ADAPTER_TRY(UmiAiTrainingEmbeddingLookup(adapter->tape,
        adapter->frozen[UMI_DECODER_POSITION_EMBEDDING],
        positionIds, count, &position));
    ADAPTER_TRY(UmiAiTrainingAdd(adapter->tape, token, position, &combined));
    ADAPTER_TRY(UmiAiTrainingTransformerBlockForwardLowRank(
        adapter->tape, combined, &parameters, &blockConfig,
        &adapter->bindings, &hidden));
    ADAPTER_TRY(UmiAiTrainingMatmul(adapter->tape, hidden,
        adapter->frozen[UMI_DECODER_LOGITS], &logits));
#undef ADAPTER_TRY
    *outLogits = logits;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderProjectionAdapterLogits(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const size_t *inputTokenIds, size_t tokenCount,
    float *outLogits, size_t capacity)
{
    UmiAiTrainingTensor *logits = NULL;
    float *staged = NULL;
    UmiStatus status, cleanup;
    if (outLogits == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = AdapterCheckBorrowedModel(adapter);
    if (status != UMI_STATUS_OK) return status;
    status = AdapterValidateContext(adapter, inputTokenIds, tokenCount);
    if (status != UMI_STATUS_OK) return status;
    const size_t needed = tokenCount * (size_t)adapter->config.vocabularySize;
    if (capacity < needed) return UMI_STATUS_CAPACITY_EXCEEDED;
    staged = malloc(needed * sizeof(*staged));
    if (staged == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = AdapterForward(adapter, inputTokenIds, tokenCount, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(logits, staged, needed);
    cleanup = UmiAiTrainingTapeRollback(adapter->tape, adapter->retainedNodes);
    if (cleanup != UMI_STATUS_OK) status = cleanup;
    if (status == UMI_STATUS_OK)
        memcpy(outLogits, staged, needed * sizeof(*staged));
    free(staged);
    return status;
}

static UmiStatus AdapterSelectedLoss(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const size_t *inputs, const size_t *targets,
    const uint8_t *mask, size_t count,
    int train, float learningRate, float *outLoss)
{
    UmiAiTrainingTensor *logits = NULL, *loss = NULL;
    float candidate = 0.0F;
    UmiStatus status, cleanup;
    size_t chosen = 0U;

    if (targets == NULL || mask == NULL || outLoss == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = AdapterCheckBorrowedModel(adapter);
    if (status != UMI_STATUS_OK) return status;
    status = AdapterValidateContext(adapter, inputs, count);
    if (status != UMI_STATUS_OK) return status;
    if (train && (!isfinite(learningRate) || learningRate <= 0.0F))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (train && adapter->completedAdapterSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i) {
        if (mask[i] > 1U) return UMI_STATUS_INVALID_ARGUMENT;
        if (mask[i] != 0U) {
            if (targets[i] >= (size_t)adapter->config.vocabularySize)
                return UMI_STATUS_INVALID_ARGUMENT;
            ++chosen;
        }
    }
    if (chosen == 0U) return UMI_STATUS_INVALID_ARGUMENT;

    status = AdapterForward(adapter, inputs, count, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingSelectedCrossEntropy(
            adapter->tape, logits, targets, count, mask, count, &loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, &candidate, 1U);
    if (status == UMI_STATUS_OK &&
        (!isfinite(candidate) || candidate < 0.0F))
        status = UMI_STATUS_INVALID_STATE;
    if (train && status == UMI_STATUS_OK)
        status = UmiAiTrainingBackward(adapter->tape, loss);
    if (train && status == UMI_STATUS_OK)
        status = UmiAiTrainingSgdStepMany(adapter->tape,
                    adapter->trainables, adapter->trainableCount,
                    learningRate);

    cleanup = UmiAiTrainingTapeRollback(adapter->tape, adapter->retainedNodes);
    if (cleanup != UMI_STATUS_OK) return cleanup;
    if (status != UMI_STATUS_OK) return status;
    if (train) ++adapter->completedAdapterSteps;
    *outLoss = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderProjectionAdapterEvaluate(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    const uint8_t *lossMask, size_t tokenCount, float *outLoss)
{
    return AdapterSelectedLoss(adapter, inputTokenIds, targetTokenIds,
                               lossMask, tokenCount, 0, 0.0F, outLoss);
}

UmiStatus UmiAiTrainingDecoderProjectionAdapterTrainStep(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    const uint8_t *lossMask, size_t tokenCount, float learningRate,
    UmiAiTrainingDecoderProjectionStep *outReport)
{
    UmiAiTrainingDecoderProjectionStep candidate = {0};
    UmiStatus status;
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = AdapterSelectedLoss(adapter, inputTokenIds, targetTokenIds,
                  lossMask, tokenCount, 1, learningRate,
                  &candidate.selectedCrossEntropyBeforeUpdate);
    if (status != UMI_STATUS_OK) return status;
    candidate.supervisedTokenCount = 0U;
    for (size_t index = 0U; index < tokenCount; ++index)
        candidate.supervisedTokenCount += (size_t)(lossMask[index] != 0U);
    candidate.completedAdapterSteps = adapter->completedAdapterSteps;
    *outReport = candidate;
    return UMI_STATUS_OK;
}
