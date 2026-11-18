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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/* Describe additional reduction payloads without allocating a decoder.
 * graph is the existing capacity report for min(effective, microbatch)
 * sequences. maximumGraphs includes groups that an objective mask might skip.
 * Every size is logical payload; this is not a process peak or RAM reservation.
 * Optimizer moments/workspace, other graph nodes and allocator overhead are
 * excluded. Shape limits match the current decoder and accumulated-token cap. */
typedef struct UmiAiTrainingAccumulationPlan {
    UmiAiTrainingCapacityPlan graph;
    size_t effectiveSequences;
    size_t effectiveTokens;
    size_t requestedMicrobatchSequences;
    size_t maximumGraphs;
    size_t lastGraphSequences;
    size_t largestParameterElements;
    size_t accumulatorBytes;
    size_t reducedGradientBytes;
    size_t scratchBytes;
    size_t peakReductionPayloadBytes;
} UmiAiTrainingAccumulationPlan;

/* Counts must be positive and each requested count times context must fit the
 * accumulated-token contract, even if the requested microbatch exceeds the
 * effective batch. All rejected calls preserve outPlan. No inputs are retained.
 * The existing capacity-plan ABI and function remain unchanged. */
UmiStatus UmiAiTrainingAccumulationPlanCompute(
    const UmiAiTrainingDecoderConfig *config,
    size_t effectiveSequences, size_t microbatchSequences,
    UmiAiTrainingAccumulationPlan *outPlan);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_CAPACITY_PLAN_H */
