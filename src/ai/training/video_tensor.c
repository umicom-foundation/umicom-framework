/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/video_tensor.c
 *
 * PURPOSE:
 *   Validate every decoded frame before packing temporal tensors without allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/video_tensor.h"

UmiStatus UmiAiTrainingVideoSelectFrames(size_t totalFrames, size_t first,
    size_t step, size_t count, size_t *outIndices, size_t indexCapacity)
{
    if (outIndices == NULL || totalFrames == 0U || first >= totalFrames ||
        step == 0U || count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_AI_TRAINING_VIDEO_MAX_FRAMES || indexCapacity < count ||
        count - 1U > (totalFrames - 1U - first) / step)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* The endpoint proof above also proves every intermediate index fits
     * size_t. Do not multiply the unchecked requested count by its step. */
    for (size_t i = 0U; i < count; ++i) outIndices[i] = first + i * step;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingVideoPackRgb8(const UmiAiTrainingVideoFrame *frames,
    size_t count, size_t width, size_t height, UmiAiTrainingImageLayout layout,
    UmiAiTrainingImageRange range, float *outValues, size_t valueCapacity)
{
    if (frames == NULL || outValues == NULL || count == 0U ||
        (layout != UMI_AI_TRAINING_IMAGE_HWC && layout != UMI_AI_TRAINING_IMAGE_CHW) ||
        (range != UMI_AI_TRAINING_IMAGE_UNIT && range != UMI_AI_TRAINING_IMAGE_SIGNED_UNIT))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_AI_TRAINING_VIDEO_MAX_FRAMES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t frameValues = 0U;
    /* Two passes keep a bad final frame from publishing a partial tensor.
     * Shared image admission checks stride arithmetic without reading pixels;
     * no second image or tensor-sized scratch buffer is allocated. */
    for (size_t i = 0U; i < count; ++i) {
        if (frames[i].rgb == NULL) return UMI_STATUS_INVALID_ARGUMENT;
        UmiAiTrainingImageSpan span;
        UmiStatus status = UmiAiTrainingImageDescribeRgb8(width, height,
            frames[i].row_stride_bytes, &span);
        if (status != UMI_STATUS_OK) return status;
        if (frames[i].rgb_bytes < span.required_bytes)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        frameValues = span.value_count;
    }
    if (frameValues > UMI_AI_TRAINING_VIDEO_MAX_VALUES / count ||
        valueCapacity < frameValues * count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i) {
        /* With stable borrowed inputs, all possible image refusals have been
         * excluded above. The same service remains the normalization owner. */
        (void)UmiAiTrainingImagePackRgb8(frames[i].rgb, frames[i].rgb_bytes,
            width, height, frames[i].row_stride_bytes, layout, range,
            outValues + i * frameValues, frameValues);
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingVideoUnpackRgb8(const float *values, size_t valueCount,
    size_t count, size_t width, size_t height, UmiAiTrainingImageLayout layout,
    UmiAiTrainingImageRange range, UmiAiTrainingImageQuantization quantization,
    const UmiAiTrainingVideoRgb8Output *frames)
{
    if (values == NULL || frames == NULL || count == 0U ||
        (layout != UMI_AI_TRAINING_IMAGE_HWC && layout != UMI_AI_TRAINING_IMAGE_CHW) ||
        (range != UMI_AI_TRAINING_IMAGE_UNIT && range != UMI_AI_TRAINING_IMAGE_SIGNED_UNIT) ||
        (quantization != UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE &&
         quantization != UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_AI_TRAINING_VIDEO_MAX_FRAMES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t frameValues = 0U;
    /* Complete metadata admission precedes any float read or pixel write.
     * The shared image planner proves every stride and dimension product. */
    for (size_t i = 0U; i < count; ++i) {
        if (frames[i].rgb == NULL) return UMI_STATUS_INVALID_ARGUMENT;
        UmiAiTrainingImageSpan span;
        const UmiStatus status = UmiAiTrainingImageDescribeRgb8(width, height,
            frames[i].row_stride_bytes, &span);
        if (status != UMI_STATUS_OK) return status;
        if (frames[i].rgb_capacity_bytes < span.required_bytes)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        frameValues = span.value_count;
    }
    if (frameValues > UMI_AI_TRAINING_VIDEO_MAX_VALUES / count ||
        valueCount < frameValues * count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A bad final channel must not replace earlier output frames. Admit all
     * prefixes with the same service the image converter uses. No scratch clip
     * is needed: stable disjoint caller storage keeps this proof valid. */
    for (size_t i = 0U; i < count; ++i) {
        const UmiStatus status = UmiAiTrainingImageValidateValues(
            values + i * frameValues, frameValues, range, quantization);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t i = 0U; i < count; ++i) {
        /* Under the public lifetime/alias contract every possible refusal was
         * excluded above. Image conversion remains the sole quantization owner. */
        (void)UmiAiTrainingImageUnpackRgb8(values + i * frameValues, frameValues,
            width, height, frames[i].row_stride_bytes, layout, range,
            quantization, frames[i].rgb, frames[i].rgb_capacity_bytes);
    }
    return UMI_STATUS_OK;
}
