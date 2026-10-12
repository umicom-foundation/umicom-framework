/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/video_captions.h
 * PURPOSE: Attach a user-selected caption file through the shared video export process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_VIDEO_CAPTIONS_H
#define UMICOM_CREATIVE_WORKSPACE_VIDEO_CAPTIONS_H
#include "umicom/creative_workspace/video_render.h"
#ifdef __cplusplus
extern "C" {
#endif
/** @brief Prepare a new MP4 with a selectable caption track.
 * videoPath, srtPath and ffmpeg must be absolute native file paths; the working
 * directory must also be absolute. out must point to NULL. No file is opened
 * during preparation, and the captured plan owns copies of every path.
 *
 * The first video and optional first audio stream are copied, so their codecs
 * must already be compatible with MP4. UTF-8 SubRip captions are encoded as
 * mov_text. Existing subtitle tracks are omitted. No timing adjustment, video
 * trimming or burned-in text is performed; captions extending beyond the video
 * can extend container duration. Review cue timing before export.
 *
 * Use UmiVideoRenderExecute for exclusive output, cancellation and diagnostics.
 * Its bounded execution and result limitations apply. A compatible player must
 * enable the caption track; unsupported players may not display it. */
UmiStatus UmiVideoCaptionsPrepare(const char *videoPath, const char *srtPath, const char *ffmpeg,
                                  const char *workingDirectory, UmiVideoRenderPlan **out);
#ifdef __cplusplus
}
#endif
#endif
