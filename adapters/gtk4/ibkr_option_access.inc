/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_option_access.inc
 *
 * PURPOSE:
 *   Share option discovery and exact candidate resolution through one native connection owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Capture input before native notifications can edit a form. All request IDs
 * belong to the core connection; form fields are never ownership evidence. */
UmiStatus UmiIbkrGtkReadOptionChains(GtkWindow *window, const UmiIbkrOptionChainQuery *query)
{
    if (window == NULL || !GTK_IS_WINDOW(window) ||
        UmiIbkrOptionChainQueryValidate(query) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    UmiIbkrOptionChainQuery requested = *query;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrOptionChainRequest(ui->connection, &requested, now, &ui->optionRequest);
    if (status == UMI_STATUS_OK) {
        ui->optionDisplayedRequest = 0U;
        /* Do not discard a metadata request made through a different form. */
        OptionChainPublish(ui, now);
    }
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui); return status;
}
UmiStatus UmiIbkrGtkCopyOptionChains(GtkWindow *window, uint64_t maximumAge, UmiIbkrOptionChainSnapshot *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || maximumAge == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrOptionChainCopyLatestWithAge(ui->connection, UmiIbkrMonotonicMilliseconds(), maximumAge, out);
}
UmiStatus UmiIbkrGtkCopyOptionChain(GtkWindow *window, uint32_t expectedRequest, size_t index,
    UmiIbkrOptionChain *out)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrOptionChainItemCopyChecked(ui->connection, expectedRequest, index, out);
}
UmiStatus UmiIbkrGtkAbandonOptionChains(GtkWindow *window, uint32_t expectedRequest)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || expectedRequest == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    UmiStatus status = UmiIbkrOptionChainAbandon(ui->connection, expectedRequest);
    if (status == UMI_STATUS_OK) OptionChainPublish(ui, UmiIbkrMonotonicMilliseconds());
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui); return status;
}
UmiStatus UmiIbkrGtkResolveOption(GtkWindow *window, const UmiIbkrOptionSelection *selection, uint64_t maximumAge)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || selection == NULL || maximumAge == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_NOT_FOUND;
    UmiIbkrOptionSelection requested = *selection;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrOptionContractRequestWithAge(ui->connection, &requested,
        now, maximumAge, &ui->optionContractRequest);
    if (status == UMI_STATUS_OK) OptionChainPublish(ui, now);
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui); return status;
}
