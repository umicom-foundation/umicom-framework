/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_position_review.inc
 * PURPOSE: Compose captured positions with shared filtering and explicit quote-field preparation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Included only when the canonical native UI target supplies shared choices. */
static bool PositionReviewBusy(const ConnectionUi *ui)
{
    size_t source = 0U;
    return ui->positionBusy ||
           (ui->positionFilter != NULL &&
            UmiGtk4FilteredChoicesSelectedSource(ui->positionFilter, &source) == UMI_STATUS_BUSY);
}
static void PositionReviewRetire(ConnectionUi *ui)
{
    UmiIbkrPositionReviewDestroy(ui->positionReview);
    ui->positionReview = NULL;
}
static void PositionReviewSelected(GObject *picker, GParamSpec *property, gpointer data)
{
    (void)picker;
    (void)property;
    GtkWindow *window = g_object_ref(data);
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || PositionReviewBusy(ui))
    {
        g_object_unref(window);
        return;
    }
    ui->positionBusy = true;
    size_t source = 0U;
    UmiIbkrPositionObservation position;
    UmiIbkrPositionReviewSummary summary;
    UmiStatus status = UmiGtk4FilteredChoicesSelectedSource(ui->positionFilter, &source);
    if (status == UMI_STATUS_OK)
        status = UmiIbkrPositionReviewAt(ui->positionReview, source, &position);
    if (status == UMI_STATUS_OK)
        status = UmiIbkrPositionReviewRead(ui->positionReview, &summary);
    if (status == UMI_STATUS_OK)
    {
        char *text = g_strdup_printf(
            "Captured account: %s | %s | %s at capture\nRequest: %" PRIu64 " ms; completion: %" PRIu64
            " ms (monotonic)\n"
            "Contract: %s | %s | %s\nLocal symbol: %s | Trading class: %s\nExchange: %s | Currency: %s\n"
            "Quantity: %s | Average cost: %s\nExpiry: %s | Strike: %s | Right: %s | Multiplier: %s\n"
            "Provider text is unchanged. This review does not refresh automatically or describe current "
            "market value.",
            summary.account, summary.complete ? "complete" : "INCOMPLETE",
            summary.stale ? "STALE" : "connected", summary.requestedAtMilliseconds,
            summary.completedAtMilliseconds, position.contractId, position.symbol, position.securityType,
            position.localSymbol, position.tradingClass, position.exchange, position.currency,
            position.quantity, position.averageCost, position.expiry, position.strike, position.right,
            position.multiplier);
        gtk_label_set_text(GTK_LABEL(ui->positionDetail), text);
        g_free(text);
    }
    else
        gtk_label_set_text(GTK_LABEL(ui->positionDetail),
                           "No selected position from the active capture. Select a visible row, or capture "
                           "received positions again.");
    ui->positionBusy = false;
    g_object_unref(window);
}
/* The portable capture validates field bounds. GTK additionally needs valid
 * UTF-8 in every displayed field, including fields absent from chooser labels. */
static bool PositionReviewDisplayValid(const UmiIbkrPositionReview *review)
{
    UmiIbkrPositionReviewSummary summary;
    if (UmiIbkrPositionReviewRead(review, &summary) != UMI_STATUS_OK ||
        !g_utf8_validate(summary.account, -1, NULL))
        return false;
    for (size_t i = 0U; i < summary.count; ++i)
    {
        UmiIbkrPositionObservation position;
        if (UmiIbkrPositionReviewAt(review, i, &position) != UMI_STATUS_OK)
            return false;
#define DISPLAY_FIELD(field)                                                                                 \
    if (!g_utf8_validate(position.field, -1, NULL))                                                          \
    return false
        DISPLAY_FIELD(contractId);
        DISPLAY_FIELD(symbol);
        DISPLAY_FIELD(securityType);
        DISPLAY_FIELD(expiry);
        DISPLAY_FIELD(strike);
        DISPLAY_FIELD(right);
        DISPLAY_FIELD(multiplier);
        DISPLAY_FIELD(exchange);
        DISPLAY_FIELD(currency);
        DISPLAY_FIELD(localSymbol);
        DISPLAY_FIELD(tradingClass);
        DISPLAY_FIELD(quantity);
        DISPLAY_FIELD(averageCost);
#undef DISPLAY_FIELD
    }
    return true;
}
static void PositionReviewCapture(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWindow *window = g_object_ref(data);
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || PositionReviewBusy(ui))
    {
        g_object_unref(window);
        return;
    }
    ui->positionBusy = true;
    /* Copy before any GTK notification. Neither filtering nor selection can
     * borrow a row that the broker poller might update on the next iteration. */
    UmiIbkrConnectionSnapshot *snapshot = g_new0(UmiIbkrConnectionSnapshot, 1);
    UmiStatus status = UmiIbkrConnectionCopy(ui->connection, snapshot);
    UmiIbkrPositionReview *review = NULL;
    if (status == UMI_STATUS_OK)
        status = UmiIbkrPositionReviewCreate(snapshot, &review);
    g_free(snapshot);
    if (status == UMI_STATUS_OK && !PositionReviewDisplayValid(review))
        status = UMI_STATUS_PARSE_ERROR;
    UmiIbkrPositionReviewSummary summary = {0};
    if (status == UMI_STATUS_OK)
        status = UmiIbkrPositionReviewRead(review, &summary);
    char **rows = g_new0(char *, summary.count + 1U);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < summary.count; ++i)
    {
        UmiIbkrPositionObservation position;
        status = UmiIbkrPositionReviewAt(review, i, &position);
        if (status == UMI_STATUS_OK)
            rows[i] = g_strdup_printf("%s | %s | %s | %s | %s | %s | %s", position.symbol,
                                      position.securityType, position.currency, position.exchange,
                                      position.localSymbol, position.contractId, position.expiry);
    }
    if (status == UMI_STATUS_OK)
        status = UmiGtk4FilteredChoicesSetRows(ui->positionFilter, (const char *const *)rows, summary.count);
    g_strfreev(rows);
    if (Owner(window) != NULL)
    {
        if (status == UMI_STATUS_OK)
        {
            PositionReviewRetire(ui);
            ui->positionReview = review;
            review = NULL;
            char *text = g_strdup_printf("Captured %zu received positions for %s. %s; %s at capture. "
                                         "Select a row to inspect it. This did not send a broker request.",
                                         summary.count, summary.account,
                                         summary.complete ? "Complete response" : "INCOMPLETE response",
                                         summary.stale ? "STALE" : "connected");
            gtk_label_set_text(GTK_LABEL(ui->positionDetail), text);
            g_free(text);
        }
        else
            gtk_label_set_text(GTK_LABEL(ui->positionDetail),
                               "Positions were not captured. Read a selected account first. The previous "
                               "captured list, if any, remains available.");
    }
    UmiIbkrPositionReviewDestroy(review);
    ui->positionBusy = false;
    g_object_unref(window);
}
static void PositionReviewQuoteChoice(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWindow *window = g_object_ref(data);
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || PositionReviewBusy(ui))
    {
        g_object_unref(window);
        return;
    }
    ui->positionBusy = true;
    size_t source = 0U;
    UmiIbkrConnectionSnapshot *snapshot = g_new0(UmiIbkrConnectionSnapshot, 1);
    UmiStatus status = UmiGtk4FilteredChoicesSelectedSource(ui->positionFilter, &source);
    if (status == UMI_STATUS_OK)
        status = UmiIbkrConnectionCopy(ui->connection, snapshot);
    UmiIbkrQuoteContract contract;
    if (status == UMI_STATUS_OK)
        status = UmiIbkrPositionReviewQuote(ui->positionReview, source, snapshot, &contract);
    g_free(snapshot);
    if (status != UMI_STATUS_OK || ui->quoteSubscribed)
        gtk_label_set_text(GTK_LABEL(ui->positionDetail),
                           "Quote fields were not changed. Choose a current complete capture on this "
                           "connection and cancel any existing quote stream. "
                           "The position needs a numeric contract ID and an explicit exchange; use TWS "
                           "Contract Description when either is missing.");
    else
    {
        char identity[24];
        g_snprintf(identity, sizeof(identity), "%" PRIu32, contract.contractId);
        gtk_editable_set_text(GTK_EDITABLE(ui->quoteContract), identity);
        if (Owner(window) != NULL)
            gtk_editable_set_text(GTK_EDITABLE(ui->quoteExchange), contract.exchange);
        if (Owner(window) != NULL)
        {
            bool same =
                strcmp(gtk_editable_get_text(GTK_EDITABLE(ui->quoteContract)), identity) == 0 &&
                strcmp(gtk_editable_get_text(GTK_EDITABLE(ui->quoteExchange)), contract.exchange) == 0;
            gtk_label_set_text(
                GTK_LABEL(ui->positionDetail),
                same ? "Contract ID and exchange copied to the quote inspector. Check both against TWS, then "
                       "subscribe explicitly. Nothing was requested or traded."
                     : "Quote fields changed during copying. Review both fields before requesting market "
                       "data.");
        }
    }
    ui->positionBusy = false;
    g_object_unref(window);
}
static void PositionReviewControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *parent)
{
    GtkWidget *capture = gtk_button_new_with_label("Capture received positions for review");
    GtkWidget *quote = gtk_button_new_with_label("Copy selected contract to quote inspector");
    ui->positionPicker = gtk_drop_down_new(NULL, NULL);
    ui->positionFilter = UmiGtk4FilteredChoicesCreate(GTK_DROP_DOWN(ui->positionPicker));
    ui->positionDetail = Text("Capture already received account positions, then filter or sort their labels. "
                              "No broker refresh occurs. The original account report and CSV export remain "
                              "complete and unfiltered.");
    gtk_label_set_selectable(GTK_LABEL(ui->positionDetail), TRUE);
    GtkWidget *items[] = {capture, ui->positionFilter, ui->positionPicker, quote, ui->positionDetail};
    const char *ids[] = {"ibkr-position-capture", "ibkr-position-filter", "ibkr-position-choices",
                         "ibkr-position-quote", "ibkr-position-detail"};
    for (size_t i = 0U; i < G_N_ELEMENTS(items); ++i)
    {
        gtk_widget_set_name(items[i], ids[i]);
        gtk_box_append(GTK_BOX(parent), items[i]);
    }
    g_signal_connect_object(capture, "clicked", G_CALLBACK(PositionReviewCapture), window, 0);
    g_signal_connect_object(quote, "clicked", G_CALLBACK(PositionReviewQuoteChoice), window, 0);
    g_signal_connect_object(ui->positionPicker, "notify::selected", G_CALLBACK(PositionReviewSelected),
                            window, 0);
}
