/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_kv_step.c
 * PURPOSE: Reuse the existing native training tape to compute ONE causal
 *          stacked-decoder query against genuinely retained K/V rows.
 *          The historical Transformer block and decoder remain unchanged.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_step.h"
#include "umicom/ai/training/single_query_attention.h"
#include "umicom/ai/training/transformer.h"
#include "stacked_decoder_internal.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

/* All limits come from the established bounded stacked-decoder ABI. */
#define KV_STEP_ROW_ELEMENTS \
    (UMI_AI_STACKED_MAX_CONTEXT * UMI_AI_STACKED_MAX_CHANNELS)
#define KV_STEP_LAYER_ELEMENTS \
    (UMI_AI_STACKED_MAX_LAYERS * UMI_AI_STACKED_MAX_CHANNELS)

/* Preserve the caller's three output arrays as one transaction. Treat pointer
 * arithmetic overflow as an overlap rather than accidentally permitting it. */
static int KvStepOverlaps(const void *a, size_t an,
                          const void *b, size_t bn)
{
    uintptr_t first = (uintptr_t)a, second = (uintptr_t)b;
    if (first > UINTPTR_MAX - an || second > UINTPTR_MAX - bn) return 1;
    return first < second + bn && second < first + an;
}

static UmiStatus KvStepLayer(
    UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedKvStore *store,
    size_t layer, size_t oldRows,
    UmiAiTrainingTensor *incoming,
    float *newKey, float *newValue,
    UmiAiTrainingTensor **outState)
{
    UmiAiTrainingTape *tape = model->tape;
    const size_t width = (size_t)model->config.channels;
    const size_t rows = oldRows + 1U;
    const size_t first = UMI_STACKED_LAYER_START +
        layer * UMI_AI_STACKED_PARAMETERS_PER_LAYER;
    float qRows[KV_STEP_ROW_ELEMENTS] = {0};
    float kRows[KV_STEP_ROW_ELEMENTS] = {0};
    float vRows[KV_STEP_ROW_ELEMENTS] = {0};
    UmiAiTrainingTensor *norm = NULL, *query = NULL, *key = NULL;
    UmiAiTrainingTensor *value = NULL, *lastAttended = NULL;
    UmiAiTrainingTensor *projected = NULL;
#if 0
    /* Original multi-row tape-composition handles are retained with the
     * previous implementation below. Training still uses the differentiable
     * canonical attention operator. */
    float selected[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    UmiAiTrainingTensor *qLeaf = NULL, *kLeaf = NULL;
    UmiAiTrainingTensor *vLeaf = NULL, *attended = NULL, *selector = NULL;
#endif
    UmiAiTrainingTensor *residual = NULL, *norm2 = NULL, *expanded = NULL;
    UmiAiTrainingTensor *expandedBias = NULL, *activated = NULL;
    UmiAiTrainingTensor *contracted = NULL, *contractedBias = NULL;
    UmiAiTrainingTensor *finalState = NULL;
    UmiStatus status;

#define KV_STEP_TRY(expr) do { status = (expr); \
    if (status != UMI_STATUS_OK) return status; } while (0)
    /* Exactly the same pre-LN affine transform and Q/K/V matmul operators
     * used by the canonical Framework TransformerBlockForward. */
    KV_STEP_TRY(UmiAiTrainingLayerNorm(tape, incoming,
        model->parameters[first + UMI_STACKED_NORM1_SCALE],
        model->parameters[first + UMI_STACKED_NORM1_BIAS],
        1e-4F, &norm));
    KV_STEP_TRY(UmiAiTrainingMatmul(tape, norm,
        model->parameters[first + UMI_STACKED_QUERY], &query));
    KV_STEP_TRY(UmiAiTrainingMatmul(tape, norm,
        model->parameters[first + UMI_STACKED_KEY], &key));
    KV_STEP_TRY(UmiAiTrainingMatmul(tape, norm,
        model->parameters[first + UMI_STACKED_VALUE], &value));
    KV_STEP_TRY(UmiAiTrainingTensorValues(query,
        qRows + oldRows * width, width));
    KV_STEP_TRY(UmiAiTrainingTensorValues(key, newKey, width));
    KV_STEP_TRY(UmiAiTrainingTensorValues(value, newValue, width));
    for (size_t row = 0U; row < oldRows; ++row) {
        KV_STEP_TRY(UmiAiTrainingStackedKvStoreRead(store, layer, row,
            kRows + row * width, width,
            vRows + row * width, width));
    }
    memcpy(kRows + oldRows * width, newKey, width * sizeof(float));
    memcpy(vRows + oldRows * width, newValue, width * sizeof(float));

    /* One incremental query uses every admitted key and value row. The
     * Framework-owned inference operator performs the last-row attention
     * arithmetic, while the established tape still owns all projections,
     * normalisation, residual operations, MLP and temporary lifetimes.
     * This forward-only leaf is never used for gradient calculation. */
    const size_t values = rows * width;
    float attendedRow[UMI_AI_STACKED_MAX_CHANNELS] = {0};
    KV_STEP_TRY(UmiAiTrainingSingleQueryAttentionForward(
        qRows + oldRows * width, width,
        kRows, values, vRows, values,
        rows, width, (size_t)model->config.attentionHeads,
        attendedRow, width, NULL));
    KV_STEP_TRY(UmiAiTrainingTensorCreate(tape, 1U, width,
        attendedRow, width, 0, &lastAttended));

#if 0
    /* Historical multi-query composition preserved for review. Inference
     * formerly built zero-query prefix placeholders, processed all rows,
     * then selected the last row. The replacement above avoids this work;
     * the canonical training-time tape operator remains unchanged. */
    /* A causal output at row oldRows cannot depend on any earlier query row.
     * The ORIGINAL multi-head operator owns softmax and FP32 rounding.
     * This intentionally computes unused earlier query rows: the measured
     * attention work is rows, NOT a claimed one-query fused kernel. */
    KV_STEP_TRY(UmiAiTrainingTensorCreate(tape,
        rows, width, qRows, values, 0, &qLeaf));
    KV_STEP_TRY(UmiAiTrainingTensorCreate(tape,
        rows, width, kRows, values, 0, &kLeaf));
    KV_STEP_TRY(UmiAiTrainingTensorCreate(tape,
        rows, width, vRows, values, 0, &vLeaf));
    KV_STEP_TRY(UmiAiTrainingMultiHeadCausalAttention(tape,
        qLeaf, kLeaf, vLeaf, rows,
        (size_t)model->config.attentionHeads, &attended));
    selected[oldRows] = 1.0F;
    KV_STEP_TRY(UmiAiTrainingTensorCreate(tape, 1U, rows,
        selected, rows, 0, &selector));
    KV_STEP_TRY(UmiAiTrainingMatmul(tape,
        selector, attended, &lastAttended));

#endif

    KV_STEP_TRY(UmiAiTrainingMatmul(tape, lastAttended,
        model->parameters[first + UMI_STACKED_ATTENTION_OUTPUT], &projected));
    KV_STEP_TRY(UmiAiTrainingAdd(tape,
        incoming, projected, &residual));
    KV_STEP_TRY(UmiAiTrainingLayerNorm(tape, residual,
        model->parameters[first + UMI_STACKED_NORM2_SCALE],
        model->parameters[first + UMI_STACKED_NORM2_BIAS],
        1e-4F, &norm2));
    KV_STEP_TRY(UmiAiTrainingMatmul(tape, norm2,
        model->parameters[first + UMI_STACKED_FEED_FORWARD_UP], &expanded));
    KV_STEP_TRY(UmiAiTrainingAdd(tape, expanded,
        model->parameters[first + UMI_STACKED_FEED_FORWARD_UP_BIAS],
        &expandedBias));
    KV_STEP_TRY(UmiAiTrainingRelu(tape, expandedBias, &activated));
    KV_STEP_TRY(UmiAiTrainingMatmul(tape, activated,
        model->parameters[first + UMI_STACKED_FEED_FORWARD_DOWN], &contracted));
    KV_STEP_TRY(UmiAiTrainingAdd(tape, contracted,
        model->parameters[first + UMI_STACKED_FEED_FORWARD_DOWN_BIAS],
        &contractedBias));
    KV_STEP_TRY(UmiAiTrainingAdd(tape, residual,
        contractedBias, &finalState));
#undef KV_STEP_TRY
    *outState = finalState;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvStepPropose(
    UmiAiTrainingStackedDecoder *model,
    const UmiAiTrainingStackedKvStore *store,
    size_t nextTokenId,
    float *outKeys, size_t keyCapacity,
    float *outValues, size_t valueCapacity,
    float *outLastLogits, size_t logitCapacity,
    UmiAiTrainingStackedKvStepReport *outReport)
{
    UmiAiTrainingStackedModelIdentity identity = {0};
    UmiAiTrainingStackedKvStoreInfo storeInfo = {0};
    UmiAiTrainingStackedKvStepReport report = {0};
    float keys[KV_STEP_LAYER_ELEMENTS] = {0};
    float values[KV_STEP_LAYER_ELEMENTS] = {0};
    float logits[UMI_AI_STACKED_MAX_VOCABULARY] = {0};
    UmiAiTrainingTensor *token = NULL, *position = NULL, *state = NULL;
    UmiAiTrainingTensor *lastLogits = NULL;
    size_t savedNodes = 0U;
    UmiStatus status, cleanup;

    if (model == NULL || store == NULL || outKeys == NULL ||
        outValues == NULL || outLastLogits == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedModelIdentityCapture(model, &identity);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedKvStoreInspect(store, &storeInfo);
    if (status != UMI_STATUS_OK) return status;
    const size_t rows = storeInfo.retainedTokens;
    const size_t context = (size_t)identity.config.contextLength;
    const size_t channels = (size_t)identity.config.channels;
    const size_t layers = (size_t)identity.config.transformerLayers;
    const size_t vocabulary = (size_t)identity.config.vocabularySize;
    const size_t kvCount = channels * layers;
    if (rows == 0U || rows >= context ||
        storeInfo.contextCapacity != context ||
        storeInfo.channels != channels ||
        storeInfo.transformerLayers != layers ||
        nextTokenId >= vocabulary)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (keyCapacity < kvCount || valueCapacity < kvCount ||
        logitCapacity < vocabulary)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (KvStepOverlaps(outKeys, kvCount * sizeof(float),
                       outValues, kvCount * sizeof(float)) ||
        KvStepOverlaps(outKeys, kvCount * sizeof(float),
                       outLastLogits, vocabulary * sizeof(float)) ||
        KvStepOverlaps(outValues, kvCount * sizeof(float),
                       outLastLogits, vocabulary * sizeof(float)))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingStackedKvStoreCheckIdentity(
        store, identity.checkpointSha256);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTapeCheckpoint(model->tape, &savedNodes);
    if (status != UMI_STATUS_OK) return status;
    /* Model is externally serialised; do not rewind another owner's
     * temporary training graph or publish a result from a dirty tape. */
    if (savedNodes != model->parameterCount) return UMI_STATUS_INVALID_STATE;

    const size_t tokenId = nextTokenId;
    const size_t positionId = rows;
    status = UmiAiTrainingEmbeddingLookup(model->tape,
        model->parameters[UMI_STACKED_TOKEN_EMBEDDING],
        &tokenId, 1U, &token);
    if (status != UMI_STATUS_OK) goto restore;
    status = UmiAiTrainingEmbeddingLookup(model->tape,
        model->parameters[UMI_STACKED_POSITION_EMBEDDING],
        &positionId, 1U, &position);
    if (status != UMI_STATUS_OK) goto restore;
    status = UmiAiTrainingAdd(model->tape, token, position, &state);
    if (status != UMI_STATUS_OK) goto restore;
    for (size_t layer = 0U; layer < layers; ++layer) {
        UmiAiTrainingTensor *nextState = NULL;
        status = KvStepLayer(model, store, layer, rows, state,
            keys + layer * channels, values + layer * channels,
            &nextState);
        if (status != UMI_STATUS_OK) goto restore;
        state = nextState;
    }
    status = UmiAiTrainingMatmul(model->tape, state,
        model->parameters[model->parameterCount - 1U], &lastLogits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(lastLogits, logits, vocabulary);
    if (status == UMI_STATUS_OK)
        for (size_t i = 0U; i < vocabulary; ++i)
            if (!isfinite(logits[i])) { status = UMI_STATUS_INVALID_STATE; break; }
restore:
    cleanup = UmiAiTrainingTapeRollback(model->tape, savedNodes);
    if (cleanup != UMI_STATUS_OK) status = cleanup;
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedModelIdentityVerify(model, &identity);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedKvStoreCheckIdentity(
        store, identity.checkpointSha256);
    if (status != UMI_STATUS_OK) return status;

    report.retainedBefore = rows;
    report.retainedAfter = rows + 1U;
    report.transformerLayers = layers;
    report.cachedKeyValueRowsRead = layers * rows;
    report.newlyProjectedTokenLayerRows = (uint64_t)layers;
    /* Exactly one causal query per Transformer layer on the growing prefix;
     * prefill and every learned-position rollover are separately counted
     * by the unchanged full-context decoder. */
#if 0
    /* Historical full-tape row count, retained for review. */
    report.attentionQueryLayerRowsExecuted = (uint64_t)(layers * (rows + 1U));
#endif
    report.attentionQueryLayerRowsExecuted = (uint64_t)layers;
    report.canonicalFullContextTokenLayerRows = (uint64_t)(layers * context);
    memcpy(outKeys, keys, kvCount * sizeof(float));
    memcpy(outValues, values, kvCount * sizeof(float));
    memcpy(outLastLogits, logits, vocabulary * sizeof(float));
    if (outReport != NULL) *outReport = report;
    return UMI_STATUS_OK;
}
