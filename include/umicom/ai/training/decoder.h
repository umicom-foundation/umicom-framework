/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/decoder.h
 *
 * PURPOSE:
 *   Define one bounded native decoder-only language model built from the
 *   Framework training tape, learned token/position embeddings, a pre-LN
 *   transformer block, and an output-logits projection.
 *
 * OWNERSHIP:
 *   The decoder owns its tape, all parameters and intermediate tensors.
 *   Calls are synchronous and must be serialised by the caller.
 *   Models are experimental and not suitable for production financial use.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_DECODER_H
#define UMICOM_AI_TRAINING_DECODER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/ai/training/tape.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bounded limits serve the inspectable CPU teaching laboratory. */
/* The reversible byte tokenizer requires 256 bytes + 4 reserved tokens.
 * The actual supported vocabulary/channels combination is additionally
 * bounded by UMI_AI_TRAINING_MAX_ELEMENTS (4096) per tensor. */
#define UMI_AI_DECODER_MAX_VOCABULARY 260U
#define UMI_AI_DECODER_MAX_CONTEXT 32U
#define UMI_AI_DECODER_MAX_CHANNELS 32U
#define UMI_AI_DECODER_MAX_HIDDEN_CHANNELS 64U
#define UMI_AI_DECODER_MAX_PARAMETERS 15U

typedef struct UmiAiTrainingDecoder UmiAiTrainingDecoder;

typedef struct UmiAiTrainingDecoderConfig {
    uint32_t vocabularySize;
    uint32_t contextLength;
    uint32_t channels;
    uint32_t hiddenChannels;
    uint32_t attentionHeads;
    uint64_t initialSeed;
} UmiAiTrainingDecoderConfig;

/* A single model uses one pre-LN block, with learnable absolute positions.
 * This is an introductory, FROM-SCRATCH model, not a general coding LLM.
 * A successful call transfers ownership of the new model to the caller.
 * On failure, *outModel is unchanged. */
UmiStatus UmiAiTrainingDecoderCreate(const UmiAiTrainingDecoderConfig *config,
                                    UmiAiTrainingDecoder **outModel);
void UmiAiTrainingDecoderDestroy(UmiAiTrainingDecoder *model);

/* Returns configuration and completed optimiser steps. Output remains
 * unchanged if model/config/step pointer is null. */
UmiStatus UmiAiTrainingDecoderInspect(const UmiAiTrainingDecoder *model,
                                     UmiAiTrainingDecoderConfig *outConfig,
                                     uint64_t *outSteps);

/* Inspect the decoder's actual training-tape allocations, including live
 * trainable parameter tensors, temporary graph high-water mark and budget.
 * The decoder owns its tape; this API never exposes its allocation pointers.
 * SetMemoryBudget=0 restores unlimited committed-graph memory. A limit below
 * the currently allocated parameter storage is rejected without mutation.
 * Training/evaluation/inference calls remain synchronous and caller-serialised.
 */
UmiStatus UmiAiTrainingDecoderMemoryInspect(
    const UmiAiTrainingDecoder *model,
    UmiAiTrainingTapeMemory *outMemory);
UmiStatus UmiAiTrainingDecoderSetMemoryBudget(
    UmiAiTrainingDecoder *model, size_t budgetBytes);

/* Any token buffer represents whole sequences of exactly contextLength tokens.
 * The returned matrix is [tokenCount, vocabularySize] in row-major order.
 * A token at time t does not observe token inputs at time >t, and each
 * sequence is isolated from other sequences in the same call. On failure
 * output buffer is unchanged. Capacity counts FLOATS, not bytes. */
UmiStatus UmiAiTrainingDecoderLogits(UmiAiTrainingDecoder *model,
                                    const size_t *inputIds,
                                    size_t tokenCount,
                                    float *outLogits,
                                    size_t capacity);

/* Stable full-sequence average cross-entropy over target token IDs. A held-out
 * evaluation never updates weights or stepCount. Output is failure-atomic. */
UmiStatus UmiAiTrainingDecoderEvaluate(UmiAiTrainingDecoder *model,
                                      const size_t *inputIds,
                                      const size_t *targetIds,
                                      size_t tokenCount,
                                      float *outLoss);

/* Forward, loss, backward and multi-parameter SGD. Candidate weights are all
 * checked before any update; on failure, no model parameter or step changes.
 * This model does not sample during training; the caller selects input data.
 * Rewind after each call prevents graph growth between steps. */
UmiStatus UmiAiTrainingDecoderTrainStep(UmiAiTrainingDecoder *model,
                                       const size_t *inputIds,
                                       const size_t *targetIds,
                                       size_t tokenCount,
                                       float learningRate,
                                       float *outLoss);

/* Score only prediction rows whose selection byte is 1. Require exactly
 * tokenCount bytes, each 0 or 1, with at least one selected row. The mean is
 * over selected rows across the whole batch, not separately per sequence.
 * All input and target IDs, including excluded targets, remain valid IDs.
 *
 * This is an objective mask, NOT an attention mask: excluded input positions
 * still provide causal context, and their embeddings can receive gradients
 * through selected later predictions. No token shifting, padding, role parsing
 * or vocabulary conversion is performed. Callers supply aligned targets and
 * model-specific data policy. These declarations describe evaluation and SGD;
 * selected AdamW entry points are declared separately in optimizer.h.
 * Historical scope: this interface initially provided no AdamW/accumulation
 * mask support. Accumulation still retains its unpadded contract.
 *
 * Selection and token arrays are borrowed only for this synchronous call;
 * they must remain readable, unchanged and separate from outLoss. Evaluation
 * never updates weights or steps. Training returns the pre-update mean loss
 * and commits one SGD step. Refusal leaves weights, step count and outLoss
 * unchanged. Temporary graphs are released, including on budget refusal.
 * Existing graph-budget accounting and checkpoint bytes are unchanged.
 * Checkpoints do not store datasets or masks; preserve that policy separately.
 */
UmiStatus UmiAiTrainingDecoderEvaluateMasked(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *selected, size_t selectedCount, float *outLoss);
UmiStatus UmiAiTrainingDecoderTrainMaskedStep(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *selected, size_t selectedCount,
    float learningRate, float *outLoss);

/* Greedy next-token prediction from the LAST position of one full context.
 * Ties resolve to the lowest vocabulary ID. No sampling or external tools. */
UmiStatus UmiAiTrainingDecoderPredictNext(UmiAiTrainingDecoder *model,
                                         const size_t *inputIds,
                                         size_t tokenCount,
                                         size_t *outTokenId);

/* Padding-aware requests keep absolute positions (index modulo contextLength).
 * Require exactly tokenCount binary attention bytes. Input IDs remain valid
 * vocabulary IDs even at excluded positions. No model/config/checkpoint layout
 * changes. Selected rows cannot depend on excluded input rows; attention is
 * still causal and sequence-local. These APIs are synchronous; caller spans
 * remain readable/unchanged and disjoint from output until return.
 *
 * LogitsPadded publishes zero-filled excluded rows as placeholders, NOT valid
 * predictions/probabilities. All-zero attention is permitted for this call.
 * Output capacity is measured in floats; refusal leaves the buffer unchanged.
 */
UmiStatus UmiAiTrainingDecoderLogitsPadded(UmiAiTrainingDecoder *model,
    const size_t *inputIds, size_t tokenCount,
    const unsigned char *attention, size_t attentionCount,
    float *outLogits, size_t capacity);

/* Separate objective selection must contain tokenCount binary bytes, select at
 * least one row, and be a subset of attention selection. All target IDs remain
 * valid, including excluded ones. Prompts can be attended without being scored.
 * Return the mean over selected target rows across the batch. Evaluation leaves
 * weights/steps unchanged. SGD commits one failure-atomic parameter step and
 * returns pre-update loss. Refusal preserves weights, steps and outLoss; all
 * temporary graph nodes are rewound. Excluded residual rows still exist inside
 * the graph, so this is semantic padding exclusion, not sparse-memory execution.
 * Existing Masked APIs remain objective-only; AdamW/accumulation APIs keep their
 * existing unpadded contracts until explicitly extended.
 */
UmiStatus UmiAiTrainingDecoderEvaluatePadded(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *attention, size_t attentionCount,
    const unsigned char *selected, size_t selectedCount, float *outLoss);
UmiStatus UmiAiTrainingDecoderTrainPaddedStep(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *attention, size_t attentionCount,
    const unsigned char *selected, size_t selectedCount,
    float learningRate, float *outLoss);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_DECODER_H */
