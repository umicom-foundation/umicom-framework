/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/bar_range.h
 * PURPOSE: Describe a selected broker-bar interval without another market-data request.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_CONNECTIVITY_BAR_RANGE_H
#define UMICOM_BROKER_CONNECTIVITY_BAR_RANGE_H
#include "umicom/broker_connectivity/realtime_bars.h"
#include "umicom/chart/candle_range.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Copied chart measurements with the provider identity and observation age.
 * Prices are floating-point presentation values, not exact order calculations.
 * range indices refer to the oldest-first retained broker series at capture.
 * When volume_available is false, range.volume is an unused zero placeholder;
 * unavailable_volume_bars counts absent or unrepresentable decimal volumes. */
typedef struct UmiIbkrBarRange {
  uint32_t request_id;
  UmiIbkrQuoteContract contract;
  UmiIbkrHistoricalDataKind data_kind;
  uint32_t interval_ms;
  uint64_t captured_at_ms, observed_at_ms;
  bool streaming, volume_available;
  size_t unavailable_volume_bars;
  UmiChartCandleRange range;
} UmiIbkrBarRange;

/** Measure selected bars from a complete, fresh historical response.
 * first and count select retained rows, with at least one row required.
 * Freshness is response age, not proof that a market is currently trading or
 * that the final bar has closed. Call on the connection's owner thread,
 * serialized with its pump. No pointers into that connection are retained.
 * On any failure out is unchanged. This optional API belongs to the
 * ibkr_historical_chart library and sends no broker request. */
UmiStatus UmiIbkrHistoricalRangeMeasure(const UmiIbkrConnection *connection, uint32_t request_id,
                                        size_t first, size_t count, uint64_t now_ms,
                                        uint64_t maximum_age_ms, UmiIbkrBarRange *out);

/** Measure a selected interval in a fresh, active streaming subscription.
 * Index zero is the oldest retained bar; later arrivals may shift those rows.
 * The returned copy remains a description of this observation only. Call on
 * the connection's owner thread and recapture when the displayed window moves.
 * Missing volume never becomes a partial sum. Failure leaves out unchanged;
 * this operation does not subscribe, cancel, submit or change orders. */
UmiStatus UmiIbkrRealtimeRangeMeasure(const UmiIbkrConnection *connection, uint32_t request_id,
                                      size_t first, size_t count, uint64_t now_ms,
                                      uint64_t maximum_age_ms, UmiIbkrBarRange *out);
#ifdef __cplusplus
}
#endif
#endif
