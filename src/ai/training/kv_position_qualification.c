/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/kv_position_qualification.c
 *
 * PURPOSE:
 *   Establish strict production-decoder / KV-cache parity for the boundary
 *   between safe growing-prefix reuse and mandatory positional rebasing.
 *
 * ARCHITECTURE:
 *   UmiAiTrainingDecoderLogits remains the independent, canonical reference.
 *   UmiAiTrainingKvCachePrefill/Append remain the ONLY K/V mathematics owner.
 *   The result is staged until both logit checks, cache counter checks and
 *   unchanged decoder identity checks have completed successfully.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_position_qualification.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define UMI_AI_KV_POSITION_LOGIT_TOLERANCE 0.00004F

/* Public decoder configuration identity is compared FIELD-BY-FIELD. Do not
 * memcmp structs: layout padding is not an API guarantee. Training steps
 * are compared independently after both read-only inference operations. */
static bool SameDecoderConfig(const UmiAiTrainingDecoderConfig *left,
                              const UmiAiTrainingDecoderConfig *right)
{
    return left->vocabularySize == right->vocabularySize &&
           left->contextLength == right->contextLength &&
           left->channels == right->channels &&
           left->hiddenChannels == right->hiddenChannels &&
           left->attentionHeads == right->attentionHeads &&
           left->initialSeed == right->initialSeed;
}

/* Full-result publication remains atomic: this private operation only writes
 * a local candidate maximum and rejects all NaN/Inf and excessive deltas.
 * Comparing ONLY argmax token IDs would miss significant model divergence. */
static UmiStatus CompareLastRow(
    const float *cachedLastLogits, const float *canonicalMatrix,
    size_t contextTokens, size_t vocabularySize, float *outMaximum)
{
    const size_t lastRowOffset = (contextTokens - 1U) * vocabularySize;
    float maximum = 0.0F;

    for (size_t column = 0U; column < vocabularySize; ++column) {
        const float actual = cachedLastLogits[column];
        const float expected = canonicalMatrix[lastRowOffset + column];
        float difference;
        if (!isfinite(actual) || !isfinite(expected))
            return UMI_STATUS_INVALID_STATE;
        difference = fabsf(actual - expected);
        if (!isfinite(difference) ||
            difference > UMI_AI_KV_POSITION_LOGIT_TOLERANCE)
            return UMI_STATUS_INVALID_STATE;
        if (difference > maximum) maximum = difference;
    }
    *outMaximum = maximum;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingKvPositionQualify(
    UmiAiTrainingDecoder *model,
    const size_t *tokenIds, size_t tokenCount, size_t nextTokenId,
    UmiAiTrainingKvPositionQualification *outQualification)
{
    UmiAiTrainingKvPositionQualification candidate = {0};
    UmiAiTrainingDecoderConfig before = {0};
    UmiAiTrainingDecoderConfig after = {0};
    UmiAiTrainingKvCache *cache = NULL;
    /* Strict limits from decoder.h make these bounded stack buffers safe.
     * Canonical matrix capacity counts FLOAT VALUES, not bytes. */
    float canonical[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_VOCABULARY];
    float cached[UMI_AI_DECODER_MAX_VOCABULARY];
    size_t rolled[UMI_AI_DECODER_MAX_CONTEXT];
    uint64_t beforeSteps = 0U;
    uint64_t afterSteps = 0U;
    uint64_t computedRows = 0U;
    uint64_t rebuilds = 0U;
    size_t retained = 0U;
    size_t allocated = 0U;
    size_t vocabulary;
    size_t expectedHeap = 0U;
    UmiStatus status;

    if (model == NULL || tokenIds == NULL || outQualification == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingDecoderInspect(model, &before, &beforeSteps);
    if (status != UMI_STATUS_OK) return status;
    if (tokenCount == 0U ||
        tokenCount != (size_t)before.contextLength ||
        tokenCount > UMI_AI_DECODER_MAX_CONTEXT ||
        before.vocabularySize == 0U ||
        before.vocabularySize > UMI_AI_DECODER_MAX_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;

    vocabulary = (size_t)before.vocabularySize;
    if (nextTokenId >= vocabulary) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < tokenCount; ++index)
        if (tokenIds[index] >= vocabulary)
            return UMI_STATUS_INVALID_ARGUMENT;

    /* Consult the existing cache memory owner instead of duplicating its
     * parameter and K/V layout calculations. */
    status = UmiAiTrainingKvCacheAllocationForConfig(&before, &expectedHeap);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingKvCacheCreate(model, &cache);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingKvCacheAllocatedBytes(cache, &allocated);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (allocated != expectedHeap) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }

    /* A genuinely incremental growing prefix: position 0 is prefetched,
     * subsequent stable absolute positions reuse existing K/V rows. */
    status = UmiAiTrainingKvCachePrefill(cache, tokenIds, 1U,
        cached, vocabulary);
    if (status != UMI_STATUS_OK) goto cleanup;
    for (size_t index = 1U; index < tokenCount; ++index) {
        status = UmiAiTrainingKvCacheAppend(cache, tokenIds[index],
            cached, vocabulary);
        if (status != UMI_STATUS_OK) goto cleanup;
    }
    status = UmiAiTrainingKvCacheInspect(cache, &retained,
        &computedRows, &rebuilds);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (retained != tokenCount || computedRows != (uint64_t)tokenCount ||
        rebuilds != 0U) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    candidate.computedRowsBeforeRollover = computedRows;

    /* Independent mathematical authority: evaluate all logits through the
     * original training-tape decoder, not through another K/V Prefill. */
    status = UmiAiTrainingDecoderLogits(model, tokenIds, tokenCount,
        canonical, tokenCount * vocabulary);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = CompareLastRow(cached, canonical, tokenCount, vocabulary,
        &candidate.maximumAbsoluteDifferenceBeforeRollover);
    if (status != UMI_STATUS_OK) goto cleanup;

    /* Roll exactly once. Absolute embeddings at positions 0..N-1 now refer
     * to DIFFERENT retained tokens; stale K/V cannot be carried forward. */
    for (size_t index = 1U; index < tokenCount; ++index)
        rolled[index - 1U] = tokenIds[index];
    rolled[tokenCount - 1U] = nextTokenId;
    status = UmiAiTrainingKvCacheAppend(cache, nextTokenId,
        cached, vocabulary);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingKvCacheInspect(cache, &retained,
        &computedRows, &rebuilds);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (retained != tokenCount ||
        computedRows != (uint64_t)tokenCount * UINT64_C(2) ||
        rebuilds != UINT64_C(1)) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    candidate.computedRowsAfterRollover = computedRows;
    candidate.fullWindowRebuilds = rebuilds;

    status = UmiAiTrainingDecoderLogits(model, rolled, tokenCount,
        canonical, tokenCount * vocabulary);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = CompareLastRow(cached, canonical, tokenCount, vocabulary,
        &candidate.maximumAbsoluteDifferenceAfterRollover);
    if (status != UMI_STATUS_OK) goto cleanup;

    status = UmiAiTrainingDecoderInspect(model, &after, &afterSteps);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (afterSteps != beforeSteps || !SameDecoderConfig(&before, &after)) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }

    candidate.contextTokens = tokenCount;
    candidate.vocabularySize = vocabulary;
    candidate.comparedLogits = vocabulary * 2U;
    candidate.cacheHeapBytes = allocated;
    *outQualification = candidate;
    status = UMI_STATUS_OK;

cleanup:
    UmiAiTrainingKvCacheDestroy(cache);
    return status;
}
