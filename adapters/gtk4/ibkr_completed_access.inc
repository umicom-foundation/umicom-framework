/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_completed_access.inc
 *
 * PURPOSE:
 *   Share completed-order observations through the native connection owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Completed orders use the same inspector session as working orders, but have
 * a separate one-shot capture. The API-only filter does not mean this client
 * only, and observing a completed order never grants permission to resubmit. */
UmiStatus UmiIbkrGtkReadCompletedOrders(GtkWindow *window, bool apiOnly)
{
    if (window == NULL || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrCompletedOrdersRequest(ui->connection, apiOnly, now);
    if (status == UMI_STATUS_OK) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->completedApiOnly), apiOnly);
        if (!ui->closed) CompletedPublish(ui, now);
    }
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCopyCompletedOrders(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrCompletedOrderReport *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || out == NULL || maximumAgeMilliseconds == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrCompletedOrderReportCopy(ui->connection, UmiIbkrMonotonicMilliseconds(),
        maximumAgeMilliseconds, out);
}
