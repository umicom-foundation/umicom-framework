/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_pnl.inc
 * PURPOSE: Present independent account and position profit-and-loss subscriptions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* The window keeps a subscription identity separate from editable controls.
 * Changing the selected account must never relabel values already received for
 * another account. All wire validation and ownership live in Framework. */
static gchar *PnlText(const UmiIbkrPnlSnapshot *copy)
{
    GString *text = g_string_new(copy->message);
    g_string_append_printf(
        text, "\nAccount %s; model %s; contract %u; request %u.\n%s%s\n", copy->selection.account,
        copy->selection.modelCode[0] ? copy->selection.modelCode : "(all)",
        (unsigned)copy->selection.contractId, (unsigned)copy->requestId,
        copy->subscribed ? "Subscribed. " : "Stopped. ", copy->stale ? "STALE" : "Recently received.");
    const UmiIbkrPnlNumber *numbers[] = {&copy->daily, &copy->unrealized, &copy->realized, &copy->position,
                                         &copy->marketValue};
    const char *names[] = {"Daily P&L", "Unrealized P&L", "Realized P&L", "Position", "Market value"};
    size_t count = copy->selection.contractId == 0U ? 3U : G_N_ELEMENTS(numbers);
    for (size_t i = 0U; i < count; ++i)
    {
        g_string_append_printf(text, "%s: ", names[i]);
        if (!copy->received)
            g_string_append(text, "waiting\n");
        else if (numbers[i]->exact)
            g_string_append_printf(text, "%s\n", numbers[i]->reportedText);
        else
            g_string_append_printf(text, "unavailable in the exact decimal range%s%s\n",
                                   numbers[i]->reportedText[0] ? "; reported " : "",
                                   numbers[i]->reportedText);
    }
    g_string_append(text,
                    "\nThe broker controls P&L reset periods and valuation. "
                    "These callbacks do not include a currency field. Confirm the account settings in TWS; "
                    "do not combine accounts or currencies from this display.");
    return g_string_free(text, FALSE);
}
static void PnlPublish(ConnectionUi *ui, uint64_t now)
{
    UmiIbkrPnlSnapshot copy;
    gchar *text;
    UmiStatus status = ui->connection != NULL && ui->pnlRequest != 0U
                           ? UmiIbkrPnlCopy(ui->connection, ui->pnlRequest, now, 15000U, &copy)
                           : UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        text = PnlText(&copy);
    else
        text = g_strdup("No P&L subscription. Connect, select an advertised account and subscribe.");
    if (strcmp(gtk_label_get_text(GTK_LABEL(ui->pnlOutput)), text) != 0)
        gtk_label_set_text(GTK_LABEL(ui->pnlOutput), text);
    g_free(text);
}
static void PnlPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy)
        return;
    ui = FillEnter(ui->window);
    if (ui == NULL)
        return;
    PnlPublish(ui, now);
    FillLeave(ui);
}
static void PnlStartClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiIbkrPnlSnapshot previous;
    if (ui->connection != NULL && ui->pnlRequest != 0U &&
        UmiIbkrPnlCopy(ui->connection, ui->pnlRequest, now, 15000U, &previous) == UMI_STATUS_OK &&
        previous.subscribed)
    {
        gtk_label_set_text(GTK_LABEL(ui->pnlOutput),
                           "Stop the current P&L subscription before starting another.");
        FillLeave(ui);
        return;
    }
    UmiIbkrPnlSelection selection = {0};
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->accounts));
    const char *model = gtk_editable_get_text(GTK_EDITABLE(ui->pnlModel));
    const char *contract = gtk_editable_get_text(GTK_EDITABLE(ui->pnlContract));
    uint64_t identity = 0U;
    bool valid = (size_t)index < ui->snapshot.accountCount && strlen(model) < sizeof selection.modelCode &&
                 (*contract == '\0' || ExecutionsInputId(ui->pnlContract, INT32_MAX, &identity));
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (valid)
    {
        strcpy(selection.account, ui->snapshot.accounts[index]);
        strcpy(selection.modelCode, model);
        selection.contractId = (uint32_t)identity;
        status = UmiIbkrPnlSubscribe(ui->connection, &selection, now, &ui->pnlRequest);
    }
    if (status == UMI_STATUS_OK)
        PnlPublish(ui, now);
    else
        gtk_label_set_text(GTK_LABEL(ui->pnlOutput), "P&L was not started. Connect, select an advertised "
                                                     "account, and use an empty or positive contract ID.");
    FillLeave(ui);
}
static void PnlStopClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    UmiStatus status = UmiIbkrPnlCancel(ui->connection, ui->pnlRequest);
    if (status == UMI_STATUS_OK)
        PnlPublish(ui, UmiIbkrMonotonicMilliseconds());
    else
        gtk_label_set_text(
            GTK_LABEL(ui->pnlOutput),
            "The stop request was not queued. The subscription remains unchanged; check the connection.");
    FillLeave(ui);
}
static void PnlControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Account and position P&L");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    ui->pnlModel = gtk_entry_new();
    ui->pnlContract = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(ui->pnlModel), 63);
    gtk_entry_set_max_length(GTK_ENTRY(ui->pnlContract), 10);
    gtk_box_append(GTK_BOX(box), Text("Model code (optional)"));
    gtk_box_append(GTK_BOX(box), ui->pnlModel);
    gtk_box_append(GTK_BOX(box), Text("Contract ID (leave empty for account P&L)"));
    gtk_box_append(GTK_BOX(box), ui->pnlContract);
    ui->pnlStart = gtk_button_new_with_label("Subscribe to selected account P&L");
    ui->pnlStop = gtk_button_new_with_label("Stop P&L subscription");
    ui->pnlOutput = Text("No P&L subscription. Select an advertised account after connecting.");
    gtk_label_set_selectable(GTK_LABEL(ui->pnlOutput), TRUE);
    GtkWidget *controls[] = {ui->pnlModel, ui->pnlContract, ui->pnlStart, ui->pnlStop, ui->pnlOutput};
    const char *names[] = {"ibkr-pnl-model", "ibkr-pnl-contract", "ibkr-pnl-start", "ibkr-pnl-stop",
                           "ibkr-pnl-output"};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i)
        gtk_widget_set_name(controls[i], names[i]);
    gtk_box_append(GTK_BOX(box), ui->pnlStart);
    gtk_box_append(GTK_BOX(box), ui->pnlStop);
    gtk_box_append(GTK_BOX(box), ui->pnlOutput);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->pnlStart, "clicked", G_CALLBACK(PnlStartClicked), window, 0);
    g_signal_connect_object(ui->pnlStop, "clicked", G_CALLBACK(PnlStopClicked), window, 0);
}
