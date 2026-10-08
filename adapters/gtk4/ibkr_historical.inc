/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_historical.inc
 * PURPOSE: Present finite broker chart captures using shared request and rendering owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* A retained drawing widget owns its scene, while the weak window link prevents
 * a later paint from accessing a closed connection window. */
struct HistoricalCanvas
{
    GWeakRef window;
    UmiChartRenderScene *scene;
    uint32_t request;
    size_t first, count;
    uint64_t receivedBars; /* Streaming callers use this to retire cached pixels. */
};
static void HistoricalCanvasFree(gpointer data)
{
    HistoricalCanvas *canvas = data;
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
    umi_chart_render_scene_destroy(canvas->scene);
#endif
    g_weak_ref_clear(&canvas->window);
    g_free(canvas);
}
static void HistoricalRetire(ConnectionUi *ui)
{
    ui->historyRequest = 0U;
    if (ui->historyCanvas)
    {
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
        umi_chart_render_scene_destroy(ui->historyCanvas->scene);
#endif
        ui->historyCanvas->scene = NULL;
        ui->historyCanvas->request = 0U;
    }
}
static void HistoricalDraw(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer data)
{
    (void)area;
    HistoricalCanvas *canvas = data;
    GtkWindow *window = g_weak_ref_get(&canvas->window);
    if (!window)
        return;
    ConnectionUi *ui = Owner(window);
    if (ui && canvas->scene && width > 0 && height > 0)
    {
#ifdef UMI_IBKR_HAS_HISTORY_CHART
        UmiChartCairoRendererContext context;
        UmiChartRenderer renderer;
        if (umi_chart_cairo_renderer_init(&context, cr, &renderer) == UMI_STATUS_OK)
            (void)umi_chart_renderer_render_scene(&renderer, canvas->scene, (double)width, (double)height);
#else
        (void)cr;
#endif
    }
    g_object_unref(window);
}
static UmiStatus HistoricalReadQuery(ConnectionUi *ui, UmiIbkrHistoricalQuery *out)
{
    static const uint32_t sizes[] = {60U, 300U, 900U, 1800U, 3600U};
    guint size = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->historySize));
    guint kind = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->historyKind));
    guint64 contract;
    const char *identity = gtk_editable_get_text(GTK_EDITABLE(ui->quoteContract));
    const char *exchange = gtk_editable_get_text(GTK_EDITABLE(ui->quoteExchange));
    const char *end = gtk_editable_get_text(GTK_EDITABLE(ui->historyEnd));
    UmiIbkrHistoricalQuery q = {0};
    if (size >= G_N_ELEMENTS(sizes) || kind > 3U ||
        !g_ascii_string_to_unsigned(identity, 10, 1, (guint64)INT_MAX, &contract, NULL) ||
        strlen(exchange) >= sizeof q.contract.exchange || strlen(end) >= sizeof q.endUtc)
        return UMI_STATUS_INVALID_ARGUMENT;
    q.contract.contractId = (uint32_t)contract;
    strcpy(q.contract.exchange, exchange);
    strcpy(q.endUtc, end);
    q.durationSeconds = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->historyDuration));
    q.barSeconds = sizes[size];
    q.dataKind = (UmiIbkrHistoricalDataKind)(kind + 1U);
    q.regularHours = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->historyRth)) != FALSE;
    UmiStatus status = UmiIbkrHistoricalQueryValidate(&q);
    if (status == UMI_STATUS_OK)
        *out = q;
    return status;
}
static void HistoricalPublish(ConnectionUi *ui, uint64_t now)
{
    UmiIbkrHistoricalSnapshot capture = {0};
    UmiStatus status = ui->historyRequest
                           ? UmiIbkrHistoricalCopy(ui->connection, ui->historyRequest, now, 300000U, &capture)
                           : UMI_STATUS_NOT_FOUND;
    HistoricalCanvas *canvas = ui->historyCanvas;
    GString *text =
        g_string_new(status == UMI_STATUS_OK ? capture.message : "No historical capture on this connection.");
    if (status == UMI_STATUS_OK)
        g_string_append_printf(
            text, "\nContract %u on %s | %u-second bars | %zu bars | %s\nBroker range: %s to %s",
            (unsigned)capture.query.contract.contractId, capture.query.contract.exchange,
            (unsigned)capture.query.barSeconds, capture.count,
            capture.stale ? "stale or incomplete" : "captured; not live", capture.startText, capture.endText);
    if (status == UMI_STATUS_OK && capture.complete && !capture.stale && capture.count)
    {
        size_t visible = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->historyVisible));
        size_t offset = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->historyOffset));
        if (offset >= capture.count)
            offset = capture.count - 1U;
        size_t remaining = capture.count - offset;
        if (visible > remaining)
            visible = remaining;
        size_t first = remaining - visible;
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
        if (!canvas->scene || canvas->request != capture.requestId || canvas->first != first ||
            canvas->count != visible)
        {
            UmiChartRenderScene *scene = NULL;
            status = UmiIbkrHistoricalSceneCreate(ui->connection, capture.requestId, first, visible, now,
                                                  300000U, &scene);
            umi_chart_render_scene_destroy(canvas->scene);
            canvas->scene = scene;
            if (status == UMI_STATUS_OK)
            {
                canvas->request = capture.requestId;
                canvas->first = first;
                canvas->count = visible;
            }
        }
#endif
        if (status != UMI_STATUS_OK)
            g_string_append_printf(text,
                                   "\nChart unavailable: %s. Retained decimal records remain available.",
                                   umi_status_text(status));
        UmiIbkrHistoricalBar bar;
        if (UmiIbkrHistoricalBarCopy(ui->connection, capture.requestId, remaining - 1U, &bar) ==
            UMI_STATUS_OK)
        {
            GDateTime *stamp = g_date_time_new_from_unix_utc(bar.timeMilliseconds / 1000);
            gchar *time = stamp ? g_date_time_format(stamp, "%Y-%m-%d %H:%M:%S UTC") : NULL;
            g_string_append_printf(
                text, "\nShowing bars %zu-%zu. Last visible: %s\nO %s | H %s | L %s | C %s | Volume %s",
                first + 1U, remaining, time ? time : "unavailable", bar.open.reportedText,
                bar.high.reportedText, bar.low.reportedText, bar.close.reportedText,
                bar.volumeAvailable ? bar.volume.reportedText : "unavailable");
            g_free(time);
            if (stamp)
                g_date_time_unref(stamp);
        }
    }
    else
    {
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
        umi_chart_render_scene_destroy(canvas->scene);
#endif
        canvas->scene = NULL;
        canvas->request = 0U;
    }
#ifndef UMI_IBKR_HAS_HISTORY_CHART
    g_string_append(text, "\nThis build has no Cairo chart adapter; the capture summary remains available.");
#endif
    gtk_widget_queue_draw(ui->historyDrawing);
    if (!ui->closed && strcmp(gtk_label_get_text(GTK_LABEL(ui->historyOutput)), text->str))
        gtk_label_set_text(GTK_LABEL(ui->historyOutput), text->str);
    g_string_free(text, TRUE);
}
static void HistoricalPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy)
        return;
    ui = FillEnter(ui->window);
    if (!ui)
        return;
    HistoricalPublish(ui, now);
    FillLeave(ui);
}
static void HistoricalReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    UmiIbkrHistoricalQuery q;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = HistoricalReadQuery(ui, &q);
    if (status == UMI_STATUS_OK)
        status = UmiIbkrHistoricalRequest(ui->connection, &q, now, &ui->historyRequest);
    if (status == UMI_STATUS_OK)
        HistoricalPublish(ui, now);
    else
    {
        gchar *message = g_strdup_printf("Historical capture not started: %s. Check the contract, route, "
                                         "duration and UTC end time; allow 15 seconds between requests.",
                                         umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(ui->historyOutput), message);
        g_free(message);
    }
    FillLeave(ui);
}
static void HistoricalCancelClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrHistoricalCancel(ui->connection, ui->historyRequest, now);
    if (status == UMI_STATUS_OK)
        HistoricalPublish(ui, now);
    else
        gtk_label_set_text(GTK_LABEL(ui->historyOutput), "No pending historical request was cancelled.");
    FillLeave(ui);
}
static void HistoricalRangeChanged(GtkSpinButton *spin, gpointer data)
{
    (void)spin;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    HistoricalPublish(ui, UmiIbkrMonotonicMilliseconds());
    FillLeave(ui);
}
static void HistoricalControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Historical candlestick chart");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8), *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    const char *sizes[] = {"1 minute", "5 minutes", "15 minutes", "30 minutes", "1 hour", NULL};
    const char *kinds[] = {"Trades", "Midpoint", "Bid", "Ask", NULL};
    ui->historySize = gtk_drop_down_new_from_strings(sizes);
    ui->historyKind = gtk_drop_down_new_from_strings(kinds);
    ui->historyDuration = gtk_spin_button_new_with_range(60, 86400, 60);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->historyDuration), 14400);
    ui->historyEnd = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->historyEnd), "Empty for latest, or YYYYMMDD HH:MM:SS UTC");
    ui->historyRth = gtk_check_button_new_with_label("Regular trading hours only");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->historyRth), TRUE);
    ui->historyVisible = gtk_spin_button_new_with_range(1, 512, 16);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->historyVisible), 128);
    ui->historyOffset = gtk_spin_button_new_with_range(0, 511, 16);
    Field(GTK_GRID(grid), "Bar size", ui->historySize, 0, "ibkr-history-size");
    Field(GTK_GRID(grid), "Data", ui->historyKind, 1, "ibkr-history-kind");
    Field(GTK_GRID(grid), "Duration in seconds", ui->historyDuration, 2, "ibkr-history-duration");
    Field(GTK_GRID(grid), "End in UTC", ui->historyEnd, 3, "ibkr-history-end");
    Field(GTK_GRID(grid), "Visible bars (zoom)", ui->historyVisible, 4, "ibkr-history-visible");
    Field(GTK_GRID(grid), "Older bars offset (pan)", ui->historyOffset, 5, "ibkr-history-offset");
    ui->historyRead = gtk_button_new_with_label("Read historical bars");
    ui->historyCancel = gtk_button_new_with_label("Cancel historical request");
    ui->historyOutput = Text("No historical capture on this connection.");
    ui->historyDrawing = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(ui->historyDrawing), 960);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(ui->historyDrawing), 340);
    gtk_widget_set_name(ui->historyRead, "ibkr-history-read");
    gtk_widget_set_name(ui->historyCancel, "ibkr-history-cancel");
    gtk_widget_set_name(ui->historyOutput, "ibkr-history-output");
    gtk_widget_set_name(ui->historyRth, "ibkr-history-rth");
    gtk_widget_set_name(ui->historyDrawing, "ibkr-history-chart");
    gtk_label_set_selectable(GTK_LABEL(ui->historyOutput), TRUE);
    ui->historyCanvas = g_new0(HistoricalCanvas, 1);
    g_weak_ref_init(&ui->historyCanvas->window, window);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(ui->historyDrawing), HistoricalDraw, ui->historyCanvas,
                                   HistoricalCanvasFree);
    gtk_box_append(GTK_BOX(box),
                   Text("Use the Contract ID and Exchange fields above. Capture up to 512 intraday bars. "
                        "Zoom and pan change the captured view without requesting more data."));
    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_append(GTK_BOX(box), ui->historyRth);
    gtk_box_append(GTK_BOX(box), ui->historyRead);
    gtk_box_append(GTK_BOX(box), ui->historyCancel);
    gtk_box_append(GTK_BOX(box), ui->historyDrawing);
    gtk_box_append(GTK_BOX(box), ui->historyOutput);
    HistoricalExportControls(window, ui, box);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->historyRead, "clicked", G_CALLBACK(HistoricalReadClicked), window, 0);
    g_signal_connect_object(ui->historyCancel, "clicked", G_CALLBACK(HistoricalCancelClicked), window, 0);
    g_signal_connect_object(ui->historyVisible, "value-changed", G_CALLBACK(HistoricalRangeChanged), window,
                            0);
    g_signal_connect_object(ui->historyOffset, "value-changed", G_CALLBACK(HistoricalRangeChanged), window,
                            0);
}
