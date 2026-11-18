/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/image_view.c
 *
 * PURPOSE:
 *   Admit an RGB source and rectangle before publishing a borrowed cropped view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_view.h"

UmiStatus UmiAiTrainingImageCropRgb8(const UmiAiTrainingRgb8View *source,
    size_t left, size_t top, size_t width, size_t height,
    UmiAiTrainingRgb8View *outView)
{
    if (source == NULL || outView == NULL || source->pixels == NULL ||
        width == 0U || height == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingImageSpan full;
    UmiStatus status = UmiAiTrainingImageDescribeRgb8(source->width,
        source->height, source->row_stride_bytes, &full);
    if (status != UMI_STATUS_OK) return status;
    if (source->bytes < full.required_bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Subtract after admitting the origin: left+width could wrap for hostile
     * metadata. A crop never legitimizes an incomplete or oversized source. */
    if (left >= source->width || top >= source->height ||
        width > source->width - left || height > source->height - top)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingImageSpan cropped;
    status = UmiAiTrainingImageDescribeRgb8(width, height,
        source->row_stride_bytes, &cropped);
    if (status != UMI_STATUS_OK) return status;
    /* Full-span admission bounds top*stride and left*3. The rectangle proof
     * above bounds their sum and the cropped extent inside that same span. */
    const size_t offset = top * source->row_stride_bytes + left * 3U;
    if (offset > full.required_bytes ||
        cropped.required_bytes > full.required_bytes - offset)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiAiTrainingRgb8View candidate = {
        source->pixels + offset, cropped.required_bytes, width, height,
        source->row_stride_bytes
    };
    /* One publication supports in-place descriptor narrowing without changing
     * the pixel owner. No previous view is released or invalidated here. */
    *outView = candidate;
    return UMI_STATUS_OK;
}
