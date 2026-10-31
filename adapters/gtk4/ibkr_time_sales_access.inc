/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_time_sales_access.inc
 *
 * PURPOSE:
 *   Share one native tick stream and its owned report through the broker connection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The inspector owns one visible tick stream. The core can retain several
 * independent streams; native views must not hide one that still needs a stop. */
UmiStatus UmiIbkrGtkStartTimeSales(GtkWindow *window, const UmiIbkrTimeSalesQuery *query)
{
    if (window == NULL || !GTK_IS_WINDOW(window) ||
        UmiIbkrTimeSalesQueryValidate(query) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    UmiIbkrTimeSalesQuery requested = *query;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UMI_STATUS_OK;
    if (ui->timeSalesRequest != 0U) {
        UmiIbkrTimeSalesSnapshot previous;
        status = UmiIbkrTimeSalesCopy(ui->connection, ui->timeSalesRequest, now, 15000U, &previous);
        if (status == UMI_STATUS_OK && previous.needsCancel) status = UMI_STATUS_ALREADY_EXISTS;
        else if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK)
        status = UmiIbkrTimeSalesRequest(ui->connection, &requested, now, &ui->timeSalesRequest);
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkStopTimeSales(GtkWindow *window, uint32_t expectedRequest)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->timeSalesRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    UmiStatus status = UmiIbkrTimeSalesCancel(ui->connection, expectedRequest, UmiIbkrMonotonicMilliseconds());
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCopyTimeSales(GtkWindow *window, uint64_t maximumAge, UmiIbkrTimeSalesReport *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || maximumAge == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->timeSalesRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrTimeSalesReportCopy(ui->connection, ui->timeSalesRequest,
        UmiIbkrMonotonicMilliseconds(), maximumAge, out);
}
