/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/supervised_window.h
 * PURPOSE:
 *   Prepare one bounded, teacher-forced prompt/completion next-token window
 *   with an explicit assistant-response loss mask. Reuse the canonical
 *   reversible byte tokenizer; do not implement another token vocabulary.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SUPERVISED_WINDOW_H
#define UMICOM_AI_TRAINING_SUPERVISED_WINDOW_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingSupervisedWindowInfo {
    size_t contextTokens;
    size_t promptBytes;
    size_t completionBytes;
    size_t supervisedTargets;   /* Completion bytes plus exactly one EOS. */
    size_t ignoredTargets;      /* Prompt and right-padding positions. */
} UmiAiTrainingSupervisedWindowInfo;

/* Construct a complete fixed-length [input, next-token target, lossMask]
 * training example from ALREADY REVIEWED prompt/completion bytes:
 *
 *   stream = [BOS] + prompt + completion + [EOS]
 *   inputs[i] = stream[i]
 *   targets[i] = stream[i+1]
 *   mask[i] = 1 only when targets[i] is a completion byte or final EOS.
 *
 * The remaining positions are right-padded with PAD=0 and excluded from loss.
 * At most one contiguous sequence is prepared; no truncation is permitted.
 * The explicit context length is 1..UMI_AI_DECODER_MAX_CONTEXT. The complete
 * prompt and completion plus BOS/EOS must fit into contextLength+1 tokens.
 * Both prompt and completion must be nonempty; raw NUL bytes are supported by
 * the existing byte tokenizer, but callers preparing text for supervised
 * fine-tuning must separately verify UTF-8, content rights, provenance,
 * privacy review, duplicates and train/evaluation isolation through the
 * existing Framework source-admission services.
 *
 * This service does NOT read files, transmit media or train a model. Output
 * arrays must be distinct and must not alias source buffers. Each capacity
 * is counted in elements (size_t, size_t and uint8_t respectively).
 * Rejected operations leave all caller outputs unchanged. */
UmiStatus UmiAiTrainingSupervisedByteWindowBuild(
    const uint8_t *promptBytes, size_t promptLength,
    const uint8_t *completionBytes, size_t completionLength,
    size_t contextLength,
    size_t *outInputs, size_t inputCapacity,
    size_t *outTargets, size_t targetCapacity,
    uint8_t *outLossMask, size_t maskCapacity,
    UmiAiTrainingSupervisedWindowInfo *outInfo);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_SUPERVISED_WINDOW_H */
