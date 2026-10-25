/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/transformer_low_rank.h
 * PURPOSE:
 *   Attach existing trainable low-rank projections to the canonical native
 *   Transformer block without copying attention, normalisation or MLP maths.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRANSFORMER_LOW_RANK_H
#define UMICOM_AI_TRAINING_TRANSFORMER_LOW_RANK_H

#include "umicom/ai/training/low_rank.h"
#include "umicom/ai/training/transformer.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Each optional pointer selects ONE Framework-owned projection adapter.
 * NULL means use the exact canonical matrix multiplication for that role.
 * Create the low-rank leaves on the SAME tape as the Transformer parameters.
 * Each adapter's copied, frozen base must match the corresponding canonical
 * weight matrix byte-for-byte before a graph can be constructed. */
typedef struct UmiAiTrainingTransformerLowRankBindings {
    const UmiAiTrainingLowRankProjection *query;
    const UmiAiTrainingLowRankProjection *key;
    const UmiAiTrainingLowRankProjection *value;
    const UmiAiTrainingLowRankProjection *attentionOutput;
    const UmiAiTrainingLowRankProjection *feedForwardUp;
    const UmiAiTrainingLowRankProjection *feedForwardDown;
} UmiAiTrainingTransformerLowRankBindings;

#define UMI_AI_TRANSFORMER_LOW_RANK_MAX_TRAINABLES 12U

/* Run the established Transformer block's six projection steps with chosen
 * low-rank residuals. Base/adapter identities and all six canonical shapes
 * are checked before executing new tape graph nodes. Any failure preserves
 * *outTensor and the existing graph prefix. No decoder weights or optimisers
 * are changed. A NULL bindings value reproduces the canonical block.
 *
 * This adapts a real Transformer BLOCK; it does not yet attach adapters to
 * the opaque decoder lifecycle or provide an adapter model checkpoint. */
UmiStatus UmiAiTrainingTransformerBlockForwardLowRank(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingTransformerBlockParameters *parameters,
    const UmiAiTrainingTransformerBlockConfig *config,
    const UmiAiTrainingTransformerLowRankBindings *bindings,
    UmiAiTrainingTensor **outTensor);

/* List ONLY the adapter-down and adapter-up trainable leaves in a stable
 * projection order for the existing tape-owned multi-parameter optimiser.
 * No canonical base weight is returned. Duplicate handles are rejected.
 * Capacity counts pointers, not bytes. No output is modified on failure. */
UmiStatus UmiAiTrainingTransformerLowRankTrainables(
    const UmiAiTrainingTransformerLowRankBindings *bindings,
    UmiAiTrainingTensor **outTrainables, size_t capacity,
    size_t *outCount);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TRANSFORMER_LOW_RANK_H */
