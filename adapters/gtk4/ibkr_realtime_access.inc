/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_realtime_access.inc
 *
 * PURPOSE:
 *   Share five-second broker streams without transferring session ownership or replacing active requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

UmiStatus UmiIbkrGtkStartRealtime(GtkWindow *window, const UmiIbkrRealtimeQuery *query)
{
    if (window == NULL || !GTK_IS_WINDOW(window) ||
        UmiIbkrRealtimeQueryValidate(query) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    /* Copy before publication can emit callbacks that edit the caller's fields. */
    UmiIbkrRealtimeQuery requested = *query;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    /* The core permits several streams, but this inspector owns one visible
     * slot. Refuse replacement until its previous subscription is cancelled. */
    UmiIbkrRealtimeSnapshot previous;
    UmiStatus status = ui->streamRequest != 0U &&
        UmiIbkrRealtimeCopy(ui->connection, ui->streamRequest, now, 15000U, &previous) == UMI_STATUS_OK &&
        previous.needsCancel ? UMI_STATUS_BUSY :
            UmiIbkrRealtimeRequest(ui->connection, &requested, now, &ui->streamRequest);
    if (status == UMI_STATUS_OK) StreamingPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkStopRealtime(GtkWindow *window, uint32_t expectedRequest)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->streamRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrRealtimeCancel(ui->connection, expectedRequest, now);
    if (status == UMI_STATUS_OK) StreamingPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCopyRealtime(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrRealtimeSnapshot *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || maximumAgeMilliseconds == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->streamRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrRealtimeCopy(ui->connection, ui->streamRequest,
        UmiIbkrMonotonicMilliseconds(), maximumAgeMilliseconds, out);
}
UmiStatus UmiIbkrGtkCopyRealtimeBar(GtkWindow *window, uint32_t expectedRequest,
    size_t index, UmiIbkrRealtimeBar *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->streamRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrRealtimeBarCopy(ui->connection, expectedRequest, index, out);
}
UmiStatus UmiIbkrGtkCreateRealtimeScene(GtkWindow *window, uint32_t expectedRequest,
    size_t firstBar, size_t barCount, uint64_t maximumAgeMilliseconds, UmiChartRenderScene **out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U || out == NULL ||
        maximumAgeMilliseconds == 0U || barCount == 0U || barCount > UMI_IBKR_REALTIME_BAR_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->streamRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
    return UmiIbkrRealtimeSceneCreate(ui->connection, expectedRequest, firstBar, barCount,
        UmiIbkrMonotonicMilliseconds(), maximumAgeMilliseconds, out);
#else
    (void)firstBar;
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}
