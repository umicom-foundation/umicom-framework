/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/creative_workspace/video_render.c
 * PURPOSE: Stream encoded video to an exclusive output while separately retaining diagnostics.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/clock.h"
#include "video_render_internal.h"
#include <string.h>

UmiStatus UmiVideoRenderExecute(const UmiVideoRenderPlan *plan, const char *output,
                                unsigned timeoutMs, const UmiCancellationToken *cancel,
                                UmiVideoRenderReport *report) {
  if (report == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  memset(report, 0, sizeof(*report));
  if (plan == NULL || timeoutMs == 0U || timeoutMs > UMI_VIDEO_RENDER_MAX_TIMEOUT_MS)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = UmiOutputFileValidatePath(output);
  if (status != UMI_STATUS_OK)
    return status;
  if (umi_cancellation_token_is_requested(cancel))
    return UMI_STATUS_CANCELLED;
  UmiOutputFile *file = NULL;
  UmiProcessChannel *channel = NULL;
  status = UmiOutputFileCreate(output, &file);
  if (status != UMI_STATUS_OK)
    return status;
  report->output_created = true;
  UmiProcessChannelRequest request = {plan->program, plan->arguments, plan->count, plan->directory};
  /* The channel owns a process group/job and drains stderr independently.
   * Keeping all calls on this worker also satisfies its Linux lifetime rule. */
  status = UmiProcessChannelOpenProgram(&request, NULL, 0U, &channel);
  if (status == UMI_STATUS_OK)
    status = UmiProcessChannelCloseInput(channel);
  UmiClock clock = umi_clock_system();
  uint64_t started = clock.monotonic_nanoseconds(&clock), written = 0U;
  unsigned char prefix[12] = {0};
  size_t prefixSize = 0U;
  bool eof = false;
  while (status == UMI_STATUS_OK) {
    if (umi_cancellation_token_is_requested(cancel)) {
      status = UMI_STATUS_CANCELLED;
      break;
    }
    if ((clock.monotonic_nanoseconds(&clock) - started) / UINT64_C(1000000) >= timeoutMs) {
      status = UMI_STATUS_TIMEOUT;
      break;
    }
    /* A stdout close can precede process exit. Continue polling in that
     * state; otherwise drain stdout through EOF even after root exit. */
    if (!eof) {
      unsigned char bytes[16384];
      size_t count = 0U;
      UmiStatus read = UmiProcessChannelRead(channel, bytes, sizeof(bytes), &count, 50U);
      if (read != UMI_STATUS_OK && read != UMI_STATUS_TIMEOUT) {
        status = read;
        break;
      }
      if (read == UMI_STATUS_OK) {
        eof = count == 0U;
        if ((uint64_t)count > UMI_VIDEO_RENDER_MAX_BYTES - written) {
          status = UMI_STATUS_CAPACITY_EXCEEDED;
          break;
        }
        size_t copy = count < sizeof(prefix) - prefixSize ? count : sizeof(prefix) - prefixSize;
        memcpy(prefix + prefixSize, bytes, copy);
        prefixSize += copy;
        status = UmiOutputFileWrite(file, bytes, count);
        if (status != UMI_STATUS_OK)
          break;
        written += count;
      }
    }
    status = UmiProcessChannelPoll(channel, &report->process);
    if (status != UMI_STATUS_OK || (eof && !report->process.running))
      break;
    if (eof)
      (void)clock.sleep_milliseconds(&clock, 10U);
  }
  /* Termination is scoped to this render. Keep partial output for inspection
   * and make it impossible to mistake a cancelled stream for a finished job. */
  if (channel != NULL && status != UMI_STATUS_OK) {
    (void)UmiProcessChannelTerminate(channel);
    (void)UmiProcessChannelPoll(channel, &report->process);
  }
  if (status == UMI_STATUS_OK && (report->process.exitCode != 0 || prefixSize != sizeof(prefix) ||
                                  memcmp(prefix + 4, "ftyp", 4U) != 0))
    status = UMI_STATUS_IO_ERROR;
  UmiProcessChannelDestroy(channel);
  UmiStatus closed = UmiOutputFileClose(file);
  if (status == UMI_STATUS_OK)
    status = closed;
  (void)UmiOutputFileRead(file, &report->file);
  UmiOutputFileDestroy(file);
  umi_clock_dispose(&clock);
  report->completed = status == UMI_STATUS_OK;
  return status;
}
