/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/image_flip.h
 *
 * PURPOSE:
 *   Copy checked RGB views into explicit horizontal or vertical mirrored packed storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_IMAGE_FLIP_H
#define UMICOM_AI_TRAINING_IMAGE_FLIP_H
#include "umicom/ai/training/image_view.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_TRAINING_FLIP_HORIZONTAL 1U
#define UMI_AI_TRAINING_FLIP_VERTICAL 2U

/* Admit a positive RGB8 view with the shared ImageDescribeRgb8 rules, then
 * copy exactly width*height*3 bytes to tightly packed RGB8 output. flags is
 * zero (identity), HORIZONTAL, VERTICAL, or their bitwise OR. Unknown bits
 * are INVALID_ARGUMENT. Horizontal reverses pixels within each row; vertical
 * reverses row order. RGB channel order inside each pixel never changes.
 * Both flags equal a 180-degree orientation change, not a 90-degree rotation.
 *
 * Input row padding and pixels outside a borrowed crop are skipped. Output
 * row stride is width*3. byteCapacity is BYTES, not floats. All metadata,
 * spans and capacity are admitted before the first pixel read or output write.
 * Every failure preserves the complete output. Success leaves its unused tail
 * unchanged. No output descriptor or owned allocation is returned.
 *
 * Source descriptor, its declared readable pixel span and writable output
 * span must be valid, stable and disjoint; in-place flips are not supported.
 * The caller owns lifetimes and serializes access. Nothing is retained and
 * no allocation or I/O occurs. Shape checks cannot establish pointer validity.
 *
 * The caller selects the transform and must keep captions, boxes, masks and
 * related video frames consistent. This function supplies no RNG, label policy,
 * decoder, resize, color conversion, normalization, tensor layout or trainer.
 */
UmiStatus UmiAiTrainingImageFlipRgb8(const UmiAiTrainingRgb8View *source,
    unsigned flags, uint8_t *outRgb, size_t byteCapacity);
#ifdef __cplusplus
}
#endif
#endif
