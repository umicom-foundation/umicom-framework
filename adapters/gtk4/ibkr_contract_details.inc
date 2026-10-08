/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_contract_details.inc
 * PURPOSE: Present exact-instrument descriptions from the existing read-only connection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static gchar *ContractDetailsText(ConnectionUi *ui, uint64_t now)
{
    if (ui->contractRequest == 0U || ui->connection == NULL)
        return g_strdup("No contract description is captured for this connection. Subscribe to a quote, then "
                        "read its description.");
    UmiIbkrContractDetailsSnapshot *copy = g_try_malloc(sizeof *copy);
    if (copy == NULL)
        return g_strdup("Unable to allocate the contract description.");
    UmiStatus status = UmiIbkrContractDetailsCopy(ui->connection, ui->contractRequest, now, copy);
    if (status != UMI_STATUS_OK)
    {
        g_free(copy);
        return g_strdup(umi_status_text(status));
    }
    GString *text = g_string_new(copy->message);
    g_string_append_printf(text, "\nRequested contract %u on %s. %zu description(s); %s%s.\n",
                           (unsigned)copy->requestedContract.contractId, copy->requestedContract.exchange,
                           copy->count, copy->complete ? "end marker received" : "response incomplete",
                           copy->stale ? "; stale" : "");
    for (size_t index = 0U; index < copy->count; ++index)
    {
        const UmiIbkrContractDescription *row = &copy->items[index];
        g_string_append_printf(
            text,
            "\n%s / %s / %s (contract %u)\nProvider name: %s\nLocal symbol: %s; trading class: %s\n"
            "Exchange: %s; primary: %s\nExpiry: %s; strike: %s; right: %s; multiplier: %s\n"
            "Minimum tick: %s; price magnifier: %s\nMinimum size: %s; size increment: %s; suggested "
            "increment: %s\n"
            "Reported order types: %s\nReported exchanges: %s\nMarket-rule identifiers: %s\n",
            row->symbol, row->securityType, row->currency, (unsigned)row->contractId, row->longName,
            row->localSymbol, row->tradingClass, row->exchange, row->primaryExchange, row->expiry,
            row->strike, row->right, row->multiplier, row->minimumTick, row->priceMagnifier, row->minimumSize,
            row->sizeIncrement, row->suggestedSizeIncrement, row->orderTypes, row->validExchanges,
            row->marketRuleIds);
    }
    g_string_append(
        text, "\nProvider text is historical metadata. Blank values are unknown. "
              "Order-type and exchange lists do not confirm FOK/AON support for their combinations. "
              "Price magnifiers and increments are displayed, not applied to an order. No order was sent.");
    g_free(copy);
    return g_string_free(text, FALSE);
}
static void ContractDetailsPublish(ConnectionUi *ui, uint64_t now)
{
    gchar *text = ContractDetailsText(ui, now);
    if (strcmp(gtk_label_get_text(GTK_LABEL(ui->contractOutput)), text) != 0)
        gtk_label_set_text(GTK_LABEL(ui->contractOutput), text);
    g_free(text);
}
static void ContractDetailsPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy)
        return;
    ui = FillEnter(ui->window);
    if (ui == NULL)
        return;
    /* Publication is the final widget operation. FillEnter retains the window
     * even if a label observer closes the owner during the notification. */
    ContractDetailsPublish(ui, now);
    FillLeave(ui);
}
static void ContractReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiIbkrQuoteSnapshot quote;
    UmiStatus status = FillCurrentQuote(ui, now, &quote);
    if (status == UMI_STATUS_OK && !quote.subscribed)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = UmiIbkrContractDetailsRequest(ui->connection, &quote.contract, now, &ui->contractRequest);
    if (status == UMI_STATUS_OK)
        ContractDetailsPublish(ui, now);
    else
        gtk_label_set_text(GTK_LABEL(ui->contractOutput),
                           status == UMI_STATUS_BUSY
                               ? "A contract lookup is already pending. Wait or abandon the local review."
                               : "Contract lookup was not started. Subscribe to a quote on a supported "
                                 "read-only connection first.");
    FillLeave(ui);
}
static void ContractAbandonClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    if (ui->connection != NULL && ui->contractRequest != 0U)
        (void)UmiIbkrContractDetailsAbandon(ui->connection, ui->contractRequest);
    ContractDetailsPublish(ui, UmiIbkrMonotonicMilliseconds());
    FillLeave(ui);
}
static void ContractDetailsControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Instrument description and route metadata");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    ui->contractRead = gtk_button_new_with_label("Read current quote's contract description");
    ui->contractAbandon = gtk_button_new_with_label("Abandon local contract review");
    ui->contractOutput = Text("Subscribe to a quote, then read its broker-reported instrument description.");
    gtk_widget_set_name(ui->contractRead, "ibkr-contract-read");
    gtk_widget_set_name(ui->contractAbandon, "ibkr-contract-abandon");
    gtk_widget_set_name(ui->contractOutput, "ibkr-contract-output");
    gtk_label_set_selectable(GTK_LABEL(ui->contractOutput), TRUE);
    gtk_box_append(GTK_BOX(box), ui->contractRead);
    gtk_box_append(GTK_BOX(box), ui->contractAbandon);
    gtk_box_append(GTK_BOX(box), ui->contractOutput);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->contractRead, "clicked", G_CALLBACK(ContractReadClicked), window, 0);
    g_signal_connect_object(ui->contractAbandon, "clicked", G_CALLBACK(ContractAbandonClicked), window, 0);
}
