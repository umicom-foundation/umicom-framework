/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_history.inc
 *
 * PURPOSE:
 *   Present finite broker chart captures with retained-range navigation and explicit observation identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Request controls are intentions. The captured query remains the authority
 * for every title, range and price label, including after the user edits fields. */
static gboolean WorkspaceHistoryPending(const UmiBrokerWorkspaceGtk *owner)
{
    return owner->hasHistory && owner->history.pending && !owner->history.failed &&
        !owner->history.cancelled;
}
static void WorkspaceHistoryRender(UmiBrokerWorkspaceGtk *owner)
{
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->historyBusy;
    gboolean pending = WorkspaceHistoryPending(owner);
    GtkWidget *fields[] = {owner->historyRead, owner->historyCancel, owner->historySize,
        owner->historyDuration, owner->historyKind, owner->historyEnd, owner->historyRth};
    gboolean enabled[] = {ready && !pending, ready && pending, !pending && !owner->historyBusy,
        !pending && !owner->historyBusy, !pending && !owner->historyBusy,
        !pending && !owner->historyBusy, !pending && !owner->historyBusy};
    for (size_t i = 0U; i < G_N_ELEMENTS(fields); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(fields[i], enabled[i]);
    }
    if (owner->closed) return;
    UmiIbkrHistoricalSnapshot *capture = &owner->history;
    gboolean usable = owner->hasHistory && capture->complete && !capture->stale &&
        !capture->failed && !capture->cancelled && capture->count > 0U;
    WorkspaceHistoryCanvas *canvas = owner->historyCanvas;
    size_t first = 0U, count = 0U;
    if (usable) {
        size_t offset = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(owner->historyOffset));
        size_t visible = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(owner->historyVisible));
        if (offset >= capture->count) offset = capture->count - 1U;
        size_t end = capture->count - offset;
        count = visible < end ? visible : end;
        first = end - count;
        if (!canvas->attempted || canvas->request != capture->requestId ||
            canvas->first != first || canvas->count != count) {
            WorkspaceHistoryClear(canvas);
            canvas->request = capture->requestId;
            canvas->first = first;
            canvas->count = count;
            canvas->attempted = TRUE;
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
            canvas->status = UmiIbkrGtkCreateHistoryScene(owner->connection, capture->requestId,
                first, count, 300000U, &canvas->scene);
#else
            canvas->status = UMI_STATUS_NOT_IMPLEMENTED;
#endif
        }
    } else WorkspaceHistoryClear(canvas);
    gtk_widget_queue_draw(owner->historyDrawing);
    const char *kind = capture->query.dataKind == UMI_IBKR_HISTORY_TRADES ? "Trades" :
        capture->query.dataKind == UMI_IBKR_HISTORY_MIDPOINT ? "Midpoint" :
        capture->query.dataKind == UMI_IBKR_HISTORY_BID ? "Bid" :
        capture->query.dataKind == UMI_IBKR_HISTORY_ASK ? "Ask" : "Not selected";
    char *status = g_strdup_printf("Capture: contract %u on %s • %s • %u-second bars • %zu bars • %s\n%s",
        (unsigned)capture->query.contract.contractId, capture->query.contract.exchange, kind,
        (unsigned)capture->query.barSeconds, capture->count,
        capture->failed ? "Failed" : capture->cancelled ? "Cancelled" : capture->pending ? "Receiving" :
            capture->stale ? "STALE / unavailable" : capture->complete ? "Historical capture; not live" : "Not requested",
        capture->message);
    LabelText(owner->historyStatus, status);
    g_free(status);
    if (owner->closed) return;
    GString *details = g_string_new(NULL);
    if (usable) {
        g_string_append_printf(details, "Showing bars %zu–%zu of %zu. ", first + 1U, first + count, capture->count);
        UmiIbkrHistoricalBar bar;
        if (UmiIbkrGtkCopyHistoryBar(owner->connection, capture->requestId, first + count - 1U, &bar) == UMI_STATUS_OK) {
            GDateTime *stamp = g_date_time_new_from_unix_utc(bar.timeMilliseconds / 1000);
            char *utc = stamp == NULL ? NULL : g_date_time_format(stamp, "%Y-%m-%d %H:%M:%S UTC");
            g_string_append_printf(details, "Last visible: %s\nOpen %s • High %s • Low %s • Close %s • Volume %s",
                utc == NULL ? "Unavailable time" : utc, bar.open.reportedText, bar.high.reportedText,
                bar.low.reportedText, bar.close.reportedText,
                bar.volumeAvailable ? bar.volume.reportedText : "Unavailable");
            g_free(utc);
            if (stamp != NULL) g_date_time_unref(stamp);
        } else g_string_append(details, "The capture changed; refresh before reading its prices.");
        if (canvas->status != UMI_STATUS_OK)
            g_string_append_printf(details, "\nChart unavailable: %s. Retained reported values remain above.",
                umi_status_text(canvas->status));
    } else if (capture->complete && !capture->stale && capture->count == 0U)
        g_string_append(details, "Completed capture contains no bars. No prices are invented.");
    else g_string_append(details, "A complete, current capture is required before drawing a chart.");
#ifndef UMI_IBKR_HAS_HISTORY_CHART
    g_string_append(details, "\nThe Cairo chart adapter is not available in this build.");
#endif
    LabelText(owner->historyDetails, details->str);
    g_string_free(details, TRUE);
}
static void WorkspaceHistoryRefresh(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrHistoricalSnapshot capture;
    UmiStatus status = owner->connection == NULL ? UMI_STATUS_CANCELLED :
        UmiIbkrGtkCopyHistory(owner->connection, 300000U, &capture);
    if (status == UMI_STATUS_OK) {
        owner->history = capture;
        owner->hasHistory = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        memset(&owner->history, 0, sizeof(owner->history));
        owner->history.stale = true;
        owner->hasHistory = FALSE;
    } else if (owner->hasHistory) {
        owner->history.stale = true;
        owner->history.pending = false;
    }
    WorkspaceHistoryRender(owner);
}
static UmiStatus WorkspaceHistoryQuery(UmiBrokerWorkspaceGtk *owner, UmiIbkrHistoricalQuery *out)
{
    static const uint32_t sizes[] = {60U, 300U, 900U, 1800U, 3600U};
    guint size = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->historySize));
    guint kind = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->historyKind));
    guint64 contract;
    const char *identity = gtk_editable_get_text(GTK_EDITABLE(owner->quoteContract));
    const char *exchange = gtk_editable_get_text(GTK_EDITABLE(owner->quoteExchange));
    const char *end = gtk_editable_get_text(GTK_EDITABLE(owner->historyEnd));
    UmiIbkrHistoricalQuery query = {0};
    if (size >= G_N_ELEMENTS(sizes) || kind > 3U ||
        !g_ascii_string_to_unsigned(identity, 10, 1, (guint64)INT_MAX, &contract, NULL) ||
        strlen(exchange) >= sizeof(query.contract.exchange) || strlen(end) >= sizeof(query.endUtc))
        return UMI_STATUS_INVALID_ARGUMENT;
    query.contract.contractId = (uint32_t)contract;
    strcpy(query.contract.exchange, exchange);
    strcpy(query.endUtc, end);
    query.durationSeconds = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(owner->historyDuration));
    query.barSeconds = sizes[size];
    query.dataKind = (UmiIbkrHistoricalDataKind)(kind + 1U);
    query.regularHours = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->historyRth)) != FALSE;
    UmiStatus status = UmiIbkrHistoricalQueryValidate(&query);
    if (status == UMI_STATUS_OK) *out = query;
    return status;
}
static void WorkspaceHistoryRead(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->historyBusy) return;
    ++owner->references;
    owner->historyBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !WorkspaceHistoryPending(owner)) {
        UmiIbkrHistoricalQuery query;
        UmiStatus status = WorkspaceHistoryQuery(owner, &query);
        if (status == UMI_STATUS_OK) status = UmiIbkrGtkReadHistory(owner->connection, &query);
        if (!owner->closed) WorkspaceHistoryRefresh(owner);
        if (!owner->closed) {
            char *notice = status == UMI_STATUS_OK
                ? g_strdup("Historical capture requested. Wait for the complete response.")
                : g_strdup_printf("Capture not started: %s. Check the instrument, exchange, interval and UTC time. "
                    "Allow at least 15 seconds between bar requests; broker limits may also apply.", umi_status_text(status));
            LabelText(owner->historyNotice, notice);
            g_free(notice);
        }
    }
    if (!owner->closed) {
        owner->historyBusy = FALSE;
        WorkspaceHistoryRender(owner);
    }
    Release(owner);
}
static void WorkspaceHistoryCancel(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->historyBusy || !WorkspaceHistoryPending(owner)) return;
    ++owner->references;
    uint32_t expected = owner->history.requestId;
    owner->historyBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL) {
        UmiStatus status = owner->history.requestId != expected ? UMI_STATUS_NOT_FOUND :
            !WorkspaceHistoryPending(owner) ? UMI_STATUS_INVALID_STATE :
                UmiIbkrGtkCancelHistory(owner->connection, expected);
        if (!owner->closed) WorkspaceHistoryRefresh(owner);
        if (!owner->closed) LabelText(owner->historyNotice, status == UMI_STATUS_OK
            ? "Historical request cancelled. Orders and quote subscriptions are unchanged."
            : "The request could not be cancelled. Review its current state before trying again.");
    }
    if (!owner->closed) {
        owner->historyBusy = FALSE;
        WorkspaceHistoryRender(owner);
    }
    Release(owner);
}
static void WorkspaceHistoryRange(GtkSpinButton *spin, gpointer data)
{
    (void)spin;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->historyBusy) return;
    /* This refresh copies the existing capture and renders a selected range.
     * It does not queue a broker request when zooming or panning. */
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
}
static void WorkspaceHistoryField(GtkFlowBox *row, const char *title, GtkWidget *control)
{
    GtkWidget *field = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_append(GTK_BOX(field), Cell(title, TRUE));
    gtk_box_append(GTK_BOX(field), control);
    gtk_flow_box_insert(row, field, -1);
}
static GtkWidget *WorkspaceHistoryPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *query = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(query), GTK_SELECTION_NONE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(query), 1U);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(query), 6U);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(query), 8U);
    owner->historySize = gtk_drop_down_new_from_strings((const char *[]){"1 minute", "5 minutes", "15 minutes", "30 minutes", "1 hour", NULL});
    owner->historyKind = gtk_drop_down_new_from_strings((const char *[]){"Trades", "Midpoint", "Bid", "Ask", NULL});
    owner->historyDuration = gtk_spin_button_new_with_range(60.0, 86400.0, 60.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(owner->historyDuration), 3600.0);
    owner->historyEnd = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->historyEnd), "Latest, or YYYYMMDD HH:MM:SS UTC");
    owner->historyRth = gtk_check_button_new_with_label("Regular hours only");
    owner->historyRead = gtk_button_new_with_label("Read chart history");
    owner->historyCancel = gtk_button_new_with_label("Cancel chart request");
    owner->historyVisible = gtk_spin_button_new_with_range(1.0, 512.0, 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(owner->historyVisible), 80.0);
    owner->historyOffset = gtk_spin_button_new_with_range(0.0, 511.0, 1.0);
    owner->historyStatus = Cell("", FALSE);
    owner->historyNotice = Cell("", FALSE);
    owner->historyDetails = Cell("", FALSE);
    GtkWidget *named[] = {owner->historySize, owner->historyKind, owner->historyDuration, owner->historyEnd,
        owner->historyRead, owner->historyCancel, owner->historyVisible, owner->historyOffset,
        owner->historyStatus, owner->historyNotice, owner->historyDetails, owner->historyRth};
    const char *names[] = {"size", "kind", "duration", "end", "read", "cancel", "visible", "offset",
        "status", "notice", "details", "rth"};
    for (size_t i = 0U; i < G_N_ELEMENTS(named); ++i) {
        char *name = g_strconcat("broker-workspace-history-", names[i], NULL);
        gtk_widget_set_name(named[i], name); g_free(name);
    }
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Bar interval", owner->historySize);
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Price source", owner->historyKind);
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Duration (seconds)", owner->historyDuration);
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "End time (UTC)", owner->historyEnd);
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Session", owner->historyRth);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_append(GTK_BOX(actions), owner->historyRead);
    gtk_box_append(GTK_BOX(actions), owner->historyCancel);
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Capture", actions);
    GtkWidget *help = Cell("Choose the contract ID and exchange in Market data first. "
        "This page reads a finite historical capture. The last candle may be unfinished. "
        "Zoom and pan use retained bars; they do not request more data.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_box_append(GTK_BOX(page), help);
    gtk_box_append(GTK_BOX(page), query);
    GtkWidget *range = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(range), GTK_SELECTION_NONE);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(range), 2U);
    WorkspaceHistoryField(GTK_FLOW_BOX(range), "Visible bars (zoom)", owner->historyVisible);
    WorkspaceHistoryField(GTK_FLOW_BOX(range), "Bars back (pan)", owner->historyOffset);
    gtk_box_append(GTK_BOX(page), range);
    gtk_label_set_wrap(GTK_LABEL(owner->historyStatus), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->historyNotice), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->historyDetails), TRUE);
    gtk_box_append(GTK_BOX(page), owner->historyStatus);
    gtk_box_append(GTK_BOX(page), owner->historyNotice);
    owner->historyDrawing = gtk_drawing_area_new();
    gtk_widget_set_name(owner->historyDrawing, "broker-workspace-history-drawing");
    gtk_widget_set_hexpand(owner->historyDrawing, TRUE);
    gtk_widget_set_vexpand(owner->historyDrawing, TRUE);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(owner->historyDrawing), 280);
    owner->historyCanvas = g_new0(WorkspaceHistoryCanvas, 1);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(owner->historyDrawing), WorkspaceHistoryDraw,
        owner->historyCanvas, WorkspaceHistoryCanvasFree);
    gtk_box_append(GTK_BOX(page), owner->historyDrawing);
    gtk_box_append(GTK_BOX(page), owner->historyDetails);
    g_signal_connect(owner->historyRead, "clicked", G_CALLBACK(WorkspaceHistoryRead), owner);
    g_signal_connect(owner->historyCancel, "clicked", G_CALLBACK(WorkspaceHistoryCancel), owner);
    g_signal_connect(owner->historyVisible, "value-changed", G_CALLBACK(WorkspaceHistoryRange), owner);
    g_signal_connect(owner->historyOffset, "value-changed", G_CALLBACK(WorkspaceHistoryRange), owner);
    return page;
}
