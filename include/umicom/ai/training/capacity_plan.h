/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/capacity_plan.h
 *
 * PURPOSE:
 *   Validate native decoder training shapes and report exact parameter and
 *   selected activation payload sizes before constructing any model.
 *
 * ARCHITECTURE:
 *   This is a Framework-owned, read-only capacity inspection contract.
 *   Shapes come from the existing decoder's authoritative parameter schema.
 *   A capacity plan is NOT a peak resident-memory estimate; graph metadata,
 *   all intermediate activations, temporary workspaces and allocator overhead
 *   are accounted for separately by live tape inspection and host profiling.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_CAPACITY_PLAN_H
#define UMICOM_AI_TRAINING_CAPACITY_PLAN_H

#include <stddef.h>
#include "umicom/ai/training/decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingCapacityPlan {
    size_t parameterTensors;
    size_t parameterElements;
    size_t parameterValueBytes;
    size_t parameterGradientBytes;
    size_t tokensPerBatch;
    size_t logitsElements;
    size_t logitsBytes;
    size_t attentionProbabilityElements;
    size_t attentionProbabilityBytes;
    size_t minimumRetainedPayloadBytes;
    size_t maximumBatchSequences;
    size_t maximumTensorElements;
} UmiAiTrainingCapacityPlan;

/* Return exact logical FP32 sizes of the decoder's trainable parameters,
 * one batch of logits and one multi-head attention probability allocation.
 * The returned minimumRetainedPayloadBytes excludes other required graph nodes
 * and is deliberately NOT a safe training memory budget.
 *
 * Invalid model configurations return INVALID_ARGUMENT. Valid model shapes
 * or selected sequence counts exceeding the existing tensor tape limits return
 * CAPACITY_EXCEEDED. All outputs remain unchanged on rejected requests.
 * The inspection performs no training or allocations and has no side effects.
 */
UmiStatus UmiAiTrainingCapacityPlanCompute(
    const UmiAiTrainingDecoderConfig *config,
    size_t batchSequences,
    UmiAiTrainingCapacityPlan *outPlan);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_CAPACITY_PLAN_H */
