/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/decoder_logit_adapter.h
 * PURPOSE:
 *   Train a small, independent low-rank residual over the frozen native
 *   decoder's logits using the canonical C23 training tape and token loss.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_DECODER_LOGIT_ADAPTER_H
#define UMICOM_AI_TRAINING_DECODER_LOGIT_ADAPTER_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingDecoderLogitAdapter UmiAiTrainingDecoderLogitAdapter;

typedef struct UmiAiTrainingDecoderLogitStep {
    float crossEntropyBeforeUpdate;
    size_t tokenCount;
    uint64_t completedAdapterSteps;
} UmiAiTrainingDecoderLogitStep;

typedef struct UmiAiTrainingDecoderLogitInfo {
    size_t vocabularySize;
    size_t contextLength;
    size_t rank;
    size_t trainableElements;
    uint64_t frozenDecoderSteps;
    uint64_t completedAdapterSteps;
} UmiAiTrainingDecoderLogitInfo;

/* The decoder is BORROWED and must remain alive, unchanged, and exclusively
 * accessible until Destroy. A private tape owns adapter matrices. The first
 * adapted logits equal the frozen decoder's logits because adapterUp starts
 * at zero. This is a logit-residual adaptation experiment, NOT LoRA injected
 * into the Transformer's Q/K/V/MLP projections. No model or checkpoint format
 * is modified. Initialise *outAdapter=NULL before creation. */
UmiStatus UmiAiTrainingDecoderLogitAdapterCreate(
    UmiAiTrainingDecoder *decoder, size_t rank, uint64_t initialSeed,
    UmiAiTrainingDecoderLogitAdapter **outAdapter);

void UmiAiTrainingDecoderLogitAdapterDestroy(
    UmiAiTrainingDecoderLogitAdapter *adapter);

/* Inspect adapter capacity and optimiser-step identity without exposing
 * private parameter memory. Failure preserves caller output. */
UmiStatus UmiAiTrainingDecoderLogitAdapterInspect(
    const UmiAiTrainingDecoderLogitAdapter *adapter,
    UmiAiTrainingDecoderLogitInfo *outInfo);

/* Exactly one complete context is evaluated. Logits are float32 and arranged
 * [contextLength, vocabularySize]. The caller supplies enough float elements,
 * not bytes. No sampling or generated source execution is performed. A model
 * trained since Create is rejected. Failure leaves the output unchanged. */
UmiStatus UmiAiTrainingDecoderLogitAdapterLogits(
    UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputTokenIds, size_t tokenCount,
    float *outLogits, size_t capacity);

/* Evaluate the mean categorical cross-entropy for one complete context.
 * Targets are one vocabulary ID per input token, suitable for explicitly
 * prepared next-token examples. No gradient or weight update occurs. */
UmiStatus UmiAiTrainingDecoderLogitAdapterEvaluate(
    UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    size_t tokenCount, float *outLoss);

/* Train ONLY the two adapter matrices with the Framework's CrossEntropy,
 * Backward and failure-atomic SgdStepMany operations. The original decoder
 * parameters and optimiser steps remain untouched. The reported loss is
 * measured BEFORE the update. The adapter stages the complete graph and
 * rewinds its temporary nodes after each step, preserving trained leaves.
 * The caller serialises access. Invalid input and rejected SGD updates leave
 * all published outputs unchanged. This is not yet full Transformer fine-
 * tuning or an assistant-response-only masked-loss trainer. */
UmiStatus UmiAiTrainingDecoderLogitAdapterTrainStep(
    UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    size_t tokenCount, float learningRate,
    UmiAiTrainingDecoderLogitStep *outReport);

#ifdef __cplusplus
}
#endif
#endif
