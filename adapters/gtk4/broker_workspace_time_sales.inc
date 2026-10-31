/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_time_sales.inc
 *
 * PURPOSE:
 *   Present bounded Time & Sales observations through the existing broker session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The native view is a projection of a single owned report, never a second
 * broker session. Stable labels preserve selection during quiet refreshes. */
static void WorkspaceTapeStale(UmiBrokerWorkspaceGtk *owner)
{
    owner->tape.capture.stale = true;
    for (size_t i = 0U; i < UMI_IBKR_TIME_SALES_VIEW_LIMIT; ++i) owner->tape.rows[i].stale = true;
}
static const char *WorkspaceTapeKind(UmiIbkrTimeSalesKind kind)
{
    switch (kind) {
    case UMI_IBKR_TICKS_LAST: return "Last";
    case UMI_IBKR_TICKS_ALL_LAST: return "All Last";
    case UMI_IBKR_TICKS_BID_ASK: return "Bid / Ask";
    case UMI_IBKR_TICKS_MIDPOINT: return "Midpoint";
    default: return "Unknown";
    }
}
static const char *WorkspaceTapeNumber(const UmiIbkrOrderNumber *number)
{
    return number->reportedText[0] != '\0' ? number->reportedText : "Not reported";
}
static void WorkspaceTapeRender(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrTimeSalesReport *report = &owner->tape;
    gboolean active = owner->hasTape && report->capture.needsCancel;
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->tapeBusy;
    GtkWidget *controls[] = {owner->tapeStart, owner->tapeStop, owner->tapeKind, owner->tapeIgnoreSize};
    gboolean enabled[] = {ready && !active, ready && active, !active && !owner->tapeBusy,
        !active && !owner->tapeBusy};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(controls[i], enabled[i]);
    }
    if (owner->closed) return;
    char *status = owner->hasTape ? g_strdup_printf(
        "Contract %u • %s • %s • %s%s\nShowing %zu of %zu retained ticks • %" PRIu64
        " received • %" PRIu64 " evicted • %zu older retained ticks outside this view\n%s",
        (unsigned)report->capture.query.contract.contractId, report->capture.query.contract.exchange,
        WorkspaceTapeKind(report->capture.query.kind),
        report->capture.failed ? "Failed" : report->capture.cancelled ? "Stopped" :
        report->capture.active ? "Subscribed" : "Inactive",
        report->capture.stale ? " • STALE / unavailable" : " • Recent receipt",
        report->count, report->capture.count, report->capture.receivedEvents, report->capture.droppedEvents,
        report->omittedStoredEvents, report->capture.message)
        : g_strdup("No tick stream. Choose a contract and exchange in Market data, then start Time & Sales.");
    LabelText(owner->tapeStatus, status); g_free(status);
    for (size_t r = 0U; r < UMI_IBKR_TIME_SALES_VIEW_LIMIT; ++r) {
        if (owner->closed) return;
        const UmiIbkrTimeSalesReportRow *row = &report->rows[r];
        const UmiIbkrTimeSalesEvent *event = &row->event;
        gboolean visible = owner->hasTape && r < report->count;
        gboolean trade = event->kind == UMI_IBKR_TICKS_LAST || event->kind == UMI_IBKR_TICKS_ALL_LAST;
        gboolean bidAsk = event->kind == UMI_IBKR_TICKS_BID_ASK;
        char sequence[32], flags[160];
        (void)g_snprintf(sequence, sizeof(sequence), "%" PRIu64, event->sequence);
        (void)g_snprintf(flags, sizeof(flags), "%u%s%s", event->flags,
            !(event->flags & 1U) ? "" : trade ? " • past limit" : bidAsk ? " • bid past low" : "",
            !(event->flags & 2U) ? "" : trade ? " • unreported" : bidAsk ? " • ask past high" : "");
        GDateTime *time = visible && event->epochSeconds <= UINT64_C(253402300799)
            ? g_date_time_new_from_unix_utc((gint64)event->epochSeconds) : NULL;
        char *stamp = time == NULL ? g_strdup("Unavailable") : g_date_time_format(time, "%Y-%m-%d %H:%M:%S");
        if (time != NULL) g_date_time_unref(time);
        const char *values[] = {sequence, stamp, WorkspaceTapeKind(event->kind),
            !bidAsk ? WorkspaceTapeNumber(&event->price) : "—",
            trade ? WorkspaceTapeNumber(&event->size) : "—",
            bidAsk ? WorkspaceTapeNumber(&event->bidPrice) : "—",
            bidAsk ? WorkspaceTapeNumber(&event->bidSize) : "—",
            bidAsk ? WorkspaceTapeNumber(&event->askPrice) : "—",
            bidAsk ? WorkspaceTapeNumber(&event->askSize) : "—",
            trade ? event->exchange : "—", flags, trade ? event->specialConditions : "—",
            row->stale || report->capture.stale ? "STALE" : "Recent receipt"};
        for (size_t c = 0U; c < G_N_ELEMENTS(values); ++c) {
            if (owner->closed) break;
            GtkWidget *cell = owner->tapeCells[r][c];
            gtk_widget_set_visible(cell, visible);
            if (owner->closed) break;
            char *display = g_utf8_make_valid(visible ? values[c] : "", -1);
            if (strcmp(gtk_label_get_text(GTK_LABEL(cell)), display) != 0)
                gtk_label_set_text(GTK_LABEL(cell), display);
            g_free(display);
        }
        g_free(stamp);
    }
}
static void WorkspaceTapeRefresh(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrTimeSalesReport *copy = g_new0(UmiIbkrTimeSalesReport, 1);
    UmiStatus status = owner->connection == NULL ? UMI_STATUS_CANCELLED :
        UmiIbkrGtkCopyTimeSales(owner->connection, 15000U, copy);
    if (status == UMI_STATUS_OK && copy->count <= UMI_IBKR_TIME_SALES_VIEW_LIMIT) {
        owner->tape = *copy; owner->hasTape = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        memset(&owner->tape, 0, sizeof(owner->tape)); owner->hasTape = FALSE;
    } else WorkspaceTapeStale(owner);
    g_free(copy);
    WorkspaceTapeRender(owner);
}
static void WorkspaceTapeStart(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->tapeBusy) return;
    /* Capture form intent before refresh notifications can change any input. */
    UmiIbkrTimeSalesQuery query = {0};
    UmiStatus status = UmiIbkrQuoteContractParse(
        gtk_editable_get_text(GTK_EDITABLE(owner->quoteContract)),
        gtk_editable_get_text(GTK_EDITABLE(owner->quoteExchange)), &query.contract);
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->tapeKind));
    query.kind = selected < 4U ? (UmiIbkrTimeSalesKind)(selected + 1U) : (UmiIbkrTimeSalesKind)0;
    query.ignoreSize = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->tapeIgnoreSize)) != FALSE;
    if (status == UMI_STATUS_OK) status = UmiIbkrTimeSalesQueryValidate(&query);
    ++owner->references; owner->tapeBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && status == UMI_STATUS_OK) {
        if (owner->connection == NULL || owner->snapshot.state != UMI_IBKR_READY || owner->snapshot.stale)
            status = UMI_STATUS_UNAVAILABLE;
        else if (owner->hasTape && owner->tape.capture.needsCancel) status = UMI_STATUS_ALREADY_EXISTS;
        else status = UmiIbkrGtkStartTimeSales(owner->connection, &query);
    }
    if (!owner->closed) WorkspaceTapeRefresh(owner);
    owner->tapeBusy = FALSE;
    if (!owner->closed) WorkspaceTapeRender(owner);
    if (!owner->closed) LabelText(owner->tapeNotice, status == UMI_STATUS_OK
        ? "Tick stream requested. Data appears as the broker sends it."
        : status == UMI_STATUS_INVALID_ARGUMENT
        ? "Enter a positive contract ID and exchange. Ignore size changes applies only to Bid / Ask."
        : status == UMI_STATUS_BUSY
        ? "Tick requests are paced. Wait at least 15 seconds after the previous request and try again."
        : umi_status_text(status));
    Release(owner);
}
static void WorkspaceTapeStop(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->tapeBusy || !owner->hasTape ||
        !owner->tape.capture.needsCancel) return;
    uint32_t expected = owner->tape.capture.requestId;
    ++owner->references; owner->tapeBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!owner->closed && owner->connection != NULL)
        status = UmiIbkrGtkStopTimeSales(owner->connection, expected);
    if (!owner->closed) WorkspaceTapeRefresh(owner);
    owner->tapeBusy = FALSE;
    if (!owner->closed) WorkspaceTapeRender(owner);
    if (!owner->closed) LabelText(owner->tapeNotice, status == UMI_STATUS_OK
        ? "Tick stream stopped. Retained ticks are historical observations." : umi_status_text(status));
    Release(owner);
}
static GtkWidget *WorkspaceTapePage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6), *controls = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(controls), GTK_SELECTION_NONE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(controls), 1U);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(controls), 4U);
    const char *kinds[] = {"Last trades", "All Last trades", "Bid / Ask", "Midpoint", NULL};
    owner->tapeKind = gtk_drop_down_new_from_strings(kinds);
    owner->tapeIgnoreSize = gtk_check_button_new_with_label("Ignore size changes (Bid / Ask only)");
    owner->tapeStart = gtk_button_new_with_label("Start Time & Sales");
    owner->tapeStop = gtk_button_new_with_label("Stop tick stream");
    owner->tapeStatus = Cell("", FALSE); owner->tapeNotice = Cell("", FALSE);
    GtkWidget *fields[] = {owner->tapeKind, owner->tapeIgnoreSize, owner->tapeStart, owner->tapeStop,
        owner->tapeStatus, owner->tapeNotice};
    const char *names[] = {"kind", "ignore-size", "start", "stop", "status", "notice"};
    for (size_t i = 0U; i < G_N_ELEMENTS(fields); ++i) {
        char *name = g_strconcat("broker-workspace-tape-", names[i], NULL);
        gtk_widget_set_name(fields[i], name); g_free(name);
        if (i < 4U) gtk_flow_box_insert(GTK_FLOW_BOX(controls), fields[i], -1);
    }
    GtkWidget *help = Cell("Uses the contract and exchange in Market data. Live tick permissions are required; "
        "a delayed quote selection does not enable delayed ticks. Shows the newest 64 retained events in receipt order. "
        "Equal ticks remain distinct. UTC is exchange time; recent receipt is not an exchange-time freshness guarantee.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE); gtk_box_append(GTK_BOX(box), help);
    gtk_box_append(GTK_BOX(box), controls);
    gtk_label_set_wrap(GTK_LABEL(owner->tapeStatus), TRUE); gtk_label_set_wrap(GTK_LABEL(owner->tapeNotice), TRUE);
    gtk_box_append(GTK_BOX(box), owner->tapeStatus); gtk_box_append(GTK_BOX(box), owner->tapeNotice);
    GtkWidget *grid = gtk_grid_new(), *scroll = gtk_scrolled_window_new();
    const char *titles[] = {"Sequence", "Exchange time (UTC)", "Kind", "Price", "Size", "Bid", "Bid size",
        "Ask", "Ask size", "Venue", "Flags", "Conditions", "Receipt state"};
    for (int c = 0; c < 13; ++c) gtk_grid_attach(GTK_GRID(grid), Cell(titles[c], TRUE), c, 0, 1, 1);
    for (size_t r = 0U; r < UMI_IBKR_TIME_SALES_VIEW_LIMIT; ++r) {
        for (int c = 0; c < 13; ++c) {
            GtkWidget *cell = Cell("", FALSE); owner->tapeCells[r][c] = cell;
            char *name = g_strdup_printf("broker-workspace-tape-cell-%zu-%d", r, c);
            gtk_widget_set_name(cell, name); g_free(name);
            gtk_grid_attach(GTK_GRID(grid), cell, c, (int)r + 1, 1, 1);
            gtk_widget_set_visible(cell, FALSE);
        }
    }
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), grid);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scroll, TRUE); gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    g_signal_connect(owner->tapeStart, "clicked", G_CALLBACK(WorkspaceTapeStart), owner);
    g_signal_connect(owner->tapeStop, "clicked", G_CALLBACK(WorkspaceTapeStop), owner);
    return box;
}
