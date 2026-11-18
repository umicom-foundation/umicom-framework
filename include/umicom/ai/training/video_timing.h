/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/video_timing.h
 *
 * PURPOSE:
 *   Select reviewed decoded frame indices using an explicit integer timestamp policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_AI_TRAINING_VIDEO_TIMING_H
#define UMICOM_AI_TRAINING_VIDEO_TIMING_H
#include "umicom/ai/training/video_tensor.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_AI_TRAINING_VIDEO_MAX_TIMESTAMPS 4096U

/* Select the latest source frame whose timestamp is <= each requested time.
 * Source timestamps are strictly increasing; queries are nondecreasing.
 * All uint64 values share the SAME caller-defined unit and origin. Each query
 * must lie in the closed interval [sourceTimes[0], sourceTimes[sourceCount-1]].
 * Before-first/after-last requests are refused, never clamped or extrapolated.
 * Equal/repeated query times and multiple queries between frames can return
 * the same index: this is explicit hold-previous behavior, not interpolation.
 *
 * sourceCount is 1..MAX_TIMESTAMPS; queryCount is 1..VIDEO_MAX_FRAMES.
 * The caller owns readable timestamp arrays and a disjoint output index array;
 * capacity is SIZE_T ELEMENTS, not bytes. All spans must remain stable and valid
 * throughout the call. Metadata validation cannot establish pointer validity.
 * Failure leaves ALL output unchanged, including unused capacity. Success
 * writes exactly queryCount indices and preserves the tail. All source times,
 * including unselected ones, are validated before the first output write.
 *
 * No allocation, timestamp conversion, frame-rate inference, sorting, decoding,
 * pixel copying, audio alignment or training occurs. A decoder must expose
 * reviewed presentation times before this operation. Use the returned indices
 * to construct borrowed frame descriptors for VideoPackRgb8. Hold sampling may
 * bias training; it does not establish the timing policy required by a model. */
UmiStatus UmiAiTrainingVideoSelectAtOrBefore(const uint64_t *sourceTimes,
    size_t sourceCount,const uint64_t *queryTimes,size_t queryCount,
    size_t *outIndices,size_t indexCapacity);
#ifdef __cplusplus
}
#endif
#endif
