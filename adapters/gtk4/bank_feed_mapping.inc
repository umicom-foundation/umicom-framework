/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/bank_feed_mapping.inc
 * PURPOSE: Capture explicit CSV mappings and retire reports when their interpretation changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* The mapping form belongs to the shared report window. Applications expose
 * one launcher and do not duplicate bank parsing, worker ownership or settings. */
static GtkWidget *FeedMappingEntry(GtkGrid *grid, int row, const char *label,
                                    const char *name, const char *value)
{
    GtkWidget *caption = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
    GtkWidget *entry = gtk_entry_new();
    gtk_widget_set_name(entry, name);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_editable_set_text(GTK_EDITABLE(entry), value);
    gtk_grid_attach(grid, caption, 0, row, 1, 1);
    gtk_grid_attach(grid, entry, 1, row, 1, 1);
    return entry;
}
static GtkWidget *FeedMappingChoice(GtkGrid *grid, int row, const char *label,
                                     const char *name, const char *const *choices)
{
    GtkWidget *caption = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
    GtkWidget *choice = gtk_drop_down_new_from_strings(choices);
    gtk_widget_set_name(choice, name);
    gtk_grid_attach(grid, caption, 0, row, 1, 1);
    gtk_grid_attach(grid, choice, 1, row, 1, 1);
    return choice;
}
static GtkWidget *FeedMappingCreate(BankFeedUi *ui)
{
    GtkWidget *expander = gtk_expander_new("Match your CSV columns");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_expander_set_child(GTK_EXPANDER(expander), box);
    GtkWidget *help = gtk_label_new(
        "Enter the exact header names in your file. Clear names for absent optional columns. "
        "For an account or currency constant, clear its column name first. For separate amounts, "
        "clear Signed amount and supply both Debit and Credit headers. Dates and decimal amounts "
        "use the formats below; values are never guessed or trimmed.");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_box_append(GTK_BOX(box), help);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroll, -1, 220);
    gtk_box_append(GTK_BOX(box), scroll);
    GtkGrid *grid = GTK_GRID(gtk_grid_new());
    gtk_grid_set_row_spacing(grid, 6U);
    gtk_grid_set_column_spacing(grid, 12U);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), GTK_WIDGET(grid));
    UmiBankFeedCsvMapping defaults;
    UmiBankFeedCsvMappingInit(&defaults);
    const char *const labels[] = {"Account header", "Transaction ID header", "Reference header",
        "Description header", "Booking date header", "Currency header", "Signed amount header",
        "Status header", "Mutability header", "Debit header", "Credit header"};
    const char *const names[] = {"bank-map-account", "bank-map-transaction", "bank-map-reference",
        "bank-map-information", "bank-map-date", "bank-map-currency", "bank-map-amount",
        "bank-map-status", "bank-map-mutability", "bank-map-debit", "bank-map-credit"};
    for (size_t i = 0U; i < UMI_BANK_CSV_FIELD_COUNT; ++i) {
        ui->mapHeaders[i] = FeedMappingEntry(grid, (int)i, labels[i], names[i], defaults.headers[i]);
        g_signal_connect_object(ui->mapHeaders[i], "changed", G_CALLBACK(FeedChanged), ui->window, 0);
    }
    ui->mapAccount = FeedMappingEntry(grid, 11, "Account constant", "bank-map-account-value", "");
    ui->mapCurrency = FeedMappingEntry(grid, 12, "Currency constant", "bank-map-currency-value", "");
    const char *const dates[] = {"YYYY-MM-DD", "DD/MM/YYYY", "MM/DD/YYYY", NULL};
    const char *const amounts[] = {"Signed: negative is debit", "Separate debit and credit", NULL};
    const char *const statuses[] = {"Pending", "Booked", NULL};
    const char *const mutabilities[] = {"Mutable", "Immutable", NULL};
    ui->mapDate = FeedMappingChoice(grid, 13, "Date format", "bank-map-date-order", dates);
    ui->mapAmount = FeedMappingChoice(grid, 14, "Amount format", "bank-map-amount-mode", amounts);
    ui->mapStatus = FeedMappingChoice(grid, 15, "When status header is absent", "bank-map-booked", statuses);
    ui->mapMutability = FeedMappingChoice(grid, 16, "When mutability header is absent",
                                          "bank-map-mutable", mutabilities);
    g_signal_connect_object(ui->mapAccount, "changed", G_CALLBACK(FeedChanged), ui->window, 0);
    g_signal_connect_object(ui->mapCurrency, "changed", G_CALLBACK(FeedChanged), ui->window, 0);
    GtkWidget *choices[] = {ui->mapDate, ui->mapAmount, ui->mapStatus, ui->mapMutability};
    for (size_t i = 0U; i < G_N_ELEMENTS(choices); ++i)
        g_signal_connect_object(choices[i], "notify::selected",
                                 G_CALLBACK(FeedFormatChanged), ui->window, 0);
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
    gtk_widget_set_visible(expander, FALSE);
    return expander;
}
/* Capture whole strings or fail: truncating a header could select the wrong
 * source column. The worker sees a value copy, never a pointer into a widget. */
static bool FeedMappingText(GtkWidget *entry, char *out, size_t capacity)
{
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    size_t length = strlen(text);
    if (length >= capacity) return false;
    memcpy(out, text, length + 1U);
    return true;
}
static bool FeedMappingRead(BankFeedUi *ui, UmiBankFeedCsvMapping *mapping)
{
    memset(mapping, 0, sizeof *mapping);
    for (size_t i = 0U; i < UMI_BANK_CSV_FIELD_COUNT; ++i)
        if (!FeedMappingText(ui->mapHeaders[i], mapping->headers[i], sizeof mapping->headers[i]))
            return false;
    if (!FeedMappingText(ui->mapAccount, mapping->account, sizeof mapping->account) ||
        !FeedMappingText(ui->mapCurrency, mapping->currency, sizeof mapping->currency)) return false;
    guint date = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mapDate));
    guint amount = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mapAmount));
    guint status = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mapStatus));
    guint mutability = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->mapMutability));
    if (date > 2U || amount > 1U || status > 1U || mutability > 1U) return false;
    mapping->dateOrder = (UmiBankCsvDateOrder)date;
    mapping->amountMode = (UmiBankCsvAmountMode)amount;
    mapping->booked = status == 1U;
    mapping->mutableRecord = mutability == 0U;
    return true;
}
/* Invalidation emits notifications. A handler can edit a mapping during those
 * notifications; compare its values again rather than trusting only a counter. */
static bool FeedMappingMatches(BankFeedUi *ui, const UmiBankFeedCsvMapping *expected)
{
    UmiBankFeedCsvMapping current;
    if (!FeedMappingRead(ui, &current)) return false;
    for (size_t i = 0U; i < UMI_BANK_CSV_FIELD_COUNT; ++i)
        if (strcmp(current.headers[i], expected->headers[i]) != 0) return false;
    return strcmp(current.account, expected->account) == 0 &&
           strcmp(current.currency, expected->currency) == 0 &&
           current.dateOrder == expected->dateOrder && current.amountMode == expected->amountMode &&
           current.booked == expected->booked && current.mutableRecord == expected->mutableRecord;
}
