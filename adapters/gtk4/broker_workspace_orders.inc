/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_orders.inc
 *
 * PURPOSE:
 *   Present open-order observations with explicit scope, independent status and stale evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Order observations have their own completion marker and age. They are not
 * inferred from account balances, nor filtered silently by the account selector
 * on the separate account page. Every visible row identifies its own account. */
static void WorkspaceOrdersStale(UmiBrokerWorkspaceGtk *owner)
{
    if (!owner->hasOrders) return;
    owner->orders.capture.stale = true;
    owner->orders.identity.stale = true;
    for (size_t i = 0U; i < owner->orders.capture.count && i < UMI_IBKR_ORDER_LIMIT; ++i)
        owner->orders.rows[i].openStale = owner->orders.rows[i].statusStale = true;
}
static const char *WorkspaceOrderNumber(const UmiIbkrOrderNumber *number, bool available)
{
    return !available ? "Not received" :
        number->reportedText[0] != '\0' ? number->reportedText : "Not reported";
}
static GtkWidget *WorkspaceOrdersTable(const UmiIbkrOrderReport *report)
{
    GtkWidget *grid = gtk_grid_new();
    const char *headings[] = {"Client / order", "Permanent ID", "Account", "Instrument", "Side", "Quantity",
        "Type", "Limit", "Time in force", "Status", "Filled", "Remaining", "Evidence"};
    for (int i = 0; i < 13; ++i)
        gtk_grid_attach(GTK_GRID(grid), Cell(headings[i], TRUE), i, 0, 1, 1);
    for (size_t i = 0U; i < report->capture.count && i < UMI_IBKR_ORDER_LIMIT; ++i) {
        const UmiIbkrRecoveredOrder *row = &report->rows[i];
        char key[64], permanent[32];
        (void)g_snprintf(key, sizeof(key), "%" PRId32 " / %" PRId32, row->key.clientId, row->key.orderId);
        /* Unbound manual orders may all have client/order zero. Their broker
         * permanent IDs keep those separate observations recognisable. */
        if (row->permanentId != 0U)
            (void)g_snprintf(permanent, sizeof(permanent), "%" PRIu64, row->permanentId);
        else (void)g_snprintf(permanent, sizeof(permanent), "Not received");
        const char *missing = "Not received";
        const char *evidence = report->capture.stale || row->openStale || row->statusStale
            ? "STALE / partial" : "Observed";
        const char *values[] = {key, permanent,
            row->hasOpenOrder ? row->open.account : missing,
            row->hasOpenOrder ? (row->open.localSymbol[0] != '\0' ? row->open.localSymbol : row->open.symbol) : missing,
            row->hasOpenOrder ? row->open.action : missing,
            WorkspaceOrderNumber(&row->open.totalQuantity, row->hasOpenOrder),
            row->hasOpenOrder ? row->open.orderType : missing,
            WorkspaceOrderNumber(&row->open.limitPrice, row->hasOpenOrder),
            row->hasOpenOrder ? row->open.timeInForce : missing,
            row->hasStatus ? row->status.status : missing,
            WorkspaceOrderNumber(&row->status.filled, row->hasStatus),
            WorkspaceOrderNumber(&row->status.remaining, row->hasStatus), evidence};
        for (int column = 0; column < 13; ++column)
            gtk_grid_attach(GTK_GRID(grid), Cell(values[column], FALSE), column, (int)i + 1, 1, 1);
    }
    if (report->capture.count == 0U) {
        const char *empty = report->capture.complete && !report->capture.stale && !report->capture.failed
            ? "Completed capture contains no orders in the requested scope."
            : "No complete current capture. An empty table does not establish that there are no orders.";
        gtk_grid_attach(GTK_GRID(grid), Cell(empty, FALSE), 0, 1, 13, 1);
    }
    return grid;
}
static void WorkspaceOrdersRender(UmiBrokerWorkspaceGtk *owner, gboolean replaceTable)
{
    const UmiIbkrOrderRecoverySnapshot *capture = &owner->orders.capture;
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->ordersBusy && !capture->requested;
    gtk_widget_set_sensitive(owner->ordersRead, ready);
    if (owner->closed) return;
    gtk_widget_set_sensitive(owner->ordersAll, ready);
    if (owner->closed) return;
    if (capture->requested)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(owner->ordersAll),
            capture->scope == UMI_IBKR_ORDERS_ALL_CLIENTS);
    if (owner->closed) return;
    char *status = g_strdup_printf("%s • %s • %zu rows • %s\n%s",
        capture->requested ? (capture->scope == UMI_IBKR_ORDERS_ALL_CLIENTS
            ? "All broker-visible clients / accounts" : "This API client")
            : "No capture requested",
        capture->failed ? "Failed" : capture->pending ? "Receiving" :
            capture->complete ? "Complete" : "Not complete",
        capture->count, capture->stale ? "STALE / partial evidence" : "Point-in-time observation",
        capture->message);
    LabelText(owner->ordersStatus, status);
    g_free(status);
    if (owner->closed || !replaceTable) return;
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->ordersTable),
        WorkspaceOrdersTable(&owner->orders));
}
static void WorkspaceOrdersRefresh(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrOrderReport *report = g_new0(UmiIbkrOrderReport, 1);
    UmiStatus status = owner->connection == NULL ? UMI_STATUS_NOT_FOUND :
        UmiIbkrGtkCopyOrderReport(owner->connection, 15000U, report);
    gboolean changed = FALSE;
    if (status == UMI_STATUS_OK) {
        changed = !owner->hasOrders || memcmp(&owner->orders, report, sizeof(*report)) != 0;
        owner->orders = *report;
        owner->hasOrders = TRUE;
    } else if (owner->hasOrders) {
        /* A removed or unavailable owner cannot leave retained rows current. */
        changed = !owner->orders.capture.stale;
        WorkspaceOrdersStale(owner);
    }
    g_free(report);
    WorkspaceOrdersRender(owner, changed);
}
static void WorkspaceOrdersRead(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->ordersBusy) return;
    ++owner->references;
    owner->ordersBusy = TRUE;
    /* Re-read shared evidence before acting: the inspector may already have
     * consumed this session's one recovery request since the previous timer. */
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL && !owner->snapshot.stale &&
        owner->snapshot.state == UMI_IBKR_READY && !owner->orders.capture.requested) {
        UmiIbkrOrderScope scope = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->ordersAll))
            ? UMI_IBKR_ORDERS_ALL_CLIENTS : UMI_IBKR_ORDERS_THIS_CLIENT;
        UmiStatus status = UmiIbkrGtkRecoverOrders(owner->connection, scope);
        if (!owner->closed) WorkspaceOrdersRefresh(owner);
        if (!owner->closed) LabelText(owner->ordersNotice, status == UMI_STATUS_OK
            ? "Recovery requested. Wait for the broker end marker."
            : umi_status_text(status));
    }
    if (!owner->closed) {
        owner->ordersBusy = FALSE;
        WorkspaceOrdersRender(owner, FALSE);
    }
    Release(owner);
}
static GtkWidget *WorkspaceOrdersPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    owner->ordersAll = gtk_check_button_new_with_label("Include all broker-visible clients and accounts");
    gtk_widget_set_name(owner->ordersAll, "broker-workspace-orders-all");
    gtk_widget_set_tooltip_text(owner->ordersAll,
        "Select to request orders visible from other API clients and TWS. Broker visibility limits still apply.");
    owner->ordersRead = gtk_button_new_with_label("Recover open orders");
    gtk_widget_set_name(owner->ordersRead, "broker-workspace-orders-read");
    gtk_widget_set_halign(owner->ordersRead, GTK_ALIGN_START);
    owner->ordersStatus = Cell("", FALSE);
    gtk_widget_set_name(owner->ordersStatus, "broker-workspace-orders-status");
    gtk_label_set_wrap(GTK_LABEL(owner->ordersStatus), TRUE);
    owner->ordersNotice = Cell("", FALSE);
    gtk_widget_set_name(owner->ordersNotice, "broker-workspace-orders-notice");
    gtk_label_set_wrap(GTK_LABEL(owner->ordersNotice), TRUE);
    owner->ordersTable = gtk_scrolled_window_new();
    gtk_widget_set_name(owner->ordersTable, "broker-workspace-orders-table");
    gtk_widget_set_vexpand(owner->ordersTable, TRUE);
    gtk_widget_set_hexpand(owner->ordersTable, TRUE);
    gtk_box_append(GTK_BOX(page), owner->ordersAll);
    gtk_box_append(GTK_BOX(page), owner->ordersRead);
    gtk_box_append(GTK_BOX(page), owner->ordersStatus);
    gtk_box_append(GTK_BOX(page), owner->ordersNotice);
    gtk_box_append(GTK_BOX(page), owner->ordersTable);
    GtkWidget *help = Cell("This API client can have no orders even when TWS has orders. "
        "Choose all broker-visible clients to include their visible orders. Reconnect for a new capture. "
        "Status callbacks may be missing or stale; inspect executions separately for fills and fees.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_box_append(GTK_BOX(page), help);
    g_signal_connect(owner->ordersRead, "clicked", G_CALLBACK(WorkspaceOrdersRead), owner);
    return page;
}
