/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_backtrace/test_format.c
 *
 * PURPOSE:
 *   Check source-path choices, plain-text escaping and nonpartial buffer output.
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
  const char *name = argv[1], *names[] = {"omit",
                                          "filenames",
                                          "full",
                                          "unicode",
                                          "controls",
                                          "windows",
                                          "trailing-separator",
                                          "no-source",
                                          "unknown-line",
                                          "empty-name",
                                          "empty-stack",
                                          "short",
                                          "query",
                                          "invalid-mode",
                                          "invalid-utf8",
                                          "preserved-output",
                                          "thread-controls"};
  CHECK(TraceKnown(name, names, sizeof names / sizeof names[0]));
  TraceFixture f;
  TraceFixtureCreate(&f);
  if (!strcmp(name, "unicode"))
    strcpy(f.frame.name, "caf\xc3\xa9");
  if (!strcmp(name, "controls"))
    strcpy(f.frame.name, "a\n\t\r\177\\b");
  if (!strcmp(name, "windows"))
    strcpy(f.frame.source_uri, "C:\\private\\main.c");
  if (!strcmp(name, "trailing-separator"))
    strcpy(f.frame.source_uri, "/private/");
  if (!strcmp(name, "no-source"))
    f.frame.source_uri[0] = '\0';
  if (!strcmp(name, "unknown-line")) {
    f.frame.line = 0U;
    f.frame.column = 9U;
  }
  if (!strcmp(name, "empty-name"))
    f.frame.name[0] = '\0';
  if (!strcmp(name, "invalid-utf8") || !strcmp(name, "preserved-output"))
    strcpy(f.frame.name, "\xff");
  if (!strcmp(name, "thread-controls")) {
    strcpy(f.thread.name, "worker\nInjected");
    OK(umi_debug_thread_registry_upsert(umi_debug_service_thread(f.service), &f.thread));
  }
  OK(umi_debug_stack_frame_registry_upsert(umi_debug_service_stack_frame(f.service), &f.frame));
  if (!strcmp(name, "empty-stack"))
    umi_debug_stack_frame_registry_clear(umi_debug_service_stack_frame(f.service));
  UmiDebugBacktrace *trace = NULL;
  OK(UmiDebugBacktraceCapture(f.workspace, &trace));
  char text[8192];
  memset(text, 'Q', sizeof text);
  size_t required = 123U;
  UmiDebugBacktracePaths mode = !strcmp(name, "omit")   ? UMI_DEBUG_BACKTRACE_OMIT_PATHS
                                : !strcmp(name, "full") ? UMI_DEBUG_BACKTRACE_FULL_PATHS
                                                        : UMI_DEBUG_BACKTRACE_FILENAMES;
  if (!strcmp(name, "invalid-mode"))
    mode = (UmiDebugBacktracePaths)77;
  UmiStatus status = UmiDebugBacktraceFormat(trace, mode, text, sizeof text, &required);
  if (!strcmp(name, "invalid-mode") || !strcmp(name, "invalid-utf8") ||
      !strcmp(name, "preserved-output")) {
    CHECK(status ==
          (!strcmp(name, "invalid-mode") ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_PARSE_ERROR));
    CHECK(required == 123U);
    for (size_t i = 0U; i < sizeof text; ++i)
      CHECK(text[i] == 'Q');
    goto done;
  }
  OK(status);
  CHECK(required == strlen(text) && strstr(text, "Captured call stack") != NULL);
  if (!strcmp(name, "omit"))
    CHECK(strstr(text, "/private") == NULL && strstr(text, "main.c") == NULL);
  if (!strcmp(name, "filenames") || !strcmp(name, "windows"))
    CHECK(strstr(text, "at main.c:42:3") != NULL && strstr(text, "private") == NULL);
  if (!strcmp(name, "full"))
    CHECK(strstr(text, "/private/project/main.c:42:3") != NULL);
  if (!strcmp(name, "unicode"))
    CHECK(strstr(text, "caf\xc3\xa9") != NULL);
  if (!strcmp(name, "controls"))
    CHECK(strstr(text, "a\\x0a\\x09\\x0d\\x7f\\\\b") != NULL);
  if (!strcmp(name, "thread-controls"))
    CHECK(strstr(text, "worker\\x0aInjected") != NULL);
  if (!strcmp(name, "trailing-separator"))
    CHECK(strstr(text, "(unnamed source)") != NULL);
  if (!strcmp(name, "no-source"))
    CHECK(strstr(text, "[source unavailable]") != NULL);
  if (!strcmp(name, "unknown-line"))
    CHECK(strstr(text, "at main.c\n") != NULL);
  if (!strcmp(name, "empty-name"))
    CHECK(strstr(text, "#0 (unnamed)") != NULL);
  if (!strcmp(name, "empty-stack"))
    CHECK(strstr(text, "Frames: 0\n") != NULL && strstr(text, "#0") == NULL);
  if (!strcmp(name, "short")) {
    size_t exact = required;
    memset(text, 'Q', sizeof text);
    CHECK(UmiDebugBacktraceFormat(trace, mode, text, exact, &required) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(required == exact);
    for (size_t i = 0U; i < sizeof text; ++i)
      CHECK(text[i] == 'Q');
    OK(UmiDebugBacktraceFormat(trace, mode, text, exact + 1U, &required));
    CHECK(strlen(text) == exact);
  }
  if (!strcmp(name, "query")) {
    size_t bytes = 0U;
    OK(UmiDebugBacktraceFormat(trace, mode, NULL, 0U, &bytes));
    CHECK(bytes == required);
    CHECK(UmiDebugBacktraceFormat(trace, mode, NULL, 1U, &bytes) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDebugBacktraceFormat(trace, mode, text, sizeof text, NULL) ==
          UMI_STATUS_INVALID_ARGUMENT);
  }
done:
  UmiDebugBacktraceDestroy(trace);
  TraceFixtureDestroy(&f);
  return 0;
}
