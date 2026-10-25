/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/console_report_text.c
 *
 * PURPOSE:
 *   Format stable console pages with explicit retained counts and escaped control bytes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "console_report_private.h"
#include <stdio.h>
#include <string.h>
typedef struct ConsoleWriter {
  char *out;
  size_t used;
} ConsoleWriter;
static void Write(ConsoleWriter *w, const char *text, size_t n) {
  if (w->out != NULL)
    memcpy(w->out + w->used, text, n);
  w->used += n;
}
static void Literal(ConsoleWriter *w, const char *text) { Write(w, text, strlen(text)); }
static void Text(ConsoleWriter *w, const char *text, int body) {
  const char hex[] = "0123456789abcdef";
  for (const unsigned char *p = (const unsigned char *)text; *p; ++p) {
    if (*p == '\\')
      Literal(w, "\\\\");
    else if ((*p < 32U && !(body && (*p == '\n' || *p == '\t'))) || *p == 127U) {
      char escaped[] = {'\\', 'x', hex[*p >> 4U], hex[*p & 15U]};
      Write(w, escaped, sizeof escaped);
    } else
      Write(w, (const char *)p, 1U);
  }
}
static void Number(ConsoleWriter *w, uint64_t value) {
  char text[32];
  (void)snprintf(text, sizeof text, "%llu", (unsigned long long)value);
  Literal(w, text);
}
static void Render(ConsoleWriter *w, const UmiDebugConsoleReport *report, size_t offset,
                   size_t end) {
  Literal(w, "Captured debugger output\nShowing ");
  Number(w, report->summary.result_count ? offset + 1U : 0U);
  Literal(w, "..");
  Number(w, end);
  Literal(w, " of ");
  Number(w, report->summary.result_count);
  Literal(w, " retained records (");
  Number(w, report->summary.total_match_count);
  Literal(w, " matching).\n");
  if (report->summary.truncated)
    Literal(w, "The original query retained only part of its matches.\n");
  for (size_t i = offset; i < end; ++i) {
    const UmiDebugConsoleEntrySnapshot *entry = &report->rows[i];
    Literal(w, "\n[");
    Text(w, entry->id, 0);
    Literal(w, ", time ");
    Number(w, entry->timestamp);
    char severity[32];
    (void)snprintf(severity, sizeof severity, ", severity %d", entry->severity);
    Literal(w, severity);
    Literal(w, ", category ");
    Text(w, entry->category, 0);
    Literal(w, ", session ");
    Text(w, entry->session_id, 0);
    Literal(w, "]\n");
    Text(w, entry->text, 1);
    Literal(w, "\n");
  }
}
UmiStatus UmiDebugConsoleReportFormat(const UmiDebugConsoleReport *report, size_t offset,
                                      size_t limit, char *out, size_t capacity, size_t *required) {
  if (report == NULL || required == NULL || limit == 0U ||
      limit > UMI_DEBUG_CONSOLE_REPORT_PAGE_LIMIT || (out == NULL && capacity != 0U))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t count = report->summary.result_count;
  if (offset > count || (offset == count && count != 0U))
    return UMI_STATUS_NOT_FOUND;
  size_t available = count - offset, end = offset + (limit < available ? limit : available);
  ConsoleWriter measured = {0};
  Render(&measured, report, offset, end);
  *required = measured.used;
  if (out == NULL)
    return UMI_STATUS_OK;
  if (capacity <= measured.used)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  ConsoleWriter writer = {out, 0U};
  Render(&writer, report, offset, end);
  out[writer.used] = '\0';
  return UMI_STATUS_OK;
}
