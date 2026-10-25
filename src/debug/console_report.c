/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/console_report.c
 *
 * PURPOSE:
 *   Capture a complete query result without retaining its registry or query owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "console_report_private.h"
#include "umicom/document/text_encoding.h"
#include <stdlib.h>
#include <string.h>
static int ReportText(const char *text, size_t capacity) {
  const char *end = memchr(text, '\0', capacity);
  return end != NULL &&
         umi_document_utf8_validate((const unsigned char *)text, (size_t)(end - text), NULL);
}
void UmiDebugConsoleReportDestroy(UmiDebugConsoleReport *report) {
  if (report != NULL) {
    free(report->rows);
    free(report);
  }
}
UmiStatus UmiDebugConsoleReportCreate(const UmiDebugConsoleQuery *query,
                                      UmiDebugConsoleReport **out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = NULL;
  if (query == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDebugConsoleQuerySnapshot summary;
  UmiStatus status = umi_debug_console_query_snapshot(query, &summary);
  if (status != UMI_STATUS_OK)
    return status;
  if (summary.struct_size != sizeof summary ||
      summary.api_version != UMI_DEBUG_CONSOLE_QUERY_API_VERSION ||
      summary.result_count != umi_debug_console_query_count(query) ||
      summary.total_match_count < summary.result_count)
    return UMI_STATUS_INVALID_STATE;
  if (summary.result_count > UMI_DEBUG_CONSOLE_ENTRY_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiDebugConsoleReport *report = calloc(1U, sizeof *report);
  if (report == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  report->summary = summary;
  if (summary.result_count != 0U) {
    report->rows = calloc(summary.result_count, sizeof *report->rows);
    if (report->rows == NULL) {
      free(report);
      return UMI_STATUS_OUT_OF_MEMORY;
    }
  }
  for (size_t i = 0U; i < summary.result_count; ++i) {
    UmiDebugConsoleEntrySnapshot *entry = &report->rows[i];
    status = umi_debug_console_query_at(query, i, entry);
    if (status != UMI_STATUS_OK)
      break;
    if (!ReportText(entry->id, sizeof entry->id) ||
        !ReportText(entry->session_id, sizeof entry->session_id) ||
        !ReportText(entry->category, sizeof entry->category) ||
        !ReportText(entry->text, sizeof entry->text)) {
      status = UMI_STATUS_PARSE_ERROR;
      break;
    }
  }
  if (status != UMI_STATUS_OK) {
    UmiDebugConsoleReportDestroy(report);
    return status;
  }
  *out = report;
  return UMI_STATUS_OK;
}
UmiStatus UmiDebugConsoleReportRead(const UmiDebugConsoleReport *report,
                                    UmiDebugConsoleQuerySnapshot *out) {
  if (report == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = report->summary;
  return UMI_STATUS_OK;
}
UmiStatus UmiDebugConsoleReportEntryAt(const UmiDebugConsoleReport *report, size_t index,
                                       UmiDebugConsoleEntrySnapshot *out) {
  if (report == NULL || out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (index >= report->summary.result_count)
    return UMI_STATUS_NOT_FOUND;
  *out = report->rows[index];
  return UMI_STATUS_OK;
}
