/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/workspace_details_projection.inc
 *
 * PURPOSE:
 *   Verify native instrument lookup identity, partial metadata and safe retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static UmiIbkrContractDetailsSnapshot details;
static gboolean detailsAvailable;
static unsigned detailsReads, detailsAbandons;
static UmiStatus detailsReadStatus = UMI_STATUS_OK, detailsCopyStatus = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkReadContractDetails(GtkWindow *window, const UmiIbkrQuoteContract *contract)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (detailsReadStatus != UMI_STATUS_OK) return detailsReadStatus;
    if (detailsAvailable && !details.complete && !details.failed) return UMI_STATUS_BUSY;
    memset(&details, 0, sizeof(details)); detailsAvailable = TRUE; ++detailsReads;
    details.requestId = 700U + detailsReads; details.requestedContract = *contract;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCopyContractDetails(GtkWindow *window, uint64_t age, UmiIbkrContractDetailsSnapshot *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (age != 60000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (detailsCopyStatus != UMI_STATUS_OK) return detailsCopyStatus;
    if (!detailsAvailable) return UMI_STATUS_NOT_FOUND;
    *out = details; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkAbandonContractDetails(GtkWindow *window, uint32_t expected)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!detailsAvailable || expected != details.requestId) return UMI_STATUS_NOT_FOUND;
    ++detailsAbandons; details.failed = details.stale = true; return UMI_STATUS_OK;
}
static void DetailsRows(void)
{
    details.count = 1U; details.complete = true;
    UmiIbkrContractDescription *row = &details.items[0]; row->contractId = 123U;
    strcpy(row->longName, "Example instrument"); strcpy(row->symbol, "EXAMPLE"); strcpy(row->exchange, "SMART");
    strcpy(row->currency, "GBP"); strcpy(row->minimumSize, "9007199254740993.123456789");
    strcpy(row->minimumTick, "0.000000001"); strcpy(row->marketRuleIds, "26,27");
}
static void DetailsChangeIntent(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    gtk_editable_set_text(GTK_EDITABLE(data), "456");
}
static int DetailsProjection(const char *test, GtkWidget *root, UmiBrokerWorkspaceGtk **workspace)
{
    GtkWidget *read = Find(root, "broker-workspace-details-read");
    GtkWidget *abandon = Find(root, "broker-workspace-details-abandon");
    GtkWidget *useQuote = Find(root, "broker-workspace-details-quote");
    GtkWidget *contract = Find(root, "broker-workspace-details-contract");
    GtkWidget *exchange = Find(root, "broker-workspace-details-exchange");
    GtkWidget *status = Find(root, "broker-workspace-details-status");
    GtkWidget *notice = Find(root, "broker-workspace-details-notice");
    GtkWidget *table = Find(root, "broker-workspace-details-table");
    CHECK(read != NULL && table != NULL && useQuote != NULL);
    CHECK(gtk_widget_get_sensitive(read) && !gtk_widget_get_sensitive(abandon));
    CHECK(!gtk_widget_get_sensitive(useQuote));
    if (strcmp(test, "details-retained") == 0) {
        UmiBrokerWorkspaceGtkDestroy(*workspace); *workspace = NULL;
        g_signal_emit_by_name(read, "clicked"); g_signal_emit_by_name(abandon, "clicked");
        g_signal_emit_by_name(useQuote, "clicked");
        CHECK(detailsReads == 0U && detailsAbandons == 0U); return 0;
    }
    if (strcmp(test, "details-quote") == 0) {
        quoteAvailable = TRUE; quote.contract.contractId = 123U; strcpy(quote.contract.exchange, "SMART");
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
        CHECK(gtk_widget_get_sensitive(useQuote));
        g_signal_emit_by_name(useQuote, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(contract)), "123") == 0);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(exchange)), "SMART") == 0);
        CHECK(detailsReads == 0U && subscriptions == 0U); return 0;
    }
    gtk_editable_set_text(GTK_EDITABLE(contract), "123");
    gtk_editable_set_text(GTK_EDITABLE(exchange), "SMART");
    if (strcmp(test, "details-invalid") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(contract), "0");
        g_signal_emit_by_name(read, "clicked");
        CHECK(detailsReads == 0U && Contains(notice, "positive contract")); return 0;
    }
    if (strcmp(test, "details-rejected") == 0) {
        detailsReadStatus = UMI_STATUS_CAPACITY_EXCEEDED; g_signal_emit_by_name(read, "clicked");
        CHECK(detailsReads == 0U && gtk_widget_get_sensitive(read)); return 0;
    }
    if (strcmp(test, "details-external") == 0) {
        detailsAvailable = TRUE; details.requestId = 699U; details.requestedContract.contractId = 456U;
        g_signal_emit_by_name(read, "clicked");
        CHECK(detailsReads == 0U && !gtk_widget_get_sensitive(read) && Contains(status, "456"));
        return 0;
    }
    if (strcmp(test, "details-intent") == 0)
        g_signal_connect(read, "notify::sensitive", G_CALLBACK(DetailsChangeIntent), contract);
    g_signal_emit_by_name(read, "clicked");
    g_signal_handlers_disconnect_by_data(read, contract);
    CHECK(detailsReads == 1U && details.requestedContract.contractId == 123U);
    CHECK(strcmp(details.requestedContract.exchange, "SMART") == 0);
    CHECK(subscriptions == 0U && cancellations == 0U);
    CHECK(!gtk_widget_get_sensitive(read) && gtk_widget_get_sensitive(abandon));
    g_signal_emit_by_name(read, "clicked"); CHECK(detailsReads == 1U);
    if (strcmp(test, "details-abandon") == 0 || strcmp(test, "details-replaced-abandon") == 0) {
        if (strcmp(test, "details-replaced-abandon") == 0) ++details.requestId;
        g_signal_emit_by_name(abandon, "clicked");
        CHECK(detailsAbandons == (strcmp(test, "details-abandon") == 0 ? 1U : 0U));
        CHECK(cancellations == 0U);
        if (detailsAbandons == 0U) CHECK(Contains(notice, "replaced"));
        else CHECK(gtk_widget_get_sensitive(read) && Contains(status, "Failed"));
        return 0;
    }
    DetailsRows();
    if (strcmp(test, "details-multiple") == 0) {
        details.count = 2U; details.items[1] = details.items[0];
        strcpy(details.items[1].exchange, "LSE"); strcpy(details.items[1].longName, "Second route");
    }
    if (strcmp(test, "details-partial") == 0) details.complete = false;
    if (strcmp(test, "details-empty") == 0) details.count = 0U;
    if (strcmp(test, "details-failed") == 0) { details.complete = false; details.failed = details.stale = true; }
    if (strcmp(test, "details-stale") == 0) details.stale = true;
    if (strcmp(test, "details-option") == 0) {
        details.optionLookup = true; details.requestedContract.contractId = 0U;
        strcpy(details.requestedOption.symbol, "OPTION-EXAMPLE"); strcpy(details.requestedOption.expiry, "20270101");
        strcpy(details.requestedOption.right, "C"); strcpy(details.requestedOption.strike, "100");
        strcpy(details.requestedOption.exchange, "SMART");
    }
    if (strcmp(test, "details-utf8") == 0) {
        memcpy(details.items[0].longName, "Broken \xff", sizeof("Broken \xff"));
        memcpy(details.message, "Message \xff", sizeof("Message \xff"));
    }
    if (strcmp(test, "details-reentrant") == 0)
        g_signal_connect(status, "notify::label", G_CALLBACK(CloseDuringRender), workspace);
    UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(*workspace);
    if (strcmp(test, "details-reentrant") == 0) {
        CHECK(refreshed == UMI_STATUS_CANCELLED && *workspace == NULL);
        g_signal_handlers_disconnect_by_data(status, workspace);
        g_signal_emit_by_name(read, "clicked"); CHECK(detailsReads == 1U); return 0;
    }
    CHECK(refreshed == UMI_STATUS_OK);
    if (strcmp(test, "details-empty") == 0) CHECK(Contains(table, "Completed lookup contains no"));
    else {
        CHECK(Contains(table, "0.000000001") && Contains(table, "9007199254740993.123456789"));
        CHECK(Contains(table, "Not reported") && Contains(table, "26,27"));
    }
    if (strcmp(test, "details-multiple") == 0) CHECK(Contains(table, "Second route") && Contains(table, "LSE"));
    if (strcmp(test, "details-partial") == 0) CHECK(Contains(status, "Receiving") && !gtk_widget_get_sensitive(read));
    if (strcmp(test, "details-failed") == 0) CHECK(Contains(status, "Failed") && gtk_widget_get_sensitive(read));
    if (strcmp(test, "details-stale") == 0) CHECK(Contains(status, "STALE"));
    if (strcmp(test, "details-option") == 0) CHECK(Contains(status, "Option lookup: OPTION-EXAMPLE"));
    if (strcmp(test, "details-utf8") == 0) {
        CHECK(Contains(table, "Broken \xef\xbf\xbd"));
        CHECK(g_utf8_validate(gtk_label_get_text(GTK_LABEL(status)), -1, NULL));
        CHECK((unsigned char)details.items[0].longName[7] == 0xffU);
    }
    if (strcmp(test, "details-copy-failure") == 0) {
        detailsCopyStatus = UMI_STATUS_BUSY;
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
        CHECK(Contains(status, "STALE") && Contains(table, "0.000000001"));
    }
    if (strcmp(test, "details-disconnect") == 0 || strcmp(test, "details-reconnect") == 0) {
        gtk_window_destroy(inspector);
        CHECK(Contains(status, "STALE") && !gtk_widget_get_sensitive(read));
        if (strcmp(test, "details-reconnect") == 0) {
            detailsAvailable = FALSE;
            CHECK(UmiBrokerWorkspaceGtkOpenConnection(*workspace) == UMI_STATUS_OK);
            CHECK(!Contains(table, "0.000000001") && gtk_widget_get_sensitive(read));
        }
    }
    return 0;
}
