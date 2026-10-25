/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_console_report/fixture.h
 *
 * PURPOSE:
 *   Create synthetic debugger output with controlled categories, timestamps and severity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONSOLE_REPORT_FIXTURE_H
#define UMICOM_CONSOLE_REPORT_FIXTURE_H
#include "umicom/debug/console_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static inline UmiDebugConsoleEntryRegistry *Entries(size_t count) {
  UmiDebugConsoleEntryRegistry *registry = NULL;
  OK(umi_debug_console_entry_registry_create(&registry));
  for (size_t i = 0U; i < count; ++i) {
    UmiDebugConsoleEntrySnapshot entry = {0};
    (void)snprintf(entry.id, sizeof entry.id, "row-%04zu", i);
    (void)snprintf(entry.text, sizeof entry.text, "message-%04zu", i);
    strcpy(entry.session_id, i % 2U ? "session-b" : "session-a");
    strcpy(entry.category, i % 2U ? "stderr" : "stdout");
    entry.timestamp = (uint64_t)(count - i);
    entry.severity = (int)(i % 5U);
    OK(umi_debug_console_entry_registry_upsert(registry, &entry));
  }
  return registry;
}
static inline UmiDebugConsoleReport *Capture(UmiDebugConsoleEntryRegistry *registry,
                                             const UmiDebugConsoleFilter *filter) {
  UmiDebugConsoleQuery *query = NULL;
  UmiDebugConsoleReport *report = NULL;
  OK(umi_debug_console_query_create(&query));
  OK(umi_debug_console_query_execute(query, registry, filter));
  OK(UmiDebugConsoleReportCreate(query, &report));
  umi_debug_console_query_destroy(query);
  return report;
}
static inline int Known(const char *name, const char *const *names, size_t count) {
  for (size_t i = 0; i < count; ++i)
    if (!strcmp(name, names[i]))
      return 1;
  return 0;
}
#endif
