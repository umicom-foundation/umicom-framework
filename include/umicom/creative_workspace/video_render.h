/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/creative_workspace/video_render.h
 * PURPOSE: Render explicitly selected local video recipes through an owned native tool.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CREATIVE_WORKSPACE_VIDEO_RENDER_H
#define UMICOM_CREATIVE_WORKSPACE_VIDEO_RENDER_H
#include "umicom/creative_workspace/video_sequence.h"
#include "umicom/platform/process_channel.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_VIDEO_RENDER_MAX_BYTES (UINT64_C(512) * 1024U * 1024U)
#define UMI_VIDEO_RENDER_MAX_TIMEOUT_MS 900000U
typedef struct UmiVideoRenderPlan UmiVideoRenderPlan;
typedef struct UmiVideoRenderReport {
  UmiOutputFileSnapshot file;
  UmiProcessChannelSnapshot process;
  bool output_created;
  bool completed;
} UmiVideoRenderReport;

/** @brief Capture a validated sequence and explicit absolute FFmpeg executable.
 * No file access or launch occurs. workingDirectory must be an absolute native
 * directory; execution checks it exists. out must point to NULL. Empty sequences
 * cannot render. The caller selects a trusted tool; this is not a sandbox. */
UmiStatus UmiVideoRenderPrepare(const UmiVideoSequence *sequence, const char *ffmpeg,
                                const char *workingDirectory, UmiVideoRenderPlan **out);
/** @brief Render on the calling worker with a bounded deadline and cancellation.
 * Sequence plans use the first video stream, 30 fps, letterboxing and libx264.
 * Soundtrack plans copy the selected video and replace its audio with AAC.
 * Caption plans copy video/optional audio and add a selectable text track.
 * All use local-file input protocols and fragmented MP4 capped at 512 MiB.
 * Stdout goes
 * into an exclusively created file; stderr stays in the bounded report.
 * Existing files and parents are never changed or removed. Failure may leave
 * an empty/partial new output. completed requires tool exit zero, a container
 * prefix and successful close; it is not independent playback verification.
 * Recipes reference mutable local files. Inputs are read at execution time;
 * this is not an immutable media capture or a guarantee of exact duration. */
UmiStatus UmiVideoRenderExecute(const UmiVideoRenderPlan *plan, const char *output,
                                unsigned timeoutMs, const UmiCancellationToken *cancel,
                                UmiVideoRenderReport *report);
/** @brief Release a captured render plan. NULL is accepted. */
void UmiVideoRenderPlanDestroy(UmiVideoRenderPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
