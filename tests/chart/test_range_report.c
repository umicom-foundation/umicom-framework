/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart/test_range_report.c
 * PURPOSE: Check exported row identity, formula escaping, numeric text and owned lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/range_report.h"
#include <locale.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv) {
  CHECK(argc == 2);
  UmiChartCandle candles[] = {{0, 10, 12, 8, 11, 20}, {100, 11, 15, 10, 14, 30}};
  UmiChartRangeCapture *capture = UmiChartRangeCaptureCreate();
  CHECK(capture);
  UmiChartCandleRange range;
  if (strcmp(argv[1], "zero-open") == 0)
    candles[0] = (UmiChartCandle){0, 0, 12, 0, 11, 20};
  OK(UmiChartRangeCaptureBegin(capture, candles, 2, 0, 2, 0));
  if (strcmp(argv[1], "pending") != 0)
    OK(UmiChartRangeCaptureComplete(capture, 100, &range));
  char instrument[UMI_CHART_NAME_CAPACITY + 1U];
  (void)snprintf(instrument, sizeof(instrument), "%s", "SIM,\"PAIR\"");
  if (strcmp(argv[1], "formula") == 0)
    (void)snprintf(instrument, sizeof(instrument), "%s", "=HYPERLINK(1)");
  else if (strcmp(argv[1], "control") == 0)
    instrument[0] = '\1';
  else if (strcmp(argv[1], "long") == 0) {
    memset(instrument, 'x', sizeof(instrument));
    instrument[sizeof(instrument) - 1U] = '\0';
  } else if (strcmp(argv[1], "empty-id") == 0)
    instrument[0] = '\0';
  else if (strcmp(argv[1], "locale") == 0) {
    /* Availability is host-dependent; record a skip rather than falsely
     * claiming that a comma-decimal locale was exercised. */
    const char *names[] = {"de_DE.UTF-8", "de_DE", "German_Germany.1252", "German"};
    int selected = 0;
    for (size_t index = 0; index < sizeof(names) / sizeof(names[0]); ++index)
      if (setlocale(LC_NUMERIC, names[index]) && strcmp(localeconv()->decimal_point, ",") == 0) {
        selected = 1;
        break;
      }
    if (!selected) {
      UmiChartRangeCaptureDestroy(capture);
      return 77;
    }
    candles[0].volume = 20.5;
    OK(UmiChartRangeCaptureBegin(capture, candles, 2, 0, 2, 0));
    OK(UmiChartRangeCaptureComplete(capture, 100, &range));
  }
  UmiCsvDocument *document = (UmiCsvDocument *)capture;
  UmiStatus status = UmiChartRangeReportCsv(capture, instrument, 60000U, 1234U, &document);
  if (strcmp(argv[1], "pending") == 0)
    CHECK(status == UMI_STATUS_NOT_FOUND && document == NULL);
  else if (strcmp(argv[1], "control") == 0 || strcmp(argv[1], "long") == 0 ||
           strcmp(argv[1], "empty-id") == 0)
    CHECK(status != UMI_STATUS_OK && document == NULL);
  else {
    CHECK(status == UMI_STATUS_OK && document);
    UmiChartRangeCaptureDestroy(capture);
    capture = NULL;
    const char *csv = UmiCsvDocumentData(document);
    CHECK(UmiCsvDocumentRows(document) == 4 && strstr(csv, "\"capture_monotonic_ms\""));
    CHECK(strstr(csv, "\"60000\",\"1234\"") && strstr(csv, "not an execution report"));
    CHECK(strstr(csv, "\"candle\"") && strstr(csv, "\"summary\""));
    if (strcmp(argv[1], "formula") == 0)
      CHECK(strstr(csv, "\"'=HYPERLINK(1)\""));
    else
      CHECK(strstr(csv, "\"SIM,\"\"PAIR\"\"\""));
    if (strcmp(argv[1], "zero-open") == 0)
      CHECK(strstr(csv, "\"14\",\"\",\"100\""));
    else if (strcmp(argv[1], "locale") == 0)
      CHECK(strstr(csv, "\"50.5\""));
    else if (strcmp(argv[1], "values") == 0)
      CHECK(strstr(csv, "\"10\",\"15\",\"8\",\"14\",\"50\",\"4\",\"40\""));
    else
      CHECK(strcmp(argv[1], "formula") == 0 || strcmp(argv[1], "owned") == 0);
  }
  UmiCsvDocumentDestroy(document);
  UmiChartRangeCaptureDestroy(capture);
  return 0;
}
