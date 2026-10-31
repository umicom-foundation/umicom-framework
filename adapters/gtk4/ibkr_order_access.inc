/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_order_access.inc
 *
 * PURPOSE:
 *   Expose order observations through the existing native connection owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* These operations share the inspector's transport and request lifetime.
 * They request observations only; receiving an order does not grant mutation
 * authority over it. Native notifications may retire the retained window. */
UmiStatus UmiIbkrGtkCopyOrderReport(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrOrderReport *outReport)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || outReport == NULL ||
        maximumAgeMilliseconds == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrOrderReportCopy(ui->connection, UmiIbkrMonotonicMilliseconds(),
        maximumAgeMilliseconds, outReport);
}

UmiStatus UmiIbkrGtkRecoverOrders(GtkWindow *window, UmiIbkrOrderScope scope)
{
    if (window == NULL || !GTK_IS_WINDOW(window) ||
        (scope != UMI_IBKR_ORDERS_THIS_CLIENT && scope != UMI_IBKR_ORDERS_ALL_CLIENTS))
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrOrdersRequest(ui->connection, scope, now);
    if (status == UMI_STATUS_OK) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->ordersAll),
            scope == UMI_IBKR_ORDERS_ALL_CLIENTS);
        if (!ui->closed) OrdersPublish(ui, now);
    }
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
