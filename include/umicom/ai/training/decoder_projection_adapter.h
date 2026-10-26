/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/decoder_projection_adapter.h
 *
 * PURPOSE:
 *   Train selected Transformer projection adapters against the real native
 *   decoder's frozen parameter snapshot, using assistant-response-only loss.
 *
 * ARCHITECTURE AND OWNERSHIP:
 *   The canonical decoder, embedding operators, Transformer block, low-rank
 *   projections, selected-row cross-entropy and transactional SGD remain the
 *   only implementations of their respective mathematical operations.
 *   This component owns an independent training tape containing COPIES of the
 *   decoder's frozen parameter tensors plus selected low-rank trainable leaves.
 *   Keeping a separate tape is essential: the historical decoder rewinds its
 *   own tape to its fixed parameter prefix after inference/training operations.
 *   Attaching new leaves to that internal tape would invalidate their lifetime.
 *
 *   The original decoder is borrowed and must remain alive and unchanged until
 *   this adapter is destroyed. Its exact canonical checkpoint SHA-256 is
 *   checked before every public operation. Caller serialises access to it.
 *   The original decoder and its checkpoint format are not modified.
 *
 * LIMITS:
 *   One fixed-length educational decoder context per training/evaluation
 *   call, using the existing bounded FP32 tape and its numerical contracts.
 *   This is supervised adapter-only SGD, not full-model fine-tuning, AdamW
 *   continuation, signed model promotion or a production-scale LLM runtime.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_DECODER_PROJECTION_ADAPTER_H
#define UMICOM_AI_TRAINING_DECODER_PROJECTION_ADAPTER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/transformer_low_rank.h"

#ifdef __cplusplus
extern "C" {
#endif

/* These bit flags select existing Transformer projection roles, not decoder
 * checkpoint parameter indices. Combine selected roles with bitwise OR. */
#define UMI_AI_DECODER_ADAPT_QUERY              UINT32_C(0x01)
#define UMI_AI_DECODER_ADAPT_KEY                UINT32_C(0x02)
#define UMI_AI_DECODER_ADAPT_VALUE              UINT32_C(0x04)
#define UMI_AI_DECODER_ADAPT_ATTENTION_OUTPUT   UINT32_C(0x08)
#define UMI_AI_DECODER_ADAPT_FEED_FORWARD_UP    UINT32_C(0x10)
#define UMI_AI_DECODER_ADAPT_FEED_FORWARD_DOWN  UINT32_C(0x20)
#define UMI_AI_DECODER_ADAPT_ALL                UINT32_C(0x3f)

#define UMI_AI_DECODER_ADAPTER_SHA256_HEX_CAPACITY 65U

typedef struct UmiAiTrainingDecoderProjectionAdapter
    UmiAiTrainingDecoderProjectionAdapter;

typedef struct UmiAiTrainingDecoderProjectionAdapterInfo {
    UmiAiTrainingDecoderConfig decoder;
    uint32_t selectedProjections;
    size_t rank;
    size_t trainableTensors;
    size_t trainableElements;
    size_t retainedTapeNodes;
    uint64_t frozenDecoderSteps;
    uint64_t completedAdapterSteps;
    char frozenCheckpointSha256[UMI_AI_DECODER_ADAPTER_SHA256_HEX_CAPACITY];
} UmiAiTrainingDecoderProjectionAdapterInfo;

typedef struct UmiAiTrainingDecoderProjectionStep {
    float selectedCrossEntropyBeforeUpdate;
    size_t supervisedTokenCount;
    uint64_t completedAdapterSteps;
} UmiAiTrainingDecoderProjectionStep;

/* Create an adapter with independently retained frozen decoder parameter
 * values. rank is 1..decoder.channels and must fit every selected projection.
 * At least one valid projection bit is required; unsupported bits are refused.
 * All adapter-up weights start at zero, so initial output logits reproduce the
 * frozen decoder to the usual FP32 numerical tolerance. The decoder's exact
 * checkpoint bytes are bound using canonical Framework checkpoint + SHA-256.
 *
 * Initialise *outAdapter=NULL. On failure it stays unchanged. The caller
 * owns a successful adapter and must destroy it before destroying its decoder.
 * No checkpoint, file, network, global RNG or decoder parameter is modified. */
UmiStatus UmiAiTrainingDecoderProjectionAdapterCreate(
    UmiAiTrainingDecoder *decoder,
    uint32_t selectedProjections, size_t rank, uint64_t initialSeed,
    UmiAiTrainingDecoderProjectionAdapter **outAdapter);

void UmiAiTrainingDecoderProjectionAdapterDestroy(
    UmiAiTrainingDecoderProjectionAdapter *adapter);

/* Return counts and the exact frozen checkpoint identity without exposing
 * internal tape handles. An altered decoder checkpoint invalidates the
 * adapter even when the model shape and optimiser step count still match. */
UmiStatus UmiAiTrainingDecoderProjectionAdapterInspect(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    UmiAiTrainingDecoderProjectionAdapterInfo *outInfo);

/* Compute [contextLength, vocabularySize] logits with the selected adapters.
 * outLogits capacity counts floats. Failed calls leave output unchanged.
 * Neither the borrowed decoder nor adapter weights are modified. */
UmiStatus UmiAiTrainingDecoderProjectionAdapterLogits(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const size_t *inputTokenIds, size_t tokenCount,
    float *outLogits, size_t capacity);

/* Compute average cross-entropy OVER mask==1 tokens only, using the existing
 * selected-loss tape composition. Mask must contain only 0 and 1 with at
 * least one selected row; ignored targets need not contain vocabulary IDs.
 * No parameter update occurs. Rejection preserves *outLoss. */
UmiStatus UmiAiTrainingDecoderProjectionAdapterEvaluate(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    const uint8_t *lossMask, size_t tokenCount, float *outLoss);

/* Perform one selected-token cross-entropy, Backward and atomic SGD update of
 * ONLY the selected low-rank leaves. Frozen weights and the original decoder
 * are never supplied to the optimiser. Temporary tape nodes are rolled back
 * after every attempt; successive steps do not grow the graph indefinitely.
 * A successful report contains loss measured before its update. On rejection,
 * no adapter step is counted and *outReport remains unchanged. */
UmiStatus UmiAiTrainingDecoderProjectionAdapterTrainStep(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    const uint8_t *lossMask, size_t tokenCount, float learningRate,
    UmiAiTrainingDecoderProjectionStep *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_DECODER_PROJECTION_ADAPTER_H */
