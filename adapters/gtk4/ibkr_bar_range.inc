/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_bar_range.inc
 * PURPOSE: Show measurements for the visible broker-bar interval using shared analysis.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifdef UMI_IBKR_HAS_HISTORY_MODEL
#include "umicom/broker_connectivity/bar_range.h"

/* The caller holds FillEnter while assembling a label. This helper only reads
 * the owner's retained data and appends text; it cannot emit a widget signal,
 * change a subscription or submit an order. */
static void BarRangeAppend(ConnectionUi *ui, bool streaming, uint32_t request, size_t first,
                           size_t count, uint64_t now, uint64_t maximumAge, GString *text) {
  UmiIbkrBarRange result;
  UmiStatus status = streaming ? UmiIbkrRealtimeRangeMeasure(ui->connection, request, first, count,
                                                             now, maximumAge, &result)
                               : UmiIbkrHistoricalRangeMeasure(ui->connection, request, first,
                                                               count, now, maximumAge, &result);
  if (status != UMI_STATUS_OK) {
    g_string_append_printf(text, "\nVisible-bar measurement unavailable: %s.",
                           umi_status_text(status));
    return;
  }
  const UmiChartCandleRange *range = &result.range;
  g_string_append_printf(
      text,
      "\nVisible-bar measurement: %zu bars | O %.10g | H %.10g | L %.10g | C %.10g"
      "\nChange %+.10g | Price range %.10g",
      range->count, range->open, range->high, range->low, range->close, range->change,
      range->price_range);
  if (range->has_percent_change)
    g_string_append_printf(text, " | %+.6g%%", range->percent_change);
  else
    g_string_append(text, " | Percentage unavailable");
  if (result.volume_available)
    g_string_append_printf(text, "\nVolume %.10g (provider units)", range->volume);
  else
    g_string_append_printf(text, "\nVolume unavailable (missing or unusable values: %zu)",
                           result.unavailable_volume_bars);
  g_string_append_printf(
      text,
      " | Rising %zu | Falling %zu | Flat %zu\nOpening-time span %" G_GUINT64_FORMAT
      " ms; excludes the final bar's duration. Display values only.",
      range->rising, range->falling, range->unchanged, (guint64)range->elapsed_ms);
}
#endif
