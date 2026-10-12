/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/candle_inspection_gtk4.inc
 * PURPOSE: Show copied OHLCV observations beside a trading chart without issuing broker requests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Capture immediately after scene construction, before publishing GTK labels.
 * Both calculations run on the serialized workspace owner. Later pointer
 * movement reads only this copy, so a provider update cannot mix old geometry
 * with newer candle values while the display is awaiting its next refresh. */
static void ChartCaptureCandleInspection(InteractiveChart *state) {
  state->inspection_count = 0U;
  if (state->scene == NULL || !ChartAttached(state))
    return;
  UmiChartTimeframeSummary summary;
  UmiStatus status = UmiTradingWorkspaceBuildChartCandles(
      state->context->workspace, state->info.instrument_id, state->inspection_candles,
      UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY, &summary);
  if (status == UMI_STATUS_OK && summary.candle_count == state->info.retained_bars &&
      summary.source_count == state->info.source_bars &&
      summary.interval_ms == state->info.interval_ms)
    state->inspection_count = summary.candle_count;
}
/* Publish plain text, never markup. Instrument identifiers and observations
 * are data, even when they originated outside this application. */
static void ChartRefreshCandleInspection(InteractiveChart *state) {
  if (!ChartAttached(state) || state->inspection_label == NULL)
    return;
  const char *message = "Move across the price chart to inspect an observed candle.";
  char details[512];
  UmiChartPoint pointer;
  if (state->have_pointer && state->scene != NULL &&
      UmiChartPlotUnmap(&state->info.price, state->pointer_x, state->pointer_y, &pointer) ==
          UMI_STATUS_OK) {
    UmiChartCandleInspection observation;
    UmiStatus status = UmiChartCandleInspect(state->inspection_candles, state->inspection_count,
                                             state->info.window.first, state->info.window.count,
                                             pointer.time_ms, &observation);
    if (status == UMI_STATUS_OK) {
      char timestamp[64];
      if (UmiChartTimeframeFormatUtc(observation.candle.time_ms, 0, timestamp, sizeof(timestamp)) !=
          UMI_STATUS_OK)
        (void)snprintf(timestamp, sizeof(timestamp), "%s", "Time unavailable");
      (void)snprintf(
          details, sizeof(details),
          "Nearest candle | %s UTC | O %.8g  H %.8g  L %.8g  C %.8g  Volume %.8g | offset %llu ms",
          timestamp, observation.candle.open, observation.candle.high, observation.candle.low,
          observation.candle.close, observation.candle.volume,
          (unsigned long long)observation.distance_ms);
      message = details;
    } else
      message = "No observed candle is available for this chart position.";
  }
  /* Keep this publication last: notification handlers may detach the chart. */
  gtk_label_set_text(GTK_LABEL(state->inspection_label), message);
}
static void ChartCreateCandleInspection(InteractiveChart *state) {
  state->inspection_label = gtk_label_new("");
  gtk_label_set_xalign(GTK_LABEL(state->inspection_label), 0.0f);
  gtk_label_set_wrap(GTK_LABEL(state->inspection_label), TRUE);
  gtk_label_set_selectable(GTK_LABEL(state->inspection_label), TRUE);
  gtk_widget_set_tooltip_text(
      state->inspection_label,
      "O: open; H: high; L: low; C: close. Nearest visible candle, not an interpolated trade. "
      "The offset is the distance between your cursor time and the candle's opening time.");
  (void)umi_gtk4_automation_tag_widget(state->inspection_label, "trading.chart.candle-inspection");
  gtk_box_append(GTK_BOX(state->root), state->inspection_label);
}
