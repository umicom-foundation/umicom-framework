/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/video_tensor.h
 *
 * PURPOSE:
 *   Plan frame indices and pack bounded decoded video tensors through shared image admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_VIDEO_TENSOR_H
#define UMICOM_AI_TRAINING_VIDEO_TENSOR_H
#include "umicom/ai/training/image_tensor.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_TRAINING_VIDEO_MAX_FRAMES 64U
#define UMI_AI_TRAINING_VIDEO_MAX_VALUES 65536U

typedef struct UmiAiTrainingVideoFrame {
    const uint8_t *rgb;
    size_t rgb_bytes;
    size_t row_stride_bytes;
} UmiAiTrainingVideoFrame;

/* Select count indices: first, first+step, ... from [0,totalFrames).
 * count is 1..MAX_FRAMES; step is positive. Insufficient source frames or
 * destination capacity returns CAPACITY_EXCEEDED without writing anything.
 * No endpoint clamping, wrapping, repeated frames or frame-rate inference.
 * Success writes exactly count indices and leaves the output tail unchanged. */
UmiStatus UmiAiTrainingVideoSelectFrames(size_t totalFrames, size_t first,
    size_t step, size_t count, size_t *outIndices, size_t indexCapacity);

/* The caller supplies count already decoded RGB8 frames in temporal order.
 * All frames have the same positive width and height; row padding may differ.
 * ImagePackRgb8 owns normalization and each frame's HWC or CHW layout.
 * Frames remain consecutive: HWC produces [time,height,width,channel];
 * CHW produces [time,channel,height,width]. No batch dimension is added.
 * [channel,time,height,width] requires a different explicit adapter.
 *
 * count <= MAX_FRAMES; each image obeys IMAGE_MAX_VALUES; the complete output
 * has count*width*height*3 <= VIDEO_MAX_VALUES FLOATS. Capacity is in floats.
 * All frame metadata/spans are admitted before the first output write. On
 * failure the complete output stays unchanged, including unused capacity.
 * Success writes exactly the required values and leaves its tail unchanged.
 *
 * Output must not overlap frame descriptors or any pixel span. Read-only
 * frames may share pixel storage. Keep metadata and bytes stable and serialize
 * access for the entire call. Nothing is
 * retained. No allocation, decoding, resizing, color conversion, timestamp
 * resampling, training, audio or model execution occurs. Use a decoder and
 * an explicit timing policy before supplying these frames. */
UmiStatus UmiAiTrainingVideoPackRgb8(const UmiAiTrainingVideoFrame *frames,
    size_t count, size_t width, size_t height, UmiAiTrainingImageLayout layout,
    UmiAiTrainingImageRange range, float *outValues, size_t valueCapacity);
/* Each writable frame has its own borrowed capacity and row stride. Only
 * pixel bytes are written; padding, final-row padding and capacity tails remain
 * untouched. This descriptor owns no memory and is never retained. */
typedef struct UmiAiTrainingVideoRgb8Output {
    uint8_t *rgb;
    size_t rgb_capacity_bytes;
    size_t row_stride_bytes;
} UmiAiTrainingVideoRgb8Output;

/* Convert consecutive model-output frames to caller-owned RGB8 destinations.
 * count is 1..MAX_FRAMES, each image obeys IMAGE_MAX_VALUES, and the clip has
 * count*width*height*3 <= VIDEO_MAX_VALUES float values. valueCount is a float
 * capacity; extra input values are not read. HWC means THWC, CHW means TCHW;
 * this is not CTHW. Range, finite-value rules, clipping and rounding come from
 * ImageUnpackRgb8. No inverse standardization or latent decoding is inferred.
 *
 * Admit every descriptor, output capacity and required input value before
 * writing any pixel. A refusal preserves EVERY output frame. Success writes
 * pixels only, leaving row padding and all destination tails untouched.
 * Input floats, descriptors and writable destinations must be aligned as
 * required by their types, stable and pairwise disjoint; destinations must
 * not overlap each other. The caller supplies accessible backing storage and
 * serializes access. No alias detection or arbitrary-address validation occurs.
 *
 * No tensor-sized scratch allocation, file/video encoder, frame-rate policy,
 * audio, watermark, model execution or training occurs. Production media
 * output still needs its separate approved branding and encoding stages. */
UmiStatus UmiAiTrainingVideoUnpackRgb8(const float *values, size_t valueCount,
    size_t count, size_t width, size_t height, UmiAiTrainingImageLayout layout,
    UmiAiTrainingImageRange range, UmiAiTrainingImageQuantization quantization,
    const UmiAiTrainingVideoRgb8Output *frames);

#ifdef __cplusplus
}
#endif
#endif
