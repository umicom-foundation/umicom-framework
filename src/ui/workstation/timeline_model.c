/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/timeline_model.c
 *
 * PURPOSE:
 *   Implement generic timeline tracks and clips reusable by video editors, music studios, animation and event-replay workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <string.h>
#include "umicom/ui/workstation/timeline_model.h"

/*
 * Initialise ws timeline model from caller-provided values so later operations receive a
 * known state.
 */
void umi_ws_timeline_model_init(UmiWsTimelineModel *timeline) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timeline != NULL) *timeline = (UmiWsTimelineModel){0};
}

/*
 * Provide the ws timeline model add track operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_timeline_model_add_track(UmiWsTimelineModel *timeline,
                                          const char *track_id,
                                          const char *label,
                                          UmiWsTrackKind kind) {
    UmiWsTimelineTrack *track;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timeline == NULL || !umi_ws_id_valid(track_id) || label == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (kind < UMI_WS_TRACK_VIDEO || kind > UMI_WS_TRACK_ANNOTATION) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (timeline->track_count >= UMI_WS_MAX_TRACKS) return UMI_STATUS_CAPACITY_EXCEEDED;
    track = &timeline->tracks[timeline->track_count++];
    *track = (UmiWsTimelineTrack){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(track->track_id, sizeof(track->track_id), track_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(track->label, sizeof(track->label), label) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    track->kind = kind;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws timeline model add clip operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_timeline_model_add_clip(UmiWsTimelineModel *timeline,
                                         const char *clip_id,
                                         const char *track_id,
                                         int64_t start_ms,
                                         int64_t duration_ms) {
    UmiWsTimelineClip *clip;
    size_t index;
    bool track_found = false;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timeline == NULL || !umi_ws_id_valid(clip_id) || !umi_ws_id_valid(track_id) || start_ms < 0 || duration_ms <= 0) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < timeline->track_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(timeline->tracks[index].track_id, track_id) == 0) track_found = true;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!track_found) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (timeline->clip_count >= UMI_WS_MAX_CLIPS) return UMI_STATUS_CAPACITY_EXCEEDED;
    clip = &timeline->clips[timeline->clip_count++];
    *clip = (UmiWsTimelineClip){0};
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(clip->clip_id, sizeof(clip->clip_id), clip_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_copy_text(clip->track_id, sizeof(clip->track_id), track_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    clip->start_ms = start_ms;
    clip->duration_ms = duration_ms;
    return UMI_STATUS_OK;
}

/*
 * Provide the ws timeline model seek operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_timeline_model_seek(UmiWsTimelineModel *timeline, int64_t playhead_ms) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timeline == NULL || playhead_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    timeline->playhead_ms = playhead_ms;
    return UMI_STATUS_OK;
}
