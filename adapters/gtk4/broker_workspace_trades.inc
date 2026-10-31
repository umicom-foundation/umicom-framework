/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_trades.inc
 *
 * PURPOSE:
 *   Present broker executions and late commission evidence through the shared native session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* A capture identifies its own account and request. Changing the picker does
 * not relabel existing trades. Quantities are per execution; cumulative amounts
 * and correction families are never silently added into a trading balance. */
static gboolean WorkspaceTradesPending(const UmiBrokerWorkspaceGtk *owner)
{
    return owner->hasTrades && owner->trades.requestId != 0U &&
        !owner->trades.complete && !owner->trades.failed && !owner->trades.stale;
}
static GtkWidget *WorkspaceTradesTable(const UmiIbkrExecutionSnapshot *capture)
{
    GtkWidget *grid = gtk_grid_new();
    const char *headings[] = {"Execution ID", "Broker time", "Account", "Instrument", "Type",
        "Side", "Quantity", "Price", "Currency", "Exchange", "Permanent order", "Commission", "Fee evidence"};
    for (int i = 0; i < 13; ++i)
        gtk_grid_attach(GTK_GRID(grid), Cell(headings[i], TRUE), i, 0, 1, 1);
    for (size_t i = 0U; i < capture->count && i < UMI_IBKR_EXECUTION_OBSERVATION_LIMIT; ++i) {
        const UmiIbkrExecutionObservation *row = &capture->rows[i];
        const UmiIbkrExecutionFee *fee = &capture->fees[i];
        char quantity[64], price[64], permanent[32];
        if (UmiDecimalFormat(row->quantity, quantity, sizeof(quantity)) != UMI_STATUS_OK)
            (void)g_snprintf(quantity, sizeof(quantity), "Unrepresentable");
        if (UmiDecimalFormat(row->price, price, sizeof(price)) != UMI_STATUS_OK)
            (void)g_snprintf(price, sizeof(price), "Unrepresentable");
        (void)g_snprintf(permanent, sizeof(permanent), "%" PRIu64, row->permanentOrderId);
        char *commission = !fee->received ? g_strdup("Not received") : fee->conflicting
            ? g_strdup_printf("First %s %s; latest %s %s", fee->firstReportedAmount,
                fee->firstCurrency, fee->latestReportedAmount, fee->latestCurrency)
            : g_strdup_printf("%s %s", fee->firstReportedAmount, fee->firstCurrency);
        const char *values[] = {row->executionId, row->time, row->account, row->symbol, row->securityType,
            row->side, quantity, price, row->currency, row->exchange, permanent, commission,
            !fee->received ? "Unavailable" : fee->conflicting ? "Conflicting reports" :
                fee->exact ? "Reported" : "Outside exact decimal range"};
        for (int column = 0; column < 13; ++column)
            gtk_grid_attach(GTK_GRID(grid), Cell(values[column], FALSE), column, (int)i + 1, 1, 1);
        g_free(commission);
    }
    if (capture->count == 0U) {
        const char *empty = capture->complete && !capture->failed && !capture->stale
            ? "Completed capture contains no recent executions in this account and client scope."
            : "No complete capture. An empty table does not establish that no trades occurred.";
        gtk_grid_attach(GTK_GRID(grid), Cell(empty, FALSE), 0, 1, 13, 1);
    }
    return grid;
}
static void WorkspaceTradesRender(UmiBrokerWorkspaceGtk *owner, gboolean replaceTable)
{
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->tradesBusy;
    gboolean pending = WorkspaceTradesPending(owner);
    gtk_widget_set_sensitive(owner->tradesRead, ready && !pending && owner->snapshot.accountCount > 0U);
    if (owner->closed) return;
    gtk_widget_set_sensitive(owner->tradesAccounts, ready && !pending && owner->snapshot.accountCount > 0U);
    if (owner->closed) return;
    gtk_widget_set_sensitive(owner->tradesAbandon, ready && pending);
    if (owner->closed) return;
    const UmiIbkrExecutionSnapshot *capture = &owner->trades;
    char *status = g_strdup_printf("Capture account: %s • Request: %u • %zu reports • %s • %s\n%s",
        capture->account[0] != '\0' ? capture->account : "none",
        (unsigned)capture->requestId, capture->count,
        capture->failed ? "Failed / abandoned" : capture->complete ? "End marker received" :
            capture->requestId != 0U ? "Receiving" : "Not requested",
        capture->stale ? "STALE / incomplete evidence" : "Recent point-in-time capture", capture->message);
    LabelText(owner->tradesStatus, status);
    g_free(status);
    if (owner->closed || !replaceTable) return;
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->tradesTable), WorkspaceTradesTable(capture));
}
static void WorkspaceTradesRefresh(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrExecutionSnapshot *capture = g_new0(UmiIbkrExecutionSnapshot, 1);
    UmiStatus status = owner->connection == NULL ? UMI_STATUS_CANCELLED :
        UmiIbkrGtkCopyExecutions(owner->connection, capture);
    gboolean changed = FALSE;
    if (status == UMI_STATUS_OK) {
        changed = !owner->hasTrades || memcmp(&owner->trades, capture, sizeof(*capture)) != 0;
        owner->trades = *capture;
        owner->hasTrades = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        /* A new session has no capture: old-account rows must disappear even
         * when the same native inspector window is reused for reconnecting. */
        changed = owner->hasTrades;
        memset(&owner->trades, 0, sizeof(owner->trades));
        owner->trades.stale = true;
        owner->hasTrades = FALSE;
    } else if (owner->hasTrades) {
        changed = !owner->trades.stale;
        owner->trades.stale = true;
    }
    g_free(capture);
    WorkspaceTradesRender(owner, changed);
}
static void WorkspaceTradesRead(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->tradesBusy) return;
    ++owner->references;
    owner->tradesBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !WorkspaceTradesPending(owner)) {
        guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->tradesAccounts));
        if ((size_t)index < owner->snapshot.accountCount && index < UMI_IBKR_ACCOUNT_LIMIT) {
            /* Keep the identity independent of callbacks that replace the owner snapshot. */
            char account[64];
            (void)g_snprintf(account, sizeof(account), "%s", owner->snapshot.accounts[index]);
            UmiStatus status = UmiIbkrGtkReadExecutions(owner->connection, account);
            if (!owner->closed) WorkspaceTradesRefresh(owner);
            if (!owner->closed) LabelText(owner->tradesNotice, status == UMI_STATUS_OK
                ? "Recent executions requested. Commission reports may arrive after the end marker."
                : umi_status_text(status));
        }
    }
    if (!owner->closed) {
        owner->tradesBusy = FALSE;
        WorkspaceTradesRender(owner, FALSE);
    }
    Release(owner);
}
static void WorkspaceTradesAbandon(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->tradesBusy || !WorkspaceTradesPending(owner)) return;
    ++owner->references;
    uint32_t expected = owner->trades.requestId;
    owner->tradesBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL) {
        UmiStatus status = owner->trades.requestId != expected ? UMI_STATUS_NOT_FOUND :
            !WorkspaceTradesPending(owner) ? UMI_STATUS_INVALID_STATE :
                UmiIbkrGtkAbandonExecutions(owner->connection, expected);
        if (!owner->closed) WorkspaceTradesRefresh(owner);
        if (!owner->closed) LabelText(owner->tradesNotice, status == UMI_STATUS_OK
            ? "Stopped this local capture. Broker orders are unchanged."
            : "The capture changed or completed. Review its current state before stopping it.");
    }
    if (!owner->closed) {
        owner->tradesBusy = FALSE;
        WorkspaceTradesRender(owner, FALSE);
    }
    Release(owner);
}
static GtkWidget *WorkspaceTradesPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    owner->tradesAccounts = gtk_drop_down_new(NULL, NULL);
    gtk_widget_set_name(owner->tradesAccounts, "broker-workspace-trades-accounts");
    gtk_widget_set_tooltip_text(owner->tradesAccounts, "Choose an account advertised by this broker session.");
    owner->tradesRead = gtk_button_new_with_label("Read recent trades");
    gtk_widget_set_name(owner->tradesRead, "broker-workspace-trades-read");
    owner->tradesAbandon = gtk_button_new_with_label("Stop local capture");
    gtk_widget_set_name(owner->tradesAbandon, "broker-workspace-trades-abandon");
    gtk_widget_set_halign(owner->tradesRead, GTK_ALIGN_START);
    gtk_widget_set_halign(owner->tradesAbandon, GTK_ALIGN_START);
    owner->tradesStatus = Cell("", FALSE);
    gtk_widget_set_name(owner->tradesStatus, "broker-workspace-trades-status");
    gtk_label_set_wrap(GTK_LABEL(owner->tradesStatus), TRUE);
    owner->tradesNotice = Cell("", FALSE);
    gtk_widget_set_name(owner->tradesNotice, "broker-workspace-trades-notice");
    gtk_label_set_wrap(GTK_LABEL(owner->tradesNotice), TRUE);
    owner->tradesTable = gtk_scrolled_window_new();
    gtk_widget_set_name(owner->tradesTable, "broker-workspace-trades-table");
    gtk_widget_set_hexpand(owner->tradesTable, TRUE);
    gtk_widget_set_vexpand(owner->tradesTable, TRUE);
    gtk_box_append(GTK_BOX(page), owner->tradesAccounts);
    gtk_box_append(GTK_BOX(page), owner->tradesRead);
    gtk_box_append(GTK_BOX(page), owner->tradesAbandon);
    gtk_box_append(GTK_BOX(page), owner->tradesStatus);
    gtk_box_append(GTK_BOX(page), owner->tradesNotice);
    gtk_box_append(GTK_BOX(page), owner->tradesTable);
    GtkWidget *help = Cell("The broker supplies a limited recent execution window for the selected account "
        "and default client filter. This is not complete account history. Broker times are shown as reported; "
        "no local time zone is inferred. Missing fees are unavailable, not zero. Corrections remain separate "
        "reports; this view does not calculate P&L or change orders.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_box_append(GTK_BOX(page), help);
    g_signal_connect(owner->tradesRead, "clicked", G_CALLBACK(WorkspaceTradesRead), owner);
    g_signal_connect(owner->tradesAbandon, "clicked", G_CALLBACK(WorkspaceTradesAbandon), owner);
    return page;
}
