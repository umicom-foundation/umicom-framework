/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_backtrace/test_capture.c
 *
 * PURPOSE:
 *   Check copied stack lifetime, frame boundaries and stale workspace observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *names[] = {"normal",          "empty",         "running",           "missing",
                         "ownership",       "clone",         "stale-frame",       "stale-thread",
                         "stale-selection", "stale-owner",   "unrelated-console", "limits",
                         "filtered",        "output-errors", "preserve-read"};
  CHECK(TraceKnown(name, names, sizeof names / sizeof names[0]));
  TraceFixture f;
  TraceFixtureCreate(&f);
  UmiDebugStackFrameRegistry *frames = umi_debug_service_stack_frame(f.service);
  if (!strcmp(name, "empty"))
    umi_debug_stack_frame_registry_clear(frames);
  if (!strcmp(name, "running")) {
    f.thread.stopped = 0;
    OK(umi_debug_thread_registry_upsert(umi_debug_service_thread(f.service), &f.thread));
  }
  if (!strcmp(name, "missing"))
    umi_debug_thread_registry_clear(umi_debug_service_thread(f.service));
  if (!strcmp(name, "filtered")) {
    UmiDebugStackFrameSnapshot other = f.frame;
    strcpy(other.id, "other-frame");
    strcpy(other.thread_id, "other-thread");
    OK(umi_debug_stack_frame_registry_upsert(frames, &other));
  }
  if (!strcmp(name, "limits")) {
    for (size_t i = 1U; i < UMI_DEBUG_STACK_FRAME_CAPACITY; ++i) {
      (void)snprintf(f.frame.id, sizeof f.frame.id, "frame-%zu", i);
      f.frame.line = (uint32_t)i + 1U;
      OK(umi_debug_stack_frame_registry_upsert(frames, &f.frame));
    }
  }
  UmiDebugBacktrace *trace = NULL;
  UmiStatus status = UmiDebugBacktraceCapture(f.workspace, &trace);
  if (!strcmp(name, "running") || !strcmp(name, "missing")) {
    CHECK(status == (!strcmp(name, "running") ? UMI_STATUS_INVALID_STATE : UMI_STATUS_NOT_FOUND));
    CHECK(trace == NULL);
    TraceFixtureDestroy(&f);
    return 0;
  }
  OK(status);
  OK(UmiDebugBacktraceCheck(f.workspace, trace));
  UmiDebugBacktraceSummary summary;
  OK(UmiDebugBacktraceRead(trace, &summary));
  CHECK(!strcmp(summary.thread.id, "thread-7"));
  CHECK(summary.frame_count == (!strcmp(name, "empty")    ? 0U
                                : !strcmp(name, "limits") ? UMI_DEBUG_STACK_FRAME_CAPACITY
                                                          : 1U));
  bool stale = false;
  if (!strcmp(name, "ownership"))
    TraceFixtureDestroy(&f);
  else if (!strcmp(name, "clone")) {
    UmiDebugBacktrace *copy = NULL;
    OK(UmiDebugBacktraceClone(trace, &copy));
    UmiDebugBacktraceDestroy(trace);
    trace = copy;
    umi_debug_stack_frame_registry_clear(frames);
    OK(UmiDebugBacktraceRead(trace, &summary));
    CHECK(summary.frame_count == 1U);
    stale = true;
  } else if (!strcmp(name, "stale-frame")) {
    f.frame.line = 99U;
    OK(umi_debug_stack_frame_registry_upsert(frames, &f.frame));
    stale = true;
  } else if (!strcmp(name, "stale-thread")) {
    f.thread.stopped = 0;
    OK(umi_debug_thread_registry_upsert(umi_debug_service_thread(f.service), &f.thread));
    stale = true;
  } else if (!strcmp(name, "stale-selection")) {
    strcpy(f.frame.id, "frame-new");
    OK(umi_debug_stack_frame_registry_upsert(frames, &f.frame));
    OK(umi_debug_workspace_select_frame(f.workspace, "frame-new"));
    stale = true;
  } else if (!strcmp(name, "stale-owner")) {
    umi_debug_workspace_destroy(f.workspace);
    OK(umi_debug_workspace_create(f.service, f.controller, &f.workspace));
    stale = true;
  } else if (!strcmp(name, "unrelated-console")) {
    UmiDebugConsoleEntrySnapshot output = {0};
    strcpy(output.id, "message");
    strcpy(output.text, "progress");
    OK(umi_debug_console_entry_registry_upsert(umi_debug_service_console_entry(f.service),
                                               &output));
    OK(UmiDebugBacktraceCheck(f.workspace, trace));
  } else if (!strcmp(name, "output-errors")) {
    UmiDebugBacktrace *rejected = trace;
    CHECK(UmiDebugBacktraceCapture(NULL, &rejected) == UMI_STATUS_INVALID_ARGUMENT &&
          rejected == NULL);
    rejected = trace;
    CHECK(UmiDebugBacktraceClone(NULL, &rejected) == UMI_STATUS_INVALID_ARGUMENT &&
          rejected == NULL);
    CHECK(UmiDebugBacktraceCapture(f.workspace, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDebugBacktraceClone(trace, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDebugBacktraceCheck(NULL, trace) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDebugBacktraceRead(trace, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  } else if (!strcmp(name, "preserve-read")) {
    UmiDebugStackFrameSnapshot unchanged;
    memset(&unchanged, 0x5a, sizeof unchanged);
    UmiDebugStackFrameSnapshot before = unchanged;
    CHECK(UmiDebugBacktraceFrameAt(trace, SIZE_MAX, &unchanged) == UMI_STATUS_NOT_FOUND);
    CHECK(!memcmp(&unchanged, &before, sizeof before));
    CHECK(UmiDebugBacktraceFrameAt(trace, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  }
  if (stale)
    CHECK(UmiDebugBacktraceCheck(f.workspace, trace) == UMI_STATUS_BUSY);
  if (summary.frame_count != 0U) {
    UmiDebugStackFrameSnapshot first;
    OK(UmiDebugBacktraceFrameAt(trace, 0U, &first));
    CHECK(first.line == 42U && !strcmp(first.name, "main"));
  }
  UmiDebugBacktraceDestroy(trace);
  TraceFixtureDestroy(&f);
  return 0;
}
