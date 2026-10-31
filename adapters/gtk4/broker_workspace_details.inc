/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_details.inc
 *
 * PURPOSE:
 *   Display exact instrument metadata and bounded-age request state in a shared broker workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* A selected contract is explicit input, separate from an active quote stream.
 * Returned metadata retains every candidate and its own request identity. */
static gboolean WorkspaceDetailsReady(const UmiBrokerWorkspaceGtk *owner)
{
    return owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->detailsBusy;
}
static GtkWidget *WorkspaceDetailsTable(const UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    if (!owner->hasDetails || owner->details.count == 0U) {
        gtk_box_append(GTK_BOX(box), Cell(owner->hasDetails && owner->details.complete
            ? "Completed lookup contains no instrument descriptions."
            : "No descriptions received. Enter a contract ID and route, then choose Read description.", FALSE));
        return box;
    }
    const char *names[] = {"Contract ID", "Symbol", "Type", "Currency", "Local symbol", "Trading class",
        "Exchange", "Primary exchange", "Expiry", "Strike", "Right", "Multiplier", "Minimum tick",
        "Price magnifier", "Minimum size", "Size increment", "Suggested size increment",
        "Reported order types", "Reported exchanges", "Market-rule IDs", "Market"};
    for (size_t i = 0U; i < owner->details.count && i < UMI_IBKR_CONTRACT_DETAILS_LIMIT; ++i) {
        const UmiIbkrContractDescription *row = &owner->details.items[i];
        char identity[24]; (void)g_snprintf(identity, sizeof(identity), "%" PRIu32, row->contractId);
        const char *values[] = {identity, row->symbol, row->securityType, row->currency, row->localSymbol,
            row->tradingClass, row->exchange, row->primaryExchange, row->expiry, row->strike, row->right,
            row->multiplier, row->minimumTick, row->priceMagnifier, row->minimumSize, row->sizeIncrement,
            row->suggestedSizeIncrement, row->orderTypes, row->validExchanges, row->marketRuleIds, row->marketName};
        GtkWidget *title = Cell(row->longName[0] == '\0' ? "Instrument description" : row->longName, TRUE);
        gtk_label_set_wrap(GTK_LABEL(title), TRUE);
        gtk_box_append(GTK_BOX(box), title);
        GtkWidget *grid = gtk_grid_new();
        gtk_widget_set_hexpand(grid, TRUE);
        for (size_t field = 0U; field < G_N_ELEMENTS(names); ++field) {
            GtkWidget *value = Cell(values[field][0] == '\0' ? "Not reported" : values[field], FALSE);
            gtk_label_set_wrap(GTK_LABEL(value), TRUE);
            gtk_label_set_wrap_mode(GTK_LABEL(value), PANGO_WRAP_WORD_CHAR);
            gtk_widget_set_hexpand(value, TRUE);
            gtk_grid_attach(GTK_GRID(grid), Cell(names[field], TRUE), 0, (int)field, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), value, 1, (int)field, 1, 1);
        }
        gtk_box_append(GTK_BOX(box), grid);
    }
    return box;
}
static void WorkspaceDetailsRender(UmiBrokerWorkspaceGtk *owner, gboolean replaceValues)
{
    gboolean pending = owner->hasDetails && !owner->details.complete && !owner->details.failed;
    gboolean ready = WorkspaceDetailsReady(owner);
    GtkWidget *controls[] = {owner->detailsRead, owner->detailsAbandon, owner->detailsQuote,
        owner->detailsContract, owner->detailsExchange};
    gboolean enabled[] = {ready && !pending, ready && pending,
        ready && !pending && owner->hasQuote && owner->quote.contract.contractId != 0U,
        !owner->detailsBusy && !pending, !owner->detailsBusy && !pending};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(controls[i], enabled[i]);
    }
    if (owner->closed || !replaceValues) return;
    char *status = owner->hasDetails ? g_strdup_printf(
        "Request %" PRIu32 " • Contract %" PRIu32 " on %s • %zu descriptions • %s%s\n%s",
        owner->details.requestId, owner->details.requestedContract.contractId,
        owner->details.requestedContract.exchange, owner->details.count,
        owner->details.failed ? "Failed" : owner->details.complete ? "Complete" : "Receiving",
        owner->details.stale ? " • STALE" : "", owner->details.message)
        : g_strdup("No instrument description requested.");
    /* The same inspector also supports descriptive option lookups. Do not
     * present their unset conId as an exact-ID request from this page. */
    if (owner->hasDetails && owner->details.optionLookup) {
        g_free(status);
        status = g_strdup_printf("Request %" PRIu32 " • Option lookup: %s %s %s %s on %s • %s%s\n%s",
            owner->details.requestId, owner->details.requestedOption.symbol,
            owner->details.requestedOption.expiry, owner->details.requestedOption.right,
            owner->details.requestedOption.strike, owner->details.requestedOption.exchange,
            owner->details.failed ? "Failed" : owner->details.complete ? "Complete" : "Receiving",
            owner->details.stale ? " • STALE" : "", owner->details.message);
    }
    LabelText(owner->detailsStatus, status); g_free(status);
    if (!owner->closed)
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->detailsTable), WorkspaceDetailsTable(owner));
}
static void WorkspaceDetailsRefresh(UmiBrokerWorkspaceGtk *owner)
{
    if (owner->closed || owner->connection == NULL) return;
    UmiIbkrContractDetailsSnapshot *copy = g_new0(UmiIbkrContractDetailsSnapshot, 1);
    UmiStatus status = UmiIbkrGtkCopyContractDetails(owner->connection, 60000U, copy);
    gboolean changed = FALSE;
    if (status == UMI_STATUS_OK && copy->count <= UMI_IBKR_CONTRACT_DETAILS_LIMIT) {
        changed = !owner->hasDetails || memcmp(copy, &owner->details, sizeof(*copy)) != 0;
        owner->details = *copy; owner->hasDetails = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        changed = owner->hasDetails;
        memset(&owner->details, 0, sizeof(owner->details)); owner->hasDetails = FALSE;
    } else if (owner->hasDetails) {
        changed = !owner->details.stale; owner->details.stale = true;
    }
    g_free(copy);
    WorkspaceDetailsRender(owner, changed);
}
static void WorkspaceDetailsRead(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || !WorkspaceDetailsReady(owner)) return;
    ++owner->references;
    /* Read both fields before any native notification can edit their values. */
    UmiIbkrQuoteContract contract;
    UmiStatus status = UmiIbkrQuoteContractParse(
        gtk_editable_get_text(GTK_EDITABLE(owner->detailsContract)),
        gtk_editable_get_text(GTK_EDITABLE(owner->detailsExchange)), &contract);
    owner->detailsBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (status == UMI_STATUS_OK && !owner->closed) {
        if (owner->hasDetails && !owner->details.complete && !owner->details.failed) status = UMI_STATUS_BUSY;
        else if (owner->connection == NULL || owner->snapshot.state != UMI_IBKR_READY || owner->snapshot.stale)
            status = UMI_STATUS_UNAVAILABLE;
        else status = UmiIbkrGtkReadContractDetails(owner->connection, &contract);
    }
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
    owner->detailsBusy = FALSE;
    if (!owner->closed) WorkspaceDetailsRender(owner, FALSE);
    if (!owner->closed) LabelText(owner->detailsNotice, status == UMI_STATUS_OK
        ? "Description requested. Wait for the response to complete; quotes are unchanged."
        : status == UMI_STATUS_INVALID_ARGUMENT ? "Enter a positive contract ID and its exchange route."
        : status == UMI_STATUS_BUSY ? "A lookup is already pending. Wait or abandon that local lookup."
        : umi_status_text(status));
    Release(owner);
}
static void WorkspaceDetailsAbandon(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || !WorkspaceDetailsReady(owner) || !owner->hasDetails) return;
    uint32_t expected = owner->details.requestId;
    ++owner->references; owner->detailsBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!owner->closed && owner->connection != NULL)
        status = UmiIbkrGtkAbandonContractDetails(owner->connection, expected);
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
    owner->detailsBusy = FALSE;
    if (!owner->closed) WorkspaceDetailsRender(owner, FALSE);
    if (!owner->closed) LabelText(owner->detailsNotice, status == UMI_STATUS_OK
        ? "Local lookup abandoned. No market subscription or order was cancelled."
        : status == UMI_STATUS_NOT_FOUND ? "That lookup was replaced. Review the current description."
        : umi_status_text(status));
    Release(owner);
}
static void WorkspaceDetailsUseQuote(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || !WorkspaceDetailsReady(owner)) return;
    ++owner->references; owner->detailsBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && owner->hasQuote && owner->quote.contract.contractId != 0U &&
        !(owner->hasDetails && !owner->details.complete && !owner->details.failed)) {
        UmiIbkrQuoteContract selected = owner->quote.contract;
        char number[24]; (void)g_snprintf(number, sizeof(number), "%" PRIu32, selected.contractId);
        gtk_editable_set_text(GTK_EDITABLE(owner->detailsContract), number);
        if (!owner->closed) gtk_editable_set_text(GTK_EDITABLE(owner->detailsExchange), selected.exchange);
        if (!owner->closed) LabelText(owner->detailsNotice, "Quote identity copied. Choose Read description to request metadata.");
    }
    owner->detailsBusy = FALSE;
    if (!owner->closed) WorkspaceDetailsRender(owner, FALSE);
    Release(owner);
}
static GtkWidget *WorkspaceDetailsPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    owner->detailsContract = gtk_entry_new();
    owner->detailsExchange = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->detailsContract), "Positive contract ID");
    gtk_entry_set_max_length(GTK_ENTRY(owner->detailsContract), 10);
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->detailsExchange), "Broker-supported route");
    gtk_entry_set_max_length(GTK_ENTRY(owner->detailsExchange), 31);
    owner->detailsRead = gtk_button_new_with_label("Read description");
    owner->detailsAbandon = gtk_button_new_with_label("Abandon lookup");
    owner->detailsQuote = gtk_button_new_with_label("Use current quote");
    gtk_grid_attach(GTK_GRID(grid), Cell("Contract ID", TRUE), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), owner->detailsContract, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), Cell("Exchange route", TRUE), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), owner->detailsExchange, 1, 1, 1, 1);
    gtk_widget_set_hexpand(owner->detailsContract, TRUE);
    gtk_widget_set_hexpand(owner->detailsExchange, TRUE);
    gtk_grid_attach(GTK_GRID(grid), owner->detailsRead, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), owner->detailsAbandon, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), owner->detailsQuote, 0, 2, 3, 1);
    gtk_box_append(GTK_BOX(box), grid);
    owner->detailsStatus = Cell("", FALSE); owner->detailsNotice = Cell("", FALSE);
    gtk_label_set_wrap(GTK_LABEL(owner->detailsStatus), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->detailsNotice), TRUE);
    gtk_box_append(GTK_BOX(box), owner->detailsStatus); gtk_box_append(GTK_BOX(box), owner->detailsNotice);
    owner->detailsTable = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(owner->detailsTable), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(owner->detailsTable, TRUE);
    gtk_box_append(GTK_BOX(box), owner->detailsTable);
    GtkWidget *note = Cell("Metadata is a point-in-time description, not permission to trade. Blank values are unknown. "
        "A minimum tick alone does not describe every price band or route; order-type lists do not guarantee combinations. "
        "Captures older than 60 seconds are labelled stale. Bond descriptions are not supported.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(note), TRUE);
    gtk_box_append(GTK_BOX(box), note);
    GtkWidget *widgets[] = {owner->detailsContract, owner->detailsExchange, owner->detailsRead,
        owner->detailsAbandon, owner->detailsQuote, owner->detailsStatus, owner->detailsNotice, owner->detailsTable};
    const char *names[] = {"contract", "exchange", "read", "abandon", "quote", "status", "notice", "table"};
    for (size_t i = 0U; i < G_N_ELEMENTS(widgets); ++i) {
        char *name = g_strdup_printf("broker-workspace-details-%s", names[i]);
        gtk_widget_set_name(widgets[i], name); g_free(name);
    }
    g_signal_connect(owner->detailsRead, "clicked", G_CALLBACK(WorkspaceDetailsRead), owner);
    g_signal_connect(owner->detailsAbandon, "clicked", G_CALLBACK(WorkspaceDetailsAbandon), owner);
    g_signal_connect(owner->detailsQuote, "clicked", G_CALLBACK(WorkspaceDetailsUseQuote), owner);
    return box;
}
