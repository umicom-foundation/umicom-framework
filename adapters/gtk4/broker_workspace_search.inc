/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_search.inc
 *
 * PURPOSE:
 *   Find and review instrument candidates through the shared broker session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Search results belong to the existing broker session. The workspace owns
 * only a display copy; applying a candidate prepares fields, never traffic. */
static gboolean WorkspaceSearchReady(const UmiBrokerWorkspaceGtk *owner)
{
    return owner->connection != NULL && owner->snapshot.state == UMI_IBKR_READY &&
        !owner->snapshot.stale && !owner->searchBusy;
}
static void WorkspaceSearchRender(UmiBrokerWorkspaceGtk *owner, gboolean replaceValues)
{
    gboolean ready = WorkspaceSearchReady(owner);
    gboolean pending = owner->hasSearch && !owner->search.complete &&
        !owner->search.failed && !owner->search.stale;
    gboolean usable = ready && owner->hasSearch && owner->search.complete &&
        !owner->search.failed && !owner->search.stale && !owner->quoteBusy &&
        !(owner->hasQuote && owner->quote.subscribed);
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->searchResults));
    GtkWidget *controls[] = {owner->searchStart, owner->searchAbandon, owner->searchApply, owner->searchPattern};
    gboolean enabled[] = {ready && !pending, ready && pending,
        usable && selected < owner->search.count && selected < UMI_IBKR_SYMBOL_LIMIT,
        !pending && !owner->searchBusy};
    for (size_t i = 0U; i < G_N_ELEMENTS(controls); ++i) {
        if (owner->closed) return;
        gtk_widget_set_sensitive(controls[i], enabled[i]);
    }
    if (owner->closed || !replaceValues) return;
    char *text = owner->hasSearch ? g_strdup_printf(
        "Search: %s • Request %" PRIu32 " • %zu candidates • %s%s\n%s",
        owner->search.pattern, owner->search.requestId, owner->search.count,
        owner->search.complete ? "Complete" : "Incomplete",
        owner->search.stale ? " • STALE" : "", owner->search.message) :
        g_strdup("Enter a symbol or company name, then choose Search.");
    LabelText(owner->searchStatus, text);
    g_free(text);
}
static void WorkspaceSearchRefresh(UmiBrokerWorkspaceGtk *owner)
{
    if (owner->closed || owner->connection == NULL) return;
    UmiIbkrSymbolSearchSnapshot *copy = g_new0(UmiIbkrSymbolSearchSnapshot, 1);
    UmiStatus status = UmiIbkrGtkCopyInstrumentSearch(owner->connection, copy);
    gboolean changed = FALSE, modelChanged = FALSE;
    if (status == UMI_STATUS_OK) {
        changed = !owner->hasSearch || memcmp(copy, &owner->search, sizeof(*copy)) != 0;
        modelChanged = !owner->hasSearch || copy->requestId != owner->search.requestId ||
            copy->count != owner->search.count ||
            memcmp(copy->rows, owner->search.rows, sizeof(copy->rows)) != 0;
        owner->search = *copy;
        owner->hasSearch = TRUE;
    } else if (status == UMI_STATUS_NOT_FOUND) {
        changed = modelChanged = owner->hasSearch;
        memset(&owner->search, 0, sizeof(owner->search));
        owner->hasSearch = FALSE;
    } else if (owner->hasSearch) {
        changed = !owner->search.stale;
        owner->search.stale = true;
    }
    g_free(copy);
    if (modelChanged) {
        GtkStringList *items = gtk_string_list_new(NULL);
        for (size_t i = 0U; i < owner->search.count && i < UMI_IBKR_SYMBOL_LIMIT; ++i) {
            const UmiIbkrSymbolCandidate *row = &owner->search.rows[i];
            char *raw = g_strdup_printf("%s • %s • %s • Listing: %s • ID %" PRIu32 " • %s",
                row->symbol, row->securityType, row->currency, row->primaryExchange,
                row->contractId, row->description);
            char *display = g_utf8_make_valid(raw, -1);
            gtk_string_list_append(items, display);
            g_free(display); g_free(raw);
        }
        owner->searchRendering = TRUE;
        gtk_drop_down_set_model(GTK_DROP_DOWN(owner->searchResults), G_LIST_MODEL(items));
        g_object_unref(items);
        /* The user must choose a candidate. Never silently use the first match. */
        if (!owner->closed)
            gtk_drop_down_set_selected(GTK_DROP_DOWN(owner->searchResults), GTK_INVALID_LIST_POSITION);
        owner->searchRendering = FALSE;
    }
    if (!owner->closed) WorkspaceSearchRender(owner, changed);
}
static void WorkspaceSearchSelected(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->searchRendering || owner->refreshing) return;
    ++owner->references;
    WorkspaceSearchRender(owner, FALSE);
    Release(owner);
}
static void WorkspaceSearchStart(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || !WorkspaceSearchReady(owner)) return;
    ++owner->references;
    /* Copy entry text before publication; sensitivity notifications can edit it. */
    char *pattern = g_strdup(gtk_editable_get_text(GTK_EDITABLE(owner->searchPattern)));
    owner->searchBusy = TRUE;
    WorkspaceSearchRender(owner, FALSE);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!owner->closed && owner->connection != NULL)
        status = UmiIbkrGtkSearchInstruments(owner->connection, pattern);
    g_free(pattern);
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
    owner->searchBusy = FALSE;
    if (!owner->closed) WorkspaceSearchRender(owner, TRUE);
    if (!owner->closed) LabelText(owner->searchNotice, status == UMI_STATUS_OK
        ? "Search requested. Choose a returned candidate after completion."
        : status == UMI_STATUS_BUSY ? "A search is pending or requests are too close together. Wait before retrying."
        : status == UMI_STATUS_INVALID_ARGUMENT ? "Enter a nonempty symbol or company name."
        : umi_status_text(status));
    Release(owner);
}
static void WorkspaceSearchAbandon(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || !WorkspaceSearchReady(owner) || !owner->hasSearch) return;
    ++owner->references;
    owner->searchBusy = TRUE;
    WorkspaceSearchRender(owner, FALSE);
    UmiStatus status = UMI_STATUS_CANCELLED;
    if (!owner->closed && owner->connection != NULL)
        status = UmiIbkrGtkAbandonInstrumentSearch(owner->connection);
    if (!owner->closed) (void)UmiBrokerWorkspaceGtkRefresh(owner);
    owner->searchBusy = FALSE;
    if (!owner->closed) WorkspaceSearchRender(owner, TRUE);
    if (!owner->closed) LabelText(owner->searchNotice, status == UMI_STATUS_OK
        ? "Local search abandoned. Quotes and orders are unchanged." : umi_status_text(status));
    Release(owner);
}
static void WorkspaceSearchApply(GtkButton *button, gpointer data)
{
    (void)button;
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->refreshing || owner->searchRendering || !WorkspaceSearchReady(owner) ||
        owner->quoteBusy || !owner->hasSearch || (owner->hasQuote && owner->quote.subscribed)) return;
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->searchResults));
    if (index >= owner->search.count || index >= UMI_IBKR_SYMBOL_LIMIT) return;
    uint32_t request = owner->search.requestId, identity = owner->search.rows[index].contractId;
    ++owner->references;
    owner->searchBusy = TRUE;
    /* Refresh before applying: a lookup in the inspector may have replaced this
     * model since the last timer. The visible row must still name that request. */
    (void)UmiBrokerWorkspaceGtkRefresh(owner);
    gboolean valid = !owner->closed && owner->connection != NULL &&
        owner->snapshot.state == UMI_IBKR_READY && !owner->snapshot.stale &&
        owner->hasSearch && owner->search.complete && !owner->search.failed && !owner->search.stale &&
        owner->search.requestId == request && index < owner->search.count &&
        owner->search.rows[index].contractId == identity && identity != 0U &&
        !(owner->hasQuote && owner->quote.subscribed);
    if (valid) {
        owner->quoteBusy = TRUE;
        char number[24];
        (void)g_snprintf(number, sizeof(number), "%" PRIu32, identity);
        gtk_editable_set_text(GTK_EDITABLE(owner->quoteContract), number);
        /* A listing exchange does not establish a valid request route. Clear the
         * previous instrument's route so it cannot accidentally be reused. */
        if (!owner->closed) gtk_editable_set_text(GTK_EDITABLE(owner->quoteExchange), "");
        owner->quoteBusy = FALSE;
    }
    owner->searchBusy = FALSE;
    if (!owner->closed) WorkspaceSearchRender(owner, FALSE);
    if (!owner->closed) LabelText(owner->searchNotice, valid
        ? "Contract ID selected. Enter its broker-supported exchange, choose a data mode, then Subscribe."
        : "That result is no longer current. Search again and choose a completed candidate.");
    Release(owner);
}
static void WorkspaceSearchControls(UmiBrokerWorkspaceGtk *owner, GtkWidget *box)
{
    GtkWidget *section = gtk_expander_new("Find an instrument");
    GtkWidget *content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_expander_set_child(GTK_EXPANDER(section), content);
    gtk_expander_set_expanded(GTK_EXPANDER(section), TRUE);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    owner->searchPattern = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(owner->searchPattern), "Symbol or company name");
    gtk_entry_set_max_length(GTK_ENTRY(owner->searchPattern), 127);
    gtk_widget_set_hexpand(owner->searchPattern, TRUE);
    owner->searchStart = gtk_button_new_with_label("Search");
    owner->searchAbandon = gtk_button_new_with_label("Abandon search");
    owner->searchResults = gtk_drop_down_new(NULL, NULL);
    gtk_widget_set_hexpand(owner->searchResults, TRUE);
    owner->searchApply = gtk_button_new_with_label("Use selected contract ID");
    owner->searchStatus = Cell("", FALSE);
    owner->searchNotice = Cell("", FALSE);
    gtk_label_set_wrap(GTK_LABEL(owner->searchStatus), TRUE);
    gtk_label_set_wrap(GTK_LABEL(owner->searchNotice), TRUE);
    GtkWidget *widgets[] = {owner->searchPattern, owner->searchStart, owner->searchAbandon,
        owner->searchResults, owner->searchApply, owner->searchStatus, owner->searchNotice};
    const char *names[] = {"pattern", "start", "abandon", "results", "apply", "status", "notice"};
    for (size_t i = 0U; i < G_N_ELEMENTS(widgets); ++i) {
        char *name = g_strdup_printf("broker-workspace-search-%s", names[i]);
        gtk_widget_set_name(widgets[i], name); g_free(name);
    }
    gtk_box_append(GTK_BOX(row), owner->searchPattern);
    gtk_box_append(GTK_BOX(row), owner->searchStart);
    gtk_box_append(GTK_BOX(row), owner->searchAbandon);
    gtk_box_append(GTK_BOX(content), row);
    gtk_box_append(GTK_BOX(content), owner->searchResults);
    gtk_widget_set_halign(owner->searchApply, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(content), owner->searchApply);
    gtk_box_append(GTK_BOX(content), owner->searchStatus);
    gtk_box_append(GTK_BOX(content), owner->searchNotice);
    gtk_box_append(GTK_BOX(box), section);
    g_signal_connect(owner->searchStart, "clicked", G_CALLBACK(WorkspaceSearchStart), owner);
    g_signal_connect(owner->searchAbandon, "clicked", G_CALLBACK(WorkspaceSearchAbandon), owner);
    g_signal_connect(owner->searchApply, "clicked", G_CALLBACK(WorkspaceSearchApply), owner);
    g_signal_connect(owner->searchResults, "notify::selected", G_CALLBACK(WorkspaceSearchSelected), owner);
}
