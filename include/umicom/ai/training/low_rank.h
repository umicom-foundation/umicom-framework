/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/low_rank.h
 * PURPOSE:
 *   Compose a frozen dense projection with trainable low-rank adapters using
 *   the existing C23 tensor tape, automatic differentiation and optimisers.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_LOW_RANK_H
#define UMICOM_AI_TRAINING_LOW_RANK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/tape.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Projection convention: input [batch, inputChannels], base matrix
 * [inputChannels, outputChannels], adapterDown [inputChannels, rank],
 * adapterUp [rank, outputChannels]. The result is:
 *
 *   input * base + (input * adapterDown) * adapterUp
 *
 * The base matrix is always created as a nontrainable tape leaf. The adapter
 * matrices are trainable leaves. Their scale is represented in their values;
 * no new autodifferentiation operator, model, or optimiser is introduced.
 * This is one dense-projection adaptation primitive, not end-to-end decoder
 * fine-tuning or a production-scale language-model runtime. */
typedef struct UmiAiTrainingLowRankProjection {
    UmiAiTrainingTensor *baseWeights;
    UmiAiTrainingTensor *adapterDown;
    UmiAiTrainingTensor *adapterUp;
    size_t inputChannels;
    size_t outputChannels;
    size_t rank;
} UmiAiTrainingLowRankProjection;

/* Create three tape-owned leaves as one transaction. The down projection is
 * initialised reproducibly from the Framework RNG, while the up projection
 * begins at zero so the first adapted output equals the frozen baseline.
 * The base values are copied. After success the caller must retain the tape
 * and not rewind past the created leaves. Failure leaves *outProjection and
 * the previous tape graph unchanged. The input array is never modified. */
UmiStatus UmiAiTrainingLowRankCreate(
    UmiAiTrainingTape *tape,
    const float *baseWeights,
    size_t inputChannels, size_t outputChannels, size_t rank,
    uint64_t initialSeed,
    UmiAiTrainingLowRankProjection *outProjection);

/* Compose existing matrix-multiplication and addition tape operations.
 * All intermediate nodes are rolled back if one operator fails. The result
 * is published only on success; *outTensor is not overwritten on failure.
 * The caller supplies its own input tensor with matching feature width. */
UmiStatus UmiAiTrainingLowRankForward(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingLowRankProjection *projection,
    UmiAiTrainingTensor **outTensor);

/* Return the only two leaves that should be passed to the existing Framework
 * SGD or AdamW optimiser for adapter-only training. Gradients and checkpoints
 * remain governed by the canonical tape and optimiser interfaces. */
UmiStatus UmiAiTrainingLowRankTrainables(
    const UmiAiTrainingLowRankProjection *projection,
    UmiAiTrainingTensor **outDown,
    UmiAiTrainingTensor **outUp);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_LOW_RANK_H */
