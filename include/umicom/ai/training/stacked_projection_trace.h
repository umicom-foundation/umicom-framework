/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_projection_trace.h
 * PURPOSE:
 *   Observe real K/V projection rows from the canonical stacked Transformer
 *   without replacing its attention, embeddings or numerical training tape.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_PROJECTION_TRACE_H
#define UMICOM_AI_TRAINING_STACKED_PROJECTION_TRACE_H

#include <stddef.h>
#include "umicom/ai/training/stacked_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Trace exactly one FULL context through the original stacked model, using
 * the already established optional projection hook of the Transformer block.
 * The hook invokes the SAME tape Matmul for all six projections and only
 * copies the resulting KEY and VALUE tensors; it does not calculate separate
 * attention or alter the original model, parameters or completed SGD steps.
 *
 * outKeys/outValues layout: [layer][context position][channel], row-major.
 * outLogits layout: [context position][vocabulary token].
 * Capacities count FP32 elements, not bytes. All three output regions must
 * be disjoint from each other and from the token input. Input tokens must contain contextLength legal vocabulary IDs.
 *
 * The model is caller-owned and must not be trained or used concurrently.
 * On any failure all caller outputs remain unchanged; temporary tape nodes
 * are rolled back after every attempt. Rollback clears gradient readiness,
 * matching the existing Framework tape contract. This is an observation API,
 * not an incremental attention backend, model checkpoint or speed benchmark. */
UmiStatus UmiAiTrainingStackedDecoderTraceProjections(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputIds, size_t tokenCount,
    float *outKeys, size_t keyCapacity,
    float *outValues, size_t valueCapacity,
    float *outLogits, size_t logitCapacity);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_PROJECTION_TRACE_H */
