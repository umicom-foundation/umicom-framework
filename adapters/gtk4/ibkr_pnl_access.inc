/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_pnl_access.inc
 *
 * PURPOSE:
 *   Expose the shared account and position profit-and-loss stream with expected-request cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The connection's visible P&L stream has one identity shared by inspector and
 * workspace. Selection is copied before publishing native notifications. */
UmiStatus UmiIbkrGtkSubscribePnl(GtkWindow *window, const UmiIbkrPnlSelection *selection)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || selection == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    UmiIbkrPnlSelection requested = *selection;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UMI_STATUS_OK;
    if (ui->pnlRequest != 0U) {
        UmiIbkrPnlSnapshot previous;
        status = UmiIbkrPnlCopy(ui->connection, ui->pnlRequest, now, 15000U, &previous);
        if (status == UMI_STATUS_OK && previous.subscribed) status = UMI_STATUS_ALREADY_EXISTS;
        else if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK)
        status = UmiIbkrPnlSubscribe(ui->connection, &requested, now, &ui->pnlRequest);
    if (status == UMI_STATUS_OK) PnlPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCancelPnl(GtkWindow *window, uint32_t expectedRequest)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->pnlRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    UmiStatus status = UmiIbkrPnlCancel(ui->connection, expectedRequest);
    if (status == UMI_STATUS_OK) PnlPublish(ui, UmiIbkrMonotonicMilliseconds());
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCopyPnl(GtkWindow *window, uint64_t maximumAgeMilliseconds, UmiIbkrPnlSnapshot *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || maximumAgeMilliseconds == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->pnlRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrPnlCopy(ui->connection, ui->pnlRequest, UmiIbkrMonotonicMilliseconds(),
        maximumAgeMilliseconds, out);
}
