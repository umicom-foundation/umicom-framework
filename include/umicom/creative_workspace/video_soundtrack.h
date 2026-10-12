/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/video_soundtrack.h
 * PURPOSE: Attach a local soundtrack through the shared owned video-render process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_VIDEO_SOUNDTRACK_H
#define UMICOM_CREATIVE_WORKSPACE_VIDEO_SOUNDTRACK_H
#include "umicom/creative_workspace/video_render.h"
#ifdef __cplusplus
extern "C" {
#endif
/** @brief Capture a local video and audio file for a new MP4 export.
 * All paths are explicit absolute native paths. No file is opened here; out
 * must point to NULL. The first video stream is copied without re-encoding.
 * The first audio stream from audioPath replaces any original audio, starting
 * at the beginning. Short audio is padded with silence; long audio ends with
 * the video. FFmpeg must support AAC and the source video codec in MP4.
 * Execute with UmiVideoRenderExecute; its cancellation, output ownership and
 * result limitations apply. This neither mixes audio nor changes a recipe. */
UmiStatus UmiVideoSoundtrackPrepare(const char *videoPath, const char *audioPath,
                                    const char *ffmpeg, const char *workingDirectory,
                                    UmiVideoRenderPlan **out);
#ifdef __cplusplus
}
#endif
#endif
