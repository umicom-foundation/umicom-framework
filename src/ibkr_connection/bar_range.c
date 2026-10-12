/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ibkr_connection/bar_range.c
 * PURPOSE: Measure owned broker observations through the shared candle analysis model.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/broker_connectivity/bar_range.h"
#include "bar_chart_private.h"
#include <stdlib.h>

/* The two provider paths share the same bar record. Keep conversion and missing
 * field handling here so native, web and reporting consumers agree. These reads
 * cannot call user callbacks or advance the connection's pump. */
static UmiStatus MeasureRows(const UmiIbkrConnection *connection, uint32_t request, size_t first,
                             size_t count, UmiIbkrBarRange *result) {
  UmiChartCandle *candles = calloc(count, sizeof *candles);
  if (!candles)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = UMI_STATUS_OK;
  for (size_t index = 0U; index < count; ++index) {
    UmiIbkrHistoricalBar bar;
    status = result->streaming ? UmiIbkrRealtimeBarCopy(connection, request, first + index, &bar)
                               : UmiIbkrHistoricalBarCopy(connection, request, first + index, &bar);
    if (status != UMI_STATUS_OK)
      break;
    /* The original decimal record stays untouched. A value that could not
     * be represented by the decimal parser must not appear as zero volume
     * or contribute a misleading partial total. */
    if (!bar.volumeAvailable || !bar.volume.exact) {
      ++result->unavailable_volume_bars;
      bar.volumeAvailable = false;
    }
    bool available;
    status = UmiIbkrMarketBarCandle(&bar, &candles[index], &available);
    if (status != UMI_STATUS_OK)
      break;
  }
  if (status == UMI_STATUS_OK)
    status = UmiChartCandleRangeAnalyze(candles, count, 0U, count - 1U, &result->range);
  free(candles);
  if (status != UMI_STATUS_OK)
    return status;
  result->range.first_index = first;
  result->range.last_index = first + count - 1U;
  result->volume_available = result->unavailable_volume_bars == 0U;
  if (!result->volume_available)
    result->range.volume = 0.0;
  return UMI_STATUS_OK;
}

UmiStatus UmiIbkrHistoricalRangeMeasure(const UmiIbkrConnection *connection, uint32_t request,
                                        size_t first, size_t count, uint64_t now, uint64_t age,
                                        UmiIbkrBarRange *out) {
  if (!connection || !out || count == 0U || count > UMI_IBKR_HISTORICAL_BAR_LIMIT)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiIbkrHistoricalSnapshot snapshot;
  UmiStatus status = UmiIbkrHistoricalCopy(connection, request, now, age, &snapshot);
  if (status != UMI_STATUS_OK)
    return status;
  if (snapshot.stale || !snapshot.complete)
    return UMI_STATUS_INVALID_STATE;
  if (first > snapshot.count || count > snapshot.count - first)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Publish only after every selected row passes validation. Keeping this
   * local also leaves the caller's previous capture intact on failure. */
  UmiIbkrBarRange result = {0};
  result.request_id = request;
  result.contract = snapshot.query.contract;
  result.data_kind = snapshot.query.dataKind;
  result.interval_ms = snapshot.query.barSeconds * 1000U;
  result.captured_at_ms = now;
  result.observed_at_ms = snapshot.completedAtMilliseconds;
  status = MeasureRows(connection, request, first, count, &result);
  if (status == UMI_STATUS_OK)
    *out = result;
  return status;
}

UmiStatus UmiIbkrRealtimeRangeMeasure(const UmiIbkrConnection *connection, uint32_t request,
                                      size_t first, size_t count, uint64_t now, uint64_t age,
                                      UmiIbkrBarRange *out) {
  if (!connection || !out || count == 0U || count > UMI_IBKR_REALTIME_BAR_LIMIT)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiIbkrRealtimeSnapshot snapshot;
  UmiStatus status = UmiIbkrRealtimeCopy(connection, request, now, age, &snapshot);
  if (status != UMI_STATUS_OK)
    return status;
  if (snapshot.stale || !snapshot.active)
    return UMI_STATUS_INVALID_STATE;
  if (first > snapshot.count || count > snapshot.count - first)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiIbkrBarRange result = {0};
  result.request_id = request;
  result.contract = snapshot.query.contract;
  result.data_kind = snapshot.query.dataKind;
  result.interval_ms = 5000U;
  result.captured_at_ms = now;
  result.observed_at_ms = snapshot.receivedAtMilliseconds;
  result.streaming = true;
  status = MeasureRows(connection, request, first, count, &result);
  if (status == UMI_STATUS_OK)
    *out = result;
  return status;
}
