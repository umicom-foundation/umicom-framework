/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_executions.inc
 * PURPOSE: Present account-scoped execution capture and an explicitly observational quantity comparison.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void ExecutionsFeeText(GString *text, const UmiIbkrExecutionFee *fee)
{
    if (!fee->received)
    {
        g_string_append(text, "  Commission: not received.\n");
        return;
    }
    if (fee->conflicting)
    {
        g_string_append_printf(text, "  Commission needs review: first %s %s; latest %s %s.\n",
                               fee->firstReportedAmount, fee->firstCurrency, fee->latestReportedAmount,
                               fee->latestCurrency);
        return;
    }
    g_string_append_printf(
        text, "  Reported commission: %s %s%s.\n", fee->firstReportedAmount, fee->firstCurrency,
        fee->exact ? "" : " (outside the supported exact decimal range; total unavailable)");
}
static gchar *ExecutionsText(const UmiIbkrExecutionSnapshot *copy)
{
    GString *text = g_string_new(copy->message);
    g_string_append_printf(
        text, "\nAccount %s; request %u; %zu reports; %" PRIu64 " exact replays ignored. %s%s\n",
        copy->account, (unsigned)copy->requestId, copy->count, copy->duplicateCount,
        copy->complete ? "End marker received. " : "Incomplete. ", copy->stale ? "STALE" : "");
    for (size_t i = 0U; i < copy->count; ++i)
    {
        const UmiIbkrExecutionObservation *row = &copy->rows[i];
        char quantity[64], price[64];
        UmiStatus a = UmiDecimalFormat(row->quantity, quantity, sizeof quantity);
        UmiStatus b = UmiDecimalFormat(row->price, price, sizeof price);
        if (a != UMI_STATUS_OK || b != UMI_STATUS_OK)
        {
            g_string_append(text, "Unrepresentable row.\n");
            continue;
        }
        g_string_append_printf(text,
                               "\n%s | %s | %s %s | %s @ %s %s\n"
                               "  permanent order %" PRIu64 "; contract %u; client %" PRId32
                               "; API order %" PRId32 "; %s; %s\n",
                               row->executionId, row->time, row->symbol, row->side, quantity, price,
                               row->currency, row->permanentOrderId, (unsigned)row->contractId, row->clientId,
                               row->orderId, row->securityType, row->exchange);
        ExecutionsFeeText(text, &copy->fees[i]);
    }
    g_string_append(text, "\nQuantity is per execution; cumulative quantity is not added again. "
                          "Commission reports may arrive separately. Missing fees are unavailable, not zero. "
                          "Full order status is not included in this capture.");
    return g_string_free(text, FALSE);
}
static bool ExecutionsInputId(GtkWidget *entry, uint64_t maximum, uint64_t *out)
{
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    if (*text == '\0')
        return false;
    uint64_t value = 0U;
    for (const char *p = text; *p != '\0'; ++p)
    {
        if (*p < '0' || *p > '9')
            return false;
        unsigned digit = (unsigned)(*p - '0');
        if (value > (maximum - digit) / 10U)
            return false;
        value = value * 10U + digit;
    }
    if (value == 0U)
        return false;
    *out = value;
    return true;
}
static gchar *ExecutionsReviewText(ConnectionUi *ui, const UmiIbkrExecutionSnapshot *copy)
{
    uint64_t permanent, contract;
    UmiDecimal requested;
    guint side = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->executionSide));
    if (!ExecutionsInputId(ui->executionPermanent, INT64_MAX, &permanent) ||
        !ExecutionsInputId(ui->executionContract, INT32_MAX, &contract) || side > 1U ||
        FillInputDecimal(ui->executionQuantity, &requested) != UMI_STATUS_OK)
        return g_strdup(
            "Enter a positive permanent order ID, contract ID and intended stock quantity to compare.");
    UmiIbkrExecutionQuantityReview review;
    UmiStatus status = UmiIbkrExecutionsReviewQuantity(copy, permanent, (uint32_t)contract,
                                                       side == 0U ? "BOT" : "SLD", requested, &review);
    if (status != UMI_STATUS_OK)
        return g_strdup_printf("Quantity comparison unavailable: %s. A complete current capture and matching "
                               "stock order are required.",
                               umi_status_text(status));
    if (review.correctionNeedsReview)
        return g_strdup("Related execution IDs need correction review. Both reports are retained; no total "
                        "is chosen from arrival order.");
    if (review.unsupportedExecutionShape)
        return g_strdup("This comparison supports ordinary positive-price stock executions. Other assets and "
                        "combo-style IDs remain visible above.");
    char quantity[64];
    if (UmiDecimalFormat(review.observedQuantity, quantity, sizeof quantity) != UMI_STATUS_OK)
        return g_strdup("Unable to format the observed quantity.");
    gchar *quantityText =
        g_strdup_printf("%zu execution reports total %s: %s the intended quantity. "
                        "This does not certify final order status, FOK/AON enforcement or commissions.",
                        review.executionCount, quantity,
                        review.equalsRequested  ? "equal to"
                        : review.belowRequested ? "below"
                                                : "above");

    UmiIbkrExecutionCommissionReview commission;
    UmiStatus feeStatus = UmiIbkrExecutionsReviewCommission(copy, permanent, (uint32_t)contract,
                                                            side == 0U ? "BOT" : "SLD", &commission);
    gchar *feeText;
    char feeAmount[64];
    if (feeStatus == UMI_STATUS_OK)
        feeStatus = UmiDecimalFormat(commission.amount, feeAmount, sizeof feeAmount);
    if (feeStatus == UMI_STATUS_OK)
        feeText = g_strdup_printf("\nCaptured commissions: %s %s across %zu reports.", feeAmount,
                                  commission.currency, commission.executionCount);
    else
        feeText = g_strdup_printf("\nCommission total unavailable: %s. Missing, mixed-currency or "
                                  "conflicting fees are not treated as zero.",
                                  umi_status_text(feeStatus));
    gchar *result = g_strconcat(quantityText, feeText, NULL);
    g_free(quantityText);
    g_free(feeText);
    return result;
}

static void ExecutionsPublish(ConnectionUi *ui, uint64_t now)
{
    UmiIbkrExecutionSnapshot *copy = NULL;
    gchar *details = NULL, *review = NULL;
    if (ui->connection == NULL || ui->executionRequest == 0U)
    {
        details =
            g_strdup("No execution capture. Connect, select an account, then read its recent executions.");
        review = g_strdup("A recent execution capture is required before comparison.");
    }
    else
    {
        copy = g_try_malloc(sizeof *copy);
        UmiStatus status = copy == NULL
                               ? UMI_STATUS_OUT_OF_MEMORY
                               : UmiIbkrExecutionsCopy(ui->connection, ui->executionRequest, now, copy);
        if (status == UMI_STATUS_OK)
        {
            details = ExecutionsText(copy);
            review = ExecutionsReviewText(ui, copy);
        }
        else
        {
            details = g_strdup(umi_status_text(status));
            review = g_strdup("Quantity comparison unavailable.");
        }
    }
    g_free(copy);
    /* All strings are owned before publication. Notification handlers may close
     * the window; never access another child after logical closure. */
    if (strcmp(gtk_label_get_text(GTK_LABEL(ui->executionOutput)), details) != 0)
        gtk_label_set_text(GTK_LABEL(ui->executionOutput), details);
    if (!ui->closed && strcmp(gtk_label_get_text(GTK_LABEL(ui->executionReviewOutput)), review) != 0)
        gtk_label_set_text(GTK_LABEL(ui->executionReviewOutput), review);
    g_free(details);
    g_free(review);
}
static void ExecutionsPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy)
        return;
    ui = FillEnter(ui->window);
    if (ui == NULL)
        return;
    ExecutionsPublish(ui, now);
    FillLeave(ui);
}
static void ExecutionsReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->accounts));
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    if (ui->connection != NULL && (size_t)index < ui->snapshot.accountCount)
        status = UmiIbkrExecutionsRequest(ui->connection, ui->snapshot.accounts[index], now,
                                          &ui->executionRequest);
    if (status == UMI_STATUS_OK)
        ExecutionsPublish(ui, now);
    else
        gtk_label_set_text(
            GTK_LABEL(ui->executionOutput),
            status == UMI_STATUS_BUSY
                ? "Execution capture is pending. Wait or abandon this local capture."
                : "Execution capture was not started. Connect and select an advertised account first.");
    FillLeave(ui);
}
static void ExecutionsAbandonClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (ui == NULL)
        return;
    if (ui->connection != NULL && ui->executionRequest != 0U)
        (void)UmiIbkrExecutionsAbandon(ui->connection, ui->executionRequest);
    ExecutionsPublish(ui, UmiIbkrMonotonicMilliseconds());
    FillLeave(ui);
}
static void ExecutionsInputChanged(GtkEditable *entry, gpointer data)
{
    (void)entry;
    ConnectionUi *ui = Owner(GTK_WINDOW(data));
    if (ui != NULL)
        ExecutionsPaint(ui, UmiIbkrMonotonicMilliseconds());
}
static void ExecutionsSideChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    ExecutionsInputChanged(NULL, data);
}
static void ExecutionsControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Recent executions and stock quantity comparison");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    ui->executionRead = gtk_button_new_with_label("Read selected account's recent executions");
    ui->executionAbandon = gtk_button_new_with_label("Abandon local execution capture");
    ui->executionOutput = Text("No execution capture. Connect and select an account first.");
    ui->executionReviewOutput =
        Text("Enter order identity and intended stock quantity after capturing executions.");
    ui->executionPermanent = gtk_entry_new();
    ui->executionContract = gtk_entry_new();
    ui->executionQuantity = gtk_entry_new();
    const char *sides[] = {"Bought (BOT)", "Sold (SLD)", NULL};
    ui->executionSide = gtk_drop_down_new_from_strings(sides);
    GtkWidget *entries[] = {ui->executionPermanent, ui->executionContract, ui->executionQuantity};
    const char *labels[] = {"Permanent order ID", "Contract ID", "Intended stock quantity"};
    const char *names[] = {"ibkr-execution-permanent", "ibkr-execution-contract", "ibkr-execution-quantity"};
    for (size_t i = 0U; i < G_N_ELEMENTS(entries); ++i)
    {
        gtk_entry_set_max_length(GTK_ENTRY(entries[i]), 96);
        gtk_widget_set_name(entries[i], names[i]);
        gtk_box_append(GTK_BOX(box), Text(labels[i]));
        gtk_box_append(GTK_BOX(box), entries[i]);
    }
    gtk_widget_set_name(ui->executionRead, "ibkr-execution-read");
    gtk_widget_set_name(ui->executionAbandon, "ibkr-execution-abandon");
    gtk_widget_set_name(ui->executionOutput, "ibkr-execution-output");
    gtk_widget_set_name(ui->executionReviewOutput, "ibkr-execution-review");
    gtk_widget_set_name(ui->executionSide, "ibkr-execution-side");
    gtk_label_set_selectable(GTK_LABEL(ui->executionOutput), TRUE);
    gtk_label_set_selectable(GTK_LABEL(ui->executionReviewOutput), TRUE);
    gtk_box_append(GTK_BOX(box), ui->executionSide);
    gtk_box_append(GTK_BOX(box), ui->executionRead);
    gtk_box_append(GTK_BOX(box), ui->executionAbandon);
    gtk_box_append(GTK_BOX(box), ui->executionOutput);
    gtk_box_append(GTK_BOX(box), ui->executionReviewOutput);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->executionRead, "clicked", G_CALLBACK(ExecutionsReadClicked), window, 0);
    g_signal_connect_object(ui->executionAbandon, "clicked", G_CALLBACK(ExecutionsAbandonClicked), window, 0);
    for (size_t i = 0U; i < G_N_ELEMENTS(entries); ++i)
        g_signal_connect_object(entries[i], "changed", G_CALLBACK(ExecutionsInputChanged), window, 0);
    g_signal_connect_object(ui->executionSide, "notify::selected", G_CALLBACK(ExecutionsSideChanged), window,
                            0);
}
