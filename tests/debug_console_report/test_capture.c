/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_console_report/test_capture.c
 *
 * PURPOSE:
 *   Check console query ownership, filtering, capacity and deterministic sorting.
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
  const char *name = argv[1], *names[] = {"ownership", "invalid-query", "invalid-utf8", "null",
                                          "filtered",  "limit",         "unmatched",    "maximum",
                                          "oldest",    "newest",        "severity",     "ties"};
  CHECK(Known(name, names, sizeof names / sizeof names[0]));
  UmiDebugConsoleEntryRegistry *registry =
      Entries(!strcmp(name, "maximum") ? UMI_DEBUG_CONSOLE_ENTRY_CAPACITY : 150U);
  UmiDebugConsoleFilter filter;
  umi_debug_console_filter_init(&filter);
  if (!strcmp(name, "filtered")) {
    strcpy(filter.session_id, "session-b");
    strcpy(filter.category, "stderr");
    strcpy(filter.text, "MESSAGE-");
    filter.minimum_severity = 2;
  }
  if (!strcmp(name, "limit"))
    filter.limit = 5U;
  if (!strcmp(name, "unmatched"))
    strcpy(filter.text, "not present");
  if (!strcmp(name, "newest"))
    filter.sort = UMI_DEBUG_CONSOLE_SORT_NEWEST_FIRST;
  if (!strcmp(name, "severity"))
    filter.sort = UMI_DEBUG_CONSOLE_SORT_SEVERITY_FIRST;
  if (!strcmp(name, "ties")) {
    for (size_t i = 0; i < 150U; ++i) {
      UmiDebugConsoleEntrySnapshot e;
      OK(umi_debug_console_entry_registry_at(registry, i, &e));
      e.timestamp = 7U;
      OK(umi_debug_console_entry_registry_upsert(registry, &e));
    }
  }
  UmiDebugConsoleQuery *query = NULL;
  OK(umi_debug_console_query_create(&query));
  if (!strcmp(name, "invalid-query")) {
    UmiDebugConsoleReport *bad = NULL;
    CHECK(UmiDebugConsoleReportCreate(query, &bad) == UMI_STATUS_INVALID_STATE && bad == NULL);
    goto done;
  }
  if (!strcmp(name, "invalid-utf8")) {
    UmiDebugConsoleEntrySnapshot entry;
    OK(umi_debug_console_entry_registry_at(registry, 0, &entry));
    strcpy(entry.text, "\xff");
    OK(umi_debug_console_entry_registry_upsert(registry, &entry));
  }
  OK(umi_debug_console_query_execute(query, registry, &filter));
  UmiDebugConsoleReport *report = NULL;
  UmiStatus status = UmiDebugConsoleReportCreate(query, &report);
  if (!strcmp(name, "invalid-utf8")) {
    CHECK(status == UMI_STATUS_PARSE_ERROR && report == NULL);
    goto done;
  }
  OK(status);
  UmiDebugConsoleQuerySnapshot summary;
  OK(UmiDebugConsoleReportRead(report, &summary));
  size_t expected = !strcmp(name, "maximum")     ? UMI_DEBUG_CONSOLE_ENTRY_CAPACITY
                    : !strcmp(name, "limit")     ? 5U
                    : !strcmp(name, "unmatched") ? 0U
                    : !strcmp(name, "filtered")  ? 45U
                                                 : 150U;
  CHECK(summary.result_count == expected);
  if (!strcmp(name, "limit"))
    CHECK(summary.truncated && summary.total_match_count == 150U);
  if (!strcmp(name, "ownership")) {
    umi_debug_console_entry_registry_clear(registry);
    OK(umi_debug_console_query_execute(query, registry, &filter));
    UmiDebugConsoleEntrySnapshot first;
    OK(UmiDebugConsoleReportEntryAt(report, 0U, &first));
    CHECK(!strcmp(first.id, "row-0149"));
    CHECK(summary.result_count == 150U);
  }
  if (!strcmp(name, "null")) {
    UmiDebugConsoleReport *bad = report;
    CHECK(UmiDebugConsoleReportCreate(NULL, &bad) == UMI_STATUS_INVALID_ARGUMENT && bad == NULL);
    CHECK(UmiDebugConsoleReportCreate(query, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDebugConsoleReportRead(report, NULL) == UMI_STATUS_INVALID_ARGUMENT);
  }
  for (size_t i = 0U; i < summary.result_count; ++i) {
    UmiDebugConsoleEntrySnapshot entry;
    OK(UmiDebugConsoleReportEntryAt(report, i, &entry));
    if (!strcmp(name, "filtered"))
      CHECK(!strcmp(entry.session_id, "session-b") && !strcmp(entry.category, "stderr") &&
            entry.severity >= 2);
    if (i) {
      UmiDebugConsoleEntrySnapshot previous;
      OK(UmiDebugConsoleReportEntryAt(report, i - 1U, &previous));
      if (filter.sort == UMI_DEBUG_CONSOLE_SORT_NEWEST_FIRST)
        CHECK(previous.timestamp >= entry.timestamp);
      else if (filter.sort == UMI_DEBUG_CONSOLE_SORT_SEVERITY_FIRST)
        CHECK(previous.severity > entry.severity ||
              (previous.severity == entry.severity && previous.timestamp <= entry.timestamp));
      else
        CHECK(previous.timestamp <= entry.timestamp);
      if (!strcmp(name, "ties"))
        CHECK(strcmp(previous.id, entry.id) < 0);
    }
  }
  UmiDebugConsoleReportDestroy(report);
done:
  umi_debug_console_query_destroy(query);
  umi_debug_console_entry_registry_destroy(registry);
  return 0;
}
