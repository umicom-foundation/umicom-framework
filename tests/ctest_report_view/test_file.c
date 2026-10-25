/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ctest_report_view/test_file.c
 *
 * PURPOSE:
 *   Verify explicit local report reading and refusal without altering the selected source.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#undef CHECK
#include "fixture.h"
#include "umicom/testing/ctest_report_file.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1], *names[] = {"valid", "empty",     "malformed", "missing", "relative",
                                          "null",  "directory", "ownership", "unicode"};
  CHECK(Known(name, names, sizeof names / sizeof names[0]));
  char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
  FixtureDirectory(directory);
  FixturePath(path, directory, !strcmp(name, "unicode") ? "r\xc3\xa9sults.xml" : "report.xml");
  UmiCtestReport *report = NULL;
  if (!strcmp(name, "null")) {
    CHECK(UmiCtestReportReadFile(NULL, &report) == UMI_STATUS_INVALID_ARGUMENT && report == NULL);
    CHECK(UmiCtestReportReadFile(path, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
  }
  if (!strcmp(name, "relative")) {
    CHECK(UmiCtestReportReadFile("report.xml", &report) == UMI_STATUS_INVALID_ARGUMENT &&
          report == NULL);
    return 0;
  }
  if (!strcmp(name, "directory")) {
    CHECK(UmiCtestReportReadFile(directory, &report) != UMI_STATUS_OK && report == NULL);
    return 0;
  }
  if (!strcmp(name, "missing")) {
    CHECK(UmiCtestReportReadFile(path, &report) != UMI_STATUS_OK && report == NULL);
    return 0;
  }
  char *xml = ManyXml(150);
  const char *body = !strcmp(name, "empty") ? "" : !strcmp(name, "malformed") ? "<testsuite>" : xml;
  UmiOutputFile *output = NULL;
  OK(UmiOutputFileCreate(path, &output));
  OK(UmiOutputFileWrite(output, body, strlen(body)));
  OK(UmiOutputFileClose(output));
  UmiOutputFileDestroy(output);
  UmiStatus status = UmiCtestReportReadFile(path, &report);
  if (!strcmp(name, "empty") || !strcmp(name, "malformed"))
    CHECK(status == UMI_STATUS_PARSE_ERROR && report == NULL);
  else {
    OK(status);
    memset(xml, 'x', strlen(xml));
    UmiCtestReportSummary summary;
    OK(UmiCtestReportReadSummary(report, &summary));
    CHECK(summary.tests == 150U && summary.failed == 150U);
  }
  free(xml);
  UmiCtestReportDestroy(report);
  return 0;
}
