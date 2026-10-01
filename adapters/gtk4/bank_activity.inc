/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_activity.inc
 * PURPOSE: Keep editable filters separate from the exact captured banking activity report.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included after existing form and queue helpers. Signals are owned by the
 * window; SignalUi rejects retained controls after the service is closed. */
static void ActivityClear(BankUi *ui, const char *message)
{
    UmiBankActivityDestroy(ui->activity); ui->activity = NULL;
    gtk_text_buffer_set_text(ui->activityText, message, -1);
    gtk_widget_set_sensitive(ui->activityCopy, FALSE);
}
static void ActivityFilterChanged(GtkEditable *editable, gpointer window)
{
    (void)editable; BankUi *ui = SignalUi(window); if (ui == NULL) return;
    ActivityClear(ui, "Filters changed. Choose Capture activity to prepare a new report.");
}
static void ActivityDirectionChanged(GObject *object, GParamSpec *spec, gpointer window)
{ (void)object; (void)spec; ActivityFilterChanged(NULL, window); }
static void ActivityCaptureClicked(GtkButton *button, gpointer window)
{
    (void)button; BankUi *ui = SignalUi(window); if (ui == NULL) return;
    ActivityClear(ui, "No activity captured for these filters.");
    UmiBankActivityQuery query = {0};
    const char *reference = EntryText(ui->activityReference);
    guint direction = gtk_drop_down_get_selected(ui->activityDirection);
    if (!CopyId(ui->activityAccount, &query.accountId) || strlen(reference) >= sizeof query.reference ||
        direction > 2U || UmiBankActivityDateParse(EntryText(ui->activityFrom), &query.fromDate) != UMI_STATUS_OK ||
        UmiBankActivityDateParse(EntryText(ui->activityTo), &query.toDate) != UMI_STATUS_OK) {
        ActivityClear(ui, "Enter an account ID and dates as YYYY-MM-DD, or leave a date blank. No data changed."); return;
    }
    memcpy(query.reference, reference, strlen(reference) + 1U);
    query.direction = (UmiBankActivityDirection)direction;
    UmiStatus status = UmiBankActivityQueryValidate(&query);
    if (status != UMI_STATUS_OK) {
        ActivityClear(ui, "Check the account, date order and reference. References use letters, digits, dot, dash or underscore."); return;
    }
    UmiBankActivity *next = NULL;
    status = UmiBankActivityCapture(ui->operations, &query, &next);
    size_t required = 0;
    if (status == UMI_STATUS_OK) status = UmiBankActivityDescribe(next, NULL, 0, &required);
    char *text = status == UMI_STATUS_OK ? g_try_malloc(required) : NULL;
    if (status == UMI_STATUS_OK && text == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) status = UmiBankActivityDescribe(next, text, required, NULL);
    if (status != UMI_STATUS_OK) {
        g_free(text); UmiBankActivityDestroy(next);
        ActivityClear(ui, status == UMI_STATUS_NOT_FOUND ? "Account not found. Check its exact ID." :
            status == UMI_STATUS_CAPACITY_EXCEEDED ? "The matching debit or credit total exceeds the supported integer range. Narrow the date or reference filter." :
            "Activity could not be captured. Check the account and storage status before trying again.");
        return;
    }
    ui->activity = next;
    char *valid = g_utf8_make_valid(text, -1);
    gtk_text_buffer_set_text(ui->activityText, valid, -1); g_free(valid); g_free(text);
    gtk_widget_set_sensitive(ui->activityCopy, TRUE);
}
static void ActivityCopyClicked(GtkButton *button, gpointer window)
{
    BankUi *ui = SignalUi(window); if (ui == NULL || ui->activity == NULL) return;
    UmiCsvDocument *document = NULL;
    UmiStatus status = UmiBankActivityExportCsv(ui->activity, &document);
    if (status == UMI_STATUS_OK) {
        gdk_clipboard_set_text(gtk_widget_get_clipboard(GTK_WIDGET(button)), UmiCsvDocumentData(document));
        gtk_label_set_text(ui->message, "Copied the captured activity report, its filters and revision. No new ledger data was read.");
    } else Message(ui, status);
    UmiCsvDocumentDestroy(document);
}
static void ActivityCreate(BankUi *ui)
{
    GtkWidget *page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *fields = gtk_grid_new(); gtk_grid_set_column_spacing(GTK_GRID(fields), 6);
    gtk_grid_set_row_spacing(GTK_GRID(fields), 4);
    ui->activityAccount = FormEntry(GTK_GRID(fields), "Account ID", 0, 0, 47, "");
    ui->activityFrom = FormEntry(GTK_GRID(fields), "From date (blank = unbounded)", 0, 1, 10, "");
    ui->activityTo = FormEntry(GTK_GRID(fields), "To date (blank = unbounded)", 2, 1, 10, "");
    ui->activityReference = FormEntry(GTK_GRID(fields), "Reference / journal contains", 0, 2, 47, "");
    const char *directions[] = {"All postings", "Debits only", "Credits only", NULL};
    ui->activityDirection = GTK_DROP_DOWN(QueueTag(gtk_drop_down_new_from_strings(directions), "bank.activity.direction"));
    gtk_grid_attach(GTK_GRID(fields), GTK_WIDGET(ui->activityDirection), 2, 2, 2, 1);
    GtkEntry *entries[] = {ui->activityAccount, ui->activityFrom, ui->activityTo, ui->activityReference};
    const char *tags[] = {"bank.activity.account", "bank.activity.from", "bank.activity.to", "bank.activity.reference"};
    for (size_t i = 0; i < 4; ++i) (void)QueueTag(GTK_WIDGET(entries[i]), tags[i]);
    gtk_entry_set_placeholder_text(ui->activityFrom, "YYYY-MM-DD"); gtk_entry_set_placeholder_text(ui->activityTo, "YYYY-MM-DD");
    gtk_box_append(GTK_BOX(page), fields);
    GtkWidget *hint = gtk_label_new("Business dates are inclusive. Reports retain posting order and identify captured balances. Reload committed data first to read another window's changes.");
    gtk_label_set_wrap(GTK_LABEL(hint), TRUE); gtk_label_set_xalign(GTK_LABEL(hint), 0);
    gtk_box_append(GTK_BOX(page), hint);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *capture = QueueTag(gtk_button_new_with_label("Capture activity"), "bank.activity.capture");
    ui->activityCopy = QueueTag(gtk_button_new_with_label("Copy captured activity CSV"), "bank.activity.copy-csv");
    gtk_box_append(GTK_BOX(actions), capture); gtk_box_append(GTK_BOX(actions), ui->activityCopy); gtk_box_append(GTK_BOX(page), actions);
    GtkWidget *scroll = gtk_scrolled_window_new(), *view = QueueTag(gtk_text_view_new(), "bank.activity.report");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE); gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    ui->activityText = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view); gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(page), scroll); gtk_notebook_append_page(ui->pages, page, gtk_label_new("Account activity"));
    for (size_t i = 0; i < 4; ++i)
        g_signal_connect_object(entries[i], "changed", G_CALLBACK(ActivityFilterChanged), G_OBJECT(ui->window), 0);
    g_signal_connect_object(ui->activityDirection, "notify::selected", G_CALLBACK(ActivityDirectionChanged), G_OBJECT(ui->window), 0);
    g_signal_connect_object(capture, "clicked", G_CALLBACK(ActivityCaptureClicked), G_OBJECT(ui->window), 0);
    g_signal_connect_object(ui->activityCopy, "clicked", G_CALLBACK(ActivityCopyClicked), G_OBJECT(ui->window), 0);
    ActivityClear(ui, "Choose an account and Capture activity. This copied report does not update automatically.");
}
