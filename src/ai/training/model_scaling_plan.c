/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/model_scaling_plan.c
 * PURPOSE: Overflow-safe estimates for model, adapter and cache footprint.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/model_scaling_plan.h"
#include <stdint.h>
#include <limits.h>

static int Add(uint64_t a, uint64_t b, uint64_t *out) {
    if (UINT64_MAX - a < b) return 0;
    *out = a + b;
    return 1;
}
static int Mul(uint64_t a, uint64_t b, uint64_t *out) {
    if (a != 0U && b > UINT64_MAX / a) return 0;
    *out = a * b;
    return 1;
}
UmiStatus UmiAiTrainingModelScalingEstimateBuild(
    const UmiAiTrainingModelScalingShape *s,
    UmiAiTrainingModelScalingEstimate *out)
{
    UmiAiTrainingModelScalingEstimate e = {0};
    uint64_t c2, ch, perBlock, norms, biases, tmp, layers, perAdapter;
    uint64_t total, fp32, rank, scratch;
    if (s == NULL || out == NULL || !s->vocabulary || !s->contextTokens ||
        !s->channels || !s->hiddenChannels || !s->attentionHeads ||
        !s->transformerLayers || !s->concurrentSequences ||
        s->channels % s->attentionHeads != 0U ||
        (s->adapterRank != 0U &&
         (s->adapterRank > s->channels || s->adapterRank > s->hiddenChannels)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!Mul(s->vocabulary, s->channels, &e.tokenEmbeddingParameters) ||
        !Mul(s->contextTokens, s->channels, &e.positionEmbeddingParameters) ||
        !Mul(s->channels, s->channels, &c2) ||
        !Mul(s->channels, s->hiddenChannels, &ch) ||
        !Mul(c2, 4U, &perBlock) ||
        !Mul(ch, 2U, &tmp) || !Add(perBlock, tmp, &perBlock) ||
        !Mul(s->channels, 4U, &norms) ||
        !Add(s->hiddenChannels, s->channels, &biases) ||
        !Add(perBlock, norms, &perBlock) ||
        !Add(perBlock, biases, &perBlock) ||
        !Mul(perBlock, s->transformerLayers, &e.transformerParameters) ||
        !Mul(s->channels, s->vocabulary, &e.logitsParameters) ||
        !Add(e.tokenEmbeddingParameters, e.positionEmbeddingParameters, &total) ||
        !Add(total, e.transformerParameters, &total) ||
        !Add(total, e.logitsParameters, &e.totalParameters))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Six projections: four C->C and two C<->H, each contributes
     * rank * (inputWidth + outputWidth) trainable values. */
    if (!Mul(s->channels, 8U, &perAdapter) ||
        !Add(s->channels, s->hiddenChannels, &tmp) ||
        !Mul(tmp, 2U, &tmp) || !Add(perAdapter, tmp, &perAdapter) ||
        !Mul(perAdapter, s->adapterRank, &rank) ||
        !Mul(rank, s->transformerLayers, &e.adapterParameters) ||
        !Mul(e.totalParameters, sizeof(float), &e.frozenFp32Bytes) ||
        !Mul(e.adapterParameters, sizeof(float), &e.adapterFp32Bytes) ||
        !Mul(e.adapterParameters, sizeof(float), &e.adapterSgdGradientBytes) ||
        !Mul(e.adapterParameters, 2U * sizeof(double), &e.adapterAdamwMomentBytes))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* K/V: layer * streams * context * width * 2 * sizeof(float). */
    if (!Mul(s->transformerLayers, s->concurrentSequences, &layers) ||
        !Mul(layers, s->contextTokens, &layers) ||
        !Mul(layers, s->channels, &layers) ||
        !Mul(layers, 2U * sizeof(float), &e.kvCacheFp32Bytes))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    fp32 = e.frozenFp32Bytes;
    if (!Add(fp32, e.adapterFp32Bytes, &fp32) ||
        !Add(fp32, e.adapterSgdGradientBytes, &fp32) ||
        !Add(fp32, e.adapterAdamwMomentBytes, &fp32) ||
        !Add(fp32, e.kvCacheFp32Bytes, &scratch))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    e.minimumEstimateBytes = scratch;
    *out = e;
    return UMI_STATUS_OK;
}
