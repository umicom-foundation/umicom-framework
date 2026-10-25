/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_report_view_text.c
 *
 * PURPOSE:
 *   Format bounded saved-report pages as escaped data with explicit non-pass counts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "ctest_report_view_private.h"
#include <stdio.h>
#include <string.h>
typedef struct ReportWriter {
  char *out;
  size_t used;
} ReportWriter;
static void Append(ReportWriter *writer, const char *text, size_t size) {
  if (writer->out != NULL)
    memcpy(writer->out + writer->used, text, size);
  writer->used += size;
}
static void Text(ReportWriter *writer, const char *text) {
  for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
    if (*p < 32U || *p == 127U || *p == '\\') {
      char escaped[5];
      (void)snprintf(escaped, sizeof escaped, "\\x%02X", (unsigned)*p);
      Append(writer, escaped, 4U);
    } else
      Append(writer, (const char *)p, 1U);
  }
}
static void Page(ReportWriter *w, const UmiCtestReportView *view, size_t offset, size_t end) {
  char head[640];
  const UmiCtestReportSummary *s = &view->summary.source;
  int n =
      snprintf(head, sizeof head,
               "Saved CTest observations; source and binaries are not verified.\n"
               "Tests: %zu | Passed: %zu | Failed: %zu | Timed out: %zu\n"
               "Skipped: %zu | Disabled: %zu | Not run: %zu\n"
               "Showing %zu..%zu of %zu matching records.\n\n",
               s->tests, s->passed, s->failed, s->timed_out, s->skipped, s->disabled, s->not_run,
               view->summary.matches ? offset + 1U : 0U, end, view->summary.matches);
  Append(w, head, (size_t)n);
  for (size_t i = offset; i < end; ++i) {
    const UmiCtestReportEntry *e = view->rows[i].entry;
    const char *state = UmiCtestReportOutcomeText(UmiCtestReportEntryOutcome(e));
    Append(w, "[", 1U);
    Append(w, state, strlen(state));
    Append(w, "] ", 2U);
    Text(w, e->name);
    n = snprintf(head, sizeof head, " | %llu ms\n", (unsigned long long)e->result.duration_ms);
    Append(w, head, (size_t)n);
    if (e->result.message[0]) {
      Text(w, e->result.message);
      Append(w, "\n", 1U);
    }
  }
}
UmiStatus UmiCtestReportViewFormat(const UmiCtestReportView *view, size_t offset, size_t limit,
                                   char *out, size_t capacity, size_t *required) {
  if (view == NULL || required == NULL || limit == 0U || limit > 128U ||
      (out == NULL && capacity != 0U))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (offset > view->summary.matches || (offset == view->summary.matches && offset != 0U))
    return UMI_STATUS_NOT_FOUND;
  size_t count = view->summary.matches - offset, end = offset + (count < limit ? count : limit);
  /* The input parser bounds every field and this page holds at most 128
   * records, so measurement cannot approach SIZE_MAX on supported targets. */
  ReportWriter measure = {0};
  Page(&measure, view, offset, end);
  *required = measure.used;
  if (out == NULL)
    return UMI_STATUS_OK;
  if (capacity <= measure.used)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  ReportWriter writer = {out, 0U};
  Page(&writer, view, offset, end);
  out[writer.used] = '\0';
  return UMI_STATUS_OK;
}
