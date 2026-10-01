/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/trading_ui/chart_timeframe_gtk4.inc
 * PURPOSE: Bind an instrument-scoped timeframe selector to the shared candle projection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static const uint32_t ChartIntervals[] = {0U, 60000U, 300000U, 900000U, 3600000U, 14400000U, 86400000U};
static void ChartTimeframeRefresh(InteractiveChart *state)
{
    if (!ChartAttached(state) || state->timeframe == NULL) return;
    UmiTradingWorkspaceSnapshot snapshot; UmiChartNavigation navigation;
    if (umi_trading_workspace_snapshot(state->context->workspace, &snapshot) != UMI_STATUS_OK ||
        UmiTradingWorkspaceGetChartNavigation(state->context->workspace, snapshot.selected_instrument_id, &navigation) != UMI_STATUS_OK) return;
    guint selected = 0U;
    for (guint i = 0U; i < G_N_ELEMENTS(ChartIntervals); ++i) if (ChartIntervals[i] == navigation.interval_ms) selected = i;
    state->timeframe_refresh = 1;
    (void)snprintf(state->timeframe_instrument, sizeof state->timeframe_instrument, "%s", snapshot.selected_instrument_id);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(state->timeframe), selected);
    state->timeframe_refresh = 0;
}
static void ChartTimeframeChanged(GObject *object, GParamSpec *property, gpointer root)
{
    (void)property;
    InteractiveChart *state = ChartState(root);
    if (!ChartAttached(state) || state->timeframe_refresh) return;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(object));
    if (selected >= G_N_ELEMENTS(ChartIntervals)) return;
    UmiStatus status = UmiTradingWorkspaceSetChartTimeframe(state->context->workspace,
        state->timeframe_instrument, ChartIntervals[selected]);
    if (status == UMI_STATUS_OK) {
        ChartRefresh(state);
        ChartMessage(state, "Timeframe applied to retained observations. Edge buckets may be incomplete. Save chart keeps this setting.");
    } else {
        ChartTimeframeRefresh(state);
        ChartMessage(state, status == UMI_STATUS_UNAVAILABLE ?
            "This interval would split a source bar. Choose a compatible larger interval or Source." :
            "Timeframe unchanged. Refresh the selected instrument and check its retained bars.");
    }
}
static void ChartTimeframeCreate(InteractiveChart *state, GtkWidget *top)
{
    const char *names[] = {"Source", "1m", "5m", "15m", "1h", "4h", "1d UTC", NULL};
    state->timeframe = gtk_drop_down_new_from_strings(names);
    (void)umi_gtk4_automation_tag_widget(state->timeframe, "trading.chart.timeframe");
    gtk_accessible_update_property(GTK_ACCESSIBLE(state->timeframe), GTK_ACCESSIBLE_PROPERTY_LABEL, "Chart timeframe", -1);
    gtk_widget_set_tooltip_text(state->timeframe,
        "Aggregate retained observations into fixed UTC buckets. No history download or missing-bar fill; edge buckets can be incomplete.");
    gtk_box_append(GTK_BOX(top), gtk_label_new("Timeframe"));
    gtk_box_append(GTK_BOX(top), state->timeframe);
    g_signal_connect_object(state->timeframe, "notify::selected", G_CALLBACK(ChartTimeframeChanged), G_OBJECT(state->root), 0);
}
