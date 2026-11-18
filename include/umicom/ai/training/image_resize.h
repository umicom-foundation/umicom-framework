/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/image_resize.h
 *
 * PURPOSE:
 *   Declare bounded center-mapped RGB resampling with explicit caller-owned storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_IMAGE_RESIZE_H
#define UMICOM_AI_TRAINING_IMAGE_RESIZE_H
#include "umicom/ai/training/image_view.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Resample an already decoded RGB8 view into packed RGB8 output.
 * For destination coordinate d, select floor((2*d+1)*sourceSize/(2*targetSize)).
 * This maps pixel centers and resolves an exact nearest-center tie toward the
 * higher source coordinate. Copy all three channels from that selected pixel.
 * Both source and target obey ImageDescribeRgb8's IMAGE_MAX_VALUES bound.
 *
 * All metadata and capacities are admitted before the first output write.
 * Success writes width*height*3 bytes and leaves the remaining capacity alone;
 * every refusal leaves the entire destination unchanged. Source row padding
 * and any pixels outside a borrowed crop are not interpreted as image pixels.
 *
 * The caller provides valid stable source storage and writable output storage.
 * Source descriptor, readable source span and output span must be disjoint.
 * No in-place or partial-overlap operation is supported. No pointer is retained.
 * No allocation, decoding, antialias filtering, color conversion, letterboxing,
 * random augmentation, label adjustment, tensor normalization or training.
 * Nearest sampling can discard fine detail and is not a universal model policy.
 */
UmiStatus UmiAiTrainingImageResizeNearestRgb8(const UmiAiTrainingRgb8View *source,
    size_t width, size_t height, uint8_t *outRgb, size_t byteCapacity);

/* Bilinear alternative with the same storage, bounds and transactional refusal
 * contract as the nearest function above. Map each destination center to
 * (d + 0.5)*sourceSize/targetSize - 0.5 and clamp it to the source border.
 * Interpolate the four neighboring RGB bytes with exact rational weights and
 * round each final channel once to the nearest byte, resolving half ties up.
 * This acts on encoded RGB byte values; it is not gamma-correct interpolation.
 * It neither expands the filter footprint when shrinking nor performs area
 * antialiasing. Fine detail may alias; categorical masks must use an explicit
 * label-preserving policy instead. No interpolation policy is inferred from a
 * model name. The caller chooses the policy required by the model or workflow.
 */
UmiStatus UmiAiTrainingImageResizeBilinearRgb8(const UmiAiTrainingRgb8View *source,
    size_t width, size_t height, uint8_t *outRgb, size_t byteCapacity);

#ifdef __cplusplus
}
#endif
#endif
