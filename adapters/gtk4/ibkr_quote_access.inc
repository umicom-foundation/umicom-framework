/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_quote_access.inc
 *
 * PURPOSE:
 *   Share one broker quote subscription between inspector controls and application workspaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* This adapter shares the inspector's selected stream. Keeping ownership here
 * prevents application panels from opening a second API client or reusing a
 * request from an earlier connection. Native publication can retire the owner. */
UmiStatus UmiIbkrGtkCopyQuote(GtkWindow *window, uint64_t maximumAgeMilliseconds,
    UmiIbkrQuoteSnapshot *outQuote)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || outQuote == NULL ||
        maximumAgeMilliseconds == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL || ui->quoteRequest == 0U) return UMI_STATUS_NOT_FOUND;
    return UmiIbkrQuoteCopy(ui->connection, ui->quoteRequest,
        UmiIbkrMonotonicMilliseconds(), maximumAgeMilliseconds, outQuote);
}

static UmiStatus QuotePublishResult(GtkWindow *window, UmiStatus status,
    const char *success)
{
    ConnectionUi *ui = Owner(window);
    if (ui != NULL) Render(ui, UmiIbkrMonotonicMilliseconds());
    ui = Owner(window);
    if (ui != NULL)
        gtk_label_set_text(GTK_LABEL(ui->quoteNotice),
            status == UMI_STATUS_OK ? success : umi_status_text(status));
    ui = Owner(window);
    if (ui != NULL) ui->quoteAccessBusy = FALSE;
    else status = UMI_STATUS_CANCELLED;
    return status;
}

/* Retain the real-time default for existing native callers. The explicit
 * variant shares the same owner, reentrancy guard and transport request. */
UmiStatus UmiIbkrGtkQuoteSubscribe(GtkWindow *window,
    const UmiIbkrQuoteContract *contract)
{
    return UmiIbkrGtkQuoteSubscribeWithDataType(window, contract, UMI_IBKR_DATA_REALTIME);
}

UmiStatus UmiIbkrGtkQuoteSubscribeWithDataType(GtkWindow *window,
    const UmiIbkrQuoteContract *contract, UmiIbkrMarketDataType requestedDataType)
{
    if (window == NULL || !GTK_IS_WINDOW(window) || contract == NULL ||
        requestedDataType < UMI_IBKR_DATA_REALTIME || requestedDataType > UMI_IBKR_DATA_DELAYED_FROZEN)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    /* A connection or report command may currently publish this same owner. */
#if 0
    if (ui->quoteAccessBusy) return UMI_STATUS_INVALID_STATE;
#endif
    if (ui->quoteAccessBusy || ui->commandBusy || ui->fillBusy) return UMI_STATUS_INVALID_STATE;
#ifdef UMI_IBKR_HAS_FILTERED_CHOICES
    if (PositionReviewBusy(ui)) return UMI_STATUS_INVALID_STATE;
#endif
    uint64_t now = UmiIbkrMonotonicMilliseconds();
    UmiIbkrQuoteSnapshot previous;
    if (ui->quoteRequest != 0U &&
        UmiIbkrQuoteCopy(ui->connection, ui->quoteRequest, now, 15000U, &previous) == UMI_STATUS_OK &&
        previous.subscribed) return UMI_STATUS_ALREADY_EXISTS;
    /* Copy before GTK emits any notifications, including when a caller passes
     * a contract borrowed from its own control model. */
    UmiIbkrQuoteContract selected = *contract;
    g_object_ref(window);
    ui->quoteAccessBusy = TRUE;
    /* Preserve the original call for comparison; explicit policy is now
     * carried into Framework rather than inferred from the visible quote. */
#if 0
    UmiStatus status = UmiIbkrQuoteSubscribe(ui->connection, &selected, now, &ui->quoteRequest);
#endif
    UmiStatus status = UmiIbkrQuoteSubscribeWithDataType(ui->connection, &selected,
        requestedDataType, now, &ui->quoteRequest);
    if (status == UMI_STATUS_OK) {
        ui->quoteSubscribed = TRUE;
        char identity[24];
        (void)g_snprintf(identity, sizeof(identity), "%" PRIu32, selected.contractId);
        gtk_editable_set_text(GTK_EDITABLE(ui->quoteContract), identity);
        ui = Owner(window);
        if (ui != NULL) gtk_editable_set_text(GTK_EDITABLE(ui->quoteExchange), selected.exchange);
        ui = Owner(window);
        if (ui != NULL) gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->quoteDataType),
            (guint)requestedDataType - 1U);
    }
    status = QuotePublishResult(window, status,
        "Subscribed data requested. The provider determines permissions and availability.");
    g_object_unref(window);
    return status;
}

UmiStatus UmiIbkrGtkQuoteCancel(GtkWindow *window)
{
    if (window == NULL || !GTK_IS_WINDOW(window)) return UMI_STATUS_INVALID_ARGUMENT;
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || !UmiGtk4WindowRegistered(window)) return UMI_STATUS_CANCELLED;
    if (ui->connection == NULL) return UMI_STATUS_UNAVAILABLE;
    if (ui->quoteRequest == 0U) return UMI_STATUS_NOT_FOUND;
    /* A connection or report command may currently publish this same owner. */
#if 0
    if (ui->quoteAccessBusy) return UMI_STATUS_INVALID_STATE;
#endif
    if (ui->quoteAccessBusy || ui->commandBusy || ui->fillBusy) return UMI_STATUS_INVALID_STATE;
    g_object_ref(window);
    ui->quoteAccessBusy = TRUE;
    UmiStatus status = UmiIbkrQuoteCancel(ui->connection, ui->quoteRequest);
    if (status == UMI_STATUS_OK) {
        FillWatchRetire(ui);
        ui->quoteSubscribed = FALSE;
    }
    status = QuotePublishResult(window, status,
        "Quote stream cancelled. Retained observations are stale.");
    g_object_unref(window);
    return status;
}
