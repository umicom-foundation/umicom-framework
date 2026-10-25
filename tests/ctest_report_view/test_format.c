/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_format.c
 *
 * PURPOSE:
 *   Verify escaped pages, output bounds and unchanged source observations.
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
  const char *name = argv[1], *names[] = {"first",
                                          "middle",
                                          "last",
                                          "empty",
                                          "short",
                                          "query",
                                          "offset",
                                          "bounds",
                                          "unicode",
                                          "controls",
                                          "unchanged-source",
                                          "all-counts"};
  CHECK(Known(name, names, sizeof names / sizeof names[0]));
  UmiCtestReport *report = !strcmp(name, "empty")        ? Many(0)
                           : !strcmp(name, "all-counts") ? Mixed()
                                                         : Many(150);
  if (!strcmp(name, "unicode") || !strcmp(name, "controls")) {
    UmiCtestReportDestroy(report);
    const char xml[] =
        "<testsuite tests='1'><testcase name='caf&#233;' status='fail' time='0'><failure "
        "message='line&#10;next&#9;tab\\tail'/></testcase></testsuite>";
    OK(UmiCtestReportImport(xml, sizeof xml - 1U, &report));
  }
  UmiCtestReportFilter filter;
  UmiCtestReportFilterInit(&filter);
  UmiCtestReportView *view = NULL;
  OK(UmiCtestReportViewCreate(report, &filter, &view));
  size_t offset = !strcmp(name, "middle") ? 64U : !strcmp(name, "last") ? 128U : 0U;
  char *text = PageText(view, offset, 64U);
  CHECK(strstr(text, "source and binaries are not verified"));
  if (!strcmp(name, "first"))
    CHECK(strstr(text, "Showing 1..64 of 150") && !strstr(text, "row-000085"));
  if (!strcmp(name, "middle"))
    CHECK(strstr(text, "Showing 65..128 of 150"));
  if (!strcmp(name, "last"))
    CHECK(strstr(text, "Showing 129..150 of 150"));
  if (!strcmp(name, "empty"))
    CHECK(strstr(text, "Showing 0..0 of 0"));
  if (!strcmp(name, "unicode"))
    CHECK(strstr(text, "caf\xc3\xa9"));
  if (!strcmp(name, "controls"))
    CHECK(strstr(text, "line\\x0Anext\\x09tab\\x5Ctail"));
  if (!strcmp(name, "all-counts"))
    CHECK(strstr(text, "Skipped: 1 | Disabled: 1 | Not run: 1") &&
          strstr(text, "Showing 1..5 of 5"));
  if (!strcmp(name, "unchanged-source")) {
    UmiCtestReportSummary summary;
    OK(UmiCtestReportReadSummary(report, &summary));
    CHECK(summary.tests == 150 && summary.failed == 150);
    CHECK(UmiCtestReportRequirePassed(report) == UMI_STATUS_INVALID_STATE);
  }
  size_t required = 0;
  char out[4] = {'k', 'e', 'e', 'p'}, before[4];
  memcpy(before, out, 4);
  if (!strcmp(name, "short")) {
    CHECK(UmiCtestReportViewFormat(view, 0, 64, out, 4, &required) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!memcmp(out, before, 4) && required > 4);
  }
  if (!strcmp(name, "query")) {
    OK(UmiCtestReportViewFormat(view, 0, 64, NULL, 0, &required));
    CHECK(required == strlen(text));
  }
  if (!strcmp(name, "offset")) {
    CHECK(UmiCtestReportViewFormat(view, SIZE_MAX, 64, out, 4, &required) == UMI_STATUS_NOT_FOUND);
    CHECK(!memcmp(out, before, 4));
  }
  if (!strcmp(name, "bounds")) {
    CHECK(UmiCtestReportViewFormat(view, 0, 0, out, 4, &required) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestReportViewFormat(view, 0, 129, out, 4, &required) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiCtestReportViewFormat(view, 0, 64, NULL, 1, &required) == UMI_STATUS_INVALID_ARGUMENT);
  }
  free(text);
  UmiCtestReportViewDestroy(view);
  UmiCtestReportDestroy(report);
  return 0;
}
