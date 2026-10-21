/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/sequence_shape.h
 *
 * PURPOSE:
 *   Express a rank-three [batch, time, channels] model input while preserving
 *   the validated row-major rank-two training-tape ABI of Batch 01.
 *
 * ENGINEERING NOTE:
 *   A 3D sequence can be flattened losslessly to [batch * time, channels].
 *   Causal attention is then called with the per-sequence time length; the
 *   native operator ensures tokens cannot attend across batch boundaries.
 *   This is a shape/layout adapter, NOT general N-dimensional autodiff.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_SEQUENCE_SHAPE_H
#define UMICOM_AI_TRAINING_SEQUENCE_SHAPE_H

#include <stddef.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingSequenceShape {
    size_t batch;
    size_t time;
    size_t channels;
} UmiAiTrainingSequenceShape;

/* Validate, then publish an immutable-by-convention layout descriptor.
 * Failure leaves the output unchanged; count must fit the tape's bounds. */
UmiStatus UmiAiTrainingSequenceShapeCreate(size_t batch, size_t time,
                                           size_t channels,
                                           UmiAiTrainingSequenceShape *outShape);

/* Translate (batch, time, channel) to the row-major flattened scalar offset.
 * The caller retains ownership of outOffset. Failed indexes do not modify it. */
UmiStatus UmiAiTrainingSequenceOffset(const UmiAiTrainingSequenceShape *shape,
                                     size_t batchIndex, size_t timeIndex,
                                     size_t channelIndex, size_t *outOffset);

/* Return the 2D shape needed by the existing training tape. The two output
 * pointers must not alias: both dimensions are published together. */
UmiStatus UmiAiTrainingSequenceFlatten(const UmiAiTrainingSequenceShape *shape,
                                      size_t *outRows, size_t *outColumns);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_SEQUENCE_SHAPE_H */
