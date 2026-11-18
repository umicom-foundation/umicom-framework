/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/image_flip.c
 *
 * PURPOSE:
 *   Reuse shared RGB shape admission before copying mirrored pixels without row padding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_flip.h"

UmiStatus UmiAiTrainingImageFlipRgb8(const UmiAiTrainingRgb8View *source,
    unsigned flags, uint8_t *outRgb, size_t byteCapacity)
{
    if (source == NULL || source->pixels == NULL || outRgb == NULL ||
        (flags & ~(UMI_AI_TRAINING_FLIP_HORIZONTAL | UMI_AI_TRAINING_FLIP_VERTICAL)) != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingImageSpan span;
    const UmiStatus status = UmiAiTrainingImageDescribeRgb8(source->width,
        source->height, source->row_stride_bytes, &span);
    if (status != UMI_STATUS_OK) return status;
    if (source->bytes < span.required_bytes || byteCapacity < span.value_count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* The shared admission bounds every multiplication below. value_count is
     * also the byte count here because each channel is one unsigned byte.
     * No operation after admission can fail, so refused output is untouched. */
    for (size_t y = 0U; y < source->height; ++y) {
        const size_t sy = (flags & UMI_AI_TRAINING_FLIP_VERTICAL) != 0U
            ? source->height - 1U - y : y;
        for (size_t x = 0U; x < source->width; ++x) {
            const size_t sx = (flags & UMI_AI_TRAINING_FLIP_HORIZONTAL) != 0U
                ? source->width - 1U - x : x;
            const size_t input = sy * source->row_stride_bytes + sx * 3U;
            const size_t output = (y * source->width + x) * 3U;
            outRgb[output] = source->pixels[input];
            outRgb[output + 1U] = source->pixels[input + 1U];
            outRgb[output + 2U] = source->pixels[input + 2U];
        }
    }
    return UMI_STATUS_OK;
}
