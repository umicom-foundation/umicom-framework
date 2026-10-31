/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_scanner_access.inc
 *
 * PURPOSE:
 *   Share scanner subscription ownership and generation-pinned results with native workspaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The inspector remains the connection owner. Workspace requests use its
 * scanner slot and pacing rather than opening another socket or hiding a scan. */
UmiStatus UmiIbkrGtkStartScanner(GtkWindow *window, const UmiIbkrScannerQuery *query)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || query == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiIbkrScannerQueryValidate(query);
    if (status != UMI_STATUS_OK) return status;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    UmiIbkrScannerQuery requested = *query;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    if (ui->scannerRequest != 0U) {
        UmiIbkrScannerSnapshot previous;
        status = UmiIbkrScannerCopy(ui->connection, ui->scannerRequest, now, 60000U, &previous);
        if (status == UMI_STATUS_OK && previous.needsCancel) status = UMI_STATUS_ALREADY_EXISTS;
        else if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_OK;
    }
    if (status == UMI_STATUS_OK)
        status = UmiIbkrScannerRequest(ui->connection, &requested, now, &ui->scannerRequest);
    if (status == UMI_STATUS_OK) ScannerPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCancelScanner(GtkWindow *window, uint32_t expectedRequest)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->scannerRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrScannerCancel(ui->connection, expectedRequest, now);
    if (status == UMI_STATUS_OK) ScannerPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCopyScanner(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrScannerSnapshot *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || maximumAgeMilliseconds == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->scannerRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrScannerCopy(ui->connection, ui->scannerRequest, UmiIbkrMonotonicMilliseconds(),
        maximumAgeMilliseconds, out);
}
UmiStatus UmiIbkrGtkCopyScannerRow(GtkWindow *window, uint32_t expectedRequest,
    uint64_t generation, size_t index, UmiIbkrScannerRow *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U || generation == 0U ||
        index >= UMI_IBKR_SCANNER_ROW_LIMIT || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->scannerRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrScannerRowCopy(ui->connection, expectedRequest, generation, index, out);
}
