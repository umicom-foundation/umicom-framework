/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_connection_commands.inc
 *
 * PURPOSE:
 *   Own connection commands through notifications and retire closed windows before transport work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* GTK callbacks are synchronous but may reenter commands or destroy their
 * window. Keep the owner alive and reserve its command slot until publication
 * completes. This is event-loop serialisation, not a cross-thread lock. */
static ConnectionUi *ConnectionCommandEnter(GtkWindow *window)
{
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window) || ui->commandBusy ||
        ui->profileBusy || ui->fillBusy || ui->quoteAccessBusy) return NULL;
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
    if (PositionReviewBusy(ui)) return NULL;
#endif
    g_object_ref(window);
    ui->commandBusy = TRUE;
    return ui;
}
static void ConnectionCommandLeave(ConnectionUi *ui)
{
    ui->commandBusy = FALSE;
    g_object_unref(ui->window);
}
static gboolean ConnectionFieldsMatch(ConnectionUi *ui, const UmiIbkrConnectionOptions *options)
{
    return gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mode)) ==
            (options->environment == UMI_TRADING_LIVE ? 1U : 0U) &&
        gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->program)) == (guint)options->program &&
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->port)) == options->adapter.port &&
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->client)) == options->adapter.clientId &&
        (gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->ack)) != FALSE) == options->acknowledgeLive;
}
static void ConnectClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = ConnectionCommandEnter(GTK_WINDOW(data));
    if (ui == NULL) return;
    if (Connected(ui)) { ConnectionCommandLeave(ui); return; }
    const guint mode = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mode));
    const guint program = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->program));
    UmiIbkrConnectionOptions options = UmiIbkrConnectionOptionsDefault();
    options.environment = mode == 1U ? UMI_TRADING_LIVE : UMI_TRADING_PAPER;
    options.program = program == 1U ? UMI_IBKR_GATEWAY : UMI_IBKR_TWS;
    options.adapter.paperOnly = options.environment == UMI_TRADING_PAPER ? 1 : 0;
    options.adapter.port = (uint16_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->port));
    options.adapter.clientId = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->client));
    options.acknowledgeLive = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->ack)) != FALSE;
    UmiStatus status = mode > 1U || program > 1U ? UMI_STATUS_INVALID_ARGUMENT :
        UmiIbkrConnectionValidate(&options);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(ui->status),
            "Connection not opened. Check the profile and acknowledge Live access when selected.");
        ConnectionCommandLeave(ui); return;
    }
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
    PositionReviewRetire(ui);
#endif
    FillWatchRetire(ui);
    if (ui->source != 0U) { g_source_remove(ui->source); ui->source = 0U; }
    UmiIbkrConnectionDestroy(ui->connection);
    ui->connection = NULL; ui->populated = FALSE;
    HistoricalRetire(ui); StreamingRetire(ui);
    ui->catalogLoaded = FALSE; ui->catalogReset = TRUE;
    ui->scannerRequest = 0U; ui->scannerDisplayedRequest = 0U; ui->scannerDisplayedGeneration = 0U;
    ui->optionRequest = 0U; ui->optionDisplayedRequest = 0U; ui->optionContractRequest = 0U;
    ui->quoteRequest = 0U; ui->quoteSubscribed = FALSE;
    ui->contractRequest = 0U; ui->executionRequest = 0U; ui->pnlRequest = 0U;
    ui->symbolSearchRequest = 0U; ui->symbolSearchDisplayed = 0U;
    ui->depthRequest = 0U; ui->timeSalesRequest = 0U;
    ui->ruleId = 0U; ui->ruleQuoteRequest = 0U; ui->ruleContractRequest = 0U;
    memset(&ui->snapshot, 0, sizeof ui->snapshot);
    /* Each publication is a lifetime boundary. No new transport may be opened
     * after an observer closes this inspector or changes the captured intent. */
    gtk_label_set_text(GTK_LABEL(ui->quoteOutput), "No quote requested on this connection.");
    if (!ui->closed) gtk_label_set_text(GTK_LABEL(ui->quoteNotice), "");
    if (!ui->closed) gtk_drop_down_set_model(GTK_DROP_DOWN(ui->accounts), NULL);
    if (ui->closed) { ConnectionCommandLeave(ui); return; }
    if (!ConnectionFieldsMatch(ui, &options)) {
        gtk_label_set_text(GTK_LABEL(ui->status),
            "Settings changed while preparing the connection. Review them and choose Connect again.");
        if (!ui->closed) Controls(ui);
        ConnectionCommandLeave(ui); return;
    }
    status = UmiIbkrConnectionCreate(&options, &ui->connection);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(ui->status), umi_status_text(status));
        if (!ui->closed) Controls(ui);
        ConnectionCommandLeave(ui); return;
    }
    const uint64_t now = UmiIbkrMonotonicMilliseconds();
    status = UmiIbkrConnectionOpen(ui->connection, now);
    Render(ui, now);
    if (!ui->closed && status == UMI_STATUS_OK) {
        ui->lastPaint = now;
        ui->source = g_timeout_add(25U, Poll, data);
        if (ui->source == 0U) { UmiIbkrConnectionClose(ui->connection); Render(ui, now); }
    }
    ConnectionCommandLeave(ui);
}
static void DisconnectClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = ConnectionCommandEnter(GTK_WINDOW(data));
    if (ui == NULL) return;
    if (ui->source != 0U) { g_source_remove(ui->source); ui->source = 0U; }
    UmiIbkrConnectionClose(ui->connection);
    Render(ui, UmiIbkrMonotonicMilliseconds());
    ConnectionCommandLeave(ui);
}
static void ReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = ConnectionCommandEnter(GTK_WINDOW(data));
    if (ui == NULL) return;
    const guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->accounts));
    if (!Connected(ui) || (size_t)index >= ui->snapshot.accountCount) {
        ConnectionCommandLeave(ui); return;
    }
    const UmiIbkrAccountReportKind kind = (UmiIbkrAccountReportKind)
        gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->accountReportKind));
    const char *currency = kind == UMI_IBKR_ACCOUNT_CURRENCY
        ? gtk_editable_get_text(GTK_EDITABLE(ui->accountReportCurrency)) : "";
    /* Retain one-shot dispatch for review. Both native entry points use the
     * shared refresh owner, including account admission and reply isolation. */
#if 0
    UmiStatus status = UmiIbkrConnectionReadAccountReport(ui->connection,
        ui->snapshot.accounts[index], kind, currency, UmiIbkrMonotonicMilliseconds());
#endif
    UmiStatus status = UmiIbkrConnectionRefreshAccountReport(ui->connection,
        ui->snapshot.accounts[index], kind, currency, UmiIbkrMonotonicMilliseconds());
    Render(ui, UmiIbkrMonotonicMilliseconds());
    if (!ui->closed && status != UMI_STATUS_OK)
        gtk_label_set_text(GTK_LABEL(ui->status), umi_status_text(status));
    ConnectionCommandLeave(ui);
}
