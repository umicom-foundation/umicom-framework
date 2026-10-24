/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/rotary_optimisation.h
 *
 * PURPOSE:
 *   Make one complete, observable native C23 training exercise from existing
 *   tape-owned matmul, multi-head causal attention, RoPE, MSE and SGD services.
 *
 * ARCHITECTURE:
 *   This is an educational qualification of the existing rotary tape bridge,
 *   NOT a new tensor engine, general decoder, checkpoint family or an
 *   efficient/fused attention kernel. Query and key projection matrices are
 *   the only trainable parameters; input features, values and target remain
 *   fixed for the bounded experiment. Model state never leaves Framework.
 *
 * OWNERSHIP:
 *   The input problem and plan are caller-owned and never modified.
 *   All tape tensors are owned and destroyed inside the function. Every output
 *   byte is published only after the entire run and inspection succeed.
 *   There are no global random states, files, scripts or asynchronous tasks.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_ROTARY_OPTIMISATION_H
#define UMICOM_AI_TRAINING_ROTARY_OPTIMISATION_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/rotary_tape.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_ROTARY_OPTIMISATION_TOKENS 3U
#define UMI_AI_ROTARY_OPTIMISATION_CHANNELS 4U
#define UMI_AI_ROTARY_OPTIMISATION_HEADS 2U
#define UMI_AI_ROTARY_OPTIMISATION_MAX_STEPS 8U

/* Fixed 3x4 teaching problem; the projections are both 4x4 matrices.
 * Position indices are preserved as absolute token coordinates. */
typedef struct UmiAiTrainingRotaryOptimisationProblem {
    float tokenFeatures[12];
    float values[12];
    float desiredAttention[12];
    float queryWeights[16];
    float keyWeights[16];
    uint64_t firstTokenPosition;
    double frequencyBase;
} UmiAiTrainingRotaryOptimisationProblem;

typedef struct UmiAiTrainingRotaryOptimisationPlan {
    size_t gradientSteps;  /* 1..8 explicit SGD steps */
    float learningRate;    /* >0, <=1, finite */
} UmiAiTrainingRotaryOptimisationPlan;

typedef struct UmiAiTrainingRotaryOptimisationResult {
    float initialLoss;
    float finalLoss;
    float lossAfterStep[UMI_AI_ROTARY_OPTIMISATION_MAX_STEPS];
    float finalQueryWeights[16];
    float finalKeyWeights[16];
    float initialQueryGradientL2;
    float initialKeyGradientL2;
    size_t completedSteps;
    size_t peakTapeBytes;  /* persistent tape allocations, not process RSS */
    int lossReduced;       /* diagnostic; not a guarantee for arbitrary inputs */
} UmiAiTrainingRotaryOptimisationResult;

/* Compose the real Framework tape and RoPE bridge. Compute baseline loss,
 * take exactly gradientSteps parameter updates, then independently recompute
 * the loss with the updated parameters. Valid output requires finite losses,
 * gradients and final parameters; reduction is reported, never presumed.
 * Invalid inputs, capacity failure and internal failure leave outResult
 * byte-for-byte untouched; problem and plan are always unchanged. */
UmiStatus UmiAiTrainingRotaryOptimisationRun(
    const UmiAiTrainingRotaryOptimisationProblem *problem,
    const UmiAiTrainingRotaryOptimisationPlan *plan,
    UmiAiTrainingRotaryOptimisationResult *outResult);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_ROTARY_OPTIMISATION_H */
