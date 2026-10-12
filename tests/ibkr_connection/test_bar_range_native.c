/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_bar_range_native.c
 * PURPOSE: Exercise native range-summary text against decoded broker fixtures.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "realtime_fixture.h"
#include <glib.h>
typedef struct ConnectionUi {
  UmiIbkrConnection *connection;
} ConnectionUi;
#define UMI_IBKR_HAS_HISTORY_MODEL 1
#include "../../adapters/gtk4/ibkr_bar_range.inc"

static int Review(bool streaming, const char *mode) {
  Fixture *f = New();
  CHECK(f && Connect(f) == 0);
  uint32_t request;
  if (streaming) {
    UmiIbkrRealtimeQuery query = StreamQuery();
    CHECK(UmiIbkrRealtimeRequest(f->c, &query, 10U, &request) == UMI_STATUS_OK);
    CHECK(StreamFeed(f, request, 1791363600U) == 0);
    CHECK(StreamFeed(f, request, 1791363605U) == 0);
  } else {
    UmiIbkrHistoricalQuery query = HistoricalQuery();
    CHECK(UmiIbkrHistoricalRequest(f->c, &query, 10U, &request) == UMI_STATUS_OK);
    CHECK(HistoricalFeed(f, request) == 0);
  }
  CHECK(StreamPump(f, 11U) == 0);
  UmiIbkrHistoricalBar *bars =
      streaming ? UmiIbkrRealtimeFind(f->c, request)->bars : f->c->history->bars;
  if (!strcmp(mode, "missing"))
    bars[0].volumeAvailable = false;
  if (!strcmp(mode, "precision"))
    bars[0].open.exact = false;
  if (!strcmp(mode, "zero-open")) {
    bars[0].open.value.coefficient = 0;
    bars[0].low.value.coefficient = 0;
  }
  if (!strcmp(mode, "zero-volume")) {
    bars[0].volume.value.coefficient = 0;
    bars[1].volume.value.coefficient = 0;
  }
  ConnectionUi ui = {f->c};
  GString *text = g_string_new("Existing broker summary");
  BarRangeAppend(&ui, streaming, request, 0U, 2U, !strcmp(mode, "stale") ? 112U : 12U, 100U, text);
  CHECK(g_str_has_prefix(text->str, "Existing broker summary"));
  if (!strcmp(mode, "stale") || !strcmp(mode, "precision")) {
    CHECK(strstr(text->str, "measurement unavailable:") != NULL);
    CHECK(strstr(text->str, "Volume ") == NULL);
  } else {
    CHECK(strstr(text->str, "measurement: 2 bars") != NULL);
    CHECK(strstr(text->str, "Rising 2 | Falling 0 | Flat 0") != NULL);
    CHECK(strstr(text->str, "Display values only.") != NULL);
    if (!strcmp(mode, "missing"))
      CHECK(strstr(text->str, "Volume unavailable (missing or unusable values: 1)") != NULL);
    else if (!strcmp(mode, "zero-volume"))
      CHECK(strstr(text->str, "Volume 0 (provider units)") != NULL);
    else
      CHECK(strstr(text->str, streaming ? "Volume 200" : "Volume 300") != NULL);
    if (!strcmp(mode, "zero-open"))
      CHECK(strstr(text->str, "Percentage unavailable") != NULL);
    else
      CHECK(strstr(text->str, streaming ? "+10%" : "+20%") != NULL);
  }
  g_string_free(text, TRUE);
  Delete(f);
  return 0;
}
int main(int argc, char **argv) {
  StreamReferences();
  const char *const cases[] = {"values",    "missing",   "stale",
                               "precision", "zero-open", "zero-volume"};
  if (argc != 2 || !HistoricalKnownCase(argv[1], cases, G_N_ELEMENTS(cases)))
    return 2;
  CHECK(Review(false, argv[1]) == 0);
  CHECK(Review(true, argv[1]) == 0);
  return 0;
}
