/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_depth_access.inc
 *
 * PURPOSE:
 *   Share one visible market-depth subscription through the native broker connection owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The native connection retains its one visible depth request. Workspace
 * controls share that identity with the inspector, instead of starting an
 * undisplayed parallel stream or abandoning a request when fields change. */
UmiStatus UmiIbkrGtkSubscribeDepth(GtkWindow *window, const UmiIbkrDepthSelection *selection)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || selection == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    UmiIbkrDepthSelection requested = *selection;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UMI_STATUS_OK;
    if (ui->depthRequest != 0U) {
        UmiIbkrDepthSnapshot *previous = g_try_new(UmiIbkrDepthSnapshot, 1);
        status = previous == NULL ? UMI_STATUS_OUT_OF_MEMORY :
            UmiIbkrDepthCopy(ui->connection, ui->depthRequest, now, 15000U, previous);
        if (status == UMI_STATUS_OK && previous->subscribed) status = UMI_STATUS_ALREADY_EXISTS;
        else if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_OK;
        g_free(previous);
    }
    if (status == UMI_STATUS_OK)
        status = UmiIbkrDepthSubscribe(ui->connection, &requested, now, &ui->depthRequest);
    if (status == UMI_STATUS_OK) DepthPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCancelDepth(GtkWindow *window, uint32_t expectedRequest)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->depthRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    UmiStatus status = UmiIbkrDepthCancel(ui->connection, expectedRequest);
    if (status == UMI_STATUS_OK) DepthPublish(ui, UmiIbkrMonotonicMilliseconds());
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCopyDepth(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrDepthSnapshot *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || maximumAgeMilliseconds == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->depthRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrDepthCopy(ui->connection, ui->depthRequest, UmiIbkrMonotonicMilliseconds(),
        maximumAgeMilliseconds, out);
}
