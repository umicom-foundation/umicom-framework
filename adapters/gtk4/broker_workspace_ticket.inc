/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_ticket.inc
 *
 * PURPOSE:
 *   Present explicit broker account authority, immutable order reviews and observed execution results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The ticket projects the Framework review, never a second execution engine.
 * Editing any field consumes the local review token; submission remains a
 * separate gesture and broker status is not inferred from a successful write. */
static void WorkspaceTicketRefresh(UmiBrokerWorkspaceGtk *owner)
{
    if (!owner->ticketSubmit || owner->closed) return;
    UmiIbkrExecutionReport *report = g_new0(UmiIbkrExecutionReport, 1);
    UmiStatus status = owner->connection
        ? UmiIbkrGtkExecutionCopy(owner->connection, report) : UMI_STATUS_NOT_FOUND;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    if (status != UMI_STATUS_OK || !report->enabled || !report->reviewPending ||
        now < owner->ticketReviewedAt || now - owner->ticketReviewedAt > 30000U)
        owner->ticketToken = 0U;
    gboolean confirmed = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->ticketConfirm));
    gtk_widget_set_sensitive(owner->ticketSubmit, owner->ticketToken != 0U && confirmed && !owner->ticketBusy);
    if (!owner->closed) {
        GString *text = g_string_new(NULL);
        g_string_append_printf(text, "%s • Account: %s • Requested mode: %s\n",
            report->enabled ? "Order entry enabled for this session" : "Read-only session",
            owner->snapshot.selectedAccount[0] ? owner->snapshot.selectedAccount : "Select and read an account first",
            UmiBrokerAccessModeName(owner->plan.mode));
        for (size_t i = 0U; i < report->count; ++i) {
            const UmiIbkrOrderSubmission *row = &report->orders[i];
            g_string_append_printf(text, "#%u • %s %u • contract %u • %s %s • %s\n%s\n",
                (unsigned)row->orderId, row->draft.sell ? "SELL" : "BUY", (unsigned)row->draft.quantity,
                (unsigned)row->draft.contractId, row->draft.limitPrice, row->draft.exchange,
                row->uncertain ? "UNCERTAIN" : row->filled ? "Filled" : row->cancelled ? "Cancelled" :
                row->rejected ? "Rejected" : row->brokerStatus[0] ? row->brokerStatus : "Awaiting broker",
                row->message);
        }
        if (strcmp(gtk_label_get_text(GTK_LABEL(owner->ticketStatus)), text->str))
            LabelText(owner->ticketStatus, text->str);
        g_string_free(text, TRUE);
    }
    g_free(report);
}
static void WorkspaceTicketEdited(GtkEditable *editable, gpointer data)
{
    (void)editable; UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed) return;
    ++owner->references; owner->ticketToken = 0U;
    gtk_widget_set_sensitive(owner->ticketSubmit, FALSE);
    if (!owner->closed) LabelText(owner->ticketReview, "Ticket changed. Read metadata and review again before submitting.");
    Release(owner);
}
static void WorkspaceTicketChoice(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec; WorkspaceTicketEdited(NULL, data);
}
static void WorkspaceTicketToggle(GtkCheckButton *button, gpointer data)
{
    (void)button; WorkspaceTicketEdited(NULL, data);
}
static void WorkspaceTicketAction(GtkButton *button, gpointer data)
{
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->ticketBusy || !owner->connection) return;
    ++owner->references; owner->ticketBusy = TRUE;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    const char *notice = NULL;
    guint action = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "ticket-action"));
    if (action != 4U) owner->ticketToken = 0U;
    /* Snapshot the edited values before native notifications can run. */
    char *idText = g_strdup(gtk_editable_get_text(GTK_EDITABLE(owner->ticketContract)));
    char *exchange = g_strdup(gtk_editable_get_text(GTK_EDITABLE(owner->ticketExchange)));
    char *price = g_strdup(gtk_editable_get_text(GTK_EDITABLE(owner->ticketPrice)));
    char *quantityText = g_strdup(gtk_editable_get_text(GTK_EDITABLE(owner->ticketQuantity)));
    char account[64]; memcpy(account, owner->snapshot.selectedAccount, sizeof account);
    guint64 contract = 0U, quantity = 0U;
    gboolean identity = g_ascii_string_to_unsigned(idText, 10, 1U, INT32_MAX, &contract, NULL) &&
        exchange[0] && strlen(exchange) < 64U;
    gboolean confirmed = gtk_check_button_get_active(GTK_CHECK_BUTTON(owner->ticketConfirm));
    if (action == 0U) {
        UmiTradingEnvironment mode = owner->plan.mode == UMI_BROKER_ACCESS_LIVE ? UMI_TRADING_LIVE : UMI_TRADING_PAPER;
        status = UmiIbkrGtkExecutionEnable(owner->connection, account, mode, confirmed);
        notice = "Order entry enabled only for the selected account in this connection.";
    } else if (action == 1U) {
        status = UmiIbkrGtkExecutionDisable(owner->connection);
        notice = "New orders disabled. Existing submitted orders remain with the broker.";
    } else if (action == 2U && identity) {
        UmiIbkrQuoteContract selected = {0}; selected.contractId = (uint32_t)contract;
        strcpy(selected.exchange, exchange);
        status = UmiIbkrGtkReadContractDetails(owner->connection, &selected);
        notice = "Instrument metadata requested. Wait for completion, then read price rules.";
    } else if (action == 6U && identity) {
        status = UmiIbkrGtkOrderReadPriceRule(owner->connection, exchange);
        notice = "Price rule requested or already retained. Review after it completes.";
    } else if (action == 3U && identity &&
        g_ascii_string_to_unsigned(quantityText, 10, 1U, 1000000U, &quantity, NULL) && strlen(price) < 48U) {
        UmiIbkrOrderDraft draft = {0}; draft.contractId = (uint32_t)contract; draft.quantity = (uint32_t)quantity;
        strcpy(draft.account, account); strcpy(draft.exchange, exchange); strcpy(draft.limitPrice, price);
        draft.sell = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->ticketSide)) == 1U;
        UmiIbkrOrderReview review = {0};
        status = confirmed ? UmiIbkrGtkOrderPrepare(owner->connection, &draft, &review) : UMI_STATUS_PERMISSION_DENIED;
        if (status == UMI_STATUS_OK && !owner->closed) {
            owner->ticketToken = review.token; owner->ticketReviewedAt = review.createdAtMilliseconds;
            char *text = g_strdup_printf("%s • %s\n%s %u × %s (%s, %s) at limit %s %s\n"
                "Contract %u • route %s • DAY • regular hours only\n"
                "Review the account, side, quantity, price and contract. Submit consumes this review once.",
                UmiBrokerAccessModeName(owner->plan.mode), review.draft.account,
                draft.sell ? "SELL" : "BUY", (unsigned)draft.quantity, review.instrument.localSymbol,
                review.instrument.securityType, review.instrument.expiry, draft.limitPrice, review.instrument.currency,
                (unsigned)draft.contractId, draft.exchange);
            LabelText(owner->ticketReview, text); g_free(text);
        }
        notice = "Review expires after 30 seconds. Editing requires a new review.";
    } else if (action == 4U) {
        uint64_t token = owner->ticketToken;
        owner->ticketToken = 0U; uint32_t orderId = 0U;
        status = confirmed && token ? UmiIbkrGtkOrderSubmit(owner->connection, token, &orderId)
            : UMI_STATUS_PERMISSION_DENIED;
        notice = "Order queued once. Check its broker status and executions; a queue entry is not acceptance.";
    } else if (action == 5U) {
        guint64 orderId = 0U;
        if (g_ascii_string_to_unsigned(gtk_editable_get_text(GTK_EDITABLE(owner->ticketCancelId)),
                10, 0U, INT32_MAX, &orderId, NULL))
            status = UmiIbkrGtkOrderCancelSubmitted(owner->connection, (uint32_t)orderId);
        notice = "Cancellation requested. The order can still fill until the broker confirms.";
    }
    g_free(idText); g_free(exchange); g_free(price); g_free(quantityText);
    owner->ticketBusy = FALSE;
    if (!owner->closed) LabelText(owner->ticketNotice, status == UMI_STATUS_OK ? notice :
        status == UMI_STATUS_INVALID_STATE ? "Not ready. Read the account, resolve this exact contract and its price rule, then review again." :
        umi_status_text(status));
    if (!owner->closed) WorkspaceTicketRefresh(owner);
    Release(owner);
}
static GtkWidget *WorkspaceTicketPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    owner->ticketConfirm = gtk_check_button_new_with_label("I checked the account and Paper / Live mode in Gateway");
    gtk_box_append(GTK_BOX(page), owner->ticketConfirm);
    const char *sides[] = {"Buy", "Sell", NULL};
    owner->ticketSide = gtk_drop_down_new_from_strings(sides);
    owner->ticketContract = gtk_entry_new(); owner->ticketExchange = gtk_entry_new();
    owner->ticketQuantity = gtk_entry_new(); owner->ticketPrice = gtk_entry_new();
    owner->ticketCancelId = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(owner->ticketQuantity), "1");
    GtkWidget *fields[] = {owner->ticketContract, owner->ticketExchange, owner->ticketSide,
        owner->ticketQuantity, owner->ticketPrice, owner->ticketCancelId};
    const char *labels[] = {"Contract ID", "Exchange route", "Side", "Quantity", "Limit price", "Submitted order ID to cancel"};
    const char *names[] = {"contract", "exchange", "side", "quantity", "price", "cancel-id"};
    GtkWidget *grid = gtk_grid_new(); gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    for (int i = 0; i < 6; ++i) {
        char *name = g_strconcat("broker-ticket-", names[i], NULL);
        gtk_widget_set_name(fields[i], name); g_free(name);
        gtk_widget_set_hexpand(fields[i], TRUE);
        gtk_grid_attach(GTK_GRID(grid), Cell(labels[i], FALSE), 0, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), fields[i], 1, i, 1, 1);
        if (i != 2) gtk_entry_set_max_length(GTK_ENTRY(fields[i]), i == 1 ? 63 : i == 4 ? 47 : 10);
    }
    gtk_box_append(GTK_BOX(page), grid);
    GtkWidget *actions = gtk_flow_box_new(); gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(actions), GTK_SELECTION_NONE);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(actions), 4);
    const char *buttons[] = {"Enable order entry", "Read only", "Read instrument", "Review order",
        "Submit reviewed order", "Cancel submitted order", "Read price rules"};
    const char *buttonNames[] = {"enable", "disable", "resolve", "review", "submit", "cancel", "rules"};
    for (guint i = 0U; i < 7U; ++i) {
        GtkWidget *button = gtk_button_new_with_label(buttons[i]); owner->ticketButtons[i] = button;
        char *name = g_strconcat("broker-ticket-", buttonNames[i], NULL); gtk_widget_set_name(button, name); g_free(name);
        g_object_set_data(G_OBJECT(button), "ticket-action", GUINT_TO_POINTER(i));
        g_signal_connect(button, "clicked", G_CALLBACK(WorkspaceTicketAction), owner);
        gtk_flow_box_insert(GTK_FLOW_BOX(actions), button, -1);
    }
    owner->ticketSubmit = owner->ticketButtons[4]; gtk_widget_set_sensitive(owner->ticketSubmit, FALSE);
    gtk_box_append(GTK_BOX(page), actions);
    owner->ticketReview = Cell("No reviewed order. Shares and futures: integer quantity, DAY limit, regular hours.", FALSE);
    owner->ticketNotice = Cell("", FALSE); owner->ticketStatus = Cell("", FALSE);
    GtkWidget *outputs[] = {owner->ticketReview, owner->ticketNotice, owner->ticketStatus};
    const char *outputNames[] = {"broker-ticket-review-text", "broker-ticket-notice", "broker-ticket-status"};
    for (size_t i = 0U; i < G_N_ELEMENTS(outputs); ++i) {
        gtk_widget_set_name(outputs[i], outputNames[i]); gtk_label_set_wrap(GTK_LABEL(outputs[i]), TRUE);
        gtk_label_set_selectable(GTK_LABEL(outputs[i]), TRUE); gtk_box_append(GTK_BOX(page), outputs[i]);
    }
    for (size_t i = 0U; i < 5U; ++i)
        if (i != 2U) g_signal_connect(fields[i], "changed", G_CALLBACK(WorkspaceTicketEdited), owner);
    g_signal_connect(owner->ticketSide, "notify::selected", G_CALLBACK(WorkspaceTicketChoice), owner);
    g_signal_connect(owner->ticketConfirm, "toggled", G_CALLBACK(WorkspaceTicketToggle), owner);
    return page;
}
/* Seeding from chart/depth copies observed identity and price, then navigates.
 * It does not grant authority, request metadata or dispatch an order. */
static void WorkspaceTicketSeed(UmiBrokerWorkspaceGtk *owner, uint32_t contract,
    const char *exchange, const char *price, gboolean sell)
{
    if (owner->closed) return;
    ++owner->references; owner->ticketToken = 0U;
    /* GTK notifications can change the observation that supplied these strings.
     * Own the seed across every setter so a callback cannot invalidate a borrow. */
    char *routeCopy = g_strdup(exchange); char *priceCopy = g_strdup(price);
#if 0
    /* Superseded borrowed-string setters are retained for ownership review.
     * The owned copies below keep the same ticket behaviour across callbacks. */
    char number[24]; (void)g_snprintf(number, sizeof number, "%u", (unsigned)contract);
    gtk_editable_set_text(GTK_EDITABLE(owner->ticketContract), number);
    if (!owner->closed) gtk_editable_set_text(GTK_EDITABLE(owner->ticketExchange), exchange);
    if (!owner->closed) gtk_editable_set_text(GTK_EDITABLE(owner->ticketPrice), price);
#endif
    char number[24]; (void)g_snprintf(number, sizeof number, "%u", (unsigned)contract);
    gtk_editable_set_text(GTK_EDITABLE(owner->ticketContract), number);
    if (!owner->closed) gtk_editable_set_text(GTK_EDITABLE(owner->ticketExchange), routeCopy);
    if (!owner->closed) gtk_editable_set_text(GTK_EDITABLE(owner->ticketPrice), priceCopy);
    g_free(routeCopy); g_free(priceCopy);
    if (!owner->closed) gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->ticketSide), sell ? 1U : 0U);
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkSelectPage(owner, UMI_BROKER_PAGE_ORDER_TICKET);
    Release(owner);
}
