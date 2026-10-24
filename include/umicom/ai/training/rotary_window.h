/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/rotary_window.h
 *
 * PURPOSE:
 *   Study a single-head, SINGLE-ATTENTION-BLOCK rolling key/value cache
 *   whose Q/K projections use RoPE and keep original absolute positions.
 *
 * SCOPE AND CORRECTNESS:
 *   Caller provides finite, already-projected Q, K, V vectors for each new
 *   token. These vectors must depend only on that token's fixed embedding,
 *   not on earlier context. This condition holds for first-block projections
 *   of a suitably designed model; it does NOT generally hold for upper
 *   attention layers, where hidden states can depend on evicted tokens.
 *   This is NOT a replacement for the existing learned-absolute decoder or
 *   a claim of parity against its historical BOS-left-padded generator.
 *
 *   One successful Append adds exactly ONE key/value row and computes its
 *   causal attended value over the retained rolling window. On eviction,
 *   older absolute positions are NOT rebased. Key/value rows are NOT rebuilt.
 *   Caller must not train or bind any legacy checkpoint to this research API.
 *
 * OWNERSHIP:
 *   Window is opaque, heap-owned by Create/Destroy. Inputs and output vectors
 *   are caller-owned. Public operations are synchronous and not thread-safe.
 *   Rejected Append calls preserve the window and caller's output completely.
 *   Reset drops retained tokens and starts a fresh absolute timeline at zero.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_ROTARY_WINDOW_H
#define UMICOM_AI_TRAINING_ROTARY_WINDOW_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/rotary_position.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingRotaryWindow UmiAiTrainingRotaryWindow;

typedef struct UmiAiTrainingRotaryWindowConfig {
    UmiAiTrainingRotaryConfig rotary;
    size_t contextCapacity;  /* 1..32, bounded single-attention-block study */
} UmiAiTrainingRotaryWindowConfig;

typedef struct UmiAiTrainingRotaryWindowStats {
    uint64_t computedRows;       /* one successfully appended Q/K/V row each */
    uint64_t evictedRows;        /* rolling-window removals */
    uint64_t fullWindowRebuilds; /* always zero for this proven first-block case */
    uint64_t nextAbsolutePosition;
    size_t retainedRows;
    size_t contextCapacity;
} UmiAiTrainingRotaryWindowStats;

/* Caller retains responsibility for the model-family and checkpoint policy.
 * Validation and allocation failure preserve *outWindow. */
UmiStatus UmiAiTrainingRotaryWindowCreate(
    const UmiAiTrainingRotaryWindowConfig *config,
    UmiAiTrainingRotaryWindow **outWindow);
void UmiAiTrainingRotaryWindowDestroy(UmiAiTrainingRotaryWindow *window);

/* Append one fixed Q/K/V projection row, calculate the causal context for
 * this row, and roll the window when full. query/key/value each contain exactly
 * headChannels floats; capacity counts float outputs. The returned context
 * is softmax(QK^T/sqrt(headChannels)) * V over only RETAINED rows.
 *
 * The function stages the entire new window before publication. Inputs and
 * outContext must not overlap the opaque window's storage. */
UmiStatus UmiAiTrainingRotaryWindowAppend(
    UmiAiTrainingRotaryWindow *window,
    const float *query, const float *key, const float *value,
    size_t channelCount, float *outContext, size_t capacity);

UmiStatus UmiAiTrainingRotaryWindowInspect(
    const UmiAiTrainingRotaryWindow *window,
    UmiAiTrainingRotaryWindowStats *outStats);
UmiStatus UmiAiTrainingRotaryWindowReset(UmiAiTrainingRotaryWindow *window);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_ROTARY_WINDOW_H */
