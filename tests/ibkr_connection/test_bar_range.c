/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_bar_range.c
 * PURPOSE: Check broker range analysis using independent injected protocol observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "realtime_fixture.h"
#include "umicom/broker_connectivity/bar_range.h"

/* Both paths receive real decoder packets from the existing in-memory transport.
 * Select the public query being reviewed; never connect this fixture to TWS. */
static UmiStatus Measure(Fixture *fixture, bool streaming, uint32_t request, size_t first,
                         size_t count, uint64_t now, UmiIbkrBarRange *out) {
  return streaming
             ? UmiIbkrRealtimeRangeMeasure(fixture->c, request, first, count, now, 100U, out)
             : UmiIbkrHistoricalRangeMeasure(fixture->c, request, first, count, now, 100U, out);
}

int main(int argc, char **argv) {
  if (argc != 3)
    return 2;
  bool streaming = strcmp(argv[1], "streaming") == 0;
  if (!streaming && strcmp(argv[1], "historical") != 0)
    return 2;
  const char *mode = argv[2];
  const char *const cases[] = {"values",
                               "subset",
                               "pending",
                               "stale",
                               "disconnect",
                               "price-precision",
                               "missing-volume",
                               "volume-precision",
                               "all-missing-volume",
                               "zero-volume",
                               "negative-volume",
                               "duplicate-time",
                               "invalid-ohlc",
                               "bounds",
                               "zero-count",
                               "capacity",
                               "null-output",
                               "null-connection",
                               "unknown-request",
                               "owned",
                               "unchanged-transport",
                               "cancelled",
                               "rolling",
                               "eviction"};
  if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
    return 2;
  if (!streaming &&
      (!strcmp(mode, "cancelled") || !strcmp(mode, "rolling") || !strcmp(mode, "eviction")))
    return 2;
  StreamReferences();
  Fixture *f = New();
  CHECK(f && Connect(f) == 0);
  uint32_t request;
  if (streaming) {
    UmiIbkrRealtimeQuery query = StreamQuery();
    CHECK(UmiIbkrRealtimeRequest(f->c, &query, 10U, &request) == UMI_STATUS_OK);
  } else {
    UmiIbkrHistoricalQuery query = HistoricalQuery();
    CHECK(UmiIbkrHistoricalRequest(f->c, &query, 10U, &request) == UMI_STATUS_OK);
  }
  if (strcmp(mode, "pending") != 0) {
    if (streaming) {
      CHECK(StreamFeed(f, request, 1791363600U) == 0);
      CHECK(StreamFeed(f, request, 1791363605U) == 0);
    } else {
      CHECK(HistoricalFeed(f, request) == 0);
    }
    CHECK(StreamPump(f, 11U) == 0);
  }
  UmiIbkrHistoricalBar *bars =
      streaming ? UmiIbkrRealtimeFind(f->c, request)->bars : f->c->history->bars;
  if (!strcmp(mode, "price-precision"))
    bars[1].open.exact = false;
  if (!strcmp(mode, "missing-volume") || !strcmp(mode, "all-missing-volume"))
    bars[0].volumeAvailable = false;
  if (!strcmp(mode, "all-missing-volume"))
    bars[1].volumeAvailable = false;
  if (!strcmp(mode, "volume-precision"))
    bars[0].volume.exact = false;
  if (!strcmp(mode, "zero-volume")) {
    bars[0].volume.value.coefficient = 0;
    bars[1].volume.value.coefficient = 0;
  }
  if (!strcmp(mode, "negative-volume"))
    bars[0].volume.value.coefficient = -1;
  if (!strcmp(mode, "duplicate-time"))
    bars[1].timeMilliseconds = bars[0].timeMilliseconds;
  if (!strcmp(mode, "invalid-ohlc"))
    bars[1].high.value.coefficient = 1;
  if (!strcmp(mode, "disconnect"))
    UmiIbkrConnectionClose(f->c);
  if (!strcmp(mode, "cancelled"))
    CHECK(UmiIbkrRealtimeCancel(f->c, request, 12U) == UMI_STATUS_OK);
  uint64_t now = !strcmp(mode, "stale") ? 112U : 12U;
  size_t first = !strcmp(mode, "subset") ? 1U : 0U;
  size_t count = !strcmp(mode, "subset") ? 1U : 2U;
  if (!strcmp(mode, "bounds"))
    first = SIZE_MAX;
  if (!strcmp(mode, "zero-count"))
    count = 0U;
  if (!strcmp(mode, "capacity"))
    count = SIZE_MAX;
  if (!strcmp(mode, "unknown-request"))
    request = UINT32_MAX;
  UmiIbkrBarRange result = {0}, previous;
  result.request_id = 12345U;
  memcpy(&previous, &result, sizeof result);
  if (!strcmp(mode, "unchanged-transport")) {
    /* Queue unread observations and make I/O fail if analysis tries to pump.
     * The result must still describe only the bars already retained. */
    if (streaming)
      CHECK(StreamFeed(f, request, 1791363610U) == 0);
    else
      CHECK(HistoricalFeed(f, request) == 0);
    f->readStatus = UMI_STATUS_UNAVAILABLE;
    f->writeStatus = UMI_STATUS_UNAVAILABLE;
  }
  size_t previousInput = f->inAt, previousOutput = f->outSize;
  unsigned previousOpens = f->opens, previousCloses = f->closes;
  UmiStatus status;
  if (!strcmp(mode, "null-connection")) {
    status = streaming
                 ? UmiIbkrRealtimeRangeMeasure(NULL, request, first, count, now, 100U, &result)
                 : UmiIbkrHistoricalRangeMeasure(NULL, request, first, count, now, 100U, &result);
  } else {
    status = Measure(f, streaming, request, first, count, now,
                     !strcmp(mode, "null-output") ? NULL : &result);
  }
  UmiStatus expected = UMI_STATUS_OK;
  if (!strcmp(mode, "pending") || !strcmp(mode, "stale") || !strcmp(mode, "disconnect") ||
      !strcmp(mode, "cancelled"))
    expected = UMI_STATUS_INVALID_STATE;
  if (!strcmp(mode, "price-precision"))
    expected = UMI_STATUS_NOT_IMPLEMENTED;
  if (!strcmp(mode, "bounds") || !strcmp(mode, "zero-count") || !strcmp(mode, "capacity") ||
      !strcmp(mode, "null-output") || !strcmp(mode, "null-connection") ||
      !strcmp(mode, "negative-volume") || !strcmp(mode, "duplicate-time") ||
      !strcmp(mode, "invalid-ohlc"))
    expected = UMI_STATUS_INVALID_ARGUMENT;
  if (!strcmp(mode, "unknown-request"))
    expected = UMI_STATUS_NOT_FOUND;
  CHECK(status == expected);
  /* Read-only analysis must leave the injected transport untouched. */
  CHECK(f->inAt == previousInput && f->outSize == previousOutput);
  CHECK(f->opens == previousOpens && f->closes == previousCloses);
  if (status != UMI_STATUS_OK) {
    CHECK(memcmp(&result, &previous, sizeof result) == 0);
  } else {
    CHECK(result.request_id == request && result.contract.contractId == 123U);
    CHECK(strcmp(result.contract.exchange, "SMART") == 0);
    CHECK(result.data_kind == UMI_IBKR_HISTORY_TRADES && result.streaming == streaming);
    CHECK(result.captured_at_ms == 12U && result.observed_at_ms == 11U);
    CHECK(result.interval_ms == (streaming ? 5000U : 60000U));
    CHECK(result.range.count == count && result.range.first_index == first);
    CHECK(result.range.last_index == 1U);
    CHECK(result.range.open == (!streaming && first == 1U ? 11.0 : 10.0));
    CHECK(result.range.close == (streaming ? 11.0 : 12.0));
    CHECK(result.range.high == (streaming ? 12.0 : 13.0));
    CHECK(result.range.low == (!streaming && first == 1U ? 10.0 : 9.0));
    CHECK(result.range.rising == count && result.range.falling == 0U);
    CHECK(result.range.elapsed_ms == (count == 1U ? 0U : result.interval_ms));
    bool missing = !strcmp(mode, "missing-volume") || !strcmp(mode, "volume-precision") ||
                   !strcmp(mode, "all-missing-volume");
    CHECK(result.volume_available == !missing);
    CHECK(result.unavailable_volume_bars == (!strcmp(mode, "all-missing-volume") ? 2U
                                             : missing                           ? 1U
                                                                                 : 0U));
    double volume = streaming ? (count == 1U ? 100.0 : 200.0) : (count == 1U ? 200.0 : 300.0);
    CHECK(result.range.volume == (missing || !strcmp(mode, "zero-volume") ? 0.0 : volume));
    if (!strcmp(mode, "owned")) {
      Delete(f);
      f = NULL;
      CHECK(result.range.count == 2U && result.contract.contractId == 123U);
      CHECK(result.range.close == (streaming ? 11.0 : 12.0));
    }
    if (!strcmp(mode, "eviction")) {
      /* Cross the ring capacity through the decoder, then verify that
       * retained index zero really refers to the new oldest observation. */
      for (uint64_t index = 2U; index <= UMI_IBKR_REALTIME_BAR_LIMIT; ++index) {
        CHECK(StreamFeed(f, request, 1791363600U + index * 5U) == 0);
        CHECK(StreamPump(f, index + 12U) == 0);
      }
      UmiIbkrBarRange next;
      CHECK(Measure(f, true, request, 0U, 2U, 525U, &next) == UMI_STATUS_OK);
      CHECK(next.range.first_time_ms == INT64_C(1791363605000));
      CHECK(next.range.last_time_ms == INT64_C(1791363610000));
      CHECK(result.range.first_time_ms == INT64_C(1791363600000));
    }
    if (!strcmp(mode, "rolling")) {
      /* A new arrival changes a new measurement, never a previously
       * returned value. Retained indices describe their capture time. */
      CHECK(StreamFeed(f, request, 1791363610U) == 0);
      CHECK(StreamPump(f, 13U) == 0);
      UmiIbkrBarRange next;
      CHECK(Measure(f, true, request, 1U, 2U, 14U, &next) == UMI_STATUS_OK);
      CHECK(next.range.first_time_ms == INT64_C(1791363605000));
      CHECK(result.range.first_time_ms == INT64_C(1791363600000));
    }
  }
  Delete(f);
  return 0;
}
