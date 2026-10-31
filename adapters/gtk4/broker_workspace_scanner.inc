/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_scanner.inc
 *
 * PURPOSE:
 *   Present ranked scanner contracts without confusing refreshed rows with the selected instrument.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* A scanner refresh replaces a whole ranked set. Selection is explicitly
 * cleared on a new generation so row movement cannot redirect user intent. */
static void WorkspaceScannerRender(UmiBrokerWorkspaceGtk *owner)
{
    gboolean ready = owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->scannerBusy;
    gboolean occupied = owner->hasScanner && owner->scanner.needsCancel;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->scannerResults));
    gboolean usable = ready && owner->hasScanner && !owner->scanner.stale && !owner->scanner.failed &&
        owner->scanner.active && selected < owner->scanner.count && selected < UMI_IBKR_SCANNER_ROW_LIMIT &&
        !owner->quoteBusy && !(owner->hasQuote && owner->quote.subscribed);
    GtkWidget *controls[] = {owner->scannerStart, owner->scannerStop, owner->scannerApply,
        owner->scannerInstrument, owner->scannerLocation, owner->scannerCode, owner->scannerRows};
    gboolean enabled[] = {ready && !occupied, ready && occupied, usable,
        !occupied && !owner->scannerBusy, !occupied && !owner->scannerBusy,
        !occupied && !owner->scannerBusy, !occupied && !owner->scannerBusy};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(controls[i], enabled[i]);
    }
    if (owner->closed) return;
    char *status = owner->hasScanner ? g_strdup_printf(
        "%s • %s • %s • %zu results • refresh %" PRIu64 " • %s%s\n%s",
        owner->scanner.query.instrument, owner->scanner.query.locationCode, owner->scanner.query.scanCode,
        owner->scanner.count, owner->scanner.generation,
        owner->scanner.failed ? "Failed" : owner->scanner.active ? "Subscribed" : "Stopped",
        owner->scanner.stale ? " • STALE / waiting" : "", owner->scanner.message) :
        g_strdup("Choose an instrument family, location and scan code, then Start scan.");
    LabelText(owner->scannerStatus, status); g_free(status);
}
static void WorkspaceScannerRefresh(UmiBrokerWorkspaceGtk *owner)
{
    UmiIbkrScannerSnapshot capture;
    UmiIbkrScannerRow *rows = g_new0(UmiIbkrScannerRow, UMI_IBKR_SCANNER_ROW_LIMIT);
    UmiStatus status = owner->connection == NULL ? UMI_STATUS_CANCELLED :
        UmiIbkrGtkCopyScanner(owner->connection, 60000U, &capture);
    gboolean replaceModel = FALSE;
    if (status == UMI_STATUS_OK) {
        if (capture.count > UMI_IBKR_SCANNER_ROW_LIMIT) status = UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t i = 0U; status == UMI_STATUS_OK && i < capture.count; ++i)
            status = UmiIbkrGtkCopyScannerRow(owner->connection, capture.requestId, capture.generation, i, &rows[i]);
        if (status == UMI_STATUS_OK) {
            replaceModel = !owner->hasScanner || owner->scanner.requestId != capture.requestId ||
                owner->scanner.generation != capture.generation || owner->scanner.count != capture.count;
            owner->scanner = capture; owner->hasScanner = TRUE;
            memcpy(owner->scannerItems, rows, sizeof(owner->scannerItems));
        } else owner->scanner.stale = true;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        replaceModel = owner->hasScanner;
        memset(&owner->scanner, 0, sizeof(owner->scanner));
        memset(owner->scannerItems, 0, sizeof(owner->scannerItems));
        owner->hasScanner = FALSE; owner->scanner.stale = true;
    } else owner->scanner.stale = true;
    g_free(rows);
    if (replaceModel) {
        GtkStringList *items = gtk_string_list_new(NULL);
        GtkWidget *table = gtk_grid_new();
        const char *headings[] = {"Rank", "Instrument", "Type", "Currency", "Listing", "Contract ID", "Distance"};
        for (int column = 0; column < 7; ++column)
            gtk_grid_attach(GTK_GRID(table), Cell(headings[column], TRUE), column, 0, 1, 1);
        for (size_t i = 0U; i < owner->scanner.count && i < UMI_IBKR_SCANNER_ROW_LIMIT; ++i) {
            const UmiIbkrScannerRow *row = &owner->scannerItems[i];
            char rank[24], id[24];
            (void)g_snprintf(rank, sizeof(rank), "%" PRIu64, (uint64_t)row->rank + 1U);
            (void)g_snprintf(id, sizeof(id), "%" PRIu32, row->contractId);
            const char *cells[] = {rank, row->localSymbol[0] ? row->localSymbol : row->symbol,
                row->securityType, row->currency, row->exchange, id, row->distance};
            for (int column = 0; column < 7; ++column)
                gtk_grid_attach(GTK_GRID(table), Cell(cells[column], FALSE), column, (int)i + 1, 1, 1);
            char *raw = g_strdup_printf("%s • %s • %s • %s • ID %s", rank, cells[1],
                row->securityType, row->currency, id);
            char *label = g_utf8_make_valid(raw, -1);
            gtk_string_list_append(items, label); g_free(label); g_free(raw);
        }
        if (owner->scanner.count == 0U)
            gtk_grid_attach(GTK_GRID(table), Cell(owner->scanner.generation != 0U ?
                "Complete refresh contains no matching contracts." : "Waiting for a complete scanner refresh.",
                FALSE), 0, 1, 7, 1);
        owner->scannerRendering = TRUE;
        gtk_drop_down_set_model(GTK_DROP_DOWN(owner->scannerResults), G_LIST_MODEL(items));
        g_object_unref(items);
        if (!owner->closed)
            gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->scannerResults), GTK_INVALID_LIST_POSITION);
        /* Hold the replacement table even if a notification destroys the owner
         * before the scroll container takes ownership of the new widget. */
        g_object_ref_sink(table);
        if (!owner->closed) gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(owner->scannerTable), table);
        g_object_unref(table);
        owner->scannerRendering = FALSE;
    }
    if (!owner->closed) WorkspaceScannerRender(owner);
}
static void WorkspaceScannerSelected(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->scannerRendering) return;
    ++owner->references;
    WorkspaceScannerRender(owner);
    Release(owner);
}
static UmiStatus WorkspaceScannerQuery(UmiBrokerWorkspaceGtk *owner, UmiIbkrScannerQuery *out)
{
    UmiIbkrScannerQuery query = {0};
    const char *instrument = gtk_editable_get_text(GTK_EDITABLE(owner->scannerInstrument));
    const char *location = gtk_editable_get_text(GTK_EDITABLE(owner->scannerLocation));
    const char *code = gtk_editable_get_text(GTK_EDITABLE(owner->scannerCode));
    if (strlen(instrument) >= sizeof(query.instrument) || strlen(location) >= sizeof(query.locationCode) ||
        strlen(code) >= sizeof(query.scanCode)) return UMI_STATUS_INVALID_ARGUMENT;
    strcpy(query.instrument, instrument); strcpy(query.locationCode, location); strcpy(query.scanCode, code);
    query.numberOfRows = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(owner->scannerRows));
    UmiStatus status = UmiIbkrScannerQueryValidate(&query);
    if (status == UMI_STATUS_OK) *out = query;
    return status;
}
static void WorkspaceScannerStart(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->scannerBusy) return;
    UmiIbkrScannerQuery query;
    UmiStatus status = WorkspaceScannerQuery(owner, &query);
    ++owner->references; owner->scannerBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale) {
        if (status == UMI_STATUS_OK)
            status = owner->hasScanner && owner->scanner.needsCancel ? UMI_STATUS_ALREADY_EXISTS :
                UmiIbkrGtkStartScanner(owner->connection, &query);
        if (!owner->closed) WorkspaceScannerRefresh(owner);
        if (!owner->closed) {
            char *notice = status == UMI_STATUS_OK ?
                g_strdup("Scan requested. Results identify contracts; they do not contain prices.") :
                g_strdup_printf("Scan not started: %s. Stop an existing scan, check its codes and allow a second between requests.",
                    umi_status_text(status));
            LabelText(owner->scannerNotice, notice); g_free(notice);
        }
    }
    owner->scannerBusy = FALSE;
    if (!owner->closed) WorkspaceScannerRender(owner);
    Release(owner);
}
static void WorkspaceScannerStop(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->scannerBusy ||
        !owner->hasScanner || !owner->scanner.needsCancel) return;
    uint32_t expected = owner->scanner.requestId;
    ++owner->references; owner->scannerBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    if (!owner->closed && owner->connection != NULL) {
        UmiStatus status = !owner->hasScanner || owner->scanner.requestId != expected ? UMI_STATUS_NOT_FOUND :
            UmiIbkrGtkCancelScanner(owner->connection, expected);
        if (!owner->closed) WorkspaceScannerRefresh(owner);
        if (!owner->closed) LabelText(owner->scannerNotice, status == UMI_STATUS_OK ?
            "Scan stopped. Retained results are stale." : "Scan was not stopped. Review the current request and connection.");
    }
    owner->scannerBusy = FALSE;
    if (!owner->closed) WorkspaceScannerRender(owner);
    Release(owner);
}
static void WorkspaceScannerApply(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->scannerBusy || owner->scannerRendering ||
        owner->quoteBusy || !owner->hasScanner) return;
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->scannerResults));
    if (index >= owner->scanner.count || index >= UMI_IBKR_SCANNER_ROW_LIMIT) return;
    uint32_t request = owner->scanner.requestId, identity = owner->scannerItems[index].contractId;
    uint64_t generation = owner->scanner.generation;
    ++owner->references; owner->scannerBusy = TRUE;
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    gboolean valid = !owner->closed && owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && owner->hasScanner && owner->scanner.active && !owner->scanner.stale &&
        !owner->scanner.failed && owner->scanner.requestId == request && owner->scanner.generation == generation &&
        index < owner->scanner.count && identity != 0U && owner->scannerItems[index].contractId == identity &&
        !(owner->hasQuote && owner->quote.subscribed);
    if (valid) {
        owner->quoteBusy = TRUE;
        char id[24]; (void)g_snprintf(id, sizeof(id), "%" PRIu32, identity);
        gtk_editable_set_text(GTK_EDITABLE(owner->quoteContract), id);
        /* A listing exchange is not an execution or quote route. Require the
         * user to review the route instead of reusing the previous instrument. */
        if (!owner->closed) gtk_editable_set_text(GTK_EDITABLE(owner->quoteExchange), "");
        owner->quoteBusy = FALSE;
    }
    owner->scannerBusy = FALSE;
    if (!owner->closed) WorkspaceScannerRender(owner);
    if (!owner->closed) LabelText(owner->scannerNotice, valid ?
        "Contract ID selected. Open Market data, enter its exchange and choose a data mode before subscribing." :
        "Result changed or is unavailable. Review a fresh scan and stop any existing quote stream.");
    Release(owner);
}
static GtkWidget *WorkspaceScannerPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *fields = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(fields), 6);
    gtk_grid_set_column_spacing(GTK_GRID(fields), 12);
    owner->scannerInstrument = gtk_entry_new();
    owner->scannerLocation = gtk_entry_new();
    owner->scannerCode = gtk_entry_new();
    owner->scannerRows = gtk_spin_button_new_with_range(1, UMI_IBKR_SCANNER_ROW_LIMIT, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(owner->scannerRows), 25);
    gtk_editable_set_text(GTK_EDITABLE(owner->scannerInstrument), "STK");
    gtk_editable_set_text(GTK_EDITABLE(owner->scannerLocation), "STK.US.MAJOR");
    gtk_editable_set_text(GTK_EDITABLE(owner->scannerCode), "TOP_PERC_GAIN");
    GtkWidget *inputs[] = {owner->scannerInstrument, owner->scannerLocation, owner->scannerCode, owner->scannerRows};
    const char *labels[] = {"Instrument family", "Location code", "Scan code", "Maximum results"};
    for (size_t i = 0U; i < G_N_ELEMENTS(inputs); ++i) {
        gtk_grid_attach(GTK_GRID(fields), Cell(labels[i], TRUE), 0, (int)i, 1, 1);
        gtk_widget_set_hexpand(inputs[i], TRUE);
        gtk_grid_attach(GTK_GRID(fields), inputs[i], 1, (int)i, 1, 1);
    }
    gtk_box_append(GTK_BOX(page), fields);
    owner->scannerStart = gtk_button_new_with_label("Start scan");
    owner->scannerStop = gtk_button_new_with_label("Stop scan");
    owner->scannerApply = gtk_button_new_with_label("Use selected contract ID");
    GtkWidget *actions = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(actions), GTK_SELECTION_NONE);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(actions), 3);
    gtk_flow_box_insert(GTK_FLOW_BOX(actions), owner->scannerStart, -1);
    gtk_flow_box_insert(GTK_FLOW_BOX(actions), owner->scannerStop, -1);
    gtk_flow_box_insert(GTK_FLOW_BOX(actions), owner->scannerApply, -1);
    gtk_box_append(GTK_BOX(page), actions);
    owner->scannerStatus = Cell("", FALSE);
    owner->scannerNotice = Cell("", FALSE);
    gtk_label_set_wrap(GTK_LABEL(owner->scannerStatus), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->scannerNotice), TRUE);
    owner->scannerResults = gtk_drop_down_new(NULL, NULL);
    owner->scannerTable = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(owner->scannerTable, TRUE);
    GtkWidget *widgets[] = {owner->scannerInstrument, owner->scannerLocation, owner->scannerCode, owner->scannerRows,
        owner->scannerStart, owner->scannerStop, owner->scannerApply, owner->scannerResults, owner->scannerTable,
        owner->scannerStatus, owner->scannerNotice};
    const char *names[] = {"instrument", "location", "code", "rows", "start", "stop", "apply", "results",
        "table", "status", "notice"};
    for (size_t i = 0U; i < G_N_ELEMENTS(widgets); ++i) {
        char *name = g_strdup_printf("broker-workspace-scanner-%s", names[i]);
        gtk_widget_set_name(widgets[i], name); g_free(name);
    }
    gtk_box_append(GTK_BOX(page), owner->scannerStatus);
    gtk_box_append(GTK_BOX(page), owner->scannerNotice);
    gtk_box_append(GTK_BOX(page), owner->scannerResults);
    gtk_box_append(GTK_BOX(page), owner->scannerTable);
    GtkWidget *hint = Cell("Broker-supported codes vary. Connection and account tools provides the scanner catalogue and advanced filters.", FALSE);
    gtk_label_set_wrap(GTK_LABEL(hint), TRUE); gtk_box_append(GTK_BOX(page), hint);
    g_signal_connect(owner->scannerStart, "clicked", G_CALLBACK(WorkspaceScannerStart), owner);
    g_signal_connect(owner->scannerStop, "clicked", G_CALLBACK(WorkspaceScannerStop), owner);
    g_signal_connect(owner->scannerApply, "clicked", G_CALLBACK(WorkspaceScannerApply), owner);
    g_signal_connect(owner->scannerResults, "notify::selected", G_CALLBACK(WorkspaceScannerSelected), owner);
    return page;
}
