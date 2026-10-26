/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/model_scaling_plan.h
 * PURPOSE: Bound and describe prospective native decoder model capacity.
 * ARCHITECTURE: An allocation-free C23 estimator, not a second model engine.
 *   The existing one-block educational decoder and checkpoint are unchanged.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_MODEL_SCALING_PLAN_H
#define UMICOM_AI_TRAINING_MODEL_SCALING_PLAN_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Candidate architecture only; acceptance does not mean the currently
 * compiled decoder supports the requested number of layers or context. */
typedef struct UmiAiTrainingModelScalingShape {
    uint64_t vocabulary;
    uint64_t contextTokens;
    uint64_t channels;
    uint64_t hiddenChannels;
    uint64_t attentionHeads;
    uint64_t transformerLayers;
    uint64_t concurrentSequences;
    uint64_t adapterRank; /* 0 disables all-projection low-rank estimation. */
} UmiAiTrainingModelScalingShape;

typedef struct UmiAiTrainingModelScalingEstimate {
    uint64_t tokenEmbeddingParameters;
    uint64_t positionEmbeddingParameters;
    uint64_t transformerParameters;
    uint64_t logitsParameters;
    uint64_t totalParameters;
    uint64_t adapterParameters;
    uint64_t frozenFp32Bytes;
    uint64_t adapterFp32Bytes;
    uint64_t adapterSgdGradientBytes;
    uint64_t adapterAdamwMomentBytes; /* Two binary64 moments per adapter value. */
    uint64_t kvCacheFp32Bytes; /* Two K/V tensors, one for each layer and stream. */
    uint64_t minimumEstimateBytes; /* Sum of model, adapter, gradients, moments and KV. */
} UmiAiTrainingModelScalingEstimate;

/* Preflight for a PROPOSED fully dense decoder design. Per-block count assumes:
 * four CxC attention projections, two layer-norm affine vectors (4C),
 * CxH and HxC feed-forward matrices, and their two biases (H+C).
 * This matches the existing Framework pre-LN block parameter convention.
 * Token and learned-absolute-position embeddings and CxV logits are untied.
 * Adapter count assumes low-rank factors on all six matrices in every layer.
 * The K/V budget assumes full channels per token, not a measured allocation.
 * No activations, attention scores, temporary buffers, allocator overhead,
 * checkpoint copies, fragmentation or device-transfer space are included.
 * The returned lower-bound is NOT a sufficient memory allocation budget.
 * Reject zero/invalid shapes, uneven attention heads, rank exceeding either
 * channel dimension, and any uint64 overflow, without modifying outEstimate.
 */
UmiStatus UmiAiTrainingModelScalingEstimateBuild(
    const UmiAiTrainingModelScalingShape *shape,
    UmiAiTrainingModelScalingEstimate *outEstimate);
#ifdef __cplusplus
}
#endif
#endif
