/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_completed.inc
 * PURPOSE: Show completed broker orders alongside explicitly scoped execution comparisons.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This panel consumes Framework copies. It does not reinterpret wire fields or
 * treat a completed-order record as permission to repeat the trade. */
static void CompletedComparisonText(ConnectionUi *ui, size_t index, uint64_t now, GString *text)
{
    if (ui->executionRequest == 0U)
    {
        g_string_append(text, "  Read executions for this account to compare captured fills.\n");
        return;
    }
    UmiIbkrCompletedExecutionReview review;
    UmiStatus status = UmiIbkrCompletedOrderReviewExecutions(ui->connection, index, ui->executionRequest, now,
                                                             15000U, &review);
    if (status != UMI_STATUS_OK)
    {
        g_string_append_printf(text, "  Execution comparison unavailable: %s.\n", umi_status_text(status));
        return;
    }
    if (review.quantity.correctionNeedsReview || review.quantity.unsupportedExecutionShape)
    {
        g_string_append(text,
                        "  Execution corrections or unsupported trade structure need separate review.\n");
        return;
    }
    char quantity[64], fee[64];
    if (UmiDecimalFormat(review.quantity.observedQuantity, quantity, sizeof quantity) != UMI_STATUS_OK)
        return;
    g_string_append_printf(text, "  %zu captured executions total %s; %s the completed filled quantity.\n",
                           review.quantity.executionCount, quantity,
                           review.quantity.equalsRequested  ? "equal to"
                           : review.quantity.belowRequested ? "below"
                                                            : "above");
    if (review.feeAvailable && UmiDecimalFormat(review.commission.amount, fee, sizeof fee) == UMI_STATUS_OK)
        g_string_append_printf(text, "  Captured commission: %s %s.\n", fee, review.commission.currency);
    else
        g_string_append_printf(text, "  Commission total unavailable: %s; missing fees are not zero.\n",
                               umi_status_text(review.commissionStatus));
}
static gchar *CompletedText(ConnectionUi *ui, uint64_t now)
{
    if (ui->connection == NULL)
        return g_strdup("No completed-order capture. Connect to TWS or Gateway first.");
    UmiIbkrCompletedOrdersSnapshot capture;
    if (UmiIbkrCompletedOrdersCopy(ui->connection, now, 15000U, &capture) != UMI_STATUS_OK)
        return g_strdup("Completed-order observations are unavailable.");
    GString *text = g_string_new(capture.requested ? capture.message : "No completed orders requested.");
    if (capture.requested)
    {
        g_string_append_printf(text, "\nScope: %s. %zu records. %s\n",
                               capture.apiOnly ? "API-origin orders visible to this login"
                                               : "all completed orders visible to this login",
                               capture.count,
                               capture.stale ? "STALE OR INCOMPLETE CAPTURE" : "Recent completed capture");
        UmiIbkrCompletedOrder row;
        for (size_t i = 0U; i < capture.count; ++i)
        {
            if (UmiIbkrCompletedOrderCopy(ui->connection, i, now, 15000U, &row) != UMI_STATUS_OK)
                continue;
            g_string_append_printf(
                text,
                "\n%s | permanent %" PRIu64 " | %s %s %s | %s %s %s\n"
                "  Order state: %s. Completed status: %s. Broker time: %s%s.\n"
                "  Original quantity %s; reported filled %s. Limit %s; %s.\n",
                row.order.account, row.permanentId, row.order.symbol, row.order.securityType,
                row.order.currency, row.order.action, row.order.orderType, row.order.timeInForce, row.status,
                row.completedStatus, row.completedTime[0] ? row.completedTime : "(not supplied)",
                row.stale ? " | old record" : "", row.order.totalQuantity.reportedText,
                row.filledQuantity.reportedText,
                row.order.limitPrice.reportedText[0] ? row.order.limitPrice.reportedText : "(unset)",
                row.allOrNone ? "AON field reported true" : "AON field reported false");
            CompletedComparisonText(ui, i, now, text);
        }
    }
    g_string_append(
        text,
        "\nCompleted history is limited to what this broker session returns. "
        "API-only does not mean this client only. Completion can include cancellation with partial fills. "
        "Matching executions do not prove a complete history or broker enforcement of FOK/AON. "
        "Reconnect for a new completed-order capture. This panel cannot place or change an order.");
    return g_string_free(text, FALSE);
}
static void CompletedPublish(ConnectionUi *ui, uint64_t now)
{
    gchar *text = CompletedText(ui, now);
    if (strcmp(gtk_label_get_text(GTK_LABEL(ui->completedOutput)), text))
        gtk_label_set_text(GTK_LABEL(ui->completedOutput), text);
    g_free(text);
}
static void CompletedPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy)
        return;
    ui = FillEnter(ui->window);
    if (!ui)
        return;
    /* Publishing can notify external observers which close the window. The
     * shared entry guard holds its owner until the final leave operation. */
    CompletedPublish(ui, now);
    FillLeave(ui);
}
static void CompletedReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    bool apiOnly = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->completedApiOnly));
    UmiStatus status = UmiIbkrCompletedOrdersRequest(ui->connection, apiOnly, now);
    if (status == UMI_STATUS_OK)
        CompletedPublish(ui, now);
    else
        gtk_label_set_text(GTK_LABEL(ui->completedOutput),
                           "Completed-order capture was not started. Connect first; reconnect if this "
                           "session already requested it.");
    FillLeave(ui);
}
static void CompletedControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Completed orders and execution review");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    ui->completedApiOnly =
        gtk_check_button_new_with_label("Only API-origin completed orders (all visible clients)");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->completedApiOnly), TRUE);
    ui->completedRead = gtk_button_new_with_label("Read completed orders");
    ui->completedOutput = Text("No completed-order capture. Connect to TWS or Gateway first.");
    gtk_widget_set_name(ui->completedApiOnly, "ibkr-completed-api-only");
    gtk_widget_set_name(ui->completedRead, "ibkr-completed-read");
    gtk_widget_set_name(ui->completedOutput, "ibkr-completed-output");
    gtk_label_set_selectable(GTK_LABEL(ui->completedOutput), TRUE);
    gtk_box_append(GTK_BOX(box), ui->completedApiOnly);
    gtk_box_append(GTK_BOX(box), ui->completedRead);
    gtk_box_append(GTK_BOX(box), ui->completedOutput);
    /* Export uses an owned CSV copy, so the worker never borrows this session. */
    CompletedExportControls(window, ui, box);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->completedRead, "clicked", G_CALLBACK(CompletedReadClicked), window, 0);
}
