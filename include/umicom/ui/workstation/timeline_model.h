/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/timeline_model.h
 *
 * PURPOSE:
 *   Define generic timeline tracks and clips reusable by video editors, music studios, animation and event-replay workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_TIMELINE_MODEL_H
#define UMICOM_UI_WORKSTATION_TIMELINE_MODEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiWsTrackKind {
    UMI_WS_TRACK_VIDEO = 1,
    UMI_WS_TRACK_AUDIO = 2,
    UMI_WS_TRACK_MIDI = 3,
    UMI_WS_TRACK_AUTOMATION = 4,
    UMI_WS_TRACK_ANNOTATION = 5
} UmiWsTrackKind;

typedef struct UmiWsTimelineTrack {
    char track_id[UMI_UI_ID_CAPACITY];
    char label[UMI_UI_TEXT_CAPACITY];
    UmiWsTrackKind kind;
    bool muted;
    bool locked;
} UmiWsTimelineTrack;

typedef struct UmiWsTimelineClip {
    char clip_id[UMI_UI_ID_CAPACITY];
    char track_id[UMI_UI_ID_CAPACITY];
    int64_t start_ms;
    int64_t duration_ms;
} UmiWsTimelineClip;

typedef struct UmiWsTimelineModel {
    UmiWsTimelineTrack tracks[UMI_WS_MAX_TRACKS];
    UmiWsTimelineClip clips[UMI_WS_MAX_CLIPS];
    size_t track_count;
    size_t clip_count;
    int64_t playhead_ms;
} UmiWsTimelineModel;

void umi_ws_timeline_model_init(UmiWsTimelineModel *timeline);
UmiStatus umi_ws_timeline_model_add_track(UmiWsTimelineModel *timeline,
                                          const char *track_id,
                                          const char *label,
                                          UmiWsTrackKind kind);
UmiStatus umi_ws_timeline_model_add_clip(UmiWsTimelineModel *timeline,
                                         const char *clip_id,
                                         const char *track_id,
                                         int64_t start_ms,
                                         int64_t duration_ms);
UmiStatus umi_ws_timeline_model_seek(UmiWsTimelineModel *timeline, int64_t playhead_ms);

#ifdef __cplusplus
}
#endif

#endif
