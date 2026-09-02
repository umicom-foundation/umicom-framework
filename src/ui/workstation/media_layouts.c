/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/media_layouts.c
 *
 * PURPOSE:
 *   Implement reusable presets for Video Editor, AI Image/Video Creator, Music Studio and Video Studio applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/media_layouts.h"

/* Provide the add operation used by this module and its client applications. */
static UmiStatus add(UmiWsLayoutTemplate *layout, const char *id, UmiWsDockRegion region, int32_t row, int32_t col, double weight) {
    return umi_ws_layout_template_add(layout, id, region, row, col, weight);
}

/*
 * Provide the ws media layout video editor operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_media_layout_video_editor(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "video-editor.edit", "Video Editor", UMI_WS_DOMAIN_VIDEO_EDITOR);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "media.assets", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "media.preview", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "media.inspector", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "media.timeline", UMI_WS_DOCK_BOTTOM, 1, 0, 1.2)) != UMI_STATUS_OK) return s;
    return add(layout, "media.audio-meters", UMI_WS_DOCK_RIGHT, 1, 2, 0.5);
}

/*
 * Provide the ws media layout ai creator operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_media_layout_ai_creator(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "ai-creator.generate", "AI Image & Video Creator", UMI_WS_DOMAIN_AI_CREATOR);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "ai.prompt-library", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "ai.creation-canvas", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "ai.parameters", UMI_WS_DOCK_RIGHT, 0, 2, 0.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "ai.generations", UMI_WS_DOCK_BOTTOM, 1, 0, 1.0)) != UMI_STATUS_OK) return s;
    return add(layout, "ai.node-workflow", UMI_WS_DOCK_BOTTOM, 1, 1, 1.0);
}

/*
 * Provide the ws media layout music studio operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_media_layout_music_studio(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "music-studio.arrange", "Music Studio", UMI_WS_DOMAIN_MUSIC_STUDIO);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "music.browser", UMI_WS_DOCK_LEFT, 0, 0, 0.7)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "music.arrangement", UMI_WS_DOCK_CENTRE, 0, 1, 1.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "music.mixer", UMI_WS_DOCK_RIGHT, 0, 2, 0.9)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "music.waveform", UMI_WS_DOCK_BOTTOM, 1, 1, 0.8)) != UMI_STATUS_OK) return s;
    return add(layout, "music.transport", UMI_WS_DOCK_BOTTOM, 2, 1, 0.3);
}

/*
 * Provide the ws media layout video studio operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_media_layout_video_studio(UmiWsLayoutTemplate *layout) {
    UmiStatus s = umi_ws_layout_template_init(layout, "video-studio.composite", "Video Studio", UMI_WS_DOMAIN_VIDEO_STUDIO);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "video.media-pool", UMI_WS_DOCK_LEFT, 0, 0, 0.8)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "video.viewer", UMI_WS_DOCK_CENTRE, 0, 1, 1.7)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "video.compositor-graph", UMI_WS_DOCK_RIGHT, 0, 2, 1.0)) != UMI_STATUS_OK) return s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if ((s = add(layout, "video.timeline", UMI_WS_DOCK_BOTTOM, 1, 0, 1.1)) != UMI_STATUS_OK) return s;
    return add(layout, "video.inspector", UMI_WS_DOCK_RIGHT, 1, 2, 0.7);
}
