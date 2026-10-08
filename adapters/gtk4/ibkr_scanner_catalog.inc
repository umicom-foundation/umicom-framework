/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_scanner_catalog.inc
 * PURPOSE: Display and search broker XML as plain text without creating executable content.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void CatalogPublish(ConnectionUi *ui, uint64_t now)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->catalogText));
    if (ui->catalogReset)
    {
        ui->catalogReset = FALSE;
        gtk_text_buffer_set_text(buffer, "", 0);
        if (ui->closed)
            return;
    }
    UmiIbkrScannerCatalogSnapshot s;
    UmiStatus status = UmiIbkrScannerCatalogCopy(ui->connection, now, &s);
    if (status != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(ui->catalogOutput), "No catalogue requested on this connection.");
        return;
    }
    if (s.complete && !ui->catalogLoaded)
    {
        /* Copy once: the UI buffer owns its text independently of disconnect
         * or later connection destruction. GTK never interprets this as XML. */
        char *text = g_try_malloc(s.byteCount + 1U);
        if (!text)
        {
            gtk_label_set_text(GTK_LABEL(ui->catalogOutput), "Not enough memory to display the catalogue.");
            return;
        }
        status = UmiIbkrScannerCatalogTextCopy(ui->connection, text, s.byteCount + 1U);
        if (status == UMI_STATUS_OK)
        {
            ui->catalogLoaded = TRUE;
            gtk_text_buffer_set_text(buffer, text, (int)s.byteCount);
        }
        g_free(text);
        if (ui->closed)
            return;
    }
    gchar *message =
        g_strdup_printf("%s\n%zu bytes | %s", s.message, s.byteCount,
                        s.stale ? "STALE / incomplete" : "Complete catalogue for this connection");
    if (strcmp(gtk_label_get_text(GTK_LABEL(ui->catalogOutput)), message))
        gtk_label_set_text(GTK_LABEL(ui->catalogOutput), message);
    g_free(message);
}
static void CatalogPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy)
        return;
    ui = FillEnter(ui->window);
    if (!ui)
        return;
    CatalogPublish(ui, now);
    FillLeave(ui);
}
static void CatalogReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrScannerCatalogRequest(ui->connection, now);
    if (status == UMI_STATUS_OK)
        CatalogPublish(ui, now);
    else
    {
        gchar *message = g_strdup_printf(
            "Catalogue not requested: %s. Connect first; one request is allowed per connection.",
            umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(ui->catalogOutput), message);
        g_free(message);
    }
    FillLeave(ui);
}
static void CatalogFindClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    const char *needle = gtk_editable_get_text(GTK_EDITABLE(ui->catalogSearch));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->catalogText));
    GtkTextIter begin, matchBegin, matchEnd;
    gtk_text_buffer_get_iter_at_mark(buffer, &begin, gtk_text_buffer_get_insert(buffer));
    GtkTextSearchFlags flags = GTK_TEXT_SEARCH_TEXT_ONLY | GTK_TEXT_SEARCH_CASE_INSENSITIVE;
    gboolean found =
        *needle && gtk_text_iter_forward_search(&begin, needle, flags, &matchBegin, &matchEnd, NULL);
    if (*needle && !found)
    {
        gtk_text_buffer_get_start_iter(buffer, &begin);
        found = gtk_text_iter_forward_search(&begin, needle, flags, &matchBegin, &matchEnd, NULL);
    }
    if (found)
    {
        /* Put the insertion mark after the match so Find next advances and
         * wraps. Keep the buffer alive across selection notifications. */
        g_object_ref(buffer);
        gtk_text_buffer_select_range(buffer, &matchEnd, &matchBegin);
        if (!ui->closed)
            gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(ui->catalogText),
                                               gtk_text_buffer_get_insert(buffer));
        g_object_unref(buffer);
    }
    else
    {
        gtk_label_set_text(GTK_LABEL(ui->catalogOutput),
                           *needle ? "No matching catalogue text."
                                   : "Enter a scan, location or filter name to find.");
    }
    FillLeave(ui);
}
static void CatalogControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Scanner parameter catalogue");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *read = gtk_button_new_with_label("Read scanner catalogue");
    GtkWidget *find = gtk_button_new_with_label("Find next");
    GtkWidget *scroll = gtk_scrolled_window_new();
    ui->catalogSearch = gtk_entry_new();
    ui->catalogText = gtk_text_view_new();
    ui->catalogOutput = Text("Connect, then read the broker's scanner catalogue.");
    gtk_widget_set_name(read, "ibkr-catalog-read");
    gtk_widget_set_name(find, "ibkr-catalog-find");
    gtk_widget_set_name(ui->catalogSearch, "ibkr-catalog-search");
    gtk_widget_set_name(ui->catalogText, "ibkr-catalog-text");
    gtk_widget_set_name(ui->catalogOutput, "ibkr-catalog-output");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(ui->catalogText), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(ui->catalogText), TRUE);
    gtk_widget_set_size_request(scroll, -1, 240);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), ui->catalogText);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    gtk_box_append(GTK_BOX(box), Text("Read available instruments, locations, scan codes and filters. "
                                      "This viewer shows raw XML as text; review the relevant entries before "
                                      "entering scanner fields."));
    gtk_box_append(GTK_BOX(box), read);
    gtk_box_append(GTK_BOX(box), ui->catalogSearch);
    gtk_box_append(GTK_BOX(box), find);
    gtk_box_append(GTK_BOX(box), scroll);
    gtk_box_append(GTK_BOX(box), ui->catalogOutput);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(read, "clicked", G_CALLBACK(CatalogReadClicked), window, 0);
    g_signal_connect_object(find, "clicked", G_CALLBACK(CatalogFindClicked), window, 0);
}
