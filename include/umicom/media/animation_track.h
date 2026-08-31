/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media/animation_track.h
 *
 * PURPOSE:
 *   Define a deterministic scalar animation track that can animate object,
 *   image, UI, chart, game and media properties without toolkit dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MEDIA_ANIMATION_TRACK_H
#define UMICOM_MEDIA_ANIMATION_TRACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Interpolation belongs to the outgoing segment beginning at a keyframe. */
typedef enum UmiMediaAnimationInterpolation {
    UMI_MEDIA_ANIMATION_STEP = 0,
    UMI_MEDIA_ANIMATION_LINEAR = 1
} UmiMediaAnimationInterpolation;

/* A keyframe is serialisable and contains no callback or renderer pointer. */
typedef struct UmiMediaAnimationKeyframe {
    double time_seconds;
    double value;
    UmiMediaAnimationInterpolation interpolation;
} UmiMediaAnimationKeyframe;

/* The opaque track owns keyframes in ascending time order. */
typedef struct UmiMediaAnimationTrack UmiMediaAnimationTrack;

/* Track snapshots let timelines refresh only when the revision changes. */
typedef struct UmiMediaAnimationTrackSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t keyframe_count;
    size_t keyframe_capacity;
    double start_time_seconds;
    double end_time_seconds;
    uint64_t revision;
} UmiMediaAnimationTrackSnapshot;

/* Create a bounded track so editing cannot grow memory without policy. */
UmiStatus umi_media_animation_track_create(
    size_t keyframe_capacity,
    UmiMediaAnimationTrack **out_track);

/* Destroy the track and its owned keyframes. */
void umi_media_animation_track_destroy(UmiMediaAnimationTrack *track);

/* Insert one unique-time keyframe while preserving ascending order. */
UmiStatus umi_media_animation_track_add(
    UmiMediaAnimationTrack *track,
    const UmiMediaAnimationKeyframe *keyframe);

/* Replace the keyframe at an exact timestamp without changing ordering. */
UmiStatus umi_media_animation_track_update(
    UmiMediaAnimationTrack *track,
    const UmiMediaAnimationKeyframe *keyframe);

/* Remove the keyframe at an exact timestamp and close the storage gap. */
UmiStatus umi_media_animation_track_remove(
    UmiMediaAnimationTrack *track,
    double time_seconds);

/* Sample clamped step or linear animation value at any finite timestamp. */
UmiStatus umi_media_animation_track_sample(
    const UmiMediaAnimationTrack *track,
    double time_seconds,
    double *out_value);

/* Copy one keyframe by sorted index. */
UmiStatus umi_media_animation_track_at(
    const UmiMediaAnimationTrack *track,
    size_t index,
    UmiMediaAnimationKeyframe *out_keyframe);

/* Copy timing, count and revision metadata. */
UmiStatus umi_media_animation_track_snapshot(
    const UmiMediaAnimationTrack *track,
    UmiMediaAnimationTrackSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
