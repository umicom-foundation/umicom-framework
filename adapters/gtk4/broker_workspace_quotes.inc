/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_quotes.inc
 *
 * PURPOSE:
 *   Project the selected broker quote stream with explicit freshness and shared subscription controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The account and quote pages project the same inspector session. Neither
 * widget owns a socket or pumps transport. Stale values remain visible with
 * their meaning; a reconnect with no request clears the previous instrument. */
static void WorkspaceQuoteStale(UmiBrokerWorkspaceGtk *owner)
{
    owner->quote.subscribed = false;
    UmiIbkrQuoteValue *values[] = {&owner->quote.bid, &owner->quote.ask, &owner->quote.last,
        &owner->quote.bidSize, &owner->quote.askSize, &owner->quote.lastSize};
    for (size_t i = 0U; i < G_N_ELEMENTS(values); ++i) values[i]->stale = true;
    for (size_t basis = 0U; basis < UMI_IBKR_OPTION_BASIS_COUNT; ++basis)
        for (size_t measure = 0U; measure < UMI_IBKR_OPTION_MEASURE_COUNT; ++measure)
            owner->quote.options[basis].values[measure].stale = true;
}

static void WorkspaceQuoteRender(UmiBrokerWorkspaceGtk *owner, gboolean replaceValues)
{
    gboolean active = owner->hasQuote && owner->quote.subscribed;
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->quoteBusy;
    /* Retain previous controls; requested mode shares their subscription lock. */
#if 0
    GtkWidget *controls[] = {owner->quoteStart, owner->quoteStop, owner->quoteContract, owner->quoteExchange};
    gboolean enabled[] = {ready && !active, ready && active, !active && !owner->quoteBusy, !active && !owner->quoteBusy};
#endif
    GtkWidget *controls[] = {owner->quoteStart, owner->quoteStop, owner->quoteContract, owner->quoteExchange, owner->quoteDataType};
    gboolean enabled[] = {ready && !active, ready && active, !active && !owner->quoteBusy,
        !active && !owner->quoteBusy, !active && !owner->quoteBusy};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(controls[i], enabled[i]);
    }
    if (owner->closed || !replaceValues) return;
    if (owner->hasQuote) {
        /* Request and observation are different evidence; retain the original
         * display for review while showing both in the current workspace. */
#if 0
        char *status = g_strdup_printf("Contract %" PRIu32 " • %s • %s • %s%s\n%s",
            owner->quote.contract.contractId, owner->quote.contract.exchange,
            UmiIbkrMarketDataTypeName(owner->quote.dataType),
            active ? "Subscribed" : "STALE — stream stopped",
            owner->quote.failed ? " • Provider refused data" : "", owner->quote.message);
#endif
        char *status = g_strdup_printf("Contract %" PRIu32 " • %s • Requested: %s • Received: %s • %s%s\n%s",
            owner->quote.contract.contractId, owner->quote.contract.exchange,
            UmiIbkrMarketDataTypeName(owner->quote.requestedDataType),
            UmiIbkrMarketDataTypeName(owner->quote.dataType),
            active ? "Subscribed" : "STALE — stream stopped",
            owner->quote.failed ? " • Provider refused data" : "", owner->quote.message);
        LabelText(owner->quoteStatus, status);
        g_free(status);
    } else LabelText(owner->quoteStatus, "No quote requested. Enter a contract ID and exchange, then choose Subscribe.");
    const UmiIbkrQuoteValue *values[] = {&owner->quote.bid, &owner->quote.ask, &owner->quote.last,
        &owner->quote.bidSize, &owner->quote.askSize, &owner->quote.lastSize};
    for (size_t i = 0U; i < G_N_ELEMENTS(values); ++i) {
        if (owner->closed) return;
        const UmiIbkrQuoteValue *value = values[i];
        const char *text = !owner->hasQuote || !value->received ? "Not received" :
            value->unavailable ? "Unavailable" : value->text;
        LabelText(owner->quoteValues[i], text);
        if (owner->closed) return;
        char *state = !owner->hasQuote || !value->received ? g_strdup("Waiting") :
            g_strdup_printf("%s • %s", UmiIbkrMarketDataTypeName(value->dataType),
                value->stale ? "STALE" : "Received within 15 seconds");
        LabelText(owner->quoteStates[i], state);
        g_free(state);
    }
    WorkspaceOptionRiskRender(owner);
}
static void WorkspaceQuoteRefresh(UmiBrokerWorkspaceGtk *owner)
{
    if (owner->closed || owner->connection == NULL) return;
    UmiIbkrQuoteSnapshot quote;
    UmiStatus status = UmiIbkrGtkCopyQuote(owner->connection, 15000U, &quote);
    gboolean changed = FALSE;
    if (status == UMI_STATUS_OK) {
        changed = !owner->hasQuote || memcmp(&quote, &owner->quote, sizeof(quote)) != 0;
        owner->quote = quote;
        owner->hasQuote = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        changed = owner->hasQuote;
        memset(&owner->quote, 0, sizeof(owner->quote));
        owner->hasQuote = FALSE;
    } else if (owner->hasQuote) {
        quote = owner->quote;
        WorkspaceQuoteStale(owner);
        changed = memcmp(&quote, &owner->quote, sizeof(quote)) != 0;
    }
    /* Recompute controls even without quote changes: account readiness can
     * change independently, and a successful reconnect has no quote initially. */
    WorkspaceQuoteRender(owner, changed);
}

static void WorkspaceQuoteStart(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->quoteBusy || owner->connection == NULL ||
        owner->snapshot.state != UMI_IBKR_READY || owner->snapshot.stale ||
        (owner->hasQuote && owner->quote.subscribed)) return;
    /* Candidate selection publishes entry notifications; those must not issue
     * a subscription before both instrument fields have been replaced. */
    if (owner->searchBusy) return;
    UmiIbkrQuoteContract contract;
    UmiStatus status = UmiIbkrQuoteContractParse(
        gtk_editable_get_text(GTK_EDITABLE(owner->quoteContract)),
        gtk_editable_get_text(GTK_EDITABLE(owner->quoteExchange)), &contract);
    guint choice = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->quoteDataType));
    UmiIbkrMarketDataType type = choice < 4U ? (UmiIbkrMarketDataType)(choice + 1U) : UMI_IBKR_DATA_UNKNOWN;
    ++owner->references;
    owner->quoteBusy = TRUE;
    WorkspaceQuoteRender(owner, TRUE);
    /* The former real-time call is kept for review. Input is captured before
     * native notifications so reentrant edits cannot change this request. */
#if 0
    if (!owner->closed && status == UMI_STATUS_OK)
        status = owner->connection == NULL ? UMI_STATUS_CANCELLED :
            UmiIbkrGtkQuoteSubscribe(owner->connection, &contract);
#endif
    if (!owner->closed && status == UMI_STATUS_OK)
        status = owner->connection == NULL ? UMI_STATUS_CANCELLED :
            UmiIbkrGtkQuoteSubscribeWithDataType(owner->connection, &contract, type);
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
    owner->quoteBusy = FALSE;
    if (!owner->closed) WorkspaceQuoteRender(owner, TRUE);
    if (!owner->closed)
        LabelText(owner->quoteNotice, status == UMI_STATUS_OK ? "Quote subscription requested." :
            status == UMI_STATUS_INVALID_ARGUMENT ? "Use the positive contract ID and exchange from TWS Contract Description, and select a data mode." :
            umi_status_text(status));
    Release(owner);
}
static void WorkspaceQuoteStop(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->quoteBusy || owner->connection == NULL ||
        owner->snapshot.state != UMI_IBKR_READY || owner->snapshot.stale ||
        !owner->hasQuote || !owner->quote.subscribed) return;
    ++owner->references;
    owner->quoteBusy = TRUE;
    WorkspaceQuoteRender(owner, TRUE);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!owner->closed && owner->connection != NULL)
        status = UmiIbkrGtkQuoteCancel(owner->connection);
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
    owner->quoteBusy = FALSE;
    if (!owner->closed) WorkspaceQuoteRender(owner, TRUE);
    if (!owner->closed)
        LabelText(owner->quoteNotice, status == UMI_STATUS_OK ? "Quote stream stopped." : umi_status_text(status));
    Release(owner);
}

static GtkWidget *WorkspaceQuotePage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *scroll = gtk_scrolled_window_new();
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), box);
    WorkspaceSearchControls(owner, box);
    GtkWidget *fields = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(fields), 8);
    gtk_grid_set_row_spacing(GTK_GRID(fields), 6);
    owner->quoteContract = gtk_entry_new();
    owner->quoteExchange = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(owner->quoteContract), 10);
    gtk_entry_set_max_length(GTK_ENTRY(owner->quoteExchange), 63);
    gtk_widget_set_name(owner->quoteContract, "broker-workspace-contract");
    gtk_widget_set_name(owner->quoteExchange, "broker-workspace-exchange");
    gtk_grid_attach(GTK_GRID(fields), Cell("Contract ID", TRUE), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(fields), owner->quoteContract, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(fields), Cell("Exchange", TRUE), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(fields), owner->quoteExchange, 1, 1, 1, 1);
    const char *types[] = {"Real-time", "Frozen", "Delayed", "Delayed frozen", NULL};
    owner->quoteDataType = gtk_drop_down_new_from_strings(types);
    gtk_widget_set_name(owner->quoteDataType, "broker-workspace-data-type");
    gtk_grid_attach(GTK_GRID(fields), Cell("Requested data", TRUE), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(fields), owner->quoteDataType, 1, 2, 1, 1);
    gtk_widget_set_tooltip_text(owner->quoteDataType,
        "A delivery preference. TWS may return real-time data if permitted. Stop quotes before choosing another mode.");
    gtk_widget_set_hexpand(owner->quoteContract, TRUE);
    gtk_widget_set_hexpand(owner->quoteExchange, TRUE);
    gtk_widget_set_tooltip_text(owner->quoteContract, "Exact contract ID shown in TWS Contract Description");
    gtk_widget_set_tooltip_text(owner->quoteExchange, "Exchange for that contract, as shown by the broker");
    owner->quoteStart = gtk_button_new_with_label("Subscribe");
    owner->quoteStop = gtk_button_new_with_label("Stop quotes");
    gtk_widget_set_name(owner->quoteStart, "broker-workspace-subscribe");
    gtk_widget_set_name(owner->quoteStop, "broker-workspace-stop-quotes");
    gtk_grid_attach(GTK_GRID(fields), owner->quoteStart, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(fields), owner->quoteStop, 2, 1, 1, 1);
    gtk_box_append(GTK_BOX(box), fields);
    owner->quoteNotice = Cell("", FALSE);
    owner->quoteStatus = Cell("", TRUE);
    gtk_widget_set_name(owner->quoteNotice, "broker-workspace-quote-notice");
    gtk_widget_set_name(owner->quoteStatus, "broker-workspace-quote-status");
    gtk_label_set_wrap(GTK_LABEL(owner->quoteNotice), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->quoteStatus), TRUE);
    gtk_box_append(GTK_BOX(box), owner->quoteNotice);
    gtk_box_append(GTK_BOX(box), owner->quoteStatus);
    GtkWidget *grid = gtk_grid_new();
    const char *titles[] = {"Field", "Reported value", "Availability"};
    for (int i = 0; i < 3; ++i) gtk_grid_attach(GTK_GRID(grid), Cell(titles[i], TRUE), i, 0, 1, 1);
    const char *names[] = {"Bid", "Ask", "Last", "Bid size", "Ask size", "Last size"};
    const char *ids[] = {"bid", "ask", "last", "bid-size", "ask-size", "last-size"};
    for (int i = 0; i < 6; ++i) {
        owner->quoteValues[i] = Cell("Not received", FALSE);
        owner->quoteStates[i] = Cell("Waiting", FALSE);
        char *name = g_strdup_printf("broker-workspace-quote-%s", ids[i]);
        gtk_widget_set_name(owner->quoteValues[i], name);
        g_free(name);
        gtk_grid_attach(GTK_GRID(grid), Cell(names[i], TRUE), 0, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), owner->quoteValues[i], 1, i + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), owner->quoteStates[i], 2, i + 1, 1, 1);
    }
    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_append(GTK_BOX(box), WorkspaceOptionRiskPage(owner));
    GtkWidget *help = Cell("Prices and sizes are shown exactly as received. Recent receipt does not mean real-time exchange data. "
        "Each value becomes stale after 15 seconds without a new observation. "
        "Market-data permissions are controlled by your broker. Stop quotes cancels market data, never an order.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_box_append(GTK_BOX(box), help);
    g_signal_connect(owner->quoteStart, "clicked", G_CALLBACK(WorkspaceQuoteStart), owner);
    g_signal_connect(owner->quoteStop, "clicked", G_CALLBACK(WorkspaceQuoteStop), owner);
    return scroll;
}
