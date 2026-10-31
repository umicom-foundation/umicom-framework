/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/broker_workspace_watchlist.inc
 *
 * PURPOSE:
 *   Edit and explicitly save named local watchlists, then select exact instruments for broker pages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static void WorkspaceWatchRender(UmiBrokerWorkspaceGtk *owner)
{
    GtkStringList *rows = gtk_string_list_new(NULL);
    for (size_t i = 0U; i < owner->watchlist.count; ++i) {
        const UmiIbkrWatchItem *item = &owner->watchlist.items[i];
        char *text = g_strdup_printf("%s • %u • %s", item->label,
            (unsigned)item->contract.contractId, item->contract.exchange);
        char *valid = g_utf8_make_valid(text, -1);
        gtk_string_list_append(rows, valid); g_free(valid); g_free(text);
    }
    gtk_drop_down_set_model(GTK_DROP_DOWN(owner->watchRows), G_LIST_MODEL(rows));
    g_object_unref(rows);
}
/* Read through one bounded stream. A malformed or oversized file cannot replace
 * the current watchlist; a save uses the platform's consistent replacement. */
static gboolean WorkspaceWatchLoad(const char *path, UmiIbkrWatchlist *out, GError **error)
{
    GFile *file = g_file_new_for_path(path);
    GFileInputStream *stream = g_file_read(file, NULL, error);
    g_object_unref(file);
    if (!stream) return FALSE;
    char *bytes = g_malloc(65537U); gsize size = 0U;
    gboolean ok = g_input_stream_read_all(G_INPUT_STREAM(stream), bytes, 65537U, &size, NULL, error);
    g_object_unref(stream);
    GKeyFile *keys = g_key_file_new(); UmiIbkrWatchlist *next = g_new0(UmiIbkrWatchlist, 1);
    ok = ok && size <= 65536U && g_key_file_load_from_data(keys, bytes, size, G_KEY_FILE_NONE, error);
    g_free(bytes);
    if (ok) {
        char *name = g_key_file_get_string(keys, "Watchlist", "name", NULL);
        gint version = g_key_file_get_integer(keys, "Watchlist", "version", NULL);
        gint count = g_key_file_get_integer(keys, "Watchlist", "count", error);
        ok = name && strlen(name) < sizeof next->name && version == 1 &&
            count >= 0 && count <= (gint)UMI_IBKR_WATCHLIST_LIMIT && (!error || !*error);
        if (ok) { strcpy(next->name, name); next->count = (size_t)count; }
        g_free(name);
        for (size_t i = 0U; ok && i < next->count; ++i) {
            char group[24]; (void)g_snprintf(group, sizeof group, "Instrument%zu", i);
            guint64 id = g_key_file_get_uint64(keys, group, "contract", NULL);
            char *route = g_key_file_get_string(keys, group, "exchange", NULL);
            char *label = g_key_file_get_string(keys, group, "label", NULL);
            ok = id > 0U && id <= INT32_MAX && route && label &&
                strlen(route) < sizeof next->items[i].contract.exchange &&
                strlen(label) < sizeof next->items[i].label;
            if (ok) {
                next->items[i].contract.contractId = (uint32_t)id;
                strcpy(next->items[i].contract.exchange, route); strcpy(next->items[i].label, label);
            }
            g_free(route); g_free(label);
        }
    }
    ok = ok && UmiIbkrWatchlistValidate(next) == UMI_STATUS_OK;
    if (ok) *out = *next;
    g_free(next); g_key_file_unref(keys);
    return ok;
}
static gboolean WorkspaceWatchSave(const char *path, const UmiIbkrWatchlist *list, GError **error)
{
    if (UmiIbkrWatchlistValidate(list) != UMI_STATUS_OK) return FALSE;
    GKeyFile *keys = g_key_file_new();
    g_key_file_set_integer(keys, "Watchlist", "version", 1);
    g_key_file_set_string(keys, "Watchlist", "name", list->name);
    g_key_file_set_integer(keys, "Watchlist", "count", (gint)list->count);
    for (size_t i = 0U; i < list->count; ++i) {
        char group[24]; (void)g_snprintf(group, sizeof group, "Instrument%zu", i);
        g_key_file_set_uint64(keys, group, "contract", list->items[i].contract.contractId);
        g_key_file_set_string(keys, group, "exchange", list->items[i].contract.exchange);
        g_key_file_set_string(keys, group, "label", list->items[i].label);
    }
    gsize size = 0U; char *bytes = g_key_file_to_data(keys, &size, error);
    gboolean ok = bytes && g_file_set_contents_full(path, bytes, (gssize)size,
        G_FILE_SET_CONTENTS_CONSISTENT | G_FILE_SET_CONTENTS_DURABLE, 0600, error);
    g_free(bytes); g_key_file_unref(keys); return ok;
}
static void WorkspaceWatchAction(GtkButton *button, gpointer data)
{
    UmiBrokerWorkspaceGtk *owner = data;
    if (owner->closed || owner->watchBusy) return;
    ++owner->references; owner->watchBusy = TRUE;
    guint action = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "watch-action"));
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(owner->watchRows));
    char *name = g_strdup(gtk_editable_get_text(GTK_EDITABLE(owner->watchName)));
    char *path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(owner->watchPath)));
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT; GError *error = NULL;
    if (strlen(name) < sizeof owner->watchlist.name && name[0]) {
        UmiIbkrWatchlist *next = g_new(UmiIbkrWatchlist, 1); *next = owner->watchlist;
        strcpy(next->name, name);
        if (action == 0U) {
            UmiIbkrWatchItem item = {0}; guint64 id;
            const char *text = gtk_editable_get_text(GTK_EDITABLE(owner->quoteContract));
            const char *route = gtk_editable_get_text(GTK_EDITABLE(owner->quoteExchange));
            const char *label = gtk_editable_get_text(GTK_EDITABLE(owner->watchLabel));
            if (g_ascii_string_to_unsigned(text, 10, 1U, INT32_MAX, &id, NULL) &&
                strlen(route) < sizeof item.contract.exchange && strlen(label) < sizeof item.label) {
                item.contract.contractId = (uint32_t)id; strcpy(item.contract.exchange, route);
                strcpy(item.label, label); status = UmiIbkrWatchlistAdd(next, &item);
            }
        } else if (action == 1U) status = UmiIbkrWatchlistRemove(next, selected);
        else if (action == 2U && selected < next->count) {
            /* Selection does not replace an active quote's identity or auto-subscribe. */
            UmiIbkrQuoteSnapshot quote;
            gboolean active = owner->connection &&
                UmiIbkrGtkCopyQuote(owner->connection, 15000U, &quote) == UMI_STATUS_OK && quote.subscribed;
            if (!active) {
                UmiIbkrWatchItem item = next->items[selected]; char id[24];
                (void)g_snprintf(id, sizeof id, "%u", (unsigned)item.contract.contractId);
                gtk_editable_set_text(GTK_EDITABLE(owner->quoteContract), id);
                if (!owner->closed) gtk_editable_set_text(GTK_EDITABLE(owner->quoteExchange), item.contract.exchange);
                if (!owner->closed) (void)UmiBrokerWorkspaceGtkSelectPage(owner, UMI_BROKER_PAGE_MARKET);
                status = UMI_STATUS_OK;
            } else status = UMI_STATUS_BUSY;
        } else if (action == 3U && g_path_is_absolute(path) && !strstr(path, "://"))
            status = WorkspaceWatchSave(path, next, &error) ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
        else if (action == 4U && g_path_is_absolute(path) && !strstr(path, "://"))
            status = WorkspaceWatchLoad(path, next, &error) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
        if (!owner->closed && status == UMI_STATUS_OK && action != 2U) {
            owner->watchlist = *next;
            gtk_editable_set_text(GTK_EDITABLE(owner->watchName), next->name);
            if (!owner->closed) WorkspaceWatchRender(owner);
        }
        g_free(next);
    }
    if (!owner->closed) {
        char *notice = status == UMI_STATUS_OK ? g_strdup(action == 3U ? "Watchlist saved locally." :
            action == 4U ? "Watchlist opened." : "Watchlist updated. Start market data explicitly for the selected instrument.") :
            g_strdup_printf("Watchlist action not completed: %s. %s", umi_status_text(status),
                error ? error->message : "Check name, label, exact contract, route and local file path. Stop an active quote before changing instrument.");
        LabelText(owner->watchNotice, notice); g_free(notice); owner->watchBusy = FALSE;
    }
    g_clear_error(&error); g_free(name); g_free(path); Release(owner);
}
static GtkWidget *WorkspaceWatchPage(UmiBrokerWorkspaceGtk *owner)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    strcpy(owner->watchlist.name, "My watchlist");
    owner->watchName = gtk_entry_new(); owner->watchLabel = gtk_entry_new(); owner->watchPath = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(owner->watchName), owner->watchlist.name);
    owner->watchRows = gtk_drop_down_new(NULL, NULL); owner->watchNotice = Cell("", FALSE);
    GtkWidget *fields[] = {owner->watchName, owner->watchLabel, owner->watchPath, owner->watchRows};
    const char *names[] = {"name", "label", "path", "rows"};
    const char *titles[] = {"Watchlist name", "Label for the instrument selected in Market data",
        "Local watchlist file (full path; Save replaces this file)", "Saved instruments"};
    for (size_t i = 0U; i < 4U; ++i) {
        char *id = g_strconcat("broker-watchlist-", names[i], NULL);
        gtk_widget_set_name(fields[i], id); g_free(id);
        gtk_box_append(GTK_BOX(page), Cell(titles[i], TRUE)); gtk_box_append(GTK_BOX(page), fields[i]);
    }
    gtk_entry_set_max_length(GTK_ENTRY(owner->watchName), 127);
    gtk_entry_set_max_length(GTK_ENTRY(owner->watchLabel), 127);
    GtkWidget *actions = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(actions), GTK_SELECTION_NONE);
    const char *labels[] = {"Add selected instrument", "Remove selected row", "Use instrument", "Save", "Open"};
    for (guint i = 0U; i < 5U; ++i) {
        GtkWidget *button = gtk_button_new_with_label(labels[i]); owner->watchButtons[i] = button;
        const char *ids[] = {"broker-watchlist-add", "broker-watchlist-remove", "broker-watchlist-use",
            "broker-watchlist-save", "broker-watchlist-open"};
        gtk_widget_set_name(button, ids[i]);
        g_object_set_data(G_OBJECT(button), "watch-action", GUINT_TO_POINTER(i));
        g_signal_connect(button, "clicked", G_CALLBACK(WorkspaceWatchAction), owner);
        gtk_flow_box_insert(GTK_FLOW_BOX(actions), button, -1);
    }
    gtk_label_set_wrap(GTK_LABEL(owner->watchNotice), TRUE);
    gtk_box_append(GTK_BOX(page), actions); gtk_box_append(GTK_BOX(page), owner->watchNotice);
    return page;
}
