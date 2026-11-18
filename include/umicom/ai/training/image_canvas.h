/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/image_canvas.h
 *
 * PURPOSE:
 *   Define bounded source-preserving RGB canvas placement and explicit border ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_TRAINING_IMAGE_CANVAS_H
#define UMICOM_AI_TRAINING_IMAGE_CANVAS_H
#include "umicom/ai/training/image_view.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Place the complete source at (left,top) on a packed RGB8 canvas. Coordinates
 * start at its top-left. No source pixel is cropped, resized or alpha blended.
 * Every pixel outside the source rectangle receives fillRgb[0..2].
 *
 * Both source and canvas must satisfy ImageDescribeRgb8, including MAX_VALUES.
 * row padding in the source is skipped; final-row padding is not required.
 * The complete source rectangle must fit. Dimensions are positive. Arithmetic
 * and all capacities are admitted before output changes. Failure leaves every
 * output byte unchanged. Success writes width*height*3 bytes; tail unchanged.
 *
 * source and its pixels are valid, stable caller-owned spans disjoint from
 * outRgb and its descriptor. outRgb is valid for byteCapacity bytes. fillRgb
 * points to three readable bytes: these are copied before writing, so the
 * color may be borrowed from source or output. Nothing is retained.
 * Serialize access; no allocation, decoder, color conversion, implicit aspect
 * policy, mask creation, file I/O or training occurs. Padding is real input
 * data unless the consuming model explicitly treats it otherwise.
 */
UmiStatus UmiAiTrainingImageCanvasRgb8(const UmiAiTrainingRgb8View *source,
    size_t width, size_t height, size_t left, size_t top,
    const uint8_t fillRgb[3], uint8_t *outRgb, size_t byteCapacity);
#ifdef __cplusplus
}
#endif
#endif
