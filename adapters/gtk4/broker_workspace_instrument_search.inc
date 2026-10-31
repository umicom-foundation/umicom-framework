/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_instrument_search.inc
 *
 * PURPOSE:
 *   Find broker instruments and transfer explicitly selected identities into market-data controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static void WorkspaceInstrumentFind(GtkButton *button, gpointer data)
{
    (void)button; UmiBrokerWorkspaceGtk *o = data;
    if (o->closed || o->refreshing || !WorkspaceDetailsReady(o)) return;
    UmiIbkrInstrumentQuery q = {0};
    guint type = gtk_drop_down_get_selected(GTK_DROP_DOWN(o->instrumentType));
    const char *types[] = {"STK","FUT","BOND"};
    if (type >= G_N_ELEMENTS(types)) return;
    strcpy(q.securityType,types[type]);
    g_strlcpy(q.symbol,gtk_editable_get_text(GTK_EDITABLE(o->instrumentSymbol)),sizeof q.symbol);
    g_strlcpy(q.exchange,gtk_editable_get_text(GTK_EDITABLE(o->instrumentRoute)),sizeof q.exchange);
    g_strlcpy(q.currency,gtk_editable_get_text(GTK_EDITABLE(o->instrumentCurrency)),sizeof q.currency);
    g_strlcpy(q.expiry,gtk_editable_get_text(GTK_EDITABLE(o->instrumentExpiry)),sizeof q.expiry);
    ++o->references; o->detailsBusy = TRUE;
    UmiStatus status = UmiIbkrGtkSearchInstrument(o->connection,&q);
    if (!o->closed) {
        o->detailsBusy = FALSE; WorkspaceDetailsRefresh(o);
        if (!o->closed) LabelText(o->detailsNotice,status == UMI_STATUS_OK ?
            "Searching. Wait for completion, then choose the exact instrument. Narrow futures by expiry if needed." :
            umi_status_text(status));
    }
    Release(o);
}
static void WorkspaceInstrumentUse(GtkButton *button, gpointer data)
{
    (void)button; UmiBrokerWorkspaceGtk *o = data;
    if (o->closed || o->refreshing || !WorkspaceDetailsReady(o) || !o->hasDetails) return;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(o->instrumentRows));
    if (selected >= o->details.count) return;
    uint32_t request = o->details.requestId, id = o->details.items[selected].contractId;
    ++o->references; o->detailsBusy = TRUE;
    UmiIbkrContractDetailsSnapshot *copy = g_new0(UmiIbkrContractDetailsSnapshot,1);
    UmiStatus status = UmiIbkrGtkCopyContractDetails(o->connection,60000U,copy);
    UmiIbkrQuoteSnapshot quote = {0};
    if (status == UMI_STATUS_OK && (copy->requestId != request || !copy->complete ||
        copy->failed || copy->stale || selected >= copy->count || copy->items[selected].contractId != id))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK &&
        UmiIbkrGtkCopyQuote(o->connection,15000U,&quote) == UMI_STATUS_OK && quote.subscribed)
        status = UMI_STATUS_BUSY;
    if (!o->closed && status == UMI_STATUS_OK) {
        char text[24]; (void)g_snprintf(text,sizeof text,"%u",(unsigned)id);
        const char *route = copy->requestedContract.exchange;
        gtk_editable_set_text(GTK_EDITABLE(o->quoteContract),text);
        if (!o->closed) gtk_editable_set_text(GTK_EDITABLE(o->quoteExchange),route);
        if (!o->closed) gtk_editable_set_text(GTK_EDITABLE(o->detailsContract),text);
        if (!o->closed) gtk_editable_set_text(GTK_EDITABLE(o->detailsExchange),route);
        if (!o->closed) gtk_editable_set_text(GTK_EDITABLE(o->watchLabel),copy->items[selected].localSymbol[0] ?
            copy->items[selected].localSymbol : copy->items[selected].symbol);
        if (!o->closed) (void)UmiBrokerWorkspaceGtkSelectPage(o,UMI_BROKER_PAGE_MARKET);
    }
    if (!o->closed) {
        o->detailsBusy = FALSE;
        LabelText(o->detailsNotice,status == UMI_STATUS_OK ?
            "Selected. Start quotes, add to Watchlist, or read the exact description before reviewing an order." :
            "Selection unavailable. Wait for complete fresh results and stop the existing quote subscription.");
    }
    g_free(copy); Release(o);
}
static void WorkspaceInstrumentResults(UmiBrokerWorkspaceGtk *o)
{
    GtkStringList *rows = gtk_string_list_new(NULL);
    for (size_t i = 0U; i < o->details.count && i < UMI_IBKR_CONTRACT_DETAILS_LIMIT; ++i) {
        const UmiIbkrContractDescription *r = &o->details.items[i];
        char *text = g_strdup_printf("%u | %s | %s | %s | %s | %s",
            (unsigned)r->contractId,r->symbol,r->securityType,r->expiry,r->currency,r->exchange);
        gtk_string_list_append(rows,text); g_free(text);
    }
    gtk_drop_down_set_model(GTK_DROP_DOWN(o->instrumentRows),G_LIST_MODEL(rows));
    g_object_unref(rows);
}
static GtkWidget *WorkspaceInstrumentSearch(UmiBrokerWorkspaceGtk *o)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL,6);
    GtkWidget *grid = gtk_grid_new(); gtk_grid_set_column_spacing(GTK_GRID(grid),6);
    o->instrumentSymbol = gtk_entry_new(); o->instrumentRoute = gtk_entry_new();
    o->instrumentCurrency = gtk_entry_new(); o->instrumentExpiry = gtk_entry_new();
    const char *types[] = {"Shares","Futures","Bonds",NULL};
    o->instrumentType = gtk_drop_down_new_from_strings(types);
    GtkWidget *fields[] = {o->instrumentSymbol,o->instrumentRoute,o->instrumentCurrency,o->instrumentExpiry};
    const char *titles[] = {"Symbol / bond identifier","Route","Currency","Futures expiry (YYYYMM or YYYYMMDD)"};
    const char *names[] = {"symbol","route","currency","expiry"};
    const int limits[] = {95,63,15,8};
    for (size_t i = 0U; i < 4U; ++i) {
        gtk_grid_attach(GTK_GRID(grid),Cell(titles[i],TRUE),0,(int)i,1,1);
        gtk_grid_attach(GTK_GRID(grid),fields[i],1,(int)i,1,1);
        gtk_entry_set_max_length(GTK_ENTRY(fields[i]),limits[i]);
        gtk_widget_set_hexpand(fields[i],TRUE);
        char *name = g_strconcat("broker-instrument-",names[i],NULL);
        gtk_widget_set_name(fields[i],name); g_free(name);
    }
    gtk_editable_set_text(GTK_EDITABLE(o->instrumentRoute),"SMART");
    gtk_editable_set_text(GTK_EDITABLE(o->instrumentCurrency),"USD");
    o->instrumentFind = gtk_button_new_with_label("Find instruments");
    o->instrumentUse = gtk_button_new_with_label("Use selected instrument");
    o->instrumentRows = gtk_drop_down_new(NULL,NULL);
    gtk_widget_set_name(o->instrumentType,"broker-instrument-type");
    gtk_widget_set_name(o->instrumentFind,"broker-instrument-find");
    gtk_widget_set_name(o->instrumentUse,"broker-instrument-use");
    gtk_widget_set_name(o->instrumentRows,"broker-instrument-results");
    gtk_box_append(GTK_BOX(box),o->instrumentType); gtk_box_append(GTK_BOX(box),grid);
    gtk_box_append(GTK_BOX(box),o->instrumentFind); gtk_box_append(GTK_BOX(box),o->instrumentRows);
    gtk_box_append(GTK_BOX(box),o->instrumentUse);
    g_signal_connect(o->instrumentFind,"clicked",G_CALLBACK(WorkspaceInstrumentFind),o);
    g_signal_connect(o->instrumentUse,"clicked",G_CALLBACK(WorkspaceInstrumentUse),o);
    return box;
}
