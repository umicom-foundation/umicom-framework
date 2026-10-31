/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_history_access.inc
 *
 * PURPOSE:
 *   Share finite historical captures and owned chart scenes through the existing native broker session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The inspector owns the request namespace and transport. A workspace can
 * request a finite capture or copy retained evidence, but cannot replace that
 * owner. Expected IDs protect range reads and cancellation from stale controls. */
UmiStatus UmiIbkrGtkReadHistory(GtkWindow *window, const UmiIbkrHistoricalQuery *query)
{
    if (window == NULL || !GTK_IS_WINDOW(window) ||
        UmiIbkrHistoricalQueryValidate(query) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    /* Copy before publication can emit callbacks that edit the caller's fields. */
    UmiIbkrHistoricalQuery requested = *query;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrHistoricalRequest(ui->connection, &requested, now, &ui->historyRequest);
    if (status == UMI_STATUS_OK) HistoricalPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCancelHistory(GtkWindow *window, uint32_t expectedRequest)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->historyRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrHistoricalCancel(ui->connection, expectedRequest, now);
    if (status == UMI_STATUS_OK) HistoricalPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCopyHistory(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrHistoricalSnapshot *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || maximumAgeMilliseconds == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->historyRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrHistoricalCopy(ui->connection, ui->historyRequest,
        UmiIbkrMonotonicMilliseconds(), maximumAgeMilliseconds, out);
}
UmiStatus UmiIbkrGtkCopyHistoryBar(GtkWindow *window, uint32_t expectedRequest,
    size_t index, UmiIbkrHistoricalBar *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->historyRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrHistoricalBarCopy(ui->connection, expectedRequest, index, out);
}
UmiStatus UmiIbkrGtkCreateHistoryScene(GtkWindow *window, uint32_t expectedRequest,
    size_t firstBar, size_t barCount, uint64_t maximumAgeMilliseconds, UmiChartRenderScene **out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U || out == NULL ||
        maximumAgeMilliseconds == 0U || barCount == 0U || barCount > UMI_IBKR_HISTORICAL_BAR_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->historyRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
    return UmiIbkrHistoricalSceneCreate(ui->connection, expectedRequest, firstBar, barCount,
        UmiIbkrMonotonicMilliseconds(), maximumAgeMilliseconds, out);
#else
    (void)firstBar;
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}
