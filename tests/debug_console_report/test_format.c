/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_console_report/test_format.c
 *
 * PURPOSE:
 *   Check stable page boundaries, escaped output and complete buffer publication.
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
             *names[] = {"first", "middle",   "last",    "empty",   "offset",    "bounds",
                         "short", "controls", "unicode", "maximum", "truncated", "entry-bounds"};
  CHECK(Known(name, names, sizeof names / sizeof names[0]));
  UmiDebugConsoleEntryRegistry *registry = Entries(!strcmp(name, "empty") ? 0U : 150U);
  UmiDebugConsoleFilter filter;
  umi_debug_console_filter_init(&filter);
  filter.sort = UMI_DEBUG_CONSOLE_SORT_NEWEST_FIRST;
  if (!strcmp(name, "truncated"))
    filter.limit = 3U;
  if (!strcmp(name, "controls") || !strcmp(name, "unicode") || !strcmp(name, "maximum")) {
    UmiDebugConsoleEntrySnapshot entry;
    OK(umi_debug_console_entry_registry_at(registry, 0, &entry));
    if (!strcmp(name, "controls")) {
      strcpy(entry.category, "stdout\nforged");
      strcpy(entry.text, "hello\n\t\033[31m\\end\177");
    }
    if (!strcmp(name, "unicode"))
      strcpy(entry.text, "caf\xc3\xa9");
    if (!strcmp(name, "maximum")) {
      memset(entry.text, 'x', sizeof entry.text - 1U);
      entry.text[sizeof entry.text - 1U] = '\0';
    }
    OK(umi_debug_console_entry_registry_upsert(registry, &entry));
  }
  UmiDebugConsoleReport *report = Capture(registry, &filter);
  size_t offset = !strcmp(name, "middle") ? 64U : !strcmp(name, "last") ? 128U : 0U, bytes = 0U;
  OK(UmiDebugConsoleReportFormat(report, offset, 64U, NULL, 0U, &bytes));
  char *text = malloc(bytes + 1U);
  CHECK(text != NULL);
  OK(UmiDebugConsoleReportFormat(report, offset, 64U, text, bytes + 1U, &bytes));
  CHECK(strlen(text) == bytes);
  if (!strcmp(name, "first"))
    CHECK(strstr(text, "Showing 1..64 of 150") && strstr(text, "message-0063") &&
          !strstr(text, "message-0064"));
  if (!strcmp(name, "middle"))
    CHECK(strstr(text, "Showing 65..128") && strstr(text, "message-0127") &&
          !strstr(text, "message-0063"));
  if (!strcmp(name, "last"))
    CHECK(strstr(text, "Showing 129..150") && strstr(text, "message-0149") &&
          !strstr(text, "message-0127"));
  if (!strcmp(name, "empty"))
    CHECK(strstr(text, "Showing 0..0 of 0"));
  if (!strcmp(name, "controls"))
    CHECK(strstr(text, "stdout\\x0aforged") && strstr(text, "hello\n\t\\x1b[31m\\\\end\\x7f"));
  if (!strcmp(name, "unicode"))
    CHECK(strstr(text, "caf\xc3\xa9"));
  if (!strcmp(name, "maximum"))
    CHECK(strstr(text, "xxxxxxxxxxxxxxxxxxxx") && bytes > 2047U);
  if (!strcmp(name, "truncated"))
    CHECK(strstr(text, "3 retained records (150 matching)") &&
          strstr(text, "only part of its matches"));
  if (!strcmp(name, "short")) {
    memset(text, 'Q', bytes + 1U);
    size_t previous = bytes;
    CHECK(UmiDebugConsoleReportFormat(report, 0U, 64U, text, bytes, &bytes) ==
              UMI_STATUS_CAPACITY_EXCEEDED &&
          bytes == previous);
    for (size_t i = 0U; i <= bytes; ++i)
      CHECK(text[i] == 'Q');
  }
  if (!strcmp(name, "offset") || !strcmp(name, "bounds")) {
    size_t preserved = 77U;
    char value[] = "unchanged";
    UmiStatus expected =
        !strcmp(name, "offset") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_ARGUMENT;
    CHECK(UmiDebugConsoleReportFormat(report, !strcmp(name, "offset") ? SIZE_MAX : 0U,
                                      !strcmp(name, "offset") ? 64U : 0U, value, sizeof value,
                                      &preserved) == expected);
    CHECK(preserved == 77U && !strcmp(value, "unchanged"));
    CHECK(UmiDebugConsoleReportFormat(report, 150U, 64U, NULL, 0U, &preserved) ==
          UMI_STATUS_NOT_FOUND);
    CHECK(UmiDebugConsoleReportFormat(report, 0U, 129U, NULL, 0U, &preserved) ==
          UMI_STATUS_INVALID_ARGUMENT);
  }
  if (!strcmp(name, "entry-bounds")) {
    UmiDebugConsoleEntrySnapshot out;
    memset(&out, 0x5a, sizeof out);
    UmiDebugConsoleEntrySnapshot before = out;
    CHECK(UmiDebugConsoleReportEntryAt(report, SIZE_MAX, &out) == UMI_STATUS_NOT_FOUND &&
          !memcmp(&before, &out, sizeof out));
  }
  free(text);
  UmiDebugConsoleReportDestroy(report);
  umi_debug_console_entry_registry_destroy(registry);
  return 0;
}
