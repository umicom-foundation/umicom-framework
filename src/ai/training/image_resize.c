/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/image_resize.c
 *
 * PURPOSE:
 *   Reuse complete RGB span admission before deterministic whole-pixel nearest resampling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_resize.h"

/* Both sampling policies share admission so invalid metadata and short buffers
 * have the same status and transactional output contract. The source view and
 * all reachable bytes remain caller-owned; this helper allocates nothing. */
static UmiStatus AdmitResize(const UmiAiTrainingRgb8View *source,
    size_t width, size_t height, uint8_t *outRgb, size_t byteCapacity)
{
    if (source == NULL || source->pixels == NULL || outRgb == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingImageSpan input, output;
    UmiStatus status = UmiAiTrainingImageDescribeRgb8(source->width,
        source->height, source->row_stride_bytes, &input);
    if (status != UMI_STATUS_OK) return status;
    /* Check the stride construction before passing target shape admission to
     * its existing owner. Invalid dimensions never wrap into a small stride. */
    if (width > SIZE_MAX / 3U) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingImageDescribeRgb8(width, height, width * 3U, &output);
    if (status != UMI_STATUS_OK) return status;
    if (source->bytes < input.required_bytes || byteCapacity < output.value_count)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingImageResizeNearestRgb8(const UmiAiTrainingRgb8View *source,
    size_t width, size_t height, uint8_t *outRgb, size_t byteCapacity)
{
    /* The previous local admission is retained for review. AdmitResize now owns
     * this identical contract for nearest and bilinear sampling together. */
#if 0
    if (source == NULL || source->pixels == NULL || outRgb == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingImageSpan input, output;
    UmiStatus status = UmiAiTrainingImageDescribeRgb8(source->width,
        source->height, source->row_stride_bytes, &input);
    if (status != UMI_STATUS_OK) return status;
    /* Check the stride construction before passing target shape admission to
     * its existing owner. Invalid dimensions never wrap into a small stride. */
    if (width > SIZE_MAX / 3U) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = UmiAiTrainingImageDescribeRgb8(width, height, width * 3U, &output);
    if (status != UMI_STATUS_OK) return status;
    if (source->bytes < input.required_bytes || byteCapacity < output.value_count)
        return UMI_STATUS_CAPACITY_EXCEEDED;

#endif
    const UmiStatus status = AdmitResize(source, width, height, outRgb, byteCapacity);
    if (status != UMI_STATUS_OK) return status;
    /* Admitted dimensions are at most IMAGE_MAX_VALUES/3. uint64_t arithmetic
     * makes the center product independent of the host's size_t width. Every
     * selected coordinate is below its positive source dimension. The loop
     * cannot fail after admission, so no temporary image or rollback is needed. */
    for (size_t y = 0U; y < height; ++y) {
        const size_t sy = (size_t)(((UINT64_C(2) * y + 1U) * source->height) /
            (UINT64_C(2) * height));
        for (size_t x = 0U; x < width; ++x) {
            const size_t sx = (size_t)(((UINT64_C(2) * x + 1U) * source->width) /
                (UINT64_C(2) * width));
            const size_t from = sy * source->row_stride_bytes + sx * 3U;
            const size_t to = (y * width + x) * 3U;
            outRgb[to] = source->pixels[from];
            outRgb[to + 1U] = source->pixels[from + 1U];
            outRgb[to + 2U] = source->pixels[from + 2U];
        }
    }
    return UMI_STATUS_OK;
}

/* Axis weights are exact rational distances between pixel centers. Keeping the
 * remainder avoids platform-dependent floating rounding and permits one final
 * rounding after both axes have contributed. A repeated border has zero weight
 * on its second coordinate, including every one-pixel source dimension. */
typedef struct ResizeAxis {
    size_t low;
    size_t high;
    uint64_t remainder;
    uint64_t denominator;
} ResizeAxis;

static ResizeAxis MapBilinearAxis(size_t destination, size_t sourceSize,
    size_t targetSize)
{
    ResizeAxis axis = {0U, 0U, 0U, UINT64_C(2) * targetSize};
    const uint64_t centered = (UINT64_C(2) * destination + 1U) * sourceSize;
    if (centered <= targetSize) return axis;
    const uint64_t numerator = centered - targetSize;
    axis.low = (size_t)(numerator / axis.denominator);
    if (axis.low >= sourceSize - 1U) {
        axis.low = sourceSize - 1U;
        axis.high = axis.low;
        return axis;
    }
    axis.high = axis.low + 1U;
    axis.remainder = numerator % axis.denominator;
    return axis;
}

UmiStatus UmiAiTrainingImageResizeBilinearRgb8(const UmiAiTrainingRgb8View *source,
    size_t width, size_t height, uint8_t *outRgb, size_t byteCapacity)
{
    const UmiStatus status = AdmitResize(source, width, height, outRgb, byteCapacity);
    if (status != UMI_STATUS_OK) return status;
    /* Admitted width*height is at most IMAGE_MAX_VALUES/3. The product of the
     * two denominators is 4*width*height, and the weighted byte sum is at most
     * 255 times that product. uint64_t also covers every center-coordinate
     * product before clamping. No arithmetic or capacity failure remains in
     * this loop, preserving the refusal-before-write contract without a copy. */
    for (size_t y = 0U; y < height; ++y) {
        const ResizeAxis ay = MapBilinearAxis(y, source->height, height);
        for (size_t x = 0U; x < width; ++x) {
            const ResizeAxis ax = MapBilinearAxis(x, source->width, width);
            const uint64_t left = ax.denominator - ax.remainder;
            const uint64_t top = ay.denominator - ay.remainder;
            const uint64_t divisor = ax.denominator * ay.denominator;
            const size_t upper = ay.low * source->row_stride_bytes;
            const size_t lower = ay.high * source->row_stride_bytes;
            const size_t to = (y * width + x) * 3U;
            for (size_t channel = 0U; channel < 3U; ++channel) {
                const uint64_t sum =
                    left * top * source->pixels[upper + ax.low * 3U + channel] +
                    ax.remainder * top * source->pixels[upper + ax.high * 3U + channel] +
                    left * ay.remainder * source->pixels[lower + ax.low * 3U + channel] +
                    ax.remainder * ay.remainder * source->pixels[lower + ax.high * 3U + channel];
                outRgb[to + channel] = (uint8_t)((sum + divisor / 2U) / divisor);
            }
        }
    }
    return UMI_STATUS_OK;
}
