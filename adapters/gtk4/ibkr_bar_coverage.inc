/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_bar_coverage.inc
 * PURPOSE: Show retained bar coverage beside historical and streaming chart summaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Called while the connection window's existing FillEnter guard owns it.
 * Reading retained bars sends no new request and does not change broker state. */
static void BarCoverageAppend(ConnectionUi *ui, bool streaming, uint32_t request, size_t count,
                              uint32_t intervalMs, GString *text) {
  if (count == 0U || count > UMI_BAR_COVERAGE_MAX_TIMESTAMPS)
    return;
  int64_t *times = g_new(int64_t, count);
  UmiStatus status = UMI_STATUS_OK;
  for (size_t i = 0U; i < count && status == UMI_STATUS_OK; ++i) {
    if (streaming) {
      UmiIbkrRealtimeBar bar;
      status = UmiIbkrRealtimeBarCopy(ui->connection, request, i, &bar);
      if (status == UMI_STATUS_OK)
        times[i] = bar.timeMilliseconds;
    } else {
      UmiIbkrHistoricalBar bar;
      status = UmiIbkrHistoricalBarCopy(ui->connection, request, i, &bar);
      if (status == UMI_STATUS_OK)
        times[i] = bar.timeMilliseconds;
    }
  }
  UmiBarCoverage coverage;
  if (status == UMI_STATUS_OK)
    status = UmiBarCoverageAnalyze(times, count, intervalMs, &coverage);
  g_free(times);
  if (status != UMI_STATUS_OK) {
    g_string_append(text, "\nTimestamp coverage could not be described for this capture.");
    return;
  }
  g_string_append_printf(
      text,
      "\nRetained coverage: %zu gaps; %" G_GUINT64_FORMAT
      " unobserved intervals; %zu irregular steps; %zu duplicate/backward steps.",
      coverage.gap_count, (guint64)coverage.unobserved_intervals, coverage.irregular_count,
      coverage.non_increasing_count);
  if (coverage.gap_count != 0U)
    g_string_append_printf(
        text,
        "\nLargest timestamp gap: %" G_GUINT64_FORMAT
        " ms. Market closures or sparse trading can cause gaps; this is not a feed-loss diagnosis.",
        (guint64)coverage.largest_gap_ms);
}
