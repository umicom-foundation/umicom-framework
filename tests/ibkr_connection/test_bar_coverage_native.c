/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_bar_coverage_native.c
 * PURPOSE: Exercise the chart coverage adapter against deterministic retained-bar readers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/bar_coverage.h"
#include "umicom/broker_connectivity/historical_bars.h"
#include "umicom/broker_connectivity/realtime_bars.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>
#define CHECK(value)                                                                               \
  do {                                                                                             \
    if (!(value)) {                                                                                \
      fprintf(stderr, "line %d: %s\n", __LINE__, #value);                                          \
      return 1;                                                                                    \
    }                                                                                              \
  } while (0)
typedef struct ConnectionUi {
  UmiIbkrConnection *connection;
} ConnectionUi;
static unsigned reads;
static bool failRead;
static const int64_t times[] = {10000, 15000, 30000};
/* Substitute only retained record reads. The actual shared adapter formats
 * the coverage and its explanation; there are no sockets or broker requests. */
static UmiStatus Historical(const UmiIbkrConnection *connection, uint32_t request, size_t index,
                            UmiIbkrHistoricalBar *out) {
  (void)connection;
  ++reads;
  if (request != 7U || index >= G_N_ELEMENTS(times) || failRead)
    return UMI_STATUS_NOT_FOUND;
  memset(out, 0, sizeof(*out));
  out->timeMilliseconds = times[index];
  return UMI_STATUS_OK;
}
static UmiStatus Realtime(const UmiIbkrConnection *connection, uint32_t request, size_t index,
                          UmiIbkrRealtimeBar *out) {
  UmiIbkrHistoricalBar source;
  UmiStatus status = Historical(connection, request, index, &source);
  if (status == UMI_STATUS_OK) {
    memset(out, 0, sizeof(*out));
    out->timeMilliseconds = source.timeMilliseconds;
  }
  return status;
}
#define UmiIbkrHistoricalBarCopy Historical
#define UmiIbkrRealtimeBarCopy Realtime
#include "../../adapters/gtk4/ibkr_bar_coverage.inc"
#undef UmiIbkrHistoricalBarCopy
#undef UmiIbkrRealtimeBarCopy
int main(void) {
  ConnectionUi ui = {0};
  for (unsigned streaming = 0U; streaming < 2U; ++streaming) {
    GString *text = g_string_new("Existing capture summary.");
    reads = 0U;
    BarCoverageAppend(&ui, streaming != 0U, 7U, 3U, 5000U, text);
    CHECK(reads == 3U && strstr(text->str, "Existing capture summary.") != NULL);
    CHECK(strstr(text->str, "1 gaps; 2 unobserved intervals") != NULL);
    CHECK(strstr(text->str, "15000 ms") != NULL);
    CHECK(strstr(text->str, "not a feed-loss diagnosis") != NULL);
    g_string_free(text, TRUE);
  }
  GString *text = g_string_new("Kept");
  reads = 0U;
  BarCoverageAppend(&ui, false, 7U, 0U, 5000U, text);
  BarCoverageAppend(&ui, false, 7U, UMI_BAR_COVERAGE_MAX_TIMESTAMPS + 1U, 5000U, text);
  CHECK(reads == 0U && strcmp(text->str, "Kept") == 0);
  failRead = true;
  BarCoverageAppend(&ui, false, 7U, 3U, 5000U, text);
  CHECK(reads == 1U && strstr(text->str, "could not be described") != NULL);
  g_string_free(text, TRUE);
  return 0;
}
