/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_scanner.inc
 * PURPOSE: Present scanner filters and ranked discovery without selecting a changed row.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct ScannerField
{
    const char *label, *name;
    size_t offset, capacity;
} ScannerField;
#define SCANNER_FIELD(member, label)                                                                         \
    {label, "ibkr-scanner-" #member, offsetof(UmiIbkrScannerQuery, member),                                  \
     sizeof(((UmiIbkrScannerQuery *)0)->member)}
static const ScannerField scannerFields[] = {
    SCANNER_FIELD(instrument, "Instrument"),
    SCANNER_FIELD(locationCode, "Location code"),
    SCANNER_FIELD(scanCode, "Scan code"),
    SCANNER_FIELD(abovePrice, "Minimum price"),
    SCANNER_FIELD(belowPrice, "Maximum price"),
    SCANNER_FIELD(aboveVolume, "Minimum volume"),
    SCANNER_FIELD(marketCapAbove, "Minimum market cap"),
    SCANNER_FIELD(marketCapBelow, "Maximum market cap"),
    SCANNER_FIELD(moodyRatingAbove, "Moody minimum rating"),
    SCANNER_FIELD(moodyRatingBelow, "Moody maximum rating"),
    SCANNER_FIELD(spRatingAbove, "S&P minimum rating"),
    SCANNER_FIELD(spRatingBelow, "S&P maximum rating"),
    SCANNER_FIELD(maturityDateAbove, "Maturity from (YYYYMMDD)"),
    SCANNER_FIELD(maturityDateBelow, "Maturity to (YYYYMMDD)"),
    SCANNER_FIELD(couponRateAbove, "Minimum coupon"),
    SCANNER_FIELD(couponRateBelow, "Maximum coupon"),
    SCANNER_FIELD(averageOptionVolumeAbove, "Minimum average option volume"),
    SCANNER_FIELD(scannerSettingPairs, "Scanner setting pairs"),
    SCANNER_FIELD(stockTypeFilter, "Stock type filter")};
#undef SCANNER_FIELD
static bool ScannerInputs(ConnectionUi *ui, UmiIbkrScannerQuery *q)
{
    memset(q, 0, sizeof *q);
    q->numberOfRows = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->scannerRows));
    q->excludeConvertible = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->scannerExclude)) != FALSE;
    for (size_t i = 0; i < sizeof scannerFields / sizeof scannerFields[0]; ++i)
    {
        const char *value = gtk_editable_get_text(GTK_EDITABLE(ui->scannerEntries[i]));
        if (strlen(value) >= scannerFields[i].capacity)
            return false;
        strcpy((char *)q + scannerFields[i].offset, value);
    }
    const char *filters = gtk_editable_get_text(GTK_EDITABLE(ui->scannerFilters));
    if (strlen(filters) >= 512U)
        return false;
    gchar **pairs = g_strsplit(filters, ";", -1);
    bool valid = true;
    for (size_t i = 0; pairs[i] && valid; ++i)
    {
        if (!*pairs[i])
        {
            if (pairs[i + 1U])
                valid = false;
            continue;
        }
        char *equal = strchr(pairs[i], '=');
        if (!equal || equal == pairs[i] || !equal[1] || strchr(equal + 1, '=') ||
            q->filterCount == UMI_IBKR_SCANNER_FILTER_LIMIT)
        {
            valid = false;
            break;
        }
        *equal = 0;
        UmiIbkrScannerFilter *f = &q->filters[q->filterCount++];
        if (strlen(pairs[i]) >= sizeof f->tag || strlen(equal + 1) >= sizeof f->value)
        {
            valid = false;
            break;
        }
        strcpy(f->tag, pairs[i]);
        strcpy(f->value, equal + 1);
    }
    g_strfreev(pairs);
    return valid;
}
static void ScannerPublish(ConnectionUi *ui, uint64_t now)
{
    UmiIbkrScannerSnapshot s;
    if (UmiIbkrScannerCopy(ui->connection, ui->scannerRequest, now, 60000U, &s) != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(ui->scannerOutput),
                           "No scanner subscription. Connect and enter a scan.");
        return;
    }
    GString *text = g_string_new(s.message);
    g_string_append_printf(text, "\n%zu contracts | refresh %" G_GUINT64_FORMAT " | %s\n", s.count,
                           (guint64)s.generation, s.stale ? "STALE" : "Recent complete refresh");
    if (ui->scannerDisplayedRequest != s.requestId || ui->scannerDisplayedGeneration != s.generation)
    {
        GtkStringList *items = gtk_string_list_new(NULL);
        for (size_t i = 0; i < s.count; ++i)
        {
            UmiIbkrScannerRow row;
            if (UmiIbkrScannerRowCopy(ui->connection, s.requestId, s.generation, i, &row) != UMI_STATUS_OK)
                continue;
            gchar *label = g_strdup_printf("%u | %s | %s | %s | %u", row.rank + 1U, row.symbol,
                                           row.securityType, row.currency, (unsigned)row.contractId);
            gtk_string_list_append(items, label);
            g_free(label);
        }
        /* Bind identity before model notifications. The shared entry guard blocks
         * reentrant button actions until the replacement is fully published. */
        ui->scannerDisplayedRequest = s.requestId;
        ui->scannerDisplayedGeneration = s.generation;
        gtk_drop_down_set_model(GTK_DROP_DOWN(ui->scannerResults), G_LIST_MODEL(items));
        g_object_unref(items);
    }
    if (!ui->closed)
    {
        guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->scannerResults));
        UmiIbkrScannerRow row;
        if (UmiIbkrScannerRowCopy(ui->connection, s.requestId, s.generation, (size_t)index, &row) ==
            UMI_STATUS_OK)
            g_string_append_printf(
                text, "%s | %s | class %s | exchange %s\nDistance: %s | Benchmark: %s | Projection: %s\n",
                row.localSymbol, row.marketName, row.tradingClass, row.exchange, row.distance, row.benchmark,
                row.projection);
        g_string_append(
            text, "Scanner ranks are not quotes. Review the contract and choose the quote route explicitly.");
        if (strcmp(gtk_label_get_text(GTK_LABEL(ui->scannerOutput)), text->str))
            gtk_label_set_text(GTK_LABEL(ui->scannerOutput), text->str);
    }
    g_string_free(text, TRUE);
}
static void ScannerPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy || !ui->scannerRequest)
        return;
    ui = FillEnter(ui->window);
    if (!ui)
        return;
    ScannerPublish(ui, now);
    FillLeave(ui);
}
static void ScannerStartClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    UmiIbkrScannerQuery q;
    UmiIbkrScannerSnapshot previous;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    if (ScannerInputs(ui, &q))
    {
        if (ui->scannerRequest &&
            UmiIbkrScannerCopy(ui->connection, ui->scannerRequest, now, 60000U, &previous) == UMI_STATUS_OK &&
            previous.needsCancel)
            status = UMI_STATUS_BUSY;
        else
            status = UmiIbkrScannerRequest(ui->connection, &q, now, &ui->scannerRequest);
    }
    if (status == UMI_STATUS_OK)
        ScannerPublish(ui, now);
    else
    {
        gchar *message = g_strdup_printf("Scanner not started: %s. Cancel the previous scan, check filters "
                                         "and allow a second between requests.",
                                         umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(ui->scannerOutput), message);
        g_free(message);
    }
    FillLeave(ui);
}
static void ScannerStopClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    if (UmiIbkrScannerCancel(ui->connection, ui->scannerRequest, now) == UMI_STATUS_OK)
        ScannerPublish(ui, now);
    else
        gtk_label_set_text(GTK_LABEL(ui->scannerOutput), "No scanner subscription was cancelled.");
    FillLeave(ui);
}
static void ScannerSelectionChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    ScannerPublish(ui, UmiIbkrMonotonicMilliseconds());
    FillLeave(ui);
}
static void ScannerApplyClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    UmiIbkrScannerSnapshot s;
    UmiIbkrScannerRow row;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->scannerResults));
    bool valid = !ui->quoteSubscribed && ui->scannerRequest == ui->scannerDisplayedRequest &&
                 UmiIbkrScannerCopy(ui->connection, ui->scannerRequest, now, 60000U, &s) == UMI_STATUS_OK &&
                 !s.stale &&
                 UmiIbkrScannerRowCopy(ui->connection, ui->scannerRequest, ui->scannerDisplayedGeneration,
                                       (size_t)index, &row) == UMI_STATUS_OK;
    if (valid)
    {
        char id[32];
        g_snprintf(id, sizeof id, "%u", (unsigned)row.contractId);
        FillWatchRetire(ui);
        gtk_editable_set_text(GTK_EDITABLE(ui->quoteContract), id);
        if (!ui->closed)
            gtk_label_set_text(GTK_LABEL(ui->scannerOutput),
                               "Contract ID copied. Review details and the route before starting a separate "
                               "quote subscription.");
    }
    else
        gtk_label_set_text(GTK_LABEL(ui->scannerOutput),
                           "Choose a current result and stop the existing quote stream. A changed refresh "
                           "must be reviewed again.");
    FillLeave(ui);
}
static void ScannerControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Market scanner"), *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    gtk_box_append(GTK_BOX(box),
                   Text("Scanner results list contracts, not executable prices. Verify the scan codes in "
                        "TWS; available scans and filters depend on the broker."));
    for (size_t i = 0; i < sizeof scannerFields / sizeof scannerFields[0]; ++i)
    {
        ui->scannerEntries[i] = gtk_entry_new();
        Field(GTK_GRID(grid), scannerFields[i].label, ui->scannerEntries[i], (int)i, scannerFields[i].name);
    }
    gtk_editable_set_text(GTK_EDITABLE(ui->scannerEntries[0]), "STK");
    gtk_editable_set_text(GTK_EDITABLE(ui->scannerEntries[1]), "STK.US.MAJOR");
    gtk_editable_set_text(GTK_EDITABLE(ui->scannerEntries[2]), "TOP_PERC_GAIN");
    ui->scannerRows = gtk_spin_button_new_with_range(1, 50, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->scannerRows), 25);
    ui->scannerFilters = gtk_entry_new();
    Field(GTK_GRID(grid), "Maximum results", ui->scannerRows, 19, "ibkr-scanner-rows");
    Field(GTK_GRID(grid), "Generic filters: tag=value;tag=value", ui->scannerFilters, 20,
          "ibkr-scanner-filters");
    ui->scannerExclude = gtk_check_button_new_with_label("Exclude convertible bonds");
    ui->scannerResults = gtk_drop_down_new(NULL, NULL);
    gtk_widget_set_name(ui->scannerResults, "ibkr-scanner-results");
    ui->scannerOutput = Text("No scanner subscription. Connect and enter a scan.");
    gtk_widget_set_name(ui->scannerOutput, "ibkr-scanner-output");
    gtk_label_set_selectable(GTK_LABEL(ui->scannerOutput), TRUE);
    GtkWidget *start = gtk_button_new_with_label("Start scanner"),
              *stop = gtk_button_new_with_label("Cancel scanner"),
              *apply = gtk_button_new_with_label("Use selected contract ID");
    gtk_widget_set_name(start, "ibkr-scanner-start");
    gtk_widget_set_name(stop, "ibkr-scanner-stop");
    gtk_widget_set_name(apply, "ibkr-scanner-apply");
    gtk_box_append(GTK_BOX(box), grid);
    gtk_box_append(GTK_BOX(box), ui->scannerExclude);
    gtk_box_append(GTK_BOX(box), start);
    gtk_box_append(GTK_BOX(box), stop);
    gtk_box_append(GTK_BOX(box), ui->scannerResults);
    gtk_box_append(GTK_BOX(box), apply);
    gtk_box_append(GTK_BOX(box), ui->scannerOutput);
    DiscoveryExportControls(window, ui, box, 0U);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(start, "clicked", G_CALLBACK(ScannerStartClicked), window, 0);
    g_signal_connect_object(stop, "clicked", G_CALLBACK(ScannerStopClicked), window, 0);
    g_signal_connect_object(apply, "clicked", G_CALLBACK(ScannerApplyClicked), window, 0);
    g_signal_connect_object(ui->scannerResults, "notify::selected", G_CALLBACK(ScannerSelectionChanged),
                            window, 0);
}
