/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/sequence_shape.c
 *
 * PURPOSE:
 *   Checked 3D-to-2D model sequence layout for the existing training tape.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/sequence_shape.h"
#include "umicom/ai/training/tape.h"
#include <stdbool.h>

static bool ShapeIsSafe(const UmiAiTrainingSequenceShape *shape)
{
    if (shape == NULL || shape->batch == 0U || shape->time == 0U ||
        shape->channels == 0U) return false;
    if (shape->channels > UMI_AI_TRAINING_MAX_ELEMENTS) return false;
    if (shape->batch > UMI_AI_TRAINING_MAX_ELEMENTS / shape->channels) return false;
    return shape->time <= UMI_AI_TRAINING_MAX_ELEMENTS /
                             (shape->batch * shape->channels);
}

UmiStatus UmiAiTrainingSequenceShapeCreate(size_t batch, size_t time,
                                           size_t channels,
                                           UmiAiTrainingSequenceShape *outShape)
{
    UmiAiTrainingSequenceShape candidate = {batch, time, channels};
    if (outShape == NULL || !ShapeIsSafe(&candidate)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *outShape = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSequenceOffset(const UmiAiTrainingSequenceShape *shape,
                                     size_t batchIndex, size_t timeIndex,
                                     size_t channelIndex, size_t *outOffset)
{
    if (!ShapeIsSafe(shape) || outOffset == NULL ||
        batchIndex >= shape->batch || timeIndex >= shape->time ||
        channelIndex >= shape->channels) return UMI_STATUS_INVALID_ARGUMENT;
    *outOffset = ((batchIndex * shape->time) + timeIndex) *
                 shape->channels + channelIndex;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSequenceFlatten(const UmiAiTrainingSequenceShape *shape,
                                      size_t *outRows, size_t *outColumns)
{
    if (!ShapeIsSafe(shape) || outRows == NULL || outColumns == NULL ||
        outRows == outColumns) return UMI_STATUS_INVALID_ARGUMENT;
    *outRows = shape->batch * shape->time;
    *outColumns = shape->channels;
    return UMI_STATUS_OK;
}
