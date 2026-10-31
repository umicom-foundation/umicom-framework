/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_search_access.inc
 *
 * PURPOSE:
 *   Expose instrument discovery through the existing native broker session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* The inspector retains transport ownership. Its existing publication guard
 * serializes search commands from the inspector and embedded workspaces. */
UmiStatus UmiIbkrGtkCopyInstrumentSearch(GtkWindow *window,
    UmiIbkrSymbolSearchSnapshot *outSearch)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || outSearch == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->symbolSearchRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrSymbolSearchCopy(ui->connection, ui->symbolSearchRequest,
        UmiIbkrMonotonicMilliseconds(), outSearch);
}

UmiStatus UmiIbkrGtkSearchInstruments(GtkWindow *window, const char *pattern)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || pattern == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < 128U && pattern[length] != '\0') ++length;
    if (length == 0U || length == 128U) return UMI_STATUS_INVALID_ARGUMENT;
    char owned[128];
    memcpy(owned, pattern, length + 1U);
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiStatus status = UmiIbkrSymbolSearchRequest(ui->connection, owned, now, &ui->symbolSearchRequest);
    if (status == UMI_STATUS_OK) {
        ui->symbolSearchDisplayed = 0U;
        gtk_drop_down_set_model(GTK_DROP_DOWN(ui->symbolSearchResults), NULL);
        if (!ui->closed) gtk_editable_set_text(GTK_EDITABLE(ui->symbolSearchPattern), owned);
        if (!ui->closed) SymbolSearchPublish(ui, now);
    }
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}

UmiStatus UmiIbkrGtkAbandonInstrumentSearch(GtkWindow *window)
{
    if (window == NULL || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    if (ui->symbolSearchRequest == 0U) return UMI_STATUS_NOT_FOUND;
    ui = FillEnter(window);
    if (ui == NULL) return UMI_STATUS_BUSY;
    UmiStatus status = UmiIbkrSymbolSearchAbandon(ui->connection, ui->symbolSearchRequest);
    if (status == UMI_STATUS_OK) SymbolSearchPublish(ui, UmiIbkrMonotonicMilliseconds());
    if (ui->closed) status = UMI_STATUS_CANCELLED;
    FillLeave(ui);
    return status;
}
