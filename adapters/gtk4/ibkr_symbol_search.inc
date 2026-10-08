/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_symbol_search.inc
 * PURPOSE: Discover broker contract identities without guessing a trading route.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* A result is a discovery candidate, not an instruction to trade. Keep the
 * selected listing exchange visible but do not copy it into the routing field:
 * a primary listing and an order destination have different meanings. */
static gchar *SymbolSearchText(const UmiIbkrSymbolSearchSnapshot *copy)
{
    GString *text = g_string_new(copy->message);
    g_string_append_printf(text, "\nSearch: %s; request %u; %zu candidates. %s%s\n", copy->pattern,
                           (unsigned)copy->requestId, copy->count,
                           copy->complete ? "Complete. " : "Incomplete. ", copy->stale ? "STALE" : "");
    for (size_t i = 0U; i < copy->count; ++i)
    {
        const UmiIbkrSymbolCandidate *row = &copy->rows[i];
        g_string_append_printf(text, "%u | %s | %s | listing %s | %s\n  %s\n", (unsigned)row->contractId,
                               row->symbol, row->securityType, row->primaryExchange, row->currency,
                               row->description);
        if (row->issuerId[0])
            g_string_append_printf(text, "  Issuer: %s\n", row->issuerId);
        if (row->derivativeTypeCount != 0U)
        {
            g_string_append(text, "  Derivative types:");
            for (size_t j = 0U; j < row->derivativeTypeCount; ++j)
                g_string_append_printf(text, " %s", row->derivativeTypes[j]);
            g_string_append_c(text, '\n');
        }
    }
    g_string_append(text,
                    "Review contract details and routing before subscribing. "
                    "Search results are a bounded discovery response, not a full instrument catalogue.");
    return g_string_free(text, FALSE);
}
static void SymbolSearchPublish(ConnectionUi *ui, uint64_t now)
{
    UmiIbkrSymbolSearchSnapshot *copy = g_try_malloc(sizeof *copy);
    UmiStatus status = copy == NULL
                           ? UMI_STATUS_OUT_OF_MEMORY
                           : UmiIbkrSymbolSearchCopy(ui->connection, ui->symbolSearchRequest, now, copy);
    if (status != UMI_STATUS_OK)
    {
        g_free(copy);
        gtk_label_set_text(GTK_LABEL(ui->symbolSearchOutput),
                           "No instrument search. Connect and enter a symbol or name.");
        return;
    }
    gchar *text = SymbolSearchText(copy);
    if (copy->complete && ui->symbolSearchDisplayed != copy->requestId)
    {
        GtkStringList *items = gtk_string_list_new(NULL);
        for (size_t i = 0U; i < copy->count; ++i)
        {
            const UmiIbkrSymbolCandidate *row = &copy->rows[i];
            gchar *label = g_strdup_printf("%s | %s | %s | %s | %u", row->symbol, row->securityType,
                                           row->primaryExchange, row->currency, (unsigned)row->contractId);
            gtk_string_list_append(items, label);
            g_free(label);
        }
        /* Commit the request identity before GTK can notify an observer.
         * The common entry guard prevents a notification from applying a row
         * while its model is being replaced. */
        ui->symbolSearchDisplayed = copy->requestId;
        gtk_drop_down_set_model(GTK_DROP_DOWN(ui->symbolSearchResults), G_LIST_MODEL(items));
        g_object_unref(items);
    }
    g_free(copy);
    if (!ui->closed && strcmp(gtk_label_get_text(GTK_LABEL(ui->symbolSearchOutput)), text) != 0)
        gtk_label_set_text(GTK_LABEL(ui->symbolSearchOutput), text);
    g_free(text);
}
static void SymbolSearchPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy || ui->symbolSearchRequest == 0U)
        return;
    ui = FillEnter(ui->window);
    if (ui == NULL)
        return;
    SymbolSearchPublish(ui, now);
    FillLeave(ui);
}
static void SymbolSearchReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    const char *pattern = gtk_editable_get_text(GTK_EDITABLE(ui->symbolSearchPattern));
    UmiStatus status = UmiIbkrSymbolSearchRequest(ui->connection, pattern, now, &ui->symbolSearchRequest);
    if (status == UMI_STATUS_OK)
    {
        ui->symbolSearchDisplayed = 0U;
        gtk_drop_down_set_model(GTK_DROP_DOWN(ui->symbolSearchResults), NULL);
        if (!ui->closed)
            SymbolSearchPublish(ui, now);
    }
    else
        gtk_label_set_text(
            GTK_LABEL(ui->symbolSearchOutput),
            status == UMI_STATUS_BUSY
                ? "A search is pending or requests are too close together. Wait before searching again."
                : "Search was not started. Connect and enter a nonempty symbol or name.");
    FillLeave(ui);
}
static void SymbolSearchAbandonClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    (void)UmiIbkrSymbolSearchAbandon(ui->connection, ui->symbolSearchRequest);
    SymbolSearchPublish(ui, UmiIbkrMonotonicMilliseconds());
    FillLeave(ui);
}
static void SymbolSearchApplyClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    UmiIbkrSymbolSearchSnapshot *copy = g_try_malloc(sizeof *copy);
    UmiStatus status = copy == NULL ? UMI_STATUS_OUT_OF_MEMORY
                                    : UmiIbkrSymbolSearchCopy(ui->connection, ui->symbolSearchRequest,
                                                              UmiIbkrMonotonicMilliseconds(), copy);
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->symbolSearchResults));
    bool valid = status == UMI_STATUS_OK && copy->complete && !copy->stale &&
                 copy->requestId == ui->symbolSearchDisplayed && (size_t)index < copy->count &&
                 !ui->quoteSubscribed;
    char identity[32] = {0};
    if (valid)
        g_snprintf(identity, sizeof identity, "%u", (unsigned)copy->rows[index].contractId);
    g_free(copy);
    if (valid)
    {
        FillWatchRetire(ui);
        gtk_editable_set_text(GTK_EDITABLE(ui->quoteContract), identity);
        if (!ui->closed)
            gtk_label_set_text(GTK_LABEL(ui->symbolSearchOutput),
                               "Contract ID copied. Review the exchange, subscribe to its quote, then "
                               "inspect the returned contract details.");
    }
    else
        gtk_label_set_text(GTK_LABEL(ui->symbolSearchOutput),
                           "Choose a completed current result and stop any active quote stream before using "
                           "its contract ID.");
    FillLeave(ui);
}
static void SymbolSearchControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Find an instrument");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    ui->symbolSearchPattern = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(ui->symbolSearchPattern), 127);
    ui->symbolSearchRead = gtk_button_new_with_label("Search symbols and names");
    ui->symbolSearchAbandon = gtk_button_new_with_label("Abandon local search");
    const char *empty[] = {NULL};
    ui->symbolSearchResults = gtk_drop_down_new_from_strings(empty);
    ui->symbolSearchApply = gtk_button_new_with_label("Use selected contract ID");
    ui->symbolSearchOutput = Text("Search by symbol or company name after connecting.");
    gtk_label_set_selectable(GTK_LABEL(ui->symbolSearchOutput), TRUE);
    GtkWidget *controls[] = {ui->symbolSearchPattern, ui->symbolSearchRead,  ui->symbolSearchAbandon,
                             ui->symbolSearchResults, ui->symbolSearchApply, ui->symbolSearchOutput};
    const char *names[] = {"ibkr-symbol-pattern", "ibkr-symbol-search", "ibkr-symbol-abandon",
                           "ibkr-symbol-results", "ibkr-symbol-apply",  "ibkr-symbol-output"};
    gtk_box_append(GTK_BOX(box), Text("Symbol or company name"));
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i)
    {
        gtk_widget_set_name(controls[i], names[i]);
        gtk_box_append(GTK_BOX(box), controls[i]);
    }
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->symbolSearchRead, "clicked", G_CALLBACK(SymbolSearchReadClicked), window, 0);
    g_signal_connect_object(ui->symbolSearchAbandon, "clicked", G_CALLBACK(SymbolSearchAbandonClicked),
                            window, 0);
    g_signal_connect_object(ui->symbolSearchApply, "clicked", G_CALLBACK(SymbolSearchApplyClicked), window,
                            0);
}
