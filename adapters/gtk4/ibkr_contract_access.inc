/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_contract_access.inc
 *
 * PURPOSE:
 *   Expose exact instrument descriptions through the existing native broker connection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The inspector owns both the request namespace and socket. Exact contract
 * lookup does not require a market-data subscription or change an active quote.
 * The expected request protects abandonment from a replaced inspector lookup. */
UmiStatus UmiIbkrGtkReadContractDetails(GtkWindow *window, const UmiIbkrQuoteContract *contract)
{
    if (window == NULL || !GTK_IS_WINDOW(window) ||
        contract == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    UmiIbkrQuoteContract requested = *contract;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrContractDetailsRequest(ui->connection, &requested, now, &ui->contractRequest);
    if (status == UMI_STATUS_OK) ContractDetailsPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
UmiStatus UmiIbkrGtkCopyContractDetails(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrContractDetailsSnapshot *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || out == NULL || maximumAgeMilliseconds == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    /* The connection owns one metadata capture shared by exact-ID and option
     * requests. Preserve the earlier form-local lookup for review; it hid
     * results requested from the inspector option controls. */
#if 0
    if (ui->connection == NULL || ui->contractRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrContractDetailsCopyWithAge(ui->connection, ui->contractRequest,
        UmiIbkrMonotonicMilliseconds(), maximumAgeMilliseconds, out);
#endif
    if (ui->connection == NULL) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrContractDetailsCopyLatestWithAge(ui->connection,
        UmiIbkrMonotonicMilliseconds(), maximumAgeMilliseconds, out);
}
UmiStatus UmiIbkrGtkAbandonContractDetails(GtkWindow *window, uint32_t expectedRequest)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    /* The core checks expectedRequest against the actual shared owner. The
     * previous form-local check is retained because option requests exposed
     * why presentation state must not decide request ownership. */
#if 0
    if (ui->connection == NULL || ui->contractRequest != expectedRequest) return UMI_STATUS_NOT_FOUND;
#endif
    if (ui->connection == NULL) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    UmiStatus status = UmiIbkrContractDetailsAbandon(ui->connection, expectedRequest);
    if (status == UMI_STATUS_OK) ContractDetailsPublish(ui, UmiIbkrMonotonicMilliseconds());
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
