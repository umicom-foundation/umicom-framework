/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_render_plan.c
 * PURPOSE: Construct FFmpeg arguments without a shell or user-authored filter expressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/output_file.h"
#include "video_render_internal.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static bool Argument(UmiVideoRenderPlan *plan, const char *argument) {
  if (plan->count == UMI_CHANNEL_MAX_ARGUMENTS)
    return false;
  plan->arguments[plan->count++] = argument;
  return true;
}
/* Only validated integers enter the filter expression. Paths remain separate
 * argv values, so quotes, spaces and filter punctuation in filenames never
 * become filter syntax or shell commands. */
static bool Graph(UmiVideoRenderPlan *plan, const char *format, ...) {
  size_t used = strlen(plan->graph);
  va_list values;
  va_start(values, format);
  int count = vsnprintf(plan->graph + used, sizeof(plan->graph) - used, format, values);
  va_end(values);
  return count >= 0 && (size_t)count < sizeof(plan->graph) - used;
}
UmiStatus UmiVideoRenderPrepare(const UmiVideoSequence *sequence, const char *ffmpeg,
                                const char *workingDirectory, UmiVideoRenderPlan **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (*out != NULL)
    return UMI_STATUS_INVALID_STATE;
  UmiStatus status = UmiVideoSequenceValidate(sequence);
  if (status != UMI_STATUS_OK)
    return status;
  if (sequence->clip_count == 0U || ffmpeg == NULL || workingDirectory == NULL ||
      UmiOutputFileValidatePath(ffmpeg) != UMI_STATUS_OK ||
      !umi_path_is_absolute(workingDirectory) || strlen(workingDirectory) >= UMI_PATH_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiVideoRenderPlan *plan = calloc(1U, sizeof(*plan));
  if (plan == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  plan->sequence = *sequence;
  memcpy(plan->program, ffmpeg, strlen(ffmpeg) + 1U);
  memcpy(plan->directory, workingDirectory, strlen(workingDirectory) + 1U);
  bool valid = Argument(plan, "-nostdin") && Argument(plan, "-v") && Argument(plan, "error");
  for (size_t i = 0U; valid && i < sequence->clip_count; ++i) {
    const UmiVideoSequenceClip *clip = &plan->sequence.clips[i];
    uint32_t duration = clip->end_ms - clip->begin_ms;
    /* Integer formatting always uses a decimal point regardless of the
     * desktop's numeric locale. Input seeking avoids decoding the prefix. */
    (void)snprintf(plan->starts[i], sizeof(plan->starts[i]), "%u.%03u", clip->begin_ms / 1000U,
                   clip->begin_ms % 1000U);
    valid = Argument(plan, "-protocol_whitelist") && Argument(plan, "file") &&
            Argument(plan, "-ss") && Argument(plan, plan->starts[i]) && Argument(plan, "-i") &&
            Argument(plan, clip->path) &&
            Graph(plan,
                  "[%zu:v:0]trim=duration=%u.%03u,setpts=PTS-STARTPTS,"
                  "scale=%u:%u:force_original_aspect_ratio=decrease:force_divisible_by=2,"
                  "pad=%u:%u:(ow-iw)/2:(oh-ih)/2,setsar=1,fps=30[v%zu];",
                  i, duration / 1000U, duration % 1000U, sequence->width, sequence->height,
                  sequence->width, sequence->height, i);
  }
  for (size_t i = 0U; valid && i < sequence->clip_count; ++i)
    valid = Graph(plan, "[v%zu]", i);
  valid = valid && Graph(plan, "concat=n=%zu:v=1:a=0[out]", sequence->clip_count);
  const char *tail[] = {"-filter_complex",
                        plan->graph,
                        "-map",
                        "[out]",
                        "-an",
                        "-sn",
                        "-dn",
                        "-c:v",
                        "libx264",
                        "-threads",
                        "2",
                        "-pix_fmt",
                        "yuv420p",
                        "-movflags",
                        "+frag_keyframe+empty_moov",
                        "-f",
                        "mp4",
                        "pipe:1"};
  for (size_t i = 0U; valid && i < sizeof(tail) / sizeof(tail[0]); ++i)
    valid = Argument(plan, tail[i]);
  if (!valid) {
    free(plan);
    return UMI_STATUS_CAPACITY_EXCEEDED;
  }
  *out = plan;
  return UMI_STATUS_OK;
}
void UmiVideoRenderPlanDestroy(UmiVideoRenderPlan *plan) { free(plan); }
