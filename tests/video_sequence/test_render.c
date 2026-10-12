/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/video_sequence/test_render.c
 * PURPOSE: Check exclusive output, cancellation, diagnostics and encoder failure ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#undef CHECK
#include "fixture.h"
#include "umicom/creative_workspace/video_render.h"
#include "umicom/platform/clock.h"
#include "umicom/platform/directory.h"
#include "umicom/platform/threading.h"
typedef struct LateCancel {
  const char *path;
  UmiCancellationToken *token;
  bool observed;
} LateCancel;
/* Request cancellation only after the output contains bytes. This exercises
 * termination during a live render, rather than only rejecting a pre-cancelled job. */
static int CancelWrittenOutput(void *context) {
  LateCancel *request = context;
  UmiClock clock = umi_clock_system();
  uint64_t start = clock.monotonic_nanoseconds(&clock);
  while ((clock.monotonic_nanoseconds(&clock) - start) / UINT64_C(1000000) < 5000U) {
    UmiFileInfo file;
    if (umi_directory_stat(request->path, &file) == UMI_STATUS_OK && file.size >= 16U) {
      request->observed = true;
      break;
    }
    (void)clock.sleep_milliseconds(&clock, 10U);
  }
  umi_cancellation_token_request(request->token);
  umi_clock_dispose(&clock);
  return request->observed ? 0 : 1;
}
int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  const char *mode = argv[1];
  char directory[UMI_PATH_CAPACITY], output[UMI_PATH_CAPACITY], missing[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  FixturePath(output, directory, "render.mp4");
  FixturePath(missing, directory, "not-installed-encoder");
  UmiVideoSequence sequence = Sequence();
  FixturePath(sequence.clips[0].path, directory, mode);
  UmiVideoRenderPlan *plan = NULL;
  CHECK(UmiVideoRenderPrepare(&sequence, strcmp(mode, "missing-tool") == 0 ? missing : argv[2],
                              directory, &plan) == UMI_STATUS_OK);
  UmiCancellationToken *cancel = NULL;
  CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
  if (strcmp(mode, "cancelled") == 0)
    umi_cancellation_token_request(cancel);
  if (strcmp(mode, "existing") == 0) {
    UmiOutputFile *file = NULL;
    CHECK(UmiOutputFileCreate(output, &file) == UMI_STATUS_OK);
    CHECK(UmiOutputFileWrite(file, "retain", 6U) == UMI_STATUS_OK);
    UmiOutputFileDestroy(file);
  }
  UmiThread *canceller = NULL;
  LateCancel cancellation = {output, cancel, false};
  if (strcmp(mode, "late-cancel") == 0)
    CHECK(umi_thread_start(CancelWrittenOutput, &cancellation, &canceller) == UMI_STATUS_OK);
  UmiVideoRenderReport report;
  UmiStatus status = UmiVideoRenderExecute(
      plan, output, strcmp(mode, "timeout") == 0 ? 100U : 5000U, cancel, &report);
  if (canceller != NULL) {
    int exitCode = -1;
    CHECK(umi_thread_join(canceller, &exitCode) == UMI_STATUS_OK);
    umi_thread_destroy(canceller);
    CHECK(exitCode == 0 && cancellation.observed);
  }
  if (strcmp(mode, "late-cancel") == 0) {
    CHECK(status == UMI_STATUS_CANCELLED && report.output_created && !report.completed);
    CHECK(report.process.terminated && !report.process.running && report.file.bytes_written == 16U);
    size_t size = 0U;
    unsigned char *bytes = FixtureRead(output, &size);
    CHECK(size == 16U);
    free(bytes);
  } else if (strcmp(mode, "closed-output") == 0) {
    CHECK(status == UMI_STATUS_IO_ERROR && report.output_created && !report.completed);
    CHECK(!report.process.running && report.process.exitCode == 7 &&
          report.file.bytes_written == 16U);
  } else if (strcmp(mode, "stream") == 0 || strcmp(mode, "diagnostics") == 0) {
    CHECK(status == UMI_STATUS_OK && report.completed && report.file.bytes_written == 16U);
    size_t size = 0U;
    unsigned char *bytes = FixtureRead(output, &size);
    CHECK(size == 16U && memcmp(bytes + 4, "ftyp", 4U) == 0);
    free(bytes);
    if (strcmp(mode, "diagnostics") == 0)
      CHECK(strstr(report.process.diagnostics, "diagnostic-tail-marker") != NULL);
  } else if (strcmp(mode, "cancelled") == 0)
    CHECK(status == UMI_STATUS_CANCELLED && !report.output_created && !report.completed);
  else if (strcmp(mode, "timeout") == 0)
    CHECK(status == UMI_STATUS_TIMEOUT && report.output_created && !report.completed);
  else if (strcmp(mode, "existing") == 0) {
    CHECK(status == UMI_STATUS_ALREADY_EXISTS && !report.output_created);
    size_t size = 0U;
    unsigned char *bytes = FixtureRead(output, &size);
    CHECK(size == 6U && memcmp(bytes, "retain", size) == 0);
    free(bytes);
  } else if (strcmp(mode, "missing-tool") == 0 || strcmp(mode, "failed") == 0 ||
             strcmp(mode, "empty") == 0)
    CHECK(status != UMI_STATUS_OK && report.output_created && !report.completed);
  else
    return 2;
  UmiVideoRenderPlanDestroy(plan);
  umi_cancellation_token_destroy(cancel);
  return 0;
}
