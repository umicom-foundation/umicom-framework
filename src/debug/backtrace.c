/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/backtrace.c
 *
 * PURPOSE:
 *   Own selected stopped-thread frame copies and conservative workspace freshness evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "backtrace_private.h"
#include <stdlib.h>
#include <string.h>
void UmiDebugBacktraceDestroy(UmiDebugBacktrace *trace) {
  if (trace == NULL)
    return;
  free(trace->frames);
  free(trace);
}
UmiStatus UmiDebugBacktraceClone(const UmiDebugBacktrace *trace, UmiDebugBacktrace **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (trace == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDebugBacktrace *copy = calloc(1U, sizeof *copy);
  if (copy == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  copy->stamp = trace->stamp;
  copy->summary = trace->summary;
  if (trace->summary.frame_count != 0U) {
    size_t bytes = trace->summary.frame_count * sizeof *trace->frames;
    copy->frames = malloc(bytes);
    if (copy->frames == NULL) {
      free(copy);
      return UMI_STATUS_OUT_OF_MEMORY;
    }
    memcpy(copy->frames, trace->frames, bytes);
  }
  *out = copy;
  return UMI_STATUS_OK;
}
UmiStatus UmiDebugBacktraceRead(const UmiDebugBacktrace *trace, UmiDebugBacktraceSummary *out) {
  if (trace == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = trace->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiDebugBacktraceFrameAt(const UmiDebugBacktrace *trace, size_t index,
                                   UmiDebugStackFrameSnapshot *out) {
  if (trace == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= trace->summary.frame_count)
    return UMI_STATUS_NOT_FOUND;
  *out = trace->frames[index];
  return UMI_STATUS_OK;
}
UmiStatus UmiDebugBacktraceCheck(UmiDebugWorkspace *workspace, const UmiDebugBacktrace *trace) {
  if (workspace == NULL || trace == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDebugViewStamp stamp;
  UmiStatus status = UmiDebugWorkspaceViewStamp(workspace, &stamp);
  if (status != UMI_STATUS_OK)
    return status;
  return UmiDebugViewStampEqual(&stamp, &trace->stamp) ? UMI_STATUS_OK : UMI_STATUS_BUSY;
}
UmiStatus UmiDebugBacktraceCapture(UmiDebugWorkspace *workspace, UmiDebugBacktrace **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (workspace == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDebugWorkspaceSnapshot view;
  UmiStatus status = umi_debug_workspace_snapshot(workspace, &view);
  if (status != UMI_STATUS_OK)
    return status;
  if (view.selected_thread_id[0] == '\0')
    return UMI_STATUS_NOT_FOUND;
  if (view.visible_frame_count > UMI_DEBUG_STACK_FRAME_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiDebugThreadSnapshot selected = {0};
  bool found = false;
  for (size_t i = 0U; i < view.thread_count; ++i) {
    UmiDebugThreadSnapshot thread;
    status = umi_debug_workspace_thread_at(workspace, i, &thread);
    if (status != UMI_STATUS_OK)
      return status;
    if (strcmp(thread.id, view.selected_thread_id) == 0) {
      selected = thread;
      found = true;
      break;
    }
  }
  if (!found)
    return UMI_STATUS_NOT_FOUND;
  if (!selected.stopped)
    return UMI_STATUS_INVALID_STATE;
  UmiDebugBacktrace *trace = calloc(1U, sizeof *trace);
  if (trace == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  trace->summary.thread = selected;
  trace->summary.frame_count = view.visible_frame_count;
  status = UmiDebugWorkspaceViewStamp(workspace, &trace->stamp);
  if (status != UMI_STATUS_OK)
    goto failed;
  if (view.visible_frame_count != 0U) {
    trace->frames = calloc(view.visible_frame_count, sizeof *trace->frames);
    if (trace->frames == NULL) {
      status = UMI_STATUS_OUT_OF_MEMORY;
      goto failed;
    }
  }
  /* Keep display order and coordinates exactly as observed. This is a report,
   * not a request to select a frame or resolve its source against local files. */
  for (size_t i = 0U; i < view.visible_frame_count; ++i) {
    status = umi_debug_workspace_frame_at(workspace, i, &trace->frames[i]);
    if (status != UMI_STATUS_OK)
      goto failed;
    if (strcmp(trace->frames[i].thread_id, selected.id) != 0) {
      status = UMI_STATUS_BUSY;
      goto failed;
    }
  }
  status = UmiDebugBacktraceCheck(workspace, trace);
  if (status != UMI_STATUS_OK)
    goto failed;
  *out = trace;
  return UMI_STATUS_OK;
failed:
  UmiDebugBacktraceDestroy(trace);
  return status;
}
