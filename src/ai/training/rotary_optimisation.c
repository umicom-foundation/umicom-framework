/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/rotary_optimisation.c
 *
 * PURPOSE:
 *   Prove that the existing differentiable rotary-attention path can take
 *   complete, reproducible optimiser steps and retain honest loss evidence.
 *
 * ARCHITECTURE:
 *   Only Framework-owned tensor operations and the existing atomic SgdStepMany
 *   mutate tape state. Each iteration rewinds the graph to its five original
 *   leaves, preserving learned weights and releasing old intermediate nodes.
 *   The final loss uses a NEW forward pass after the last weight update.
 *
 *   This is a tiny mathematical research exercise, not a full trainable LLM.
 *   Its intentionally bounded rotary_tape composition is not a performance
 *   benchmark and the learned-absolute decoder/checkpoints are untouched.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_optimisation.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

/* Source and published output may not overlap. A caller must not publish a
 * result into its own training problem or plan: doing so would contradict
 * the documented const-input and failure-atomic publication contracts.
 * Distances are taken in uintptr_t without adding a size to an address. */
static int StorageOverlaps(const void *left, size_t leftBytes,
                           const void *right, size_t rightBytes)
{
    const uintptr_t a = (uintptr_t)left;
    const uintptr_t b = (uintptr_t)right;
    return a >= b ? a - b < rightBytes : b - a < leftBytes;
}

static int ValuesAreFinite(const float *values, size_t count)
{
    for (size_t index = 0U; index < count; ++index)
        if (!isfinite(values[index])) return 0;
    return 1;
}

static UmiStatus ValidateProblem(
    const UmiAiTrainingRotaryOptimisationProblem *problem,
    const UmiAiTrainingRotaryOptimisationPlan *plan)
{
    const UmiAiTrainingRotaryConfig config = {
        2U, 2U, problem->frequencyBase
    };
    if (plan->gradientSteps == 0U ||
        plan->gradientSteps > UMI_AI_ROTARY_OPTIMISATION_MAX_STEPS ||
        !isfinite(plan->learningRate) || plan->learningRate <= 0.0F ||
        plan->learningRate > 1.0F ||
        problem->firstTokenPosition >
            UMI_AI_ROTARY_MAX_POSITION -
            (UMI_AI_ROTARY_OPTIMISATION_TOKENS - 1U) ||
        UmiAiTrainingRotaryConfigValidate(&config) != UMI_STATUS_OK ||
        !ValuesAreFinite(problem->tokenFeatures, 12U) ||
        !ValuesAreFinite(problem->values, 12U) ||
        !ValuesAreFinite(problem->desiredAttention, 12U) ||
        !ValuesAreFinite(problem->queryWeights, 16U) ||
        !ValuesAreFinite(problem->keyWeights, 16U))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static UmiStatus ObserveGradientL2(
    const UmiAiTrainingTensor *weights, float *outMagnitude)
{
    float gradient[16];
    double totalSquared = 0.0;
    UmiStatus status = UmiAiTrainingTensorGradients(weights, gradient, 16U);
    if (status != UMI_STATUS_OK) return status;
    if (!ValuesAreFinite(gradient, 16U)) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < 16U; ++i)
        totalSquared += (double)gradient[i] * (double)gradient[i];
    if (!isfinite(totalSquared)) return UMI_STATUS_INVALID_STATE;
    *outMagnitude = (float)sqrt(totalSquared);
    return isfinite(*outMagnitude) ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

/* This operation is called only with tape-owned, validated leaf parameters.
 * It deliberately does not create or cache its own neural mathematics. */
static UmiStatus ForwardLoss(
    UmiAiTrainingTape *tape,
    UmiAiTrainingTensor *features,
    UmiAiTrainingTensor *queryWeights,
    UmiAiTrainingTensor *keyWeights,
    UmiAiTrainingTensor *valueTensor,
    UmiAiTrainingTensor *desired,
    const UmiAiTrainingRotaryConfig *rotary,
    uint64_t firstPosition,
    UmiAiTrainingTensor **outLoss)
{
    UmiAiTrainingTensor *queries = NULL, *keys = NULL, *attended = NULL;
    UmiStatus status = UmiAiTrainingMatmul(tape, features, queryWeights, &queries);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingMatmul(tape, features, keyWeights, &keys);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingRotaryTapeAttention(tape, queries, keys, valueTensor,
        rotary, UMI_AI_ROTARY_OPTIMISATION_TOKENS,
        UMI_AI_ROTARY_OPTIMISATION_HEADS, &firstPosition, 1U, &attended);
    if (status != UMI_STATUS_OK) return status;
    return UmiAiTrainingMeanSquaredError(tape, attended, desired, outLoss);
}

UmiStatus UmiAiTrainingRotaryOptimisationRun(
    const UmiAiTrainingRotaryOptimisationProblem *problem,
    const UmiAiTrainingRotaryOptimisationPlan *plan,
    UmiAiTrainingRotaryOptimisationResult *outResult)
{
    UmiAiTrainingTape *tape = NULL;
    UmiAiTrainingTensor *features = NULL, *queryWeights = NULL;
    UmiAiTrainingTensor *keyWeights = NULL, *values = NULL, *desired = NULL;
    UmiAiTrainingTensor *loss = NULL;
    UmiAiTrainingRotaryOptimisationResult result = {0};
    UmiAiTrainingTapeMemory memory = {0};
    UmiStatus status;
    size_t leafCount = 0U;
    float observedLoss = 0.0F;
    if (problem == NULL || plan == NULL || outResult == NULL ||
        StorageOverlaps(problem, sizeof(*problem), outResult, sizeof(*outResult)) ||
        StorageOverlaps(plan, sizeof(*plan), outResult, sizeof(*outResult)))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = ValidateProblem(problem, plan);
    if (status != UMI_STATUS_OK) return status;

    const UmiAiTrainingRotaryConfig rotary = {
        2U, 2U, problem->frequencyBase
    };
    status = UmiAiTrainingTapeCreate(&tape);
    if (status != UMI_STATUS_OK) return status;

#define UMI_ROTARY_RUN_STEP(operation) do { \
    status = (operation); \
    if (status != UMI_STATUS_OK) goto cleanup; \
} while (0)
    /* Five leaf parameters remain live during repeated graph rewinds.
     * Query/key weights are the only trainable leaves. */
    UMI_ROTARY_RUN_STEP(UmiAiTrainingTensorCreate(tape, 3U, 4U,
        problem->tokenFeatures, 12U, 0, &features));
    UMI_ROTARY_RUN_STEP(UmiAiTrainingTensorCreate(tape, 4U, 4U,
        problem->queryWeights, 16U, 1, &queryWeights));
    UMI_ROTARY_RUN_STEP(UmiAiTrainingTensorCreate(tape, 4U, 4U,
        problem->keyWeights, 16U, 1, &keyWeights));
    UMI_ROTARY_RUN_STEP(UmiAiTrainingTensorCreate(tape, 3U, 4U,
        problem->values, 12U, 0, &values));
    UMI_ROTARY_RUN_STEP(UmiAiTrainingTensorCreate(tape, 3U, 4U,
        problem->desiredAttention, 12U, 0, &desired));
    UMI_ROTARY_RUN_STEP(UmiAiTrainingTapeCheckpoint(tape, &leafCount));
    if (leafCount != 5U) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }

    for (size_t iteration = 0U; iteration <= plan->gradientSteps; ++iteration) {
        /* Iteration zero evaluates the starting model. Later passes evaluate
         * the weights AFTER the previous complete SGD step. */
        if (iteration > 0U)
            UMI_ROTARY_RUN_STEP(UmiAiTrainingTapeRewind(tape, leafCount));
        loss = NULL;
        UMI_ROTARY_RUN_STEP(ForwardLoss(tape, features, queryWeights,
            keyWeights, values, desired, &rotary,
            problem->firstTokenPosition, &loss));
        UMI_ROTARY_RUN_STEP(UmiAiTrainingTensorValues(loss, &observedLoss, 1U));
        if (!isfinite(observedLoss) || observedLoss < 0.0F) {
            status = UMI_STATUS_INVALID_STATE;
            goto cleanup;
        }
        if (iteration == 0U) result.initialLoss = observedLoss;
        else result.lossAfterStep[iteration - 1U] = observedLoss;
        if (iteration == plan->gradientSteps) {
            result.finalLoss = observedLoss;
            break;
        }
        UMI_ROTARY_RUN_STEP(UmiAiTrainingBackward(tape, loss));
        if (iteration == 0U) {
            UMI_ROTARY_RUN_STEP(ObserveGradientL2(queryWeights,
                &result.initialQueryGradientL2));
            UMI_ROTARY_RUN_STEP(ObserveGradientL2(keyWeights,
                &result.initialKeyGradientL2));
        }
        UmiAiTrainingTensor *trainable[] = {queryWeights, keyWeights};
        UMI_ROTARY_RUN_STEP(UmiAiTrainingSgdStepMany(tape,
            trainable, 2U, plan->learningRate));
        ++result.completedSteps;
    }

    UMI_ROTARY_RUN_STEP(UmiAiTrainingTensorValues(queryWeights,
        result.finalQueryWeights, 16U));
    UMI_ROTARY_RUN_STEP(UmiAiTrainingTensorValues(keyWeights,
        result.finalKeyWeights, 16U));
    if (!ValuesAreFinite(result.finalQueryWeights, 16U) ||
        !ValuesAreFinite(result.finalKeyWeights, 16U)) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    UMI_ROTARY_RUN_STEP(UmiAiTrainingTapeMemoryInspect(tape, &memory));
    result.peakTapeBytes = memory.peakBytes;
    result.lossReduced = result.finalLoss < result.initialLoss ? 1 : 0;
    *outResult = result;
    status = UMI_STATUS_OK;
#undef UMI_ROTARY_RUN_STEP
cleanup:
    UmiAiTrainingTapeDestroy(tape);
    return status;
}
