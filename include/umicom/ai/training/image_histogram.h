/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/image_histogram.h
 *
 * PURPOSE:
 *   Count decoded RGB8 channel values with fixed storage and shared image-span admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_AI_TRAINING_IMAGE_HISTOGRAM_H
#define UMICOM_AI_TRAINING_IMAGE_HISTOGRAM_H
#include "umicom/ai/training/image_view.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_TRAINING_RGB8_BINS 256U

typedef struct UmiAiTrainingRgb8Histogram {
    size_t pixel_count;
    /* Channel 0 is red, 1 green, 2 blue. Each channel's bin sum equals
     * pixel_count. Counts describe unsigned code values, not linear light. */
    uint32_t bins[3][UMI_AI_TRAINING_RGB8_BINS];
} UmiAiTrainingRgb8Histogram;

/* Admit the COMPLETE positive RGB8 view using ImageDescribeRgb8 before reading
 * pixels. The existing MAX_VALUES limit bounds all counters. Row padding and
 * bytes outside a borrowed crop are ignored; final-row padding is not needed.
 *
 * On success replace the complete result with exact frequencies for this view.
 * On failure leave the result unchanged. The caller supplies stable, valid,
 * disjoint source/view/output objects and serializes access. No pointer is
 * retained. An input size is an extent contract, not proof of pointer validity.
 *
 * Uses fixed automatic storage, no allocation, file access, callbacks, decoder,
 * color conversion, model execution or dataset aggregation. Frequencies alone
 * do not preserve pixel positions, measure perceptual quality or identify a
 * training/evaluation split. Values are the original RGB8 codes in [0,255]. */
UmiStatus UmiAiTrainingImageHistogramRgb8(const UmiAiTrainingRgb8View *source,
    UmiAiTrainingRgb8Histogram *outHistogram);
#ifdef __cplusplus
}
#endif
#endif
