/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_streaming.inc
 * PURPOSE: Present explicit five-second subscriptions through the shared chart and connection owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void StreamingClearCanvas(ConnectionUi *ui)
{
    HistoricalCanvas *canvas = ui->streamCanvas;
    if (!canvas)
        return;
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
    umi_chart_render_scene_destroy(canvas->scene);
#endif
    canvas->scene = NULL;
    canvas->request = 0U;
    canvas->receivedBars = 0U;
}
static void StreamingRetire(ConnectionUi *ui)
{
    ui->streamRequest = 0U;
    StreamingClearCanvas(ui);
}
static void StreamingPublish(ConnectionUi *ui, uint64_t now)
{
    UmiIbkrRealtimeSnapshot s = {0};
    UmiStatus status = ui->streamRequest
                           ? UmiIbkrRealtimeCopy(ui->connection, ui->streamRequest, now, 15000U, &s)
                           : UMI_STATUS_NOT_FOUND;
    GString *text =
        g_string_new(status == UMI_STATUS_OK ? s.message : "No streaming subscription on this connection.");
    if (status == UMI_STATUS_OK)
    {
        g_string_append_printf(
            text,
            "\nContract %u on %s | retained %zu | received %" G_GUINT64_FORMAT
            " | discarded %" G_GUINT64_FORMAT " | gaps %" G_GUINT64_FORMAT "\n%s | %s",
            (unsigned)s.query.contract.contractId, s.query.contract.exchange, s.count,
            (guint64)s.receivedBars, (guint64)s.droppedBars, (guint64)s.gapEvents,
            s.stale ? "Stale: waiting, stopped or no new bar within 15 seconds" : "Recent bar received",
            s.needsCancel ? "Cancel before replacing this subscription" : "No subscription to cancel");
    }
    if (status == UMI_STATUS_OK && !s.stale && s.count)
    {
        size_t visible = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->streamVisible));
        size_t offset = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->streamOffset));
        if (offset >= s.count)
            offset = s.count - 1U;
        size_t end = s.count - offset;
        if (visible > end)
            visible = end;
        size_t first = end - visible;
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
        HistoricalCanvas *canvas = ui->streamCanvas;
        if (!canvas->scene || canvas->request != s.requestId || canvas->receivedBars != s.receivedBars ||
            canvas->first != first || canvas->count != visible)
        {
            UmiChartRenderScene *scene = NULL;
            status =
                UmiIbkrRealtimeSceneCreate(ui->connection, s.requestId, first, visible, now, 15000U, &scene);
            umi_chart_render_scene_destroy(canvas->scene);
            canvas->scene = scene;
            if (status == UMI_STATUS_OK)
            {
                canvas->request = s.requestId;
                canvas->receivedBars = s.receivedBars;
                canvas->first = first;
                canvas->count = visible;
            }
        }
#endif
        if (status != UMI_STATUS_OK)
            g_string_append_printf(text, "\nChart unavailable: %s; raw decimal records are retained.",
                                   umi_status_text(status));
        UmiIbkrRealtimeBar bar;
        if (UmiIbkrRealtimeBarCopy(ui->connection, s.requestId, end - 1U, &bar) == UMI_STATUS_OK)
        {
            GDateTime *stamp = g_date_time_new_from_unix_utc(bar.timeMilliseconds / 1000);
            gchar *time = stamp ? g_date_time_format(stamp, "%Y-%m-%d %H:%M:%S UTC") : NULL;
            g_string_append_printf(text,
                                   "\nBars %zu-%zu | last visible %s\nO %s | H %s | L %s | C %s | Volume %s",
                                   first + 1U, end, time ? time : "unavailable", bar.open.reportedText,
                                   bar.high.reportedText, bar.low.reportedText, bar.close.reportedText,
                                   bar.volumeAvailable ? bar.volume.reportedText : "unavailable");
            g_free(time);
            if (stamp)
                g_date_time_unref(stamp);
        }
    }
    else
        StreamingClearCanvas(ui);
#ifndef UMI_IBKR_HAS_HISTORY_CHART
    g_string_append(text, "\nThis build has no Cairo chart adapter; the bar summary remains available.");
#endif
    /* Label observers may close the window. No child widgets are accessed after
     * this notification, and the outer FillEnter guard holds the owner alive. */
    gtk_widget_queue_draw(ui->streamDrawing);
    if (!ui->closed && strcmp(gtk_label_get_text(GTK_LABEL(ui->streamOutput)), text->str))
        gtk_label_set_text(GTK_LABEL(ui->streamOutput), text->str);
    g_string_free(text, TRUE);
}
static void StreamingPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy)
        return;
    ui = FillEnter(ui->window);
    if (!ui)
        return;
    StreamingPublish(ui, now);
    FillLeave(ui);
}
static void StreamingStartClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    UmiIbkrRealtimeQuery q = {0};
    guint kind = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->streamKind));
    guint64 contract;
    const char *identity = gtk_editable_get_text(GTK_EDITABLE(ui->quoteContract));
    const char *exchange = gtk_editable_get_text(GTK_EDITABLE(ui->quoteExchange));
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    if (kind < 4U && g_ascii_string_to_unsigned(identity, 10, 1, (guint64)INT_MAX, &contract, NULL) &&
        strlen(exchange) < sizeof q.contract.exchange)
    {
        q.contract.contractId = (uint32_t)contract;
        strcpy(q.contract.exchange, exchange);
        q.dataKind = (UmiIbkrHistoricalDataKind)(kind + 1U);
        q.regularHours = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->streamRth)) != FALSE;
        UmiIbkrRealtimeSnapshot previous;
        /* Keep the panel's existing request visible until it has been cancelled.
         * Otherwise a new selection could hide an active broker subscription. */
        if (ui->streamRequest &&
            UmiIbkrRealtimeCopy(ui->connection, ui->streamRequest, now, 15000U, &previous) == UMI_STATUS_OK &&
            previous.needsCancel)
            status = UMI_STATUS_BUSY;
        else
            status = UmiIbkrRealtimeRequest(ui->connection, &q, now, &ui->streamRequest);
    }
    if (status == UMI_STATUS_OK)
        StreamingPublish(ui, now);
    else
    {
        gchar *message =
            g_strdup_printf("Streaming subscription not started: %s. Check the contract and route, "
                            "cancel the previous subscription, and allow 15 seconds between bar requests.",
                            umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(ui->streamOutput), message);
        g_free(message);
    }
    FillLeave(ui);
}
static void StreamingStopClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrRealtimeCancel(ui->connection, ui->streamRequest, now);
    if (status == UMI_STATUS_OK)
        StreamingPublish(ui, now);
    else
        gtk_label_set_text(GTK_LABEL(ui->streamOutput), "No streaming subscription was cancelled.");
    FillLeave(ui);
}
static void StreamingRangeChanged(GtkSpinButton *spin, gpointer data)
{
    (void)spin;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    StreamingPublish(ui, UmiIbkrMonotonicMilliseconds());
    FillLeave(ui);
}
static void StreamingControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Streaming five-second chart");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8), *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    const char *kinds[] = {"Trades", "Midpoint", "Bid", "Ask", NULL};
    ui->streamKind = gtk_drop_down_new_from_strings(kinds);
    ui->streamRth = gtk_check_button_new_with_label("Regular trading hours only");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->streamRth), TRUE);
    ui->streamVisible = gtk_spin_button_new_with_range(1, 512, 16);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->streamVisible), 128);
    ui->streamOffset = gtk_spin_button_new_with_range(0, 511, 16);
    Field(GTK_GRID(grid), "Data", ui->streamKind, 0, "ibkr-stream-kind");
    Field(GTK_GRID(grid), "Visible bars (zoom)", ui->streamVisible, 1, "ibkr-stream-visible");
    Field(GTK_GRID(grid), "Older bars offset (pan)", ui->streamOffset, 2, "ibkr-stream-offset");
    ui->streamStart = gtk_button_new_with_label("Start five-second bars");
    ui->streamStop = gtk_button_new_with_label("Cancel streaming bars");
    ui->streamOutput = Text("No streaming subscription on this connection.");
    gtk_label_set_selectable(GTK_LABEL(ui->streamOutput), TRUE);
    ui->streamDrawing = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(ui->streamDrawing), 960);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(ui->streamDrawing), 340);
    gtk_widget_set_name(ui->streamStart, "ibkr-stream-start");
    gtk_widget_set_name(ui->streamStop, "ibkr-stream-stop");
    gtk_widget_set_name(ui->streamRth, "ibkr-stream-rth");
    gtk_widget_set_name(ui->streamOutput, "ibkr-stream-output");
    gtk_widget_set_name(ui->streamDrawing, "ibkr-stream-chart");
    /* Reuse the owned scene and weak-window drawing callback. The canvas has
     * no borrowed bar pointer and stays harmless if retained after UI closure. */
    ui->streamCanvas = g_new0(HistoricalCanvas, 1);
    g_weak_ref_init(&ui->streamCanvas->window, window);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(ui->streamDrawing), HistoricalDraw, ui->streamCanvas,
                                   HistoricalCanvasFree);
    gtk_box_append(
        GTK_BOX(box),
        Text("Use the Contract ID and Exchange above. New bars enter a rolling "
             "512-bar window. A gap may reflect closed trading hours; it is not filled with invented prices. "
             "Volume remains in the broker's reported units."));
    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_append(GTK_BOX(box), ui->streamRth);
    gtk_box_append(GTK_BOX(box), ui->streamStart);
    gtk_box_append(GTK_BOX(box), ui->streamStop);
    gtk_box_append(GTK_BOX(box), ui->streamDrawing);
    gtk_box_append(GTK_BOX(box), ui->streamOutput);
    StreamingExportControls(window, ui, box);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->streamStart, "clicked", G_CALLBACK(StreamingStartClicked), window, 0);
    g_signal_connect_object(ui->streamStop, "clicked", G_CALLBACK(StreamingStopClicked), window, 0);
    g_signal_connect_object(ui->streamVisible, "value-changed", G_CALLBACK(StreamingRangeChanged), window, 0);
    g_signal_connect_object(ui->streamOffset, "value-changed", G_CALLBACK(StreamingRangeChanged), window, 0);
}
