/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/candle_range_gtk4.inc
 * PURPOSE: Compose owned candle measurements, keyboard selection and copied CSV evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Widget callbacks use the chart root as their lifetime anchor. The capture is
 * separate from the workspace: inspecting prices must never edit a ticket. */
static void ChartRangeClear(InteractiveChart *state) {
  UmiChartRangeCaptureClear(state->range_capture);
  state->range_pending = 0;
  state->range_reveal = 0;
  state->range_instrument[0] = '\0';
}
static void ChartRangeCancelPending(InteractiveChart *state) {
  if (state->range_pending)
    ChartRangeClear(state);
}
/* A changed projection invalidates an unfinished two-click gesture. Completed
 * evidence remains readable after pan/zoom and ordinary provider updates, but
 * an instrument or timeframe change cannot relabel that evidence. */
static void ChartRangeBeforeRefresh(InteractiveChart *state, const UmiTradingChartSceneInfo *next) {
  if (strcmp(state->info.instrument_id, next->instrument_id) != 0 ||
      state->info.interval_ms != next->interval_ms) {
    ChartRangeClear(state);
    return;
  }
  if (state->range_pending && (state->info.price.start_ms != next->price.start_ms ||
                               state->info.price.end_ms != next->price.end_ms ||
                               state->info.price.minimum_value != next->price.minimum_value ||
                               state->info.price.maximum_value != next->price.maximum_value ||
                               state->info.window.first != next->window.first ||
                               state->info.window.count != next->window.count))
    ChartRangeClear(state);
}
static void ChartRangePublish(InteractiveChart *state) {
  if (!ChartAttached(state) || state->range_label == NULL)
    return;
  UmiChartCandleRange range;
  const char *message = state->range_pending
                            ? "First candle captured. Choose the second candle; Escape cancels. "
                              "Changing the chart projection cancels this gesture."
                            : "Choose Measure on the drawing rail and click two candles, or "
                              "measure visible candle numbers below.";
  char text[1300];
  if (UmiChartRangeCaptureRead(state->range_capture, &range) == UMI_STATUS_OK) {
    char first[64], last[64], percent[80];
    if (UmiChartTimeframeFormatUtc(range.first_time_ms, 0, first, sizeof(first)) != UMI_STATUS_OK)
      (void)snprintf(first, sizeof(first), "%s", "Time unavailable");
    if (UmiChartTimeframeFormatUtc(range.last_time_ms, 0, last, sizeof(last)) != UMI_STATUS_OK)
      (void)snprintf(last, sizeof(last), "%s", "Time unavailable");
    if (range.has_percent_change)
      (void)snprintf(percent, sizeof(percent), "%.8g%%", range.percent_change);
    else
      (void)snprintf(percent, sizeof(percent), "%s", "unavailable for this opening price");
    (void)snprintf(text, sizeof(text),
                   "Captured %s | %s | %zu candles\n%s to %s UTC\n"
                   "Open %.8g | Close %.8g | Change %.8g | Percent %s\n"
                   "High %.8g | Low %.8g | Price range %.8g | Volume %.8g (provider units)\n"
                   "Opening-time span %llu ms | Largest opening-time step %llu ms\n"
                   "Rising %zu | Falling %zu | Unchanged %zu\n"
                   "Retained snapshot; new prices do not update this measurement. Opening-time "
                   "span excludes the final candle duration.",
                   state->range_instrument, UmiChartTimeframeName(state->range_interval),
                   range.count, first, last, range.open, range.close, range.change, percent,
                   range.high, range.low, range.price_range, range.volume,
                   (unsigned long long)range.elapsed_ms, (unsigned long long)range.largest_step_ms,
                   range.rising, range.falling, range.unchanged);
    message = text;
  }
  /* Retain the root across native notifications. A host may detach or remove
   * a panel while expanding its result section; never publish into a retired
   * chart after such a callback. Repeated timer refreshes do not reopen it. */
  GtkWidget *root = g_object_ref(state->root);
  if (state->range_reveal) {
    state->range_reveal = 0;
    gtk_expander_set_expanded(GTK_EXPANDER(state->range_section), TRUE);
  }
  char *display = g_utf8_make_valid(message, -1);
  if (ChartAttached(state))
    gtk_label_set_text(GTK_LABEL(state->range_label), display);
  g_free(display);
  g_object_unref(root);
}
static UmiStatus ChartRangeBegin(InteractiveChart *state, int64_t time) {
  if (state->range_capture == NULL)
    state->range_capture = UmiChartRangeCaptureCreate();
  if (state->range_capture == NULL)
    return UMI_STATUS_OUT_OF_MEMORY;
  UmiStatus status = UmiChartRangeCaptureBegin(state->range_capture, state->inspection_candles,
                                               state->inspection_count, state->info.window.first,
                                               state->info.window.count, time);
  if (status == UMI_STATUS_OK) {
    (void)snprintf(state->range_instrument, sizeof(state->range_instrument), "%s",
                   state->info.instrument_id);
    state->range_interval = state->info.interval_ms;
    state->range_captured_ms = (uint64_t)(g_get_monotonic_time() / 1000);
    state->range_pending = 1;
  }
  return status;
}
static void ChartRangeClick(InteractiveChart *state, int64_t time) {
  UmiStatus status;
  if (!state->range_pending)
    status = ChartRangeBegin(state, time);
  else {
    UmiChartCandleRange range;
    status = UmiChartRangeCaptureComplete(state->range_capture, time, &range);
    if (status == UMI_STATUS_OK)
      state->range_pending = 0;
  }
  if (status != UMI_STATUS_OK) {
    ChartRangeClear(state);
    ChartMessage(
        state, "Measurement unavailable. Choose valid retained candles with representable values.");
    return;
  }
  state->range_reveal = 1;
  GtkWidget *root = g_object_ref(state->root);
  gtk_widget_queue_draw(state->area);
  gtk_widget_grab_focus(state->area);
  if (ChartAttached(state))
    ChartRangePublish(state);
  g_object_unref(root);
}
static void ChartRangeArm(GtkToggleButton *button, gpointer root) {
  InteractiveChart *state = ChartState(root);
  if (!ChartAttached(state) || !gtk_toggle_button_get_active(button))
    return;
  ChartRangeCancelPending(state);
  state->tool = 9;
  state->have_first = 0;
  state->edit_id[0] = '\0';
  gtk_widget_queue_draw(state->area);
  ChartMessage(state, "Measure: click a starting candle and an ending candle. The first click "
                      "captures the visible data.");
}
static void ChartRangeResetClicked(GtkButton *button, gpointer root) {
  (void)button;
  InteractiveChart *state = ChartState(root);
  if (!ChartAttached(state))
    return;
  ChartRangeClear(state);
  gtk_widget_queue_draw(state->area);
  ChartRangePublish(state);
}
static void ChartRangeRowsClicked(GtkButton *button, gpointer root) {
  InteractiveChart *state = ChartState(root);
  if (!ChartAttached(state) || state->scene == NULL)
    return;
  size_t first = state->info.window.first, count = state->info.window.count;
  int all = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "range-visible"));
  size_t start =
      all ? 0U : (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(state->range_start)) - 1U;
  size_t end =
      all ? count - (count != 0U ? 1U : 0U)
          : (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(state->range_end)) - 1U;
  if (count == 0U || first > state->inspection_count || count > state->inspection_count - first ||
      start >= count || end >= count) {
    ChartMessage(state, "Choose candle numbers inside the current visible window, starting at 1.");
    return;
  }
  /* Capture both times before any widget notification. The second point uses
   * owned evidence even when a provider refresh follows this callback. */
  int64_t start_time = state->inspection_candles[first + start].time_ms;
  int64_t end_time = state->inspection_candles[first + end].time_ms;
  UmiStatus status = ChartRangeBegin(state, start_time);
  UmiChartCandleRange range;
  if (status == UMI_STATUS_OK)
    status = UmiChartRangeCaptureComplete(state->range_capture, end_time, &range);
  if (status != UMI_STATUS_OK) {
    ChartRangeClear(state);
    ChartMessage(
        state,
        "Measurement unavailable. Previous measurement cleared; inspect the retained chart.");
    return;
  }
  state->range_pending = 0;
  state->range_reveal = 1;
  gtk_widget_queue_draw(state->area);
  ChartRangePublish(state);
}
static void ChartRangeCopyClicked(GtkButton *button, gpointer root) {
  (void)button;
  InteractiveChart *state = ChartState(root);
  if (!ChartAttached(state))
    return;
  UmiCsvDocument *report = NULL;
  UmiStatus status =
      UmiChartRangeReportCsv(state->range_capture, state->range_instrument, state->range_interval,
                             state->range_captured_ms, &report);
  if (status != UMI_STATUS_OK) {
    ChartMessage(state, "Complete a candle measurement before copying its report.");
    return;
  }
  /* Clipboard publication copies owned report bytes and can notify the host.
   * No chart state is touched afterward. The operating system may share a
   * clipboard with other applications; this happens only on explicit Copy. */
  GdkClipboard *clipboard = gtk_widget_get_clipboard(state->root);
  gdk_clipboard_set_text(clipboard, UmiCsvDocumentData(report));
  UmiCsvDocumentDestroy(report);
}
static GtkWidget *ChartRangeButton(InteractiveChart *state, GtkWidget *box, const char *label,
                                   const char *tag, GCallback callback) {
  GtkWidget *button = gtk_button_new_with_label(label);
  (void)umi_gtk4_automation_tag_widget(button, tag);
  g_signal_connect_object(button, "clicked", callback, G_OBJECT(state->root), 0);
  gtk_box_append(GTK_BOX(box), button);
  return button;
}
static void ChartRangeCreate(InteractiveChart *state, GtkWidget *body, GtkWidget *rail) {
  GtkWidget *tool = gtk_toggle_button_new_with_label("M");
  gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(tool), GTK_TOGGLE_BUTTON(state->tool_buttons[0]));
  gtk_widget_set_size_request(tool, 36, 32);
  gtk_widget_set_tooltip_text(
      tool, "Measure candles: choose two points. Values snap to observed candles.");
  gtk_accessible_update_property(GTK_ACCESSIBLE(tool), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                 "Measure candles", -1);
  (void)umi_gtk4_automation_tag_widget(tool, "trading.chart.measure");
  g_signal_connect_object(tool, "toggled", G_CALLBACK(ChartRangeArm), G_OBJECT(state->root), 0);
  gtk_box_append(GTK_BOX(rail), tool);
  GtkWidget *section = gtk_expander_new("Candle measurement");
  state->range_section = section;
  (void)umi_gtk4_automation_tag_widget(section, "trading.chart.measurement");
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
  GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  state->range_label = gtk_label_new("");
  gtk_label_set_wrap(GTK_LABEL(state->range_label), TRUE);
  gtk_label_set_selectable(GTK_LABEL(state->range_label), TRUE);
  gtk_label_set_xalign(GTK_LABEL(state->range_label), 0.0f);
  (void)umi_gtk4_automation_tag_widget(state->range_label, "trading.chart.measurement-result");
  gtk_box_append(GTK_BOX(box), state->range_label);
  gtk_box_append(GTK_BOX(box), gtk_label_new("Visible candle numbers, from left to right:"));
  state->range_start =
      gtk_spin_button_new_with_range(1, UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY, 1);
  state->range_end =
      gtk_spin_button_new_with_range(1, UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY, 1);
  gtk_accessible_update_property(GTK_ACCESSIBLE(state->range_start), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                 "First visible candle number", -1);
  gtk_accessible_update_property(GTK_ACCESSIBLE(state->range_end), GTK_ACCESSIBLE_PROPERTY_LABEL,
                                 "Last visible candle number", -1);
  (void)umi_gtk4_automation_tag_widget(state->range_start, "trading.chart.measurement-start");
  (void)umi_gtk4_automation_tag_widget(state->range_end, "trading.chart.measurement-end");
  gtk_box_append(GTK_BOX(actions), state->range_start);
  gtk_box_append(GTK_BOX(actions), state->range_end);
  (void)ChartRangeButton(state, actions, "Measure rows", "trading.chart.measurement-rows",
                         G_CALLBACK(ChartRangeRowsClicked));
  GtkWidget *visible =
      ChartRangeButton(state, actions, "Measure visible", "trading.chart.measurement-visible",
                       G_CALLBACK(ChartRangeRowsClicked));
  g_object_set_data(G_OBJECT(visible), "range-visible", GINT_TO_POINTER(1));
  gtk_box_append(GTK_BOX(box), actions);
  GtkWidget *reports = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  (void)ChartRangeButton(state, reports, "Copy CSV", "trading.chart.measurement-copy",
                         G_CALLBACK(ChartRangeCopyClicked));
  (void)ChartRangeButton(state, reports, "Clear measurement", "trading.chart.measurement-clear",
                         G_CALLBACK(ChartRangeResetClicked));
  gtk_box_append(GTK_BOX(box), reports);
  gtk_expander_set_child(GTK_EXPANDER(section), box);
  gtk_box_insert_child_after(GTK_BOX(state->root), section, body);
}
/* Overlay only the measured time interval. Its prices are a retained snapshot
 * and may differ from newly arrived bars, so no live price level is implied. */
static void ChartRangeDraw(InteractiveChart *state, cairo_t *cr, int width, int height) {
  UmiChartCandleRange range;
  if (UmiChartRangeCaptureRead(state->range_capture, &range) != UMI_STATUS_OK)
    return;
  const UmiChartPlotViewport *view = &state->info.price;
  if (range.last_time_ms < view->start_ms || range.first_time_ms > view->end_ms)
    return;
  int64_t first = range.first_time_ms < view->start_ms ? view->start_ms : range.first_time_ms;
  int64_t last = range.last_time_ms > view->end_ms ? view->end_ms : range.last_time_ms;
  double left, right;
  if (umi_chart_plot_map_time(view, first, &left) != UMI_STATUS_OK ||
      umi_chart_plot_map_time(view, last, &right) != UMI_STATUS_OK)
    return;
  cairo_save(cr);
  cairo_scale(cr, (double)width / UMI_TRADING_CHART_WIDTH,
              (double)height / UMI_TRADING_CHART_HEIGHT);
  cairo_rectangle(cr, view->area.x, view->area.y, view->area.width, view->area.height);
  cairo_clip(cr);
  cairo_set_source_rgba(cr, 0.98, 0.72, 0.20, 0.12);
  cairo_rectangle(cr, left, view->area.y, fmax(1.0, right - left), view->area.height);
  cairo_fill(cr);
  cairo_set_source_rgba(cr, 0.98, 0.72, 0.20, 0.85);
  cairo_set_line_width(cr, 1.2);
  cairo_move_to(cr, left, view->area.y);
  cairo_line_to(cr, left, view->area.y + view->area.height);
  cairo_move_to(cr, right, view->area.y);
  cairo_line_to(cr, right, view->area.y + view->area.height);
  cairo_stroke(cr);
  cairo_restore(cr);
}
