/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/image_moments.h
 *
 * PURPOSE:
 *   Describe exact RGB8 channel moments derived from bounded histogram ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_IMAGE_MOMENTS_H
#define UMICOM_AI_TRAINING_IMAGE_MOMENTS_H
#include "umicom/ai/training/image_histogram.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAiTrainingRgb8Moments {
    size_t pixel_count;
    double mean[3];
    double population_variance[3]; /* Mean squared deviation; denominator N. */
    uint8_t minimum[3];
    uint8_t maximum[3];
} UmiAiTrainingRgb8Moments;

/* Validate a positive histogram within IMAGE_MAX_VALUES/3 pixels, including
 * every channel's bin sum, before publishing moments. Bins are exact RGB code
 * values: mean/min/max use [0,255], variance uses squared code units. Channels
 * are red, green, blue. Variance is population variance (divide by N), not a
 * sample estimate (N-1); one pixel and a constant channel have zero variance.
 *
 * Integer sums and squared sums are exact within the reviewed image profile.
 * The nonnegative variance numerator N*sum(x*x)-sum(x)*sum(x) is formed before
 * conversion to double. The result is rounded only by the floating divisions.
 * A caller-supplied malformed histogram is refused; it is not silently repaired.
 *
 * Input/output must be valid disjoint stable objects. Failure leaves the whole
 * output unchanged. No allocation, retained pointer, color linearization,
 * standardization, standard deviation, model fitting or dataset aggregation.
 * An identical histogram can describe spatially different images; these
 * statistics are not a perceptual-quality or semantic-content measurement. */
UmiStatus UmiAiTrainingImageHistogramMoments(const UmiAiTrainingRgb8Histogram *histogram,
    UmiAiTrainingRgb8Moments *outMoments);
/* Compose the existing admitted histogram reader with the same moment service.
 * Row padding and bytes outside an admitted crop do not contribute. The complete
 * view is admitted before pixel access; all existing view limits still apply.
 * Uses a fixed local histogram, not an image-sized floating-point allocation. */
UmiStatus UmiAiTrainingImageMeasureRgb8(const UmiAiTrainingRgb8View *source,
    UmiAiTrainingRgb8Moments *outMoments);
#ifdef __cplusplus
}
#endif
#endif
