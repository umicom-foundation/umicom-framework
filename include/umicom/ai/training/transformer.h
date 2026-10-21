/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/transformer.h
 *
 * PURPOSE:
 *   Define a single pre-layer-normalised, trainable decoder Transformer block.
 *
 * ARCHITECTURE:
 *   This is an additive C23 composition of the canonical Umicom training tape:
 *   norm -> Q/K/V -> multi-head causal attention -> projection -> residual,
 *   followed by norm -> feed-forward network -> residual. The tape owns every
 *   intermediate tensor and parameter. No application-local maths is created.
 *
 *   This block is not yet a complete language model: tokeniser, learned
 *   positional embeddings, decoder stack, logits head, checkpoints, and
 *   train/evaluate data pipeline remain separate engineering milestones.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_TRANSFORMER_H
#define UMICOM_AI_TRAINING_TRANSFORMER_H

#include <stddef.h>
#include "umicom/ai/training/tape.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingTransformerBlockParameters {
    /* Layer normalisation one: [1, channels]. */
    const UmiAiTrainingTensor *normScale1;
    const UmiAiTrainingTensor *normBias1;

    /* Query, key, value, output projections: each [channels, channels]. */
    const UmiAiTrainingTensor *queryWeights;
    const UmiAiTrainingTensor *keyWeights;
    const UmiAiTrainingTensor *valueWeights;
    const UmiAiTrainingTensor *outputWeights;

    /* Layer normalisation two: [1, channels]. */
    const UmiAiTrainingTensor *normScale2;
    const UmiAiTrainingTensor *normBias2;

    /* Feed-forward expansion and reduction, with trainable broadcast biases:
     *   upWeights [channels, hiddenChannels], upBias [1, hiddenChannels];
     *   downWeights [hiddenChannels, channels], downBias [1, channels]. */
    const UmiAiTrainingTensor *upWeights;
    const UmiAiTrainingTensor *upBias;
    const UmiAiTrainingTensor *downWeights;
    const UmiAiTrainingTensor *downBias;
} UmiAiTrainingTransformerBlockParameters;

typedef struct UmiAiTrainingTransformerBlockConfig {
    size_t sequenceLength;
    size_t attentionHeads;
    float normEpsilon;
} UmiAiTrainingTransformerBlockConfig;

/* Build one complete pre-LN decoder block as a single tape transaction.
 * Input is [batch*time, channels]. The parameter shapes above are checked
 * before graph construction. The output is [batch*time, channels]. The call
 * never updates model parameters, runs an optimiser, or publishes a result
 * until all operators succeed. If any operation fails, every newly created
 * tape node is discarded and the previous graph prefix is preserved.
 * The output pointer is left unchanged on failure. Callers serialise tape
 * access and must preserve the lifetime of all parameter tensor handles. */
UmiStatus UmiAiTrainingTransformerBlockForward(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingTransformerBlockParameters *parameters,
    const UmiAiTrainingTransformerBlockConfig *config,
    UmiAiTrainingTensor **outTensor);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_TRANSFORMER_H */
