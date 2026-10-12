/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_captions.c
 * PURPOSE: Copy existing media streams and encode an explicit SubRip caption track without a shell.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/video_captions.h"
#include "video_render_internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiVideoCaptionsPrepare(const char *videoPath, const char *srtPath, const char *ffmpeg,
                                  const char *workingDirectory, UmiVideoRenderPlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (*out != NULL)
    return UMI_STATUS_INVALID_STATE;
  if (UmiOutputFileValidatePath(videoPath) != UMI_STATUS_OK ||
      UmiOutputFileValidatePath(srtPath) != UMI_STATUS_OK ||
      UmiOutputFileValidatePath(ffmpeg) != UMI_STATUS_OK || workingDirectory == NULL ||
      !umi_path_is_absolute(workingDirectory) || strlen(workingDirectory) >= UMI_PATH_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiVideoRenderPlan *plan = calloc(1U, sizeof(*plan));
  if (plan == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(plan->program, ffmpeg, strlen(ffmpeg) + 1U);
  memcpy(plan->directory, workingDirectory, strlen(workingDirectory) + 1U);
  memcpy(plan->source_paths[0], videoPath, strlen(videoPath) + 1U);
  memcpy(plan->source_paths[1], srtPath, strlen(srtPath) + 1U);
  /* Force the caption input format and select streams explicitly. The
   * optional audio mapping also accepts a silent sequence export. Deliberately
   * omit shortest: a brief subtitle track must not truncate the video. */
  const char *arguments[] = {"-nostdin",
                             "-v",
                             "error",
                             "-protocol_whitelist",
                             "file",
                             "-i",
                             plan->source_paths[0],
                             "-protocol_whitelist",
                             "file",
                             "-f",
                             "srt",
                             "-i",
                             plan->source_paths[1],
                             "-map",
                             "0:v:0",
                             "-map",
                             "0:a:0?",
                             "-map",
                             "1:s:0",
                             "-c:v",
                             "copy",
                             "-c:a",
                             "copy",
                             "-c:s",
                             "mov_text",
                             "-disposition:s:0",
                             "default",
                             "-dn",
                             "-movflags",
                             "+frag_keyframe+empty_moov",
                             "-f",
                             "mp4",
                             "pipe:1"};
  static_assert(sizeof(arguments) / sizeof(arguments[0]) <= UMI_CHANNEL_MAX_ARGUMENTS,
                "Caption export arguments must fit the process channel");
  plan->count = sizeof(arguments) / sizeof(arguments[0]);
  memcpy(plan->arguments, arguments, sizeof(arguments));
  *out = plan;
  return UMI_STATUS_OK;
}
