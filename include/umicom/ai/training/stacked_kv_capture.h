/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_kv_capture.h
 * PURPOSE:
 *   Fill the existing multi-layer KV store with real canonical Transformer
 *   key/value projection rows, verify causal prefix stability, and rebuild
 *   every layer after learned-absolute context rollover.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_KV_CAPTURE_H
#define UMICOM_AI_TRAINING_STACKED_KV_CAPTURE_H

#include "umicom/ai/training/stacked_kv_store.h"
#include "umicom/ai/training/stacked_model_identity.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedKvCapture UmiAiTrainingStackedKvCapture;

typedef struct UmiAiTrainingStackedKvCaptureInfo {
    UmiAiTrainingStackedKvStoreInfo stored;
    uint64_t completeReferenceEvaluations;
    uint64_t actuallyEvaluatedTokenLayerRows;
    size_t vocabularySize;
    float maximumPreviousRowDifference;
    UmiAiTrainingStackedModelIdentity modelIdentity;
} UmiAiTrainingStackedKvCaptureInfo;

/* Borrow a fixed, existing stacked decoder and own a separate KV store.
 * Pin canonical checkpoint identity, not a caller-provided model label.
 * Model must remain alive/unchanged and exclusive until Destroy. Initialise
 * *outCapture=NULL before calling. No model training or data persistence. */
UmiStatus UmiAiTrainingStackedKvCaptureCreate(
    UmiAiTrainingStackedDecoder *model,
    UmiAiTrainingStackedKvCapture **outCapture);
void UmiAiTrainingStackedKvCaptureDestroy(
    UmiAiTrainingStackedKvCapture *capture);

/* Prefill 1..contextLength unpadded tokens at learned positions 0..P-1.
 * The REAL full-context decoder runs with BOS padding to the RIGHT. Its
 * projection taps populate the existing Framework KV store. outLastLogits
 * receives the vocabulary logits for the last ACTUAL prefix token, not the
 * later padding rows. The operation is failure-atomic for stored state and
 * caller output. The store must be empty. */
UmiStatus UmiAiTrainingStackedKvCapturePrefill(
    UmiAiTrainingStackedKvCapture *capture,
    const size_t *prefixIds, size_t prefixCount,
    float *outLastLogits, size_t logitCapacity);

/* Advance one model token. When positions have not changed, compare every
 * previously stored K/V component with a fresh canonical projection before
 * appending only its final row. All differences must be finite and no larger
 * than 0.00004f. At capacity, shift one token and replace every row in every
 * layer with a fresh canonical result; retaining moved old rows is forbidden.
 *
 * This is a CORRECTNESS AND STORAGE INTEGRATION service, not accelerated
 * inference: each transition still evaluates the entire original decoder.
 * No claim of reduced actual decoder FLOPs or throughput is authorised.
 * Rejected operations do not alter store state, counters or output logits. */
UmiStatus UmiAiTrainingStackedKvCaptureAppend(
    UmiAiTrainingStackedKvCapture *capture, size_t nextTokenId,
    float *outLastLogits, size_t logitCapacity);

/* Inspect distinct numbers for actual full decoder calculations and cache
 * rows accepted. Stored rows are NOT the number of computed Transformer rows.
 * The pinned model identity is checked before use. */
UmiStatus UmiAiTrainingStackedKvCaptureInspect(
    const UmiAiTrainingStackedKvCapture *capture,
    UmiAiTrainingStackedKvCaptureInfo *outInfo);

/* Inspect a COPY of an original projection row. Does not grant access to the
 * opaque store, bypass model identity, or reinterpret a checkpoint. */
UmiStatus UmiAiTrainingStackedKvCaptureRead(
    const UmiAiTrainingStackedKvCapture *capture,
    size_t layer, size_t retainedPosition,
    float *outKey, size_t keyCapacity,
    float *outValue, size_t valueCapacity);

/* Cancel one prompt; retain the approved frozen model and its identity. */
UmiStatus UmiAiTrainingStackedKvCaptureReset(
    UmiAiTrainingStackedKvCapture *capture);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_KV_CAPTURE_H */
