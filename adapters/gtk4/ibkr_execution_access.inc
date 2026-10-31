/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_execution_access.inc
 *
 * PURPOSE:
 *   Share account-scoped execution observations between native broker views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* All views share the inspector's capture and request namespace. Account
 * membership is checked by the core service before it queues an observation.
 * Copy performs no I/O. The expected request ID prevents a delayed Abandon
 * action from retiring a newer capture started in another view. */
UmiStatus UmiIbkrGtkCopyExecutions(GtkWindow *window, UmiIbkrExecutionSnapshot *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->executionRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrExecutionsCopy(ui->connection, ui->executionRequest,
        UmiIbkrMonotonicMilliseconds(), out);
}
UmiStatus UmiIbkrGtkReadExecutions(GtkWindow *window, const char *account)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || account == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrExecutionsRequest(ui->connection, account, now, &ui->executionRequest);
    if (status == UMI_STATUS_OK) ExecutionsPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkAbandonExecutions(GtkWindow *window, uint32_t expectedRequest)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->executionRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    UmiStatus status = UmiIbkrExecutionsAbandon(ui->connection, expectedRequest);
    if (status == UMI_STATUS_OK) ExecutionsPublish(ui, UmiIbkrMonotonicMilliseconds());
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
