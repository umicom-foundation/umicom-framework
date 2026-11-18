/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/image_canvas.c
 *
 * PURPOSE:
 *   Admit full RGB spans and rectangle bounds before publishing a colored canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/image_canvas.h"
#include <string.h>

UmiStatus UmiAiTrainingImageCanvasRgb8(const UmiAiTrainingRgb8View *source,
    size_t width, size_t height, size_t left, size_t top,
    const uint8_t fillRgb[3], uint8_t *outRgb, size_t byteCapacity)
{
    if (source == NULL || source->pixels == NULL || fillRgb == NULL || outRgb == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingImageSpan input, output;
    UmiStatus status = UmiAiTrainingImageDescribeRgb8(source->width,
        source->height, source->row_stride_bytes, &input);
    if (status != UMI_STATUS_OK) return status;
    if (width > SIZE_MAX / 3U) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingImageDescribeRgb8(width, height, width * 3U, &output);
    if (status != UMI_STATUS_OK) return status;
    if (source->bytes < input.required_bytes || byteCapacity < output.value_count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Subtraction after origin admission avoids left+source->width wrapping.
     * The complete admitted source must fit; a small visible fragment is not
     * permission to read an invalid or oversized source descriptor. */
    if (left >= width || top >= height ||
        source->width > width - left || source->height > height - top)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Copy the tiny color before publication, allowing a color sampled from
     * the old output. All later work is bounded and cannot report a failure. */
    const uint8_t color[3] = {fillRgb[0], fillRgb[1], fillRgb[2]};
    for (size_t offset = 0U; offset < output.value_count; offset += 3U) {
        outRgb[offset] = color[0];
        outRgb[offset + 1U] = color[1];
        outRgb[offset + 2U] = color[2];
    }
    const size_t rowBytes = source->width * 3U;
    for (size_t y = 0U; y < source->height; ++y)
        memcpy(outRgb + ((top + y) * width + left) * 3U,
               source->pixels + y * source->row_stride_bytes, rowBytes);
    return UMI_STATUS_OK;
}
