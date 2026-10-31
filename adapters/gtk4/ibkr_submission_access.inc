/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_submission_access.inc
 *
 * PURPOSE:
 *   Expose reviewed order execution through the existing native connection owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* All mutations share the inspector's command reservation. No native widget
 * stores a password, transport pointer or independently executable order. */
UmiStatus UmiIbkrGtkExecutionEnable(GtkWindow *window, const char *account,
    UmiTradingEnvironment mode, bool confirmed)
{
    if (!window || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = ConnectionCommandEnter(window);
    if (!ui) return UMI_STATUS_BUSY;
    UmiStatus result = ui->connection
        ? UmiIbkrExecutionEnable(ui->connection, account, mode, confirmed) : UMI_STATUS_UNAVAILABLE;
    ConnectionCommandLeave(ui); return result;
}
UmiStatus UmiIbkrGtkExecutionDisable(GtkWindow *window)
{
    if (!window || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = ConnectionCommandEnter(window);
    if (!ui) return UMI_STATUS_BUSY;
    UmiIbkrExecutionDisable(ui->connection);
    ConnectionCommandLeave(ui); return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkOrderPrepare(GtkWindow *window, const UmiIbkrOrderDraft *draft,
    UmiIbkrOrderReview *out)
{
    if (!window || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = ConnectionCommandEnter(window);
    if (!ui) return UMI_STATUS_BUSY;
    UmiStatus result = ui->connection ? UmiIbkrOrderPrepare(ui->connection, draft,
        UmiIbkrMonotonicMilliseconds(), out) : UMI_STATUS_UNAVAILABLE;
    ConnectionCommandLeave(ui); return result;
}
UmiStatus UmiIbkrGtkOrderSubmit(GtkWindow *window, uint64_t token, uint32_t *outId)
{
    if (!window || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = ConnectionCommandEnter(window);
    if (!ui) return UMI_STATUS_BUSY;
    UmiStatus result = ui->connection ? UmiIbkrOrderSubmit(ui->connection, token,
        UmiIbkrMonotonicMilliseconds(), outId) : UMI_STATUS_UNAVAILABLE;
    ConnectionCommandLeave(ui); return result;
}
UmiStatus UmiIbkrGtkOrderCancelSubmitted(GtkWindow *window, uint32_t id)
{
    if (!window || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = ConnectionCommandEnter(window);
    if (!ui) return UMI_STATUS_BUSY;
    UmiStatus result = ui->connection ? UmiIbkrOrderCancelSubmitted(ui->connection, id,
        UmiIbkrMonotonicMilliseconds()) : UMI_STATUS_UNAVAILABLE;
    ConnectionCommandLeave(ui); return result;
}
UmiStatus UmiIbkrGtkExecutionCopy(GtkWindow *window, UmiIbkrExecutionReport *out)
{
    if (!window || !GTK_IS_WINDOW(window) || !out) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (!ui || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    return ui->connection ? UmiIbkrExecutionCopy(ui->connection, out) : UMI_STATUS_NOT_FOUND;
}
/* Price-band acquisition is a read request. It never arms or submits a ticket. */
UmiStatus UmiIbkrGtkOrderReadPriceRule(GtkWindow *window, const char *exchange)
{
    if (!window || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = ConnectionCommandEnter(window);
    if (!ui) return UMI_STATUS_BUSY;
    UmiIbkrContractDetailsSnapshot *details = g_new0(UmiIbkrContractDetailsSnapshot, 1);
    uint64_t now = UmiIbkrMonotonicMilliseconds(); uint32_t rule = 0U;
    UmiStatus result = ui->connection ? UmiIbkrContractDetailsCopyLatestWithAge(
        ui->connection, now, 60000U, details) : UMI_STATUS_UNAVAILABLE;
    if (result == UMI_STATUS_OK) result = UmiIbkrContractRouteRule(details, 0U, exchange, &rule);
    if (result == UMI_STATUS_OK) result = UmiIbkrMarketRuleRequest(ui->connection, rule, now);
    g_free(details); ConnectionCommandLeave(ui); return result;
}
