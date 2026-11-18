/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/image_view.h
 *
 * PURPOSE:
 *   Describe checked borrowed RGB crops without copying or owning pixel storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_IMAGE_VIEW_H
#define UMICOM_AI_TRAINING_IMAGE_VIEW_H
#include "umicom/ai/training/image_tensor.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingRgb8View {
    const uint8_t *pixels;
    size_t bytes;
    size_t width;
    size_t height;
    size_t row_stride_bytes;
} UmiAiTrainingRgb8View;

/* Admit the COMPLETE source with ImageDescribeRgb8, including its MAX_VALUES
 * bound, then select a positive rectangle inside it. left/top are zero-based
 * pixel coordinates; width/height are counts, not inclusive end coordinates.
 * No clipping, padding, wraparound or coordinate normalization is performed.
 *
 * Success publishes a borrowed view starting at the selected top-left pixel,
 * retaining the original row stride. bytes is the minimal readable span through
 * the last selected row, not the count of packed pixels; final padding is not
 * included. Inter-row gaps can contain pixels outside the crop. Consumers must
 * respect width/height/stride rather than serializing all bytes as packed RGB.
 * This descriptor carries no ownership, reference count or lifetime extension.
 * The original allocation must remain live and stable while any view is used.
 *
 * outView may be source itself. Otherwise descriptor storage must be disjoint;
 * neither descriptor may overlap pixel storage. All declared source bytes must
 * be backed by one readable allocation. Admission checks metadata, not pointer
 * validity. Every failure preserves outView; no pixels are read or changed.
 * No allocation, copy, decoder, resize, file access, random crop or training.
 */
UmiStatus UmiAiTrainingImageCropRgb8(const UmiAiTrainingRgb8View *source,
    size_t left, size_t top, size_t width, size_t height,
    UmiAiTrainingRgb8View *outView);
#ifdef __cplusplus
}
#endif
#endif
