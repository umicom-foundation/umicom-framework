/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_option_chain.inc
 * PURPOSE: Browse option definitions and resolve one explicit expiry, strike and right.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Option lists hold copied display strings. Their indices are valid only for
 * optionDisplayedRequest; a new connection or lookup retires that identity. */
static void OptionChoices(ConnectionUi *ui)
{
    GtkStringList *expiries = gtk_string_list_new(NULL), *strikes = gtk_string_list_new(NULL);
    UmiIbkrOptionChain *row = g_try_malloc(sizeof *row);
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->optionResults));
    if (row && ui->optionDisplayedRequest == ui->optionRequest &&
        UmiIbkrOptionChainItemCopy(ui->connection, ui->optionRequest, (size_t)index, row) == UMI_STATUS_OK)
    {
        for (size_t i = 0; i < row->expiryCount; ++i)
            gtk_string_list_append(expiries, row->expirations[i]);
        for (size_t i = 0; i < row->strikeCount; ++i)
            gtk_string_list_append(strikes, row->strikes[i].reportedText);
    }
    g_free(row);
    gtk_drop_down_set_model(GTK_DROP_DOWN(ui->optionExpiry), G_LIST_MODEL(expiries));
    if (!ui->closed)
        gtk_drop_down_set_model(GTK_DROP_DOWN(ui->optionStrike), G_LIST_MODEL(strikes));
    g_object_unref(expiries);
    g_object_unref(strikes);
}
static void OptionChainPublish(ConnectionUi *ui, uint64_t now)
{
    UmiIbkrOptionChainSnapshot s;
    if (UmiIbkrOptionChainCopy(ui->connection, ui->optionRequest, now, &s) != UMI_STATUS_OK)
    {
        gtk_label_set_text(
            GTK_LABEL(ui->optionOutput),
            "No option definitions. Enter a resolved underlying contract and request its chains.");
        return;
    }
    GString *text = g_string_new(s.message);
    g_string_append_printf(text, "\nUnderlying %s (%u), %zu exchange/class definitions. %s\n",
                           s.query.underlyingSymbol, (unsigned)s.query.underlyingContractId, s.count,
                           s.stale ? "INCOMPLETE OR STALE" : "Complete");
    if (s.complete && !s.stale && ui->optionDisplayedRequest != s.requestId)
    {
        GtkStringList *items = gtk_string_list_new(NULL);
        UmiIbkrOptionChain *row = g_try_malloc(sizeof *row);
        if (row)
        {
            for (size_t i = 0; i < s.count; ++i)
                if (UmiIbkrOptionChainItemCopy(ui->connection, s.requestId, i, row) == UMI_STATUS_OK)
                {
                    gchar *label = g_strdup_printf("%s | %s | multiplier %s | %zu expiries | %zu strikes",
                                                   row->exchange, row->tradingClass, row->multiplier,
                                                   row->expiryCount, row->strikeCount);
                    gtk_string_list_append(items, label);
                    g_free(label);
                }
            ui->optionDisplayedRequest = s.requestId;
        }
        g_free(row);
        gtk_drop_down_set_model(GTK_DROP_DOWN(ui->optionResults), G_LIST_MODEL(items));
        g_object_unref(items);
        if (!ui->closed)
            OptionChoices(ui);
    }
    if (ui->optionContractRequest)
    {
        UmiIbkrContractDetailsSnapshot *details = g_try_malloc(sizeof *details);
        if (details && UmiIbkrContractDetailsCopy(ui->connection, ui->optionContractRequest, now, details) ==
                           UMI_STATUS_OK)
        {
            g_string_append_printf(text, "Contract resolution: %s | %zu matches | %s\n", details->message,
                                   details->count,
                                   details->complete && !details->stale ? "complete" : "not ready");
            for (size_t i = 0; i < details->count; ++i)
            {
                const UmiIbkrContractDescription *row = &details->items[i];
                g_string_append_printf(text, "%u | %s | %s | %s %s | %s | %s\n", (unsigned)row->contractId,
                                       row->localSymbol, row->expiry, row->strike, row->right, row->currency,
                                       row->exchange);
            }
        }
        g_free(details);
    }
    g_string_append(text, "Expiry and strike lists describe candidates. Resolve a selected call or put "
                          "before using its contract ID. No order is placed.");
    if (!ui->closed && strcmp(gtk_label_get_text(GTK_LABEL(ui->optionOutput)), text->str))
        gtk_label_set_text(GTK_LABEL(ui->optionOutput), text->str);
    g_string_free(text, TRUE);
}
static void OptionChainPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy || !ui->optionRequest)
        return;
    ui = FillEnter(ui->window);
    if (!ui)
        return;
    OptionChainPublish(ui, now);
    FillLeave(ui);
}
static void OptionChainReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    UmiIbkrOptionChainQuery q = {0};
    guint64 id;
    const char *symbol = gtk_editable_get_text(GTK_EDITABLE(ui->optionSymbol));
    const char *type = gtk_editable_get_text(GTK_EDITABLE(ui->optionType));
    const char *exchange = gtk_editable_get_text(GTK_EDITABLE(ui->optionExchange));
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    if (g_ascii_string_to_unsigned(gtk_editable_get_text(GTK_EDITABLE(ui->quoteContract)), 10, 1,
                                   (guint64)INT_MAX, &id, NULL) &&
        strlen(symbol) < sizeof q.underlyingSymbol && strlen(type) < sizeof q.underlyingSecurityType &&
        strlen(exchange) < sizeof q.exchange)
    {
        q.underlyingContractId = (uint32_t)id;
        strcpy(q.underlyingSymbol, symbol);
        strcpy(q.underlyingSecurityType, type);
        strcpy(q.exchange, exchange);
        status = UmiIbkrOptionChainRequest(ui->connection, &q, now, &ui->optionRequest);
    }
    if (status == UMI_STATUS_OK)
    {
        ui->optionDisplayedRequest = 0U;
        ui->optionContractRequest = 0U;
        gtk_drop_down_set_model(GTK_DROP_DOWN(ui->optionResults), NULL);
        if (!ui->closed)
            OptionChoices(ui);
        if (!ui->closed)
            OptionChainPublish(ui, now);
    }
    else
    {
        gchar *message = g_strdup_printf("Option lookup not started: %s. Check the underlying and allow an "
                                         "earlier lookup to finish or abandon it.",
                                         umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(ui->optionOutput), message);
        g_free(message);
    }
    FillLeave(ui);
}
static void OptionChainAbandonClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    (void)UmiIbkrOptionChainAbandon(ui->connection, ui->optionRequest);
    OptionChainPublish(ui, UmiIbkrMonotonicMilliseconds());
    FillLeave(ui);
}
static void OptionChainChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    OptionChoices(ui);
    if (!ui->closed)
        OptionChainPublish(ui, UmiIbkrMonotonicMilliseconds());
    FillLeave(ui);
}
static void OptionResolveClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    UmiIbkrOptionSelection selection = {0};
    selection.chainRequest = ui->optionDisplayedRequest;
    selection.chainIndex = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->optionResults));
    selection.expiryIndex = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->optionExpiry));
    selection.strikeIndex = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->optionStrike));
    guint right = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->optionRight));
    selection.right = right == 0U ? 'C' : right == 1U ? 'P' : 0;
    const char *currency = gtk_editable_get_text(GTK_EDITABLE(ui->optionCurrency));
    const char *exchange = gtk_editable_get_text(GTK_EDITABLE(ui->quoteExchange));
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    if (selection.chainRequest == ui->optionRequest && strlen(currency) < sizeof selection.currency &&
        strlen(exchange) < sizeof selection.exchange)
    {
        strcpy(selection.currency, currency);
        strcpy(selection.exchange, exchange);
        status = UmiIbkrOptionContractRequest(ui->connection, &selection, now, &ui->optionContractRequest);
    }
    if (status == UMI_STATUS_OK)
        OptionChainPublish(ui, now);
    else
    {
        gchar *message = g_strdup_printf("Option resolution not started: %s. Select a completed chain, "
                                         "expiry, strike, right, currency and quote route.",
                                         umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(ui->optionOutput), message);
        g_free(message);
    }
    FillLeave(ui);
}
static void OptionApplyClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    UmiIbkrContractDetailsSnapshot *s = g_try_malloc(sizeof *s);
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    bool valid =
        s && !ui->quoteSubscribed &&
        UmiIbkrContractDetailsCopy(ui->connection, ui->optionContractRequest, now, s) == UMI_STATUS_OK &&
        s->optionLookup && s->complete && !s->stale && s->count == 1U;
    char id[32] = {0};
    if (valid)
        g_snprintf(id, sizeof id, "%u", (unsigned)s->items[0].contractId);
    g_free(s);
    if (valid)
    {
        FillWatchRetire(ui);
        gtk_editable_set_text(GTK_EDITABLE(ui->quoteContract), id);
        if (!ui->closed)
            gtk_label_set_text(
                GTK_LABEL(ui->optionOutput),
                "Resolved option ID copied. Review the quote route and explicitly request market data.");
    }
    else
        gtk_label_set_text(GTK_LABEL(ui->optionOutput), "Wait for exactly one completed contract match and "
                                                        "stop any active quote stream before applying it.");
    FillLeave(ui);
}
static void OptionChainControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Option chains"), *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8),
              *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    gtk_box_append(GTK_BOX(box),
                   Text("Use the underlying Contract ID above. Chain exchange is a discovery filter; the "
                        "quote Exchange above is the explicit route used when resolving one option."));
    ui->optionSymbol = gtk_entry_new();
    ui->optionType = gtk_entry_new();
    ui->optionExchange = gtk_entry_new();
    ui->optionCurrency = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(ui->optionType), "STK");
    ui->optionResults = gtk_drop_down_new(NULL, NULL);
    ui->optionExpiry = gtk_drop_down_new(NULL, NULL);
    ui->optionStrike = gtk_drop_down_new(NULL, NULL);
    const char *rights[] = {"Call", "Put", NULL};
    ui->optionRight = gtk_drop_down_new_from_strings(rights);
    Field(GTK_GRID(grid), "Underlying symbol", ui->optionSymbol, 0, "ibkr-option-symbol");
    Field(GTK_GRID(grid), "Underlying type (STK, IND, FUT)", ui->optionType, 1, "ibkr-option-type");
    Field(GTK_GRID(grid), "Chain exchange (blank for all)", ui->optionExchange, 2, "ibkr-option-exchange");
    Field(GTK_GRID(grid), "Exchange / class", ui->optionResults, 3, "ibkr-option-results");
    Field(GTK_GRID(grid), "Expiry", ui->optionExpiry, 4, "ibkr-option-expiry");
    Field(GTK_GRID(grid), "Strike", ui->optionStrike, 5, "ibkr-option-strike");
    Field(GTK_GRID(grid), "Right", ui->optionRight, 6, "ibkr-option-right");
    Field(GTK_GRID(grid), "Option currency", ui->optionCurrency, 7, "ibkr-option-currency");
    GtkWidget *read = gtk_button_new_with_label("Request option chains"),
              *abandon = gtk_button_new_with_label("Abandon local lookup");
    GtkWidget *resolve = gtk_button_new_with_label("Resolve selected option"),
              *apply = gtk_button_new_with_label("Use resolved option ID");
    gtk_widget_set_name(read, "ibkr-option-read");
    gtk_widget_set_name(abandon, "ibkr-option-abandon");
    gtk_widget_set_name(resolve, "ibkr-option-resolve");
    gtk_widget_set_name(apply, "ibkr-option-apply");
    ui->optionOutput =
        Text("No option definitions. Enter a resolved underlying contract and request its chains.");
    gtk_widget_set_name(ui->optionOutput, "ibkr-option-output");
    gtk_label_set_selectable(GTK_LABEL(ui->optionOutput), TRUE);
    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_append(GTK_BOX(box), read);
    gtk_box_append(GTK_BOX(box), abandon);
    gtk_box_append(GTK_BOX(box), resolve);
    gtk_box_append(GTK_BOX(box), apply);
    gtk_box_append(GTK_BOX(box), ui->optionOutput);
    DiscoveryExportControls(window, ui, box, 1U);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(read, "clicked", G_CALLBACK(OptionChainReadClicked), window, 0);
    g_signal_connect_object(abandon, "clicked", G_CALLBACK(OptionChainAbandonClicked), window, 0);
    g_signal_connect_object(resolve, "clicked", G_CALLBACK(OptionResolveClicked), window, 0);
    g_signal_connect_object(apply, "clicked", G_CALLBACK(OptionApplyClicked), window, 0);
    g_signal_connect_object(ui->optionResults, "notify::selected", G_CALLBACK(OptionChainChanged), window, 0);
}
