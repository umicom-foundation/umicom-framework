/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_completed.inc
 *
 * PURPOSE:
 *   Present completed-order scope, status and exact reported values in the broker workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Completed reports retain broker scope separately from the account picker.
 * Cancellation can include partial fills; completion is never displayed as
 * synonymous with a filled order or a complete trading history. */
static void WorkspaceCompletedStale(UmiBrokerWorkspaceGtk *owner)
{
    owner->completed.capture.stale = true;
    for (size_t i = 0U; i < owner->completed.capture.count && i < UMI_IBKR_COMPLETED_ORDER_LIMIT; ++i)
        owner->completed.rows[i].stale = true;
}
static const char *WorkspaceCompletedText(const char *value)
{
    return value[0] != '\0' ? value : "Not reported";
}
static GtkWidget *WorkspaceCompletedTable(const UmiIbkrCompletedOrderReport *report)
{
    GtkWidget *grid = gtk_grid_new();
    const char *titles[] = {"Permanent ID", "Account", "Instrument", "Type", "Currency", "Side",
        "Ordered", "Filled", "Order type", "Limit", "Time in force", "Order state",
        "Completed status", "Broker time", "Evidence"};
    for (int i = 0; i < 15; ++i) gtk_grid_attach(GTK_GRID(grid), Cell(titles[i], TRUE), i, 0, 1, 1);
    for (size_t i = 0U; i < report->capture.count && i < UMI_IBKR_COMPLETED_ORDER_LIMIT; ++i) {
        const UmiIbkrCompletedOrder *row = &report->rows[i];
        char permanent[32];
        if (row->permanentId != 0U)
            (void)g_snprintf(permanent, sizeof(permanent), "%" PRIu64, row->permanentId);
        else (void)g_snprintf(permanent, sizeof(permanent), "Not reported");
        const char *values[] = {permanent, WorkspaceCompletedText(row->order.account),
            WorkspaceCompletedText(row->order.localSymbol[0] != '\0' ? row->order.localSymbol : row->order.symbol),
            WorkspaceCompletedText(row->order.securityType), WorkspaceCompletedText(row->order.currency),
            WorkspaceCompletedText(row->order.action), WorkspaceOrderNumber(&row->order.totalQuantity, true),
            WorkspaceOrderNumber(&row->filledQuantity, true), WorkspaceCompletedText(row->order.orderType),
            WorkspaceOrderNumber(&row->order.limitPrice, true), WorkspaceCompletedText(row->order.timeInForce),
            WorkspaceCompletedText(row->status), WorkspaceCompletedText(row->completedStatus),
            WorkspaceCompletedText(row->completedTime),
            report->capture.stale || row->stale ? "STALE / partial" : "Observed"};
        for (int column = 0; column < 15; ++column)
            gtk_grid_attach(GTK_GRID(grid), Cell(values[column], FALSE), column, (int)i + 1, 1, 1);
    }
    if (report->capture.count == 0U) {
        const char *empty = report->capture.complete && !report->capture.failed && !report->capture.stale
            ? "Completed capture contains no orders in the broker's available window."
            : "No complete capture. An empty table does not establish that no orders completed.";
        gtk_grid_attach(GTK_GRID(grid), Cell(empty, FALSE), 0, 1, 15, 1);
    }
    return grid;
}
static void WorkspaceCompletedRender(UmiBrokerWorkspaceGtk *owner, gboolean replaceTable)
{
    const UmiIbkrCompletedOrdersSnapshot *capture = &owner->completed.capture;
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->completedBusy && !capture->requested;
    gtk_widget_set_sensitive(owner->completedRead, ready);
    if (owner->closed) return;
    gtk_widget_set_sensitive(owner->completedApiOnly, ready);
    if (owner->closed) return;
    if (capture->requested)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(owner->completedApiOnly), capture->apiOnly);
    if (owner->closed) return;
    char *status = g_strdup_printf("%s • %s • %zu records • %s\n%s",
        !capture->requested ? "Not requested" : capture->apiOnly
            ? "API-origin orders, all visible clients" : "All completed orders visible to this login",
        capture->failed ? "Failed" : capture->pending ? "Receiving" :
            capture->complete ? "End marker received" : "Not complete",
        capture->count, capture->stale ? "STALE / incomplete evidence" : "Point-in-time capture",
        capture->message);
    LabelText(owner->completedStatus, status);
    g_free(status);
    if (owner->closed || !replaceTable) return;
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->completedTable),
        WorkspaceCompletedTable(&owner->completed));
}
static void WorkspaceCompletedRefresh(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrCompletedOrderReport *report = g_new0(UmiIbkrCompletedOrderReport, 1);
    UmiStatus status = owner->connection == NULL ? UMI_STATUS_NOT_FOUND :
        UmiIbkrGtkCopyCompletedOrders(owner->connection, 15000U, report);
    gboolean changed = FALSE;
    if (status == UMI_STATUS_OK) {
        changed = !owner->hasCompleted || memcmp(&owner->completed, report, sizeof(*report)) != 0;
        owner->completed = *report;
        owner->hasCompleted = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        /* A new, unconnected inspector must not retain the previous login's
         * rows. Session loss is rendered stale before an explicit new session. */
        changed = owner->hasCompleted;
        memset(&owner->completed, 0, sizeof(owner->completed));
        owner->completed.capture.stale = true;
        owner->hasCompleted = FALSE;
    } else if (owner->hasCompleted) {
        changed = !owner->completed.capture.stale;
        WorkspaceCompletedStale(owner);
    }
    g_free(report);
    WorkspaceCompletedRender(owner, changed);
}
static void WorkspaceCompletedRead(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->completedBusy) return;
    /* Capture intent before refreshing: publication may notify other widgets.
     * The core refuses a second request, including a previous failed capture. */
    bool apiOnly = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->completedApiOnly));
    ++owner->references;
    owner->completedBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL && !owner->snapshot.stale &&
        owner->snapshot.state == UMI_IBKR_READY && !owner->completed.capture.requested) {
        UmiStatus status = UmiIbkrGtkReadCompletedOrders(owner->connection, apiOnly);
        if (!owner->closed) WorkspaceCompletedRefresh(owner);
        if (!owner->closed) LabelText(owner->completedNotice, status == UMI_STATUS_OK
            ? "Requested completed orders. Wait for the broker end marker; reconnect for a new capture."
            : umi_status_text(status));
    }
    if (!owner->closed) {
        owner->completedBusy = FALSE;
        WorkspaceCompletedRender(owner, FALSE);
    }
    Release(owner);
}
static GtkWidget *WorkspaceCompletedPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    owner->completedApiOnly = gtk_check_button_new_with_label("API-origin orders only (all visible clients)");
    gtk_widget_set_name(owner->completedApiOnly, "broker-workspace-completed-api-only");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(owner->completedApiOnly), TRUE);
    owner->completedRead = gtk_button_new_with_label("Read completed orders");
    gtk_widget_set_name(owner->completedRead, "broker-workspace-completed-read");
    gtk_widget_set_halign(owner->completedRead, GTK_ALIGN_START);
    owner->completedStatus = Cell("", FALSE);
    gtk_widget_set_name(owner->completedStatus, "broker-workspace-completed-status");
    gtk_label_set_wrap(GTK_LABEL(owner->completedStatus), TRUE);
    owner->completedNotice = Cell("", FALSE);
    gtk_widget_set_name(owner->completedNotice, "broker-workspace-completed-notice");
    gtk_label_set_wrap(GTK_LABEL(owner->completedNotice), TRUE);
    owner->completedTable = gtk_scrolled_window_new();
    gtk_widget_set_name(owner->completedTable, "broker-workspace-completed-table");
    gtk_widget_set_hexpand(owner->completedTable, TRUE);
    gtk_widget_set_vexpand(owner->completedTable, TRUE);
    gtk_box_append(GTK_BOX(page), owner->completedApiOnly);
    gtk_box_append(GTK_BOX(page), owner->completedRead);
    gtk_box_append(GTK_BOX(page), owner->completedStatus);
    gtk_box_append(GTK_BOX(page), owner->completedNotice);
    gtk_box_append(GTK_BOX(page), owner->completedTable);
    GtkWidget *help = Cell("Completed orders include cancellations and partial fills. API-only is not "
        "limited to this client or the account selected on another page. Broker time is shown as reported. "
        "No client/order binding is inferred from a permanent ID. Use Trades to inspect executions; "
        "advanced execution comparison and CSV export remain in Connection and account tools.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_box_append(GTK_BOX(page), help);
    g_signal_connect(owner->completedRead, "clicked", G_CALLBACK(WorkspaceCompletedRead), owner);
    return page;
}
