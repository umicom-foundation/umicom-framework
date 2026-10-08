/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_market_rule.inc
 * PURPOSE: Present route-specific price bands beside a full-quantity limit review without changing the price.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static gboolean MarketRuleBindingCurrent(ConnectionUi *ui, uint64_t now)
{
    if (ui->ruleId == 0U || ui->contractRequest != ui->ruleContractRequest ||
        ui->quoteRequest != ui->ruleQuoteRequest)
        return FALSE;
    UmiIbkrQuoteSnapshot quote;
    if (FillCurrentQuote(ui, now, &quote) != UMI_STATUS_OK || !quote.subscribed ||
        quote.contract.contractId != ui->ruleContract.contractId ||
        strcmp(quote.contract.exchange, ui->ruleContract.exchange) != 0)
        return FALSE;
    UmiIbkrContractDetailsSnapshot *details = g_try_malloc(sizeof *details);
    if (details == NULL)
        return FALSE;
    uint32_t rule = 0U;
    UmiStatus status = UmiIbkrContractDetailsCopy(ui->connection, ui->ruleContractRequest, now, details);
    if (status == UMI_STATUS_OK && details->count == 1U)
        status = UmiIbkrContractRouteRule(details, 0U, quote.contract.exchange, &rule);
    g_free(details);
    return status == UMI_STATUS_OK && rule == ui->ruleId;
}
static gchar *MarketRuleText(ConnectionUi *ui, uint64_t now)
{
    if (ui->connection == NULL || ui->ruleId == 0U)
        return g_strdup(
            "Read a complete contract description, then request the current quote route's price bands.");
    UmiIbkrMarketRuleSnapshot copy;
    UmiStatus status = UmiIbkrMarketRuleCopy(ui->connection, ui->ruleId, now, &copy);
    if (status != UMI_STATUS_OK)
        return g_strdup(umi_status_text(status));
    GString *text = g_string_new(NULL);
    g_string_append_printf(text, "Captured rule %u for contract %u on %s: %s%s.\n", (unsigned)copy.ruleId,
                           (unsigned)ui->ruleContract.contractId, ui->ruleContract.exchange,
                           copy.complete ? "complete" : "awaiting response", copy.stale ? "; stale" : "");
    for (size_t index = 0U; index < copy.count; ++index)
    {
        char lower[UMI_DECIMAL_TEXT_MAX + 1U], increment[UMI_DECIMAL_TEXT_MAX + 1U];
        if (UmiDecimalFormat(copy.bands[index].lowerBound, lower, sizeof lower) != UMI_STATUS_OK ||
            UmiDecimalFormat(copy.bands[index].increment, increment, sizeof increment) != UMI_STATUS_OK)
            continue;
        g_string_append_printf(text, "From %s: increment %s\n", lower, increment);
    }
    /* The mathematical review is tied to the captured instrument and current
     * limit field. Editing the limit updates this explanation, never the price. */
    UmiDecimal price;
    UmiTradingPriceIncrementReview review;
    if (!MarketRuleBindingCurrent(ui, now))
        g_string_append(
            text,
            "\nThe quote or contract lookup changed. Read its price bands again before reviewing a limit.");
    else if (!copy.complete || copy.stale || copy.count == 0U)
        g_string_append(text, "\nNo complete usable price-band capture is available.");
    else if (FillInputDecimal(ui->fillPrice, &price) != UMI_STATUS_OK || price.coefficient <= 0)
        g_string_append(text,
                        "\nEnter a positive limit in the full-quantity controls to review its increment.");
    else if (UmiTradingPriceIncrementReviewAt(copy.bands, copy.count, price, &review) != UMI_STATUS_OK)
        g_string_append(text, "\nThis limit is outside the supported captured bands.");
    else
    {
        char increment[UMI_DECIMAL_TEXT_MAX + 1U];
        if (UmiDecimalFormat(review.increment, increment, sizeof increment) == UMI_STATUS_OK)
            g_string_append_printf(
                text, "\nCurrent limit: %s with increment %s. The entered price was not changed.",
                review.aligned ? "aligned" : "NOT aligned", increment);
    }
    g_string_append(
        text, "\nThis is historical price metadata, not order approval. Lot sizes, account permissions, "
              "FOK/AON support and broker configuration require separate checks. Reconnect for fresh rules.");
    return g_string_free(text, FALSE);
}
static void MarketRulePublish(ConnectionUi *ui, uint64_t now)
{
    gchar *text = MarketRuleText(ui, now);
    if (strcmp(gtk_label_get_text(GTK_LABEL(ui->ruleOutput)), text) != 0)
        gtk_label_set_text(GTK_LABEL(ui->ruleOutput), text);
    g_free(text);
}
static void MarketRulePaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy || ui->ruleId == 0U)
        return;
    ui = FillEnter(ui->window);
    if (ui == NULL)
        return;
    MarketRulePublish(ui, now);
    FillLeave(ui);
}
static void MarketRuleReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiIbkrQuoteSnapshot quote;
    UmiStatus status = FillCurrentQuote(ui, now, &quote);
    UmiIbkrContractDetailsSnapshot *details = g_try_malloc(sizeof *details);
    if (details == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK && (!quote.subscribed || ui->contractRequest == 0U))
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = UmiIbkrContractDetailsCopy(ui->connection, ui->contractRequest, now, details);
    if (status == UMI_STATUS_OK &&
        (details->count != 1U || details->requestedContract.contractId != quote.contract.contractId ||
         strcmp(details->requestedContract.exchange, quote.contract.exchange) != 0))
        status = UMI_STATUS_INVALID_STATE;
    uint32_t rule = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiIbkrContractRouteRule(details, 0U, quote.contract.exchange, &rule);
    if (status == UMI_STATUS_OK)
        status = UmiIbkrMarketRuleRequest(ui->connection, rule, now);
    if (status == UMI_STATUS_OK || status == UMI_STATUS_BUSY)
    {
        ui->ruleId = rule;
        ui->ruleContract = quote.contract;
        ui->ruleQuoteRequest = quote.requestId;
        ui->ruleContractRequest = ui->contractRequest;
        g_free(details);
        MarketRulePublish(ui, now);
    }
    else
    {
        g_free(details);
        gtk_label_set_text(GTK_LABEL(ui->ruleOutput),
                           status == UMI_STATUS_TIMEOUT
                               ? "That rule request expired. Reconnect before requesting fresh price bands."
                               : "Price bands were not requested. Read one complete description for the "
                                 "current quote and route first.");
    }
    FillLeave(ui);
}
static void MarketRuleControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Route price bands and limit review");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    ui->ruleRead = gtk_button_new_with_label("Read current route's price bands");
    ui->ruleOutput =
        Text("Read a complete contract description before requesting route-specific price bands.");
    gtk_widget_set_name(ui->ruleRead, "ibkr-rule-read");
    gtk_widget_set_name(ui->ruleOutput, "ibkr-rule-output");
    gtk_label_set_selectable(GTK_LABEL(ui->ruleOutput), TRUE);
    gtk_box_append(GTK_BOX(box), ui->ruleRead);
    gtk_box_append(GTK_BOX(box), ui->ruleOutput);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->ruleRead, "clicked", G_CALLBACK(MarketRuleReadClicked), window, 0);
}
