/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/image_tensor.c
 *
 * PURPOSE:
 *   Validate decoded RGB spans before publishing bounded float tensor values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/image_tensor.h"
#include <float.h>
#include <math.h>

UmiStatus UmiAiTrainingImageDescribeRgb8(size_t width, size_t height,
    size_t rowStrideBytes, UmiAiTrainingImageSpan *outSpan)
{
    if (outSpan == NULL || width == 0U || height == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Divide before multiplying so adversarial metadata cannot wrap a size.
     * Positive width implies a positive row stride before its use as divisor. */
    if (width > UMI_AI_TRAINING_IMAGE_MAX_VALUES / 3U ||
        height > UMI_AI_TRAINING_IMAGE_MAX_VALUES / (width * 3U))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t rowBytes = width * 3U;
    if (rowStrideBytes < rowBytes) return UMI_STATUS_INVALID_ARGUMENT;
    if (height - 1U > (SIZE_MAX - rowBytes) / rowStrideBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiAiTrainingImageSpan span = {
        (height - 1U) * rowStrideBytes + rowBytes, width * height * 3U
    };
    *outSpan = span;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingImagePackRgb8(const uint8_t *rgb, size_t rgbBytes,
    size_t width, size_t height, size_t rowStrideBytes,
    UmiAiTrainingImageLayout layout, UmiAiTrainingImageRange range,
    float *outValues, size_t valueCapacity)
{
    if (rgb == NULL || outValues == NULL || width == 0U || height == 0U ||
        (layout != UMI_AI_TRAINING_IMAGE_HWC && layout != UMI_AI_TRAINING_IMAGE_CHW) ||
        (range != UMI_AI_TRAINING_IMAGE_UNIT && range != UMI_AI_TRAINING_IMAGE_SIGNED_UNIT))
        return UMI_STATUS_INVALID_ARGUMENT;
/* RGB shape validation now has one read-only owner shared by planning, image packing and video preparation. Retain the original inline checks for review. The former implementation is retained for owner review. */
#if 0
    /* Divide before multiplying: caller-supplied dimensions and strides can
     * exceed size_t even when the actual backing array is very small. The
     * model bound is separate from the last readable byte in padded storage. */
    if (width > UMI_AI_TRAINING_IMAGE_MAX_VALUES / 3U ||
        height > UMI_AI_TRAINING_IMAGE_MAX_VALUES / (width * 3U))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t rowBytes = width * 3U;
    if (rowStrideBytes < rowBytes) return UMI_STATUS_INVALID_ARGUMENT;
    if (height - 1U > (SIZE_MAX - rowBytes) / rowStrideBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t requiredBytes = (height - 1U) * rowStrideBytes + rowBytes;
    const size_t pixels = width * height;
    if (rgbBytes < requiredBytes || valueCapacity < pixels * 3U)
        return UMI_STATUS_CAPACITY_EXCEEDED;

#endif
    /* Shape admission is shared with memory planning and multi-frame clients.
     * Every caller uses the same checked row-stride and dimension contract. */
    UmiAiTrainingImageSpan span;
    UmiStatus status = UmiAiTrainingImageDescribeRgb8(width, height, rowStrideBytes, &span);
    if (status != UMI_STATUS_OK) return status;
    if (rgbBytes < span.required_bytes || valueCapacity < span.value_count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const size_t pixels = span.value_count / 3U;

    /* Every byte is a valid RGB channel, so admission is complete before the
     * first write. Convert directly to the caller's chosen order without a
     * second image, a tensor scratch allocation or an implicit alpha policy. */
    for (size_t y = 0U; y < height; ++y) {
        const uint8_t *row = rgb + y * rowStrideBytes;
        for (size_t x = 0U; x < width; ++x) {
            const size_t pixel = y * width + x;
            for (size_t channel = 0U; channel < 3U; ++channel) {
                const double unit = (double)row[x * 3U + channel] / 255.0;
                const size_t index = layout == UMI_AI_TRAINING_IMAGE_HWC ?
                    pixel * 3U + channel : channel * pixels + pixel;
                outValues[index] = (float)(range == UMI_AI_TRAINING_IMAGE_UNIT ?
                    unit : 2.0 * unit - 1.0);
            }
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingImagePackRgb8Standardized(const uint8_t *rgb, size_t rgbBytes,
    size_t width, size_t height, size_t rowStrideBytes,
    UmiAiTrainingImageLayout layout,
    const UmiAiTrainingImageStandardization *parameters,
    float *outValues, size_t valueCapacity)
{
    if (parameters == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* A positive scale makes this transform monotonic. Admitting both ends
     * of [0,1] before packing proves every later unit float can be converted
     * without an additional refusal after output publication has begun. */
    for (size_t channel = 0U; channel < 3U; ++channel) {
        const double mean = parameters->mean[channel];
        const double scale = parameters->scale[channel];
        if (!isfinite(mean) || !isfinite(scale) || scale <= 0.0)
            return UMI_STATUS_INVALID_ARGUMENT;
        const double low = -mean / scale;
        const double high = (1.0 - mean) / scale;
        if (!isfinite(low) || !isfinite(high) || low < -(double)FLT_MAX ||
            low > (double)FLT_MAX || high < -(double)FLT_MAX || high > (double)FLT_MAX)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    const UmiStatus status = UmiAiTrainingImagePackRgb8(rgb, rgbBytes,
        width, height, rowStrideBytes, layout, UMI_AI_TRAINING_IMAGE_UNIT,
        outValues, valueCapacity);
    if (status != UMI_STATUS_OK) return status;
    /* Successful canonical packing established these bounded products and
     * layout. Transform only its published prefix; preserve unused capacity.
     * Params and input stay borrowed and disjoint under the public contract. */
    const size_t pixels = width * height;
    for (size_t i = 0U; i < pixels * 3U; ++i) {
        const size_t channel = layout == UMI_AI_TRAINING_IMAGE_HWC ? i % 3U : i / pixels;
        outValues[i] = (float)(((double)outValues[i] - parameters->mean[channel]) /
            parameters->scale[channel]);
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingImageValidateValues(const float *values, size_t count,
    UmiAiTrainingImageRange range, UmiAiTrainingImageQuantization quantization)
{
    if (values == NULL || count == 0U ||
        (range != UMI_AI_TRAINING_IMAGE_UNIT && range != UMI_AI_TRAINING_IMAGE_SIGNED_UNIT) ||
        (quantization != UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE &&
         quantization != UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_AI_TRAINING_IMAGE_MAX_VALUES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const double low = range == UMI_AI_TRAINING_IMAGE_UNIT ? 0.0 : -1.0;
    /* Keeping value admission read-only gives image and clip conversion one
     * policy owner. A clip can check its final frame before exposing its first. */
    for (size_t i = 0U; i < count; ++i) {
        const double value = values[i];
        if (!isfinite(value) || (quantization == UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE &&
            (value < low || value > 1.0)))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingImageUnpackRgb8(const float *values, size_t valueCount,
    size_t width, size_t height, size_t rowStrideBytes,
    UmiAiTrainingImageLayout layout, UmiAiTrainingImageRange range,
    UmiAiTrainingImageQuantization quantization,
    uint8_t *outRgb, size_t rgbCapacityBytes)
{
    if (values == NULL || outRgb == NULL ||
        (layout != UMI_AI_TRAINING_IMAGE_HWC && layout != UMI_AI_TRAINING_IMAGE_CHW) ||
        (range != UMI_AI_TRAINING_IMAGE_UNIT && range != UMI_AI_TRAINING_IMAGE_SIGNED_UNIT) ||
        (quantization != UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE &&
         quantization != UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiTrainingImageSpan span;
    const UmiStatus status = UmiAiTrainingImageDescribeRgb8(width, height, rowStrideBytes, &span);
    if (status != UMI_STATUS_OK) return status;
    if (valueCount < span.value_count || rgbCapacityBytes < span.required_bytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const double low = range == UMI_AI_TRAINING_IMAGE_UNIT ? 0.0 : -1.0;
    /* Validate the complete tensor before publication, so even a bad last
     * channel cannot leave a partially replaced image. A separate admission
     * pass needs no temporary image and does not inspect unused input tails. */
/* Finite/range admission now has one read-only owner shared by image output and whole-clip preflight. The original loop is retained for review. The former implementation is retained for owner review. */
#if 0
    for (size_t i = 0U; i < span.value_count; ++i) {
        const double value = values[i];
        if (!isfinite(value) || (quantization == UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE &&
            (value < low || value > 1.0)))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
#endif
    const UmiStatus valueStatus = UmiAiTrainingImageValidateValues(values,
        span.value_count, range, quantization);
    if (valueStatus != UMI_STATUS_OK) return valueStatus;
    const size_t pixels = span.value_count / 3U;
    for (size_t y = 0U; y < height; ++y) {
        uint8_t *row = outRgb + y * rowStrideBytes;
        for (size_t x = 0U; x < width; ++x) {
            const size_t pixel = y * width + x;
            for (size_t channel = 0U; channel < 3U; ++channel) {
                const size_t index = layout == UMI_AI_TRAINING_IMAGE_HWC ?
                    pixel * 3U + channel : channel * pixels + pixel;
                double value = values[index];
                if (value < low) value = low;
                if (value > 1.0) value = 1.0;
                const double unit = range == UMI_AI_TRAINING_IMAGE_UNIT ?
                    value : (value + 1.0) / 2.0;
                /* Admission and clamping bound this conversion to 0..255.
                 * floor states the tie rule explicitly before the byte cast. */
                row[x * 3U + channel] = (uint8_t)floor(unit * 255.0 + 0.5);
            }
        }
    }
    return UMI_STATUS_OK;
}
