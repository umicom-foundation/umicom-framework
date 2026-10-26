/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_decoder.h
 * PURPOSE:
 *   Train and evaluate a bounded multi-layer, decoder-only Transformer by
 *   composing the Framework's existing tensor tape and Transformer blocks.
 *
 * OWNERSHIP AND COMPATIBILITY:
 *   This is a SEPARATE, explicitly identified educational model family.
 *   Existing UmiAiTrainingDecoder checkpoints and absolute-position/KV-cache
 *   inference contracts remain unchanged. A stacked checkpoint is not a
 *   legacy decoder checkpoint. All calls are synchronous and caller-serialised.
 *   Model and tensor lifetimes belong to this Framework component.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_DECODER_H
#define UMICOM_AI_TRAINING_STACKED_DECODER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/model_scaling_plan.h"
#include "umicom/ai/training/tape.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Conservative bounds are intentional: the existing native tape imposes
 * separate limits on both persistent nodes and transient graph storage. */
#define UMI_AI_STACKED_MAX_LAYERS 6U
#define UMI_AI_STACKED_MAX_CONTEXT 16U
#define UMI_AI_STACKED_MAX_CHANNELS 16U
#define UMI_AI_STACKED_MAX_HIDDEN_CHANNELS 32U
#define UMI_AI_STACKED_MAX_VOCABULARY 260U
#define UMI_AI_STACKED_PARAMETERS_PER_LAYER 12U
#define UMI_AI_STACKED_MAX_PARAMETERS (3U + UMI_AI_STACKED_MAX_LAYERS * UMI_AI_STACKED_PARAMETERS_PER_LAYER)
#define UMI_AI_STACKED_MAX_GENERATED_TOKENS 64U

typedef struct UmiAiTrainingStackedDecoder UmiAiTrainingStackedDecoder;
typedef struct UmiAiTrainingStackedDecoderConfig {
    uint32_t vocabularySize;
    uint32_t contextLength;
    uint32_t channels;
    uint32_t hiddenChannels;
    uint32_t attentionHeads;
    uint32_t transformerLayers;
    uint64_t initialSeed;
} UmiAiTrainingStackedDecoderConfig;

typedef struct UmiAiTrainingStackedDecoderInfo {
    UmiAiTrainingStackedDecoderConfig config;
    uint64_t completedTrainingSteps;
    size_t trainableParameterTensors;
    size_t trainableParameterElements;
} UmiAiTrainingStackedDecoderInfo;

typedef struct UmiAiTrainingStackedStep {
    float selectedCrossEntropyBeforeUpdate;
    size_t supervisedTokens;
    uint64_t completedTrainingSteps;
} UmiAiTrainingStackedStep;

/* Validate a concrete, currently supported model configuration and map it to
 * the EXISTING Framework model-scaling planner. That planner gives a lower
 * bound, NOT the training process's peak RAM requirement. On error the
 * caller's estimate stays unchanged. */
UmiStatus UmiAiTrainingStackedDecoderPlan(
    const UmiAiTrainingStackedDecoderConfig *config,
    UmiAiTrainingModelScalingEstimate *outEstimate);

/* A successful Create transfers a new model to the caller. Initialise
 * *outModel=NULL. Seeds use the existing Framework RNG and parameter shapes
 * mirror its single-block pre-LN Transformer. */
UmiStatus UmiAiTrainingStackedDecoderCreate(
    const UmiAiTrainingStackedDecoderConfig *config,
    UmiAiTrainingStackedDecoder **outModel);
void UmiAiTrainingStackedDecoderDestroy(UmiAiTrainingStackedDecoder *model);
UmiStatus UmiAiTrainingStackedDecoderInspect(
    const UmiAiTrainingStackedDecoder *model,
    UmiAiTrainingStackedDecoderInfo *outInfo);
UmiStatus UmiAiTrainingStackedDecoderMemoryInspect(
    const UmiAiTrainingStackedDecoder *model,
    UmiAiTrainingTapeMemory *outMemory);

/* One WHOLE context per forward request; [contextLength,vocabularySize]
 * row-major FP32 logits. Capacity is in floats, not bytes. Each Transformer
 * block is the EXISTING Framework block; causal future-token exclusion is
 * inherited at every layer. Output is unchanged on all failures. */
UmiStatus UmiAiTrainingStackedDecoderLogits(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, size_t tokenCount,
    float *outLogits, size_t capacity);

/* Train or evaluate a selected-token mean cross-entropy loss. The mask
 * contains only 0 and 1; ignored target IDs need not be valid. At least one
 * token must be selected. Training calls the Framework's transactional SGD
 * over ALL model parameters, not an alternative optimiser.
 *
 * Successful steps update weights, advance the completed-step count, and
 * rewind temporary graph nodes. An error leaves outputs and, for rejected
 * SGD candidates, parameters unchanged. This is educational full-model SGD,
 * not adapter-only training or a production LLM trainer. */
UmiStatus UmiAiTrainingStackedDecoderEvaluateSelected(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, const size_t *targets, const uint8_t *lossMask,
    size_t tokenCount, float *outLoss);
UmiStatus UmiAiTrainingStackedDecoderTrainSelected(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, const size_t *targets, const uint8_t *lossMask,
    size_t tokenCount, float learningRate,
    UmiAiTrainingStackedStep *outStep);

/* Raw greedy token ID prediction from the LAST position of one fixed context.
 * Ties resolve to the lowest ID; reserved special IDs can be returned. No
 * tokenizer, sampler, EOS stop, generated-code execution or KV cache is used.
 * GenerateGreedy retains the original context, left-shifting when full.
 * It copies generated IDs only after the entire requested run succeeds. */
UmiStatus UmiAiTrainingStackedDecoderPredictNext(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, size_t tokenCount, size_t *outTokenId);
UmiStatus UmiAiTrainingStackedDecoderGenerateGreedy(
    UmiAiTrainingStackedDecoder *model,
    const size_t *fullContext, size_t contextLength,
    size_t requestedTokens, size_t *outTokens, size_t capacity,
    size_t *outCount);
#ifdef __cplusplus
}
#endif
#endif
