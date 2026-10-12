/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_soundtrack.c
 * PURPOSE: Copy a video stream and attach explicit local audio without shell interpolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/creative_workspace/video_soundtrack.h"
#include "video_render_internal.h"
#include <stdlib.h>
#include <string.h>

UmiStatus UmiVideoSoundtrackPrepare(const char *videoPath, const char *audioPath,
                                    const char *ffmpeg, const char *workingDirectory,
                                    UmiVideoRenderPlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (*out != NULL)
    return UMI_STATUS_INVALID_STATE;
  if (UmiOutputFileValidatePath(videoPath) != UMI_STATUS_OK ||
      UmiOutputFileValidatePath(audioPath) != UMI_STATUS_OK ||
      UmiOutputFileValidatePath(ffmpeg) != UMI_STATUS_OK || workingDirectory == NULL ||
      !umi_path_is_absolute(workingDirectory) || strlen(workingDirectory) >= UMI_PATH_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiVideoRenderPlan *plan = calloc(1U, sizeof(*plan));
  if (plan == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  memcpy(plan->program, ffmpeg, strlen(ffmpeg) + 1U);
  memcpy(plan->directory, workingDirectory, strlen(workingDirectory) + 1U);
  memcpy(plan->source_paths[0], videoPath, strlen(videoPath) + 1U);
  memcpy(plan->source_paths[1], audioPath, strlen(audioPath) + 1U);
  /* Mapping names each required stream. Missing audio fails rather than
   * silently delivering another silent video. Padding retains a video whose
   * soundtrack is shorter, while shortest ends padded audio with the video. */
  const char *arguments[] = {"-nostdin",
                             "-v",
                             "error",
                             "-protocol_whitelist",
                             "file",
                             "-i",
                             plan->source_paths[0],
                             "-protocol_whitelist",
                             "file",
                             "-i",
                             plan->source_paths[1],
                             "-map",
                             "0:v:0",
                             "-map",
                             "1:a:0",
                             "-c:v",
                             "copy",
                             "-c:a",
                             "aac",
                             "-af",
                             "apad",
                             "-shortest",
                             "-sn",
                             "-dn",
                             "-movflags",
                             "+frag_keyframe+empty_moov",
                             "-f",
                             "mp4",
                             "pipe:1"};
  static_assert(sizeof(arguments) / sizeof(arguments[0]) <= UMI_CHANNEL_MAX_ARGUMENTS,
                "Soundtrack arguments must fit the process channel");
  plan->count = sizeof(arguments) / sizeof(arguments[0]);
  memcpy(plan->arguments, arguments, sizeof(arguments));
  *out = plan;
  return UMI_STATUS_OK;
}
