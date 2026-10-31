/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_streaming.inc
 *
 * PURPOSE:
 *   Display live broker bars with explicit subscription ownership, rolling history and retained chart navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The shared session owns transport and pacing. This page owns only retained
 * observations and an immutable drawing scene, so layout changes never resubscribe. */
static gboolean WorkspaceStreamActive(const UmiBrokerWorkspaceGtk *owner)
{
    return owner->hasStream && owner->stream.needsCancel;
}
static void WorkspaceStreamRender(UmiBrokerWorkspaceGtk *owner)
{
    gboolean active = WorkspaceStreamActive(owner);
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->streamBusy;
    GtkWidget *controls[] = {owner->streamStart, owner->streamStop, owner->streamKind, owner->streamRth};
    gboolean enabled[] = {ready && !active, ready && active, !active && !owner->streamBusy,
        !active && !owner->streamBusy};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(controls[i], enabled[i]);
    }
    if (owner->closed) return;
    UmiIbkrRealtimeSnapshot *capture = &owner->stream;
    gboolean usable = owner->hasStream && capture->active && !capture->stale &&
        !capture->failed && !capture->cancelled && capture->count > 0U;
    WorkspaceHistoryCanvas *canvas = owner->streamCanvas;
    size_t first = 0U, count = 0U;
    if (usable) {
        size_t offset = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(owner->streamOffset));
        size_t visible = (size_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(owner->streamVisible));
        if (offset >= capture->count) offset = capture->count - 1U;
        size_t end = capture->count - offset;
        count = visible < end ? visible : end; first = end - count;
        /* The rolling buffer keeps the same count after it fills. Its accepted
         * bar counter must therefore participate in the scene cache identity. */
        if (!canvas->attempted || canvas->request != capture->requestId ||
            canvas->received != capture->receivedBars || canvas->first != first || canvas->count != count) {
            WorkspaceHistoryClear(canvas);
            canvas->request = capture->requestId; canvas->received = capture->receivedBars;
            canvas->first = first; canvas->count = count; canvas->attempted = TRUE;
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
            canvas->status = UmiIbkrGtkCreateRealtimeScene(owner->connection, capture->requestId,
                first, count, 15000U, &canvas->scene);
#else
            canvas->status = UMI_STATUS_NOT_IMPLEMENTED;
#endif
        }
    } else WorkspaceHistoryClear(canvas);
    gtk_widget_queue_draw(owner->streamDrawing);
    const char *kind = capture->query.dataKind == UMI_IBKR_HISTORY_TRADES ? "Trades" :
        capture->query.dataKind == UMI_IBKR_HISTORY_MIDPOINT ? "Midpoint" :
        capture->query.dataKind == UMI_IBKR_HISTORY_BID ? "Bid" :
        capture->query.dataKind == UMI_IBKR_HISTORY_ASK ? "Ask" : "Not selected";
    char *status = owner->hasStream ? g_strdup_printf(
        "Contract %u on %s • %s • five-second bars • %s%s\n"
        "%zu retained • %" PRIu64 " received • %" PRIu64 " evicted • %" PRIu64 " gaps • %" PRIu64 " duplicates\n%s",
        (unsigned)capture->query.contract.contractId, capture->query.contract.exchange, kind,
        capture->failed ? "Failed" : capture->cancelled ? "Stopped" :
        capture->active ? "Subscribed" : "Inactive",
        capture->stale ? " • STALE / unavailable" : " • Recent receipt",
        capture->count, capture->receivedBars, capture->droppedBars, capture->gapEvents,
        capture->duplicateBars, capture->message)
        : g_strdup("No live bars requested. Select a contract and exchange in Market data.");
    LabelText(owner->streamStatus, status); g_free(status);
    if (owner->closed) return;
    GString *details = g_string_new(NULL);
    if (usable) {
        g_string_append_printf(details, "Showing bars %zu–%zu of %zu. ", first + 1U, first + count, capture->count);
        UmiIbkrRealtimeBar bar;
        if (UmiIbkrGtkCopyRealtimeBar(owner->connection, capture->requestId, first + count - 1U, &bar) == UMI_STATUS_OK) {
            GDateTime *stamp = g_date_time_new_from_unix_utc(bar.timeMilliseconds / 1000);
            char *utc = stamp == NULL ? NULL : g_date_time_format(stamp, "%Y-%m-%d %H:%M:%S UTC");
            g_string_append_printf(details, "Last visible: %s\nOpen %s • High %s • Low %s • Close %s • Volume %s",
                utc == NULL ? "Unavailable time" : utc, bar.open.reportedText, bar.high.reportedText,
                bar.low.reportedText, bar.close.reportedText,
                bar.volumeAvailable ? bar.volume.reportedText : "Unavailable");
            g_free(utc);
            if (stamp != NULL) g_date_time_unref(stamp);
        } else g_string_append(details, "The stream changed; refresh before reading its prices.");
        if (canvas->status != UMI_STATUS_OK)
            g_string_append_printf(details, "\nChart unavailable: %s. Reported values remain above.",
                umi_status_text(canvas->status));
    } else g_string_append(details, capture->active && !capture->failed && capture->count == 0U
        ? "Waiting for the first bar. No candles are invented."
        : "A recent, active stream is required before drawing. Stopped or stale prices are not shown as live.");
#ifndef UMI_IBKR_HAS_HISTORY_CHART
    g_string_append(details, "\nThe Cairo chart adapter is not available in this build.");
#endif
    LabelText(owner->streamDetails, details->str); g_string_free(details, TRUE);
}
static void WorkspaceStreamRefresh(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrRealtimeSnapshot capture;
    UmiStatus status = owner->connection == NULL ? UMI_STATUS_CANCELLED :
        UmiIbkrGtkCopyRealtime(owner->connection, 15000U, &capture);
    if (status == UMI_STATUS_OK && capture.count <= UMI_IBKR_REALTIME_BAR_LIMIT) {
        owner->stream = capture; owner->hasStream = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        memset(&owner->stream, 0, sizeof(owner->stream));
        owner->stream.stale = true; owner->hasStream = FALSE;
    } else owner->stream.stale = true;
    WorkspaceStreamRender(owner);
}
static void WorkspaceStreamStart(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->streamBusy) return;
    /* Capture intent before sensitivity and status notifications can edit fields. */
    UmiIbkrRealtimeQuery query = {0};
    UmiStatus status = UmiIbkrQuoteContractParse(
        gtk_editable_get_text(GTK_EDITABLE(owner->quoteContract)),
        gtk_editable_get_text(GTK_EDITABLE(owner->quoteExchange)), &query.contract);
    guint kind = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->streamKind));
    query.dataKind = kind < 4U ? (UmiIbkrHistoricalDataKind)(kind + 1U) : (UmiIbkrHistoricalDataKind)0;
    query.regularHours = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->streamRth)) != FALSE;
    if (status == UMI_STATUS_OK) status = UmiIbkrRealtimeQueryValidate(&query);
    ++owner->references; owner->streamBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed) {
        if (owner->connection == NULL || owner->snapshot.state != UMI_IBKR_READY || owner->snapshot.stale)
            status = UMI_STATUS_UNAVAILABLE;
        else if (WorkspaceStreamActive(owner)) status = UMI_STATUS_BUSY;
        else if (status == UMI_STATUS_OK) status = UmiIbkrGtkStartRealtime(owner->connection, &query);
        if (!owner->closed) WorkspaceStreamRefresh(owner);
        if (!owner->closed) {
            char *notice = status == UMI_STATUS_OK ? g_strdup("Five-second bars requested. Wait for the first receipt.")
                : g_strdup_printf("Live bars not started: %s. Check the instrument and route, stop the previous "
                    "stream and allow at least 15 seconds between bar requests. Broker subscriptions and pacing also apply.",
                    umi_status_text(status));
            LabelText(owner->streamNotice, notice); g_free(notice);
        }
    }
    if (!owner->closed) { owner->streamBusy = FALSE; WorkspaceStreamRender(owner); }
    Release(owner);
}
static void WorkspaceStreamStop(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->streamBusy || !WorkspaceStreamActive(owner)) return;
    uint32_t expected = owner->stream.requestId;
    ++owner->references; owner->streamBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL) {
        UmiStatus status = owner->stream.requestId != expected ? UMI_STATUS_NOT_FOUND :
            UmiIbkrGtkStopRealtime(owner->connection, expected);
        if (!owner->closed) WorkspaceStreamRefresh(owner);
        if (!owner->closed) LabelText(owner->streamNotice, status == UMI_STATUS_OK
            ? "Live bars stopped. Historical captures and other market subscriptions are unchanged."
            : "The observed stream could not be stopped. Review its current state before trying again.");
    }
    if (!owner->closed) { owner->streamBusy = FALSE; WorkspaceStreamRender(owner); }
    Release(owner);
}
static void WorkspaceStreamRange(GtkSpinButton *spin, gpointer data)
{
    (void)spin;
    UmiBrokerWorkspaceGtk *owner = data;
    if (!owner->closed && !owner->streamBusy) (void)UmiBrokerWorkspaceGtkRefresh(owner);
}
static GtkWidget *WorkspaceStreamPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *query = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(query), GTK_SELECTION_NONE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(query), 1U);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(query), 5U);
    gtk_flow_box_set_column_spacing(GTK_FLOW_BOX(query), 8U);
    owner->streamKind = gtk_drop_down_new_from_strings((const char *[]){"Trades", "Midpoint", "Bid", "Ask", NULL});
    owner->streamRth = gtk_check_button_new_with_label("Regular hours only");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(owner->streamRth), TRUE);
    owner->streamStart = gtk_button_new_with_label("Start live bars");
    owner->streamStop = gtk_button_new_with_label("Stop live bars");
    owner->streamVisible = gtk_spin_button_new_with_range(1.0, 512.0, 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(owner->streamVisible), 80.0);
    owner->streamOffset = gtk_spin_button_new_with_range(0.0, 511.0, 1.0);
    owner->streamStatus = Cell("", FALSE); owner->streamNotice = Cell("", FALSE); owner->streamDetails = Cell("", FALSE);
    GtkWidget *named[] = {owner->streamKind, owner->streamRth, owner->streamStart, owner->streamStop,
        owner->streamVisible, owner->streamOffset, owner->streamStatus, owner->streamNotice, owner->streamDetails};
    const char *names[] = {"kind", "rth", "start", "stop", "visible", "offset", "status", "notice", "details"};
    for (size_t i = 0U; i < G_N_ELEMENTS(named); ++i) {
        char *name = g_strconcat("broker-workspace-stream-", names[i], NULL);
        gtk_widget_set_name(named[i], name); g_free(name);
    }
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Price source", owner->streamKind);
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Session", owner->streamRth);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_append(GTK_BOX(actions), owner->streamStart); gtk_box_append(GTK_BOX(actions), owner->streamStop);
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Subscription", actions);
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Visible bars (zoom)", owner->streamVisible);
    WorkspaceHistoryField(GTK_FLOW_BOX(query), "Bars back (pan)", owner->streamOffset);
    GtkWidget *help = Cell("Use the contract and exchange selected in Market data. This page subscribes to "
        "five-second bars and retains the latest 512. Gaps may include closed trading hours. "
        "Zoom, pan and switching chart tabs do not request data. Volume uses the broker's reported units.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE); gtk_box_append(GTK_BOX(page), help);
    gtk_box_append(GTK_BOX(page), query);
    gtk_label_set_wrap(GTK_LABEL(owner->streamStatus), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->streamNotice), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->streamDetails), TRUE);
    gtk_box_append(GTK_BOX(page), owner->streamStatus); gtk_box_append(GTK_BOX(page), owner->streamNotice);
    owner->streamDrawing = gtk_drawing_area_new();
    gtk_widget_set_name(owner->streamDrawing, "broker-workspace-stream-drawing");
    gtk_widget_set_hexpand(owner->streamDrawing, TRUE); gtk_widget_set_vexpand(owner->streamDrawing, TRUE);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(owner->streamDrawing), 280);
    owner->streamCanvas = g_new0(WorkspaceHistoryCanvas, 1);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(owner->streamDrawing), WorkspaceHistoryDraw,
        owner->streamCanvas, WorkspaceHistoryCanvasFree);
    gtk_box_append(GTK_BOX(page), owner->streamDrawing); gtk_box_append(GTK_BOX(page), owner->streamDetails);
    g_signal_connect(owner->streamStart, "clicked", G_CALLBACK(WorkspaceStreamStart), owner);
    g_signal_connect(owner->streamStop, "clicked", G_CALLBACK(WorkspaceStreamStop), owner);
    g_signal_connect(owner->streamVisible, "value-changed", G_CALLBACK(WorkspaceStreamRange), owner);
    g_signal_connect(owner->streamOffset, "value-changed", G_CALLBACK(WorkspaceStreamRange), owner);
    return page;
}
static GtkWidget *WorkspaceChartPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *stack = gtk_stack_new(), *switcher = gtk_stack_switcher_new();
    gtk_widget_set_name(stack, "broker-workspace-chart-modes");
    gtk_widget_set_name(switcher, "broker-workspace-chart-switcher");
    gtk_stack_set_hhomogeneous(GTK_STACK(stack), FALSE); gtk_stack_set_vhomogeneous(GTK_STACK(stack), FALSE);
    gtk_widget_set_vexpand(stack, TRUE);
    gtk_stack_add_titled(GTK_STACK(stack), WorkspaceHistoryPage(owner), "history", "Historical");
    gtk_stack_add_titled(GTK_STACK(stack), WorkspaceStreamPage(owner), "live", "Live five-second");
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(stack));
    gtk_box_append(GTK_BOX(page), switcher); gtk_box_append(GTK_BOX(page), stack);
    return page;
}
