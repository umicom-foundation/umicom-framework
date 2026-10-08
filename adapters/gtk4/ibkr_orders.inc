/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_orders.inc
 * PURPOSE: Present recovered broker orders with explicit identity, scope and observation age.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* The broker window borrows no row after publishing a label. The Framework
 * owns decoding and lifetime; this panel only asks for copied observations. */
static gchar *OrdersText(ConnectionUi *ui, uint64_t now)
{
    if (ui->connection == NULL)
        return g_strdup("No order recovery. Connect to TWS or Gateway first.");
    UmiIbkrOrderRecoverySnapshot capture;
    UmiIbkrOrderIdentitySnapshot identity;
    if (UmiIbkrOrdersCopy(ui->connection, now, 15000U, &capture) != UMI_STATUS_OK ||
        UmiIbkrOrderIdentityCopy(ui->connection, &identity) != UMI_STATUS_OK)
        return g_strdup("Order observations are unavailable.");
    GString *text = g_string_new(capture.requested ? capture.message : "No open-order recovery requested.");
    if (identity.brokerIdReceived)
    {
        g_string_append_printf(text, "\nBroker next ID: %u. Conservative next-ID hint: %u%s%s.",
                               (unsigned)identity.brokerNextOrderId,
                               (unsigned)identity.conservativeNextOrderId,
                               identity.exhausted ? " (EXHAUSTED)" : "", identity.stale ? " (STALE)" : "");
        g_string_append(text, "\nThis hint does not reserve an ID or permit order submission.");
    }
    if (capture.requested)
    {
        g_string_append_printf(
            text, "\nScope: %s. %zu retained rows. %s\n",
            capture.scope == UMI_IBKR_ORDERS_ALL_CLIENTS ? "all visible clients/accounts" : "this API client",
            capture.count, capture.stale ? "STALE OR INCOMPLETE SNAPSHOT" : "Recent completed snapshot");
        UmiIbkrRecoveredOrder row;
        for (size_t i = 0U; i < capture.count; ++i)
        {
            if (UmiIbkrOrderCopy(ui->connection, i, now, 15000U, &row) != UMI_STATUS_OK)
                continue;
            g_string_append_printf(text, "\nClient %" PRId32 " / order %" PRId32 " / permanent %" PRIu64 "\n",
                                   row.key.clientId, row.key.orderId, row.permanentId);
            if (row.hasOpenOrder)
                g_string_append_printf(
                    text, "%s | %s %s %s | %s %s @ %s | limit %s | %s%s\n",
                    row.open.account[0] ? row.open.account : "(account unavailable)", row.open.symbol,
                    row.open.securityType, row.open.currency, row.open.action,
                    row.open.totalQuantity.reportedText, row.open.orderType,
                    row.open.limitPrice.reportedText[0] ? row.open.limitPrice.reportedText : "(unset)",
                    row.open.timeInForce, row.openStale ? " | old summary" : "");
            else
                g_string_append(text, "Status only: account and instrument have not been received.\n");
            if (row.hasStatus)
                g_string_append_printf(
                    text,
                    "%s | cumulative filled %s | remaining %s | average %s%s\n"
                    "Held: %s | duplicate statuses: %" PRIu64 "\n",
                    row.status.status, row.status.filled.reportedText, row.status.remaining.reportedText,
                    row.status.averageFillPrice.reportedText, row.statusStale ? " | old status" : "",
                    row.status.whyHeld[0] ? row.status.whyHeld : "(none reported)", row.duplicateStatuses);
            else
                g_string_append(text, "No order-status callback received.\n");
        }
    }
    g_string_append(
        text, "\nRecovery is a point-in-time view, not an all-orders subscription. "
              "Reconnect to request another snapshot. Check executions separately for fills and fees. "
              "The contract/order prefix is displayed; advanced fields are retained but not interpreted. "
              "This window cannot submit, bind, modify or cancel orders.");
    return g_string_free(text, FALSE);
}
static void OrdersPublish(ConnectionUi *ui, uint64_t now)
{
    gchar *text = OrdersText(ui, now);
    if (strcmp(gtk_label_get_text(GTK_LABEL(ui->ordersOutput)), text))
        gtk_label_set_text(GTK_LABEL(ui->ordersOutput), text);
    g_free(text);
}
static void OrdersPaint(ConnectionUi *ui, uint64_t now)
{
    if (ui->closed || ui->fillBusy)
        return;
    ui = FillEnter(ui->window);
    if (!ui)
        return;
    OrdersPublish(ui, now);
    FillLeave(ui);
}
static void OrdersReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    ConnectionUi *ui = FillEnter(GTK_WINDOW(data));
    if (!ui)
        return;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiIbkrOrderScope scope = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->ordersAll))
                                  ? UMI_IBKR_ORDERS_ALL_CLIENTS
                                  : UMI_IBKR_ORDERS_THIS_CLIENT;
    UmiStatus status = UmiIbkrOrdersRequest(ui->connection, scope, now);
    if (status == UMI_STATUS_OK)
        OrdersPublish(ui, now);
    else
        gtk_label_set_text(GTK_LABEL(ui->ordersOutput),
                           "Order recovery was not started. Connect first; reconnect if this session already "
                           "requested a snapshot.");
    FillLeave(ui);
}
static void OrdersControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    GtkWidget *section = gtk_expander_new("Open orders and broker identity");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_expander_set_child(GTK_EXPANDER(section), box);
    ui->ordersAll = gtk_check_button_new_with_label("Include orders from all visible clients/accounts");
    ui->ordersRead = gtk_button_new_with_label("Recover open orders");
    ui->ordersOutput = Text("No order recovery. Connect to TWS or Gateway first.");
    gtk_widget_set_name(ui->ordersAll, "ibkr-orders-all");
    gtk_widget_set_name(ui->ordersRead, "ibkr-orders-read");
    gtk_widget_set_name(ui->ordersOutput, "ibkr-orders-output");
    gtk_label_set_selectable(GTK_LABEL(ui->ordersOutput), TRUE);
    gtk_box_append(GTK_BOX(box), ui->ordersAll);
    gtk_box_append(GTK_BOX(box), ui->ordersRead);
    gtk_box_append(GTK_BOX(box), ui->ordersOutput);
    gtk_box_append(GTK_BOX(root), section);
    g_signal_connect_object(ui->ordersRead, "clicked", G_CALLBACK(OrdersReadClicked), window, 0);
}
