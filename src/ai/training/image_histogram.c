/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/image_histogram.c
 *
 * PURPOSE:
 *   Count RGB code values after shared shape admission and publish the complete result atomically.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/training/image_histogram.h"

_Static_assert(UMI_AI_TRAINING_IMAGE_MAX_VALUES / 3U <= UINT32_MAX,
    "Review histogram counter width before enlarging the image bound");

UmiStatus UmiAiTrainingImageHistogramRgb8(const UmiAiTrainingRgb8View *source,
    UmiAiTrainingRgb8Histogram *outHistogram)
{
    if (source == NULL || source->pixels == NULL || outHistogram == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingImageSpan span;
    const UmiStatus status = UmiAiTrainingImageDescribeRgb8(source->width,
        source->height, source->row_stride_bytes, &span);
    if (status != UMI_STATUS_OK) return status;
    if (source->bytes < span.required_bytes) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* A local candidate gives one publication point. Shape admission bounds
     * both address arithmetic and the number of increments for every bin. */
    UmiAiTrainingRgb8Histogram candidate = {0};
    candidate.pixel_count = span.value_count / 3U;
    for (size_t y = 0U; y < source->height; ++y) {
        const size_t row = y * source->row_stride_bytes;
        for (size_t x = 0U; x < source->width; ++x) {
            const size_t pixel = row + x * 3U;
            for (size_t channel = 0U; channel < 3U; ++channel)
                ++candidate.bins[channel][source->pixels[pixel + channel]];
        }
    }
    *outHistogram = candidate;
    return UMI_STATUS_OK;
}
