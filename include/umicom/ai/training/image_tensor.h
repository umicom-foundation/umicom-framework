/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/image_tensor.h
 *
 * PURPOSE:
 *   Pack decoded RGB bytes into explicitly selected bounded float layouts without allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_IMAGE_TENSOR_H
#define UMICOM_AI_TRAINING_IMAGE_TENSOR_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_TRAINING_IMAGE_MAX_VALUES 16384U

typedef enum UmiAiTrainingImageLayout {
    UMI_AI_TRAINING_IMAGE_HWC = 0, /* Height, width, channel: RGB RGB ... */
    UMI_AI_TRAINING_IMAGE_CHW = 1  /* Channel, height, width: R plane, G, B. */
} UmiAiTrainingImageLayout;
typedef enum UmiAiTrainingImageRange {
    UMI_AI_TRAINING_IMAGE_UNIT = 0,        /* byte / 255, in [0,1]. */
    UMI_AI_TRAINING_IMAGE_SIGNED_UNIT = 1  /* 2*(byte / 255)-1, in [-1,1]. */
} UmiAiTrainingImageRange;

/* The span describes storage, not an allocation or ownership transfer. */
typedef struct UmiAiTrainingImageSpan {
    size_t required_bytes; /* Includes inter-row padding, excludes final padding. */
    size_t value_count;    /* width*height*3 FLOAT elements. */
} UmiAiTrainingImageSpan;

/* Check positive dimensions, row stride, multiplication/addition overflow and
 * MAX_VALUES without reading pixels. On success publish the two required sizes;
 * on failure leave outSpan unchanged. This does not verify any backing buffer.
 * Planning before allocation and packing share exactly the same shape rules. */
UmiStatus UmiAiTrainingImageDescribeRgb8(size_t width, size_t height,
    size_t rowStrideBytes, UmiAiTrainingImageSpan *outSpan);

/* Input is already decoded RGB8, exactly three unsigned bytes per pixel,
 * red then green then blue. Rows run top to bottom, pixels left to right.
 * rowStrideBytes >= width*3; inter-row padding is skipped. rgbBytes must
 * cover (height-1)*rowStrideBytes + width*3; final-row padding is not needed.
 * Dimensions are positive and width*height*3 <= MAX_VALUES. All arithmetic
 * is checked before accessing pixels. Output capacity is FLOATS, not bytes.
 *
 * HWC output index = (y*width+x)*3+c. CHW = c*(width*height)+y*width+x.
 * No batch dimension is added. Choose layout and range from the actual
 * model contract, not a filename. Values are computed using double
 * intermediates then stored as float. This is code-value rescaling, not
 * sRGB linearization, standardization, latent scaling or color conversion.
 *
 * Inputs and output are caller-owned, disjoint spans valid for their declared
 * sizes. Keep them stable and serialize access throughout the call. Nothing
 * is retained. All output, including unused capacity, is unchanged on failure;
 * success writes exactly width*height*3 floats and leaves the tail unchanged.
 * No allocation, decoder, alpha compositing, resize, file I/O, RNG, model
 * execution or training occurs. An RGBA/BGR image needs an explicit adapter.
 */
UmiStatus UmiAiTrainingImagePackRgb8(const uint8_t *rgb, size_t rgbBytes,
    size_t width, size_t height, size_t rowStrideBytes,
    UmiAiTrainingImageLayout layout, UmiAiTrainingImageRange range,
    float *outValues, size_t valueCapacity);

/* Explicit preprocessing parameters in unit-code-value coordinates. A model
 * may call scale its standard deviation, but this service does not fit it or
 * infer it from an image, dataset, filename or model. RGB order is fixed even
 * when the output layout changes. Values are copied nowhere persistently. */
typedef struct UmiAiTrainingImageStandardization {
    float mean[3];
    float scale[3];
} UmiAiTrainingImageStandardization;

/* Reuse canonical RGB8 UNIT packing, then transform each resulting FLOAT u as
 * (double(u) - double(mean[channel])) / double(scale[channel]), rounded once
 * to float. This deliberate unit-float intermediate can differ from a fused
 * byte-to-standardized calculation. No second image or scratch tensor exists.
 *
 * Means and scales must be finite; scales must be strictly positive. Both
 * unit endpoints 0 and 1 must transform into finite values within +/-FLT_MAX
 * for all channels, even if those endpoints are absent from this image.
 * Invalid parameters return INVALID_ARGUMENT before any output write. All
 * other shape/span/layout statuses and output atomicity follow PackRgb8.
 * Parameters must also be stable, aligned and disjoint from input/output for
 * the whole call. The caller supplies actual accessible storage and serializes
 * access; no alias detection, locking or arbitrary-pointer validation occurs.
 *
 * No clipping, epsilon insertion, zero-scale replacement, dataset fitting,
 * color-space conversion, channel reorder, model execution or training occurs.
 * Use the actual model preprocessing specification; statistics alone do not
 * decide a valid normalization policy. */
UmiStatus UmiAiTrainingImagePackRgb8Standardized(const uint8_t *rgb, size_t rgbBytes,
    size_t width, size_t height, size_t rowStrideBytes,
    UmiAiTrainingImageLayout layout,
    const UmiAiTrainingImageStandardization *parameters,
    float *outValues, size_t valueCapacity);

/* Output conversion policy is separate from the model's stated value range.
 * Strict mode rejects finite out-of-range values; clipping is an explicit
 * display choice that changes data. Neither mode accepts NaN or infinity. */
typedef enum UmiAiTrainingImageQuantization {
    UMI_AI_TRAINING_IMAGE_REJECT_OUTSIDE = 0,
    UMI_AI_TRAINING_IMAGE_CLIP_OUTSIDE = 1
} UmiAiTrainingImageQuantization;

/* Read-only admission of an explicit prefix of float code values. count is
 * 1..IMAGE_MAX_VALUES, not a capacity or a byte count. The accessible input
 * must contain at least count aligned floats and stay stable during the call.
 * UNIT admits [0,1]; SIGNED_UNIT admits [-1,1]. Both policies reject nonfinite
 * values; CLIP_OUTSIDE accepts finite values outside the range but does not
 * mutate or clamp them. This helper does not inspect layout, shape or pixels.
 * It lets multi-frame clients prove every value before publishing any frame.
 * No output, allocation, retained pointer or I/O occurs. */
UmiStatus UmiAiTrainingImageValidateValues(const float *values, size_t count,
    UmiAiTrainingImageRange range, UmiAiTrainingImageQuantization quantization);

/* Convert a decoded model-output tensor to interleaved RGB8. Reuse the same
 * dimensions, layouts, positive strides and MAX_VALUES bound as DescribeRgb8.
 * valueCount is a number of FLOATS; rgbCapacityBytes is a number of BYTES.
 * Both spans may be larger than the image. Only width*height*3 input values
 * are read; inter-row padding and all output bytes outside pixels stay intact.
 *
 * UNIT means [0,1]; SIGNED_UNIT means [-1,1]. First admit every input value.
 * Reject NaN/infinity in both modes, and reject outside values in strict mode,
 * returning INVALID_ARGUMENT without writing any output byte. Clipping mode
 * clamps finite values to the stated endpoints before the range transform.
 * Convert signed values to unit coordinates with (double(value)+1)/2, then
 * round unit*255 to nearest integer, with exact halfway cases going UP.
 * This explicit rule does not use a language's default float-to-int cast.
 *
 * It is an output code-value conversion, not an inverse standardizer, latent
 * decoder, sRGB transfer function, BGR/RGBA conversion, image file encoder or
 * model invocation. A successful call does not establish visual quality.
 * Inputs and output are stable, aligned as required by their types, disjoint
 * caller-owned spans. No alias detection, allocation, I/O or retained pointers.
 * Input-length, output-capacity and shape failures preserve the full output.
 */
UmiStatus UmiAiTrainingImageUnpackRgb8(const float *values, size_t valueCount,
    size_t width, size_t height, size_t rowStrideBytes,
    UmiAiTrainingImageLayout layout, UmiAiTrainingImageRange range,
    UmiAiTrainingImageQuantization quantization,
    uint8_t *outRgb, size_t rgbCapacityBytes);

#ifdef __cplusplus
}
#endif
#endif
