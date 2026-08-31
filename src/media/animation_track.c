/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media/animation_track.c
 *
 * PURPOSE:
 *   Implement ordered keyframe editing and deterministic step or linear
 *   sampling without renderer, clock or application dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/media/animation_track.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* The track owns a bounded sorted keyframe array. */
struct UmiMediaAnimationTrack {
    UmiMediaAnimationKeyframe *keyframes;
    size_t capacity;
    size_t count;
    uint64_t revision;
};

/* Validate values before they enter a sorted animation record. */
static bool valid_keyframe(const UmiMediaAnimationKeyframe *keyframe)
{
    return keyframe != NULL && isfinite(keyframe->time_seconds) &&
        isfinite(keyframe->value) && keyframe->time_seconds >= 0.0 &&
        keyframe->interpolation >= UMI_MEDIA_ANIMATION_STEP &&
        keyframe->interpolation <= UMI_MEDIA_ANIMATION_LINEAR;
}

/* Locate the first keyframe whose timestamp is not less than the requested one. */
static size_t lower_bound(
    const UmiMediaAnimationTrack *track,
    double time_seconds)
{
    size_t index = 0U;
    /* Sorted insertion keeps this simple scan deterministic and easy to audit. */
    while (index < track->count &&
           track->keyframes[index].time_seconds < time_seconds) {
        index += 1U;
    }
    return index;
}

/* Allocate checked fixed-capacity keyframe storage. */
UmiStatus umi_media_animation_track_create(
    size_t keyframe_capacity,
    UmiMediaAnimationTrack **out_track)
{
    UmiMediaAnimationTrack *track;
    if (out_track == NULL || keyframe_capacity == 0U ||
        keyframe_capacity > SIZE_MAX / sizeof(UmiMediaAnimationKeyframe)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_track = NULL;
    track = (UmiMediaAnimationTrack *)calloc(1U, sizeof(*track));
    if (track == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    track->keyframes = (UmiMediaAnimationKeyframe *)calloc(
        keyframe_capacity, sizeof(*track->keyframes));
    /* Release the owner when child storage allocation fails. */
    if (track->keyframes == NULL) {
        free(track);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    track->capacity = keyframe_capacity;
    track->revision = 1U;
    *out_track = track;
    return UMI_STATUS_OK;
}

/* Release keyframes and owner; NULL destruction is intentionally safe. */
void umi_media_animation_track_destroy(UmiMediaAnimationTrack *track)
{
    if (track == NULL) return;
    free(track->keyframes);
    free(track);
}

/* Insert a unique-time keyframe and move the sorted tail safely with memmove. */
UmiStatus umi_media_animation_track_add(
    UmiMediaAnimationTrack *track,
    const UmiMediaAnimationKeyframe *keyframe)
{
    size_t index;
    if (track == NULL || !valid_keyframe(keyframe)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (track->count >= track->capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    index = lower_bound(track, keyframe->time_seconds);
    /* Exact duplicate timestamps are updated explicitly, never overwritten here. */
    if (index < track->count &&
        track->keyframes[index].time_seconds == keyframe->time_seconds) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (index < track->count) {
        /* memmove supports overlapping source and destination ranges safely. */
        (void)memmove(&track->keyframes[index + 1U], &track->keyframes[index],
            (track->count - index) * sizeof(*track->keyframes));
    }
    track->keyframes[index] = *keyframe;
    track->count += 1U;
    track->revision += 1U;
    return UMI_STATUS_OK;
}

/* Replace an exact timestamp while retaining the track's sorted structure. */
UmiStatus umi_media_animation_track_update(
    UmiMediaAnimationTrack *track,
    const UmiMediaAnimationKeyframe *keyframe)
{
    size_t index;
    if (track == NULL || !valid_keyframe(keyframe)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = lower_bound(track, keyframe->time_seconds);
    if (index >= track->count ||
        track->keyframes[index].time_seconds != keyframe->time_seconds) {
        return UMI_STATUS_NOT_FOUND;
    }
    track->keyframes[index] = *keyframe;
    track->revision += 1U;
    return UMI_STATUS_OK;
}

/* Remove one exact timestamp and close the gap with overlap-safe memmove. */
UmiStatus umi_media_animation_track_remove(
    UmiMediaAnimationTrack *track,
    double time_seconds)
{
    size_t index;
    if (track == NULL || !isfinite(time_seconds) || time_seconds < 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = lower_bound(track, time_seconds);
    if (index >= track->count ||
        track->keyframes[index].time_seconds != time_seconds) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (index + 1U < track->count) {
        (void)memmove(&track->keyframes[index], &track->keyframes[index + 1U],
            (track->count - index - 1U) * sizeof(*track->keyframes));
    }
    track->count -= 1U;
    (void)memset(&track->keyframes[track->count], 0,
                 sizeof(*track->keyframes));
    track->revision += 1U;
    return UMI_STATUS_OK;
}

/* Sample a clamped deterministic value from ordered keyframes. */
UmiStatus umi_media_animation_track_sample(
    const UmiMediaAnimationTrack *track,
    double time_seconds,
    double *out_value)
{
    size_t upper;
    const UmiMediaAnimationKeyframe *left;
    const UmiMediaAnimationKeyframe *right;
    double progress;
    if (track == NULL || out_value == NULL || !isfinite(time_seconds)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (track->count == 0U) return UMI_STATUS_INVALID_STATE;
    /* Sampling before or after the track clamps to the nearest endpoint. */
    if (time_seconds <= track->keyframes[0].time_seconds) {
        *out_value = track->keyframes[0].value;
        return UMI_STATUS_OK;
    }
    if (time_seconds >= track->keyframes[track->count - 1U].time_seconds) {
        *out_value = track->keyframes[track->count - 1U].value;
        return UMI_STATUS_OK;
    }
    upper = lower_bound(track, time_seconds);
    left = &track->keyframes[upper - 1U];
    right = &track->keyframes[upper];
    /* Step interpolation holds the left value until the next exact keyframe. */
    if (left->interpolation == UMI_MEDIA_ANIMATION_STEP) {
        *out_value = left->value;
        return UMI_STATUS_OK;
    }
    progress = (time_seconds - left->time_seconds) /
        (right->time_seconds - left->time_seconds);
    *out_value = left->value + (right->value - left->value) * progress;
    return UMI_STATUS_OK;
}

/* Copy one keyframe so no caller retains a pointer into mutable track storage. */
UmiStatus umi_media_animation_track_at(
    const UmiMediaAnimationTrack *track,
    size_t index,
    UmiMediaAnimationKeyframe *out_keyframe)
{
    if (track == NULL || out_keyframe == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= track->count) return UMI_STATUS_NOT_FOUND;
    *out_keyframe = track->keyframes[index];
    return UMI_STATUS_OK;
}

/* Copy count, range and revision for animation editors and render schedulers. */
UmiStatus umi_media_animation_track_snapshot(
    const UmiMediaAnimationTrack *track,
    UmiMediaAnimationTrackSnapshot *out_snapshot)
{
    if (track == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->keyframe_count = track->count;
    out_snapshot->keyframe_capacity = track->capacity;
    out_snapshot->revision = track->revision;
    /* Empty tracks retain zero start/end values from the cleared snapshot. */
    if (track->count > 0U) {
        out_snapshot->start_time_seconds = track->keyframes[0].time_seconds;
        out_snapshot->end_time_seconds =
            track->keyframes[track->count - 1U].time_seconds;
    }
    return UMI_STATUS_OK;
}
