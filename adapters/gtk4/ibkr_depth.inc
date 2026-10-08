/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_depth.inc
 * PURPOSE: Display independently owned depth ladders and their subscription lifecycle.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void DepthSideText(GString *text, const char *name, const UmiIbkrDepthRow *rows, size_t count,
                          uint64_t now)
{
    g_string_append_printf(text, "\n%s (%zu levels)\n", name, count);
    for (size_t i = 0U; i < count; ++i)
    {
        char price[96], size[96];
        if (UmiDecimalFormat(rows[i].price, price, sizeof price) != UMI_STATUS_OK ||
            UmiDecimalFormat(rows[i].size, size, sizeof size) != UMI_STATUS_OK)
            continue;
        g_string_append_printf(text, "%zu. %s x %s%s%s | age %" PRIu64 " ms%s\n", i + 1U, price, size,
                               rows[i].hasMarketMaker ? " | " : "", rows[i].marketMaker,
                               now >= rows[i].receivedAtMilliseconds ? now - rows[i].receivedAtMilliseconds
                                                                     : 0U,
                               rows[i].stale ? " | STALE" : "");
    }
}
static void DepthPublish(ConnectionUi *ui, uint64_t now)
{
    UmiIbkrDepthSnapshot *copy = g_try_malloc(sizeof *copy);
    UmiStatus status = copy == NULL ? UMI_STATUS_OUT_OF_MEMORY
                                    : UmiIbkrDepthCopy(ui->connection, ui->depthRequest, now, 15000U, copy);
    GString *text = g_string_new(NULL);
    if (status != UMI_STATUS_OK)
        g_string_append(text, "No depth stream. Choose a contract ID and exchange, then subscribe.");
    else
    {
        g_string_append_printf(text, "%s\nContract %u | %s | %s | %s%s | resets %" PRIu64 "\n", copy->message,
                               (unsigned)copy->selection.contractId, copy->selection.exchange,
                               copy->selection.smartDepth ? "SMART aggregation" : "Direct depth",
                               copy->subscribed ? "Subscribed" : "Stopped", copy->stale ? " | STALE" : "",
                               copy->resetCount);
        DepthSideText(text, "Bids", copy->bids, copy->bidCount, now);
        DepthSideText(text, "Asks", copy->asks, copy->askCount, now);
        g_string_append(text, "\nThis is received market depth, not a complete view of all liquidity. "
                              "It does not guarantee a fill, prevent partial fills or place an order.");
    }
    g_free(copy);
    if (strcmp(gtk_label_get_text(GTK_LABEL(ui->depthOutput)), text->str) != 0)
        gtk_label_set_text(GTK_LABEL(ui->depthOutput), text->str);
    g_string_free(text, TRUE);
}
static void DepthPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy || ui->depthRequest == 0U)
        return;
    ui = FillEnter(ui->window);
    if (ui == NULL)
        return;
    DepthPublish(ui, now);
    FillLeave(ui);
}
static void DepthStartClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiIbkrDepthSnapshot *previous = g_try_malloc(sizeof *previous);
    if (previous == NULL)
    {
        gtk_label_set_text(GTK_LABEL(ui->depthOutput), "Unable to allocate the depth view.");
        FillLeave(ui);
        return;
    }
    bool active =
        UmiIbkrDepthCopy(ui->connection, ui->depthRequest, now, 15000U, previous) == UMI_STATUS_OK &&
        previous->subscribed;
    g_free(previous);
    if (active)
    {
        gtk_label_set_text(GTK_LABEL(ui->depthOutput),
                           "Stop the current depth stream before starting another.");
        FillLeave(ui);
        return;
    }
    UmiIbkrDepthSelection selection = {0};
    uint64_t identity = 0U;
    const char *exchange = gtk_editable_get_text(GTK_EDITABLE(ui->quoteExchange));
    bool valid = ExecutionsInputId(ui->quoteContract, INT32_MAX, &identity) &&
                 strlen(exchange) < sizeof selection.exchange;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (valid)
    {
        selection.contractId = (uint32_t)identity;
        strcpy(selection.exchange, exchange);
        selection.rows = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->depthRows));
        selection.smartDepth = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->depthSmart));
        status = UmiIbkrDepthSubscribe(ui->connection, &selection, now, &ui->depthRequest);
    }
    if (status == UMI_STATUS_OK)
        DepthPublish(ui, now);
    else
        gtk_label_set_text(
            GTK_LABEL(ui->depthOutput),
            "Depth was not started. Connect and enter a confirmed contract ID and exchange. "
            "Use SMART with aggregation enabled, or a direct exchange with aggregation disabled.");
    FillLeave(ui);
}
static void DepthStopClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    UmiStatus status = UmiIbkrDepthCancel(ui->connection, ui->depthRequest);
    if (status == UMI_STATUS_OK)
        DepthPublish(ui, UmiIbkrMonotonicMilliseconds());
    else
        gtk_label_set_text(GTK_LABEL(ui->depthOutput),
                           "The depth stop request was not queued. Check the connection.");
    FillLeave(ui);
}
static void DepthControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Market depth");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    gtk_box_append(GTK_BOX(box), Text("Uses the contract ID and exchange entered above. Depth requires the "
                                      "broker's data permissions."));
    ui->depthRows = gtk_spin_button_new_with_range(1.0, UMI_IBKR_DEPTH_ROW_LIMIT, 1.0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->depthRows), 5.0);
    ui->depthSmart = gtk_check_button_new_with_label("Aggregate SMART depth");
    ui->depthStart = gtk_button_new_with_label("Subscribe to market depth");
    ui->depthStop = gtk_button_new_with_label("Stop depth stream");
    ui->depthOutput = Text("No depth stream. Choose a contract ID and exchange after connecting.");
    gtk_label_set_selectable(GTK_LABEL(ui->depthOutput), TRUE);
    GtkWidget *controls[] = {ui->depthRows, ui->depthSmart, ui->depthStart, ui->depthStop, ui->depthOutput};
    const char *names[] = {"ibkr-depth-rows", "ibkr-depth-smart", "ibkr-depth-start", "ibkr-depth-stop",
                           "ibkr-depth-output"};
    gtk_box_append(GTK_BOX(box), Text("Maximum rows on each side"));
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i)
    {
        gtk_widget_set_name(controls[i], names[i]);
        gtk_box_append(GTK_BOX(box), controls[i]);
    }
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->depthStart, "clicked", G_CALLBACK(DepthStartClicked), window, 0);
    g_signal_connect_object(ui->depthStop, "clicked", G_CALLBACK(DepthStopClicked), window, 0);
}
