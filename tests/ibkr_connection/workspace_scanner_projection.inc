/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/workspace_scanner_projection.inc
 *
 * PURPOSE:
 *   Verify scanner request ownership, refreshed-row identity, retained observations and native retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Replace only broker I/O: real GTK widgets, selection notifications, request
 * capture and controller retirement execute against synthetic scanner data. */
static UmiIbkrScannerSnapshot scan;
static UmiIbkrScannerRow scanRows[UMI_IBKR_SCANNER_ROW_LIMIT];
static gboolean scanAvailable;
static unsigned scanStarts, scanStops;
static UmiStatus scanStartStatus = UMI_STATUS_OK, scanStopStatus = UMI_STATUS_OK;
static UmiStatus scanRowStatus = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkCopyScanner(GtkWindow *window, uint64_t age, UmiIbkrScannerSnapshot *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (age != 60000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (!scanAvailable) return UMI_STATUS_NOT_FOUND;
    *out = scan; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCopyScannerRow(GtkWindow *window, uint32_t request,
    uint64_t generation, size_t index, UmiIbkrScannerRow *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (scanRowStatus != UMI_STATUS_OK) return scanRowStatus;
    if (!scanAvailable || request != scan.requestId || index >= scan.count) return UMI_STATUS_NOT_FOUND;
    if (generation == 0U || generation != scan.generation) return UMI_STATUS_BUSY;
    *out = scanRows[index]; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkStartScanner(GtkWindow *window, const UmiIbkrScannerQuery *query)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (scanStartStatus != UMI_STATUS_OK) return scanStartStatus;
    if (scanAvailable && scan.needsCancel) return UMI_STATUS_ALREADY_EXISTS;
    ++scanStarts; scanAvailable = TRUE; memset(&scan, 0, sizeof(scan));
    scan.query = *query; scan.requestId = 44000U + scanStarts;
    scan.active = scan.needsCancel = scan.stale = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCancelScanner(GtkWindow *window, uint32_t expected)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!scanAvailable || expected != scan.requestId) return UMI_STATUS_NOT_FOUND;
    if (scanStopStatus != UMI_STATUS_OK) return scanStopStatus;
    ++scanStops; scan.active = scan.needsCancel = false;
    scan.cancelled = scan.stale = true; return UMI_STATUS_OK;
}
static void ScannerResults(void)
{
    scan.count = 2U; ++scan.generation; scan.stale = false;
    memset(scanRows, 0, sizeof(scanRows));
    for (size_t i = 0U; i < 2U; ++i) {
        scanRows[i].rank = (unsigned)i; scanRows[i].contractId = (uint32_t)i + 100U;
        strcpy(scanRows[i].symbol, i == 0U ? "EXAMPLE-A" : "EXAMPLE-B");
        strcpy(scanRows[i].exchange, "LISTING");
        strcpy(scanRows[i].currency, "USD"); strcpy(scanRows[i].securityType, "STK");
        strcpy(scanRows[i].distance, "0.123456789123456789");
    }
}
static int ScannerProjection(const char *test, GtkWidget *root, UmiBrokerWorkspaceGtk **workspace)
{
    GtkWidget *start = Find(root, "broker-workspace-scanner-start");
    GtkWidget *stop = Find(root, "broker-workspace-scanner-stop");
    GtkWidget *apply = Find(root, "broker-workspace-scanner-apply");
    GtkWidget *instrument = Find(root, "broker-workspace-scanner-instrument");
    GtkWidget *location = Find(root, "broker-workspace-scanner-location");
    GtkWidget *code = Find(root, "broker-workspace-scanner-code");
    GtkWidget *state = Find(root, "broker-workspace-scanner-status");
    GtkWidget *notice = Find(root, "broker-workspace-scanner-notice");
    GtkWidget *table = Find(root, "broker-workspace-scanner-table");
    GtkDropDown *results = GTK_DROP_DOWN(Find(root, "broker-workspace-scanner-results"));
    GtkWidget *contract = Find(root, "broker-workspace-contract");
    GtkWidget *exchange = Find(root, "broker-workspace-exchange");
    CHECK(start != NULL && apply != NULL && contract != NULL && GTK_IS_DROP_DOWN(results));
    CHECK(gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(stop));
    CHECK(!gtk_widget_get_sensitive(apply));
    if (strcmp(test, "scanner-retained") == 0) {
        UmiBrokerWorkspaceGtkDestroy(*workspace); *workspace = NULL;
        g_signal_emit_by_name(start, "clicked"); g_signal_emit_by_name(stop, "clicked");
        g_signal_emit_by_name(apply, "clicked");
        CHECK(scanStarts == 0U && scanStops == 0U && subscriptions == 0U);
        return 0;
    }
    if (strcmp(test, "scanner-invalid") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(code), "");
        g_signal_emit_by_name(start, "clicked");
        CHECK(scanStarts == 0U && Contains(notice, "not started"));
        return 0;
    }
    if (strcmp(test, "scanner-rejected") == 0) {
        scanStartStatus = UMI_STATUS_BUSY;
        g_signal_emit_by_name(start, "clicked");
        CHECK(scanStarts == 0U && gtk_widget_get_sensitive(start) && Contains(notice, "not started"));
        return 0;
    }
    if (strcmp(test, "scanner-external-request") == 0) {
        scanAvailable = TRUE; scan.requestId = 55U; scan.needsCancel = scan.active = true;
        g_signal_emit_by_name(start, "clicked");
        CHECK(scanStarts == 0U && scan.requestId == 55U && gtk_widget_get_sensitive(stop));
        return 0;
    }
    g_signal_emit_by_name(start, "clicked");
    CHECK(scanStarts == 1U && scan.active && scan.needsCancel && scan.stale);
    CHECK(strcmp(scan.query.instrument, "STK") == 0);
    CHECK(strcmp(scan.query.locationCode, "STK.US.MAJOR") == 0);
    CHECK(strcmp(scan.query.scanCode, "TOP_PERC_GAIN") == 0 && scan.query.numberOfRows == 25U);
    CHECK(!gtk_widget_get_sensitive(start) && gtk_widget_get_sensitive(stop));
    CHECK(!gtk_widget_get_sensitive(instrument) && !gtk_widget_get_sensitive(location));
    if (strcmp(test, "scanner-request-replaced") == 0) {
        ++scan.requestId; g_signal_emit_by_name(stop, "clicked");
        CHECK(scanStops == 0U && scan.needsCancel && Contains(notice, "not stopped"));
        return 0;
    }
    if (strcmp(test, "scanner-stop") == 0 || strcmp(test, "scanner-stop-failure") == 0) {
        if (strcmp(test, "scanner-stop-failure") == 0) scanStopStatus = UMI_STATUS_CAPACITY_EXCEEDED;
        g_signal_emit_by_name(stop, "clicked");
        CHECK(scanStops == (scanStopStatus == UMI_STATUS_OK ? 1U : 0U));
        CHECK(scan.needsCancel == (scanStopStatus != UMI_STATUS_OK));
        CHECK(gtk_widget_get_sensitive(start) == (scanStopStatus == UMI_STATUS_OK));
        return 0;
    }
    ScannerResults();
    if (strcmp(test, "scanner-empty") == 0) scan.count = 0U;
    if (strcmp(test, "scanner-utf8") == 0) {
        memcpy(scanRows[0].symbol, "Broken \xff", sizeof("Broken \xff"));
        memcpy(scan.message, "Provider \xff", sizeof("Provider \xff"));
    }
    if (strcmp(test, "scanner-reentrant") == 0)
        g_signal_connect(state, "notify::label", G_CALLBACK(CloseDuringRender), workspace);
    if (strcmp(test, "scanner-model-retired") == 0)
        g_signal_connect(results, "notify::model", G_CALLBACK(CloseDuringRender), workspace);
    UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(*workspace);
    if (strcmp(test, "scanner-reentrant") == 0 || strcmp(test, "scanner-model-retired") == 0) {
        CHECK(refreshed == UMI_STATUS_CANCELLED && *workspace == NULL);
        g_signal_handlers_disconnect_by_data(state, workspace);
        g_signal_handlers_disconnect_by_data(results, workspace);
        g_signal_emit_by_name(start, "clicked"); g_signal_emit_by_name(apply, "clicked");
        CHECK(scanStarts == 1U && subscriptions == 0U);
        return 0;
    }
    CHECK(refreshed == UMI_STATUS_OK);
    CHECK(gtk_drop_down_get_selected(results) == GTK_INVALID_LIST_POSITION);
    if (strcmp(test, "scanner-empty") == 0) {
        CHECK(Contains(table, "Complete refresh contains no matching contracts"));
        CHECK(!gtk_widget_get_sensitive(apply));
        return 0;
    }
    CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(results)) == 2U);
    CHECK(Contains(table, "0.123456789123456789"));
    gtk_drop_down_set_selected(results, 1U);
    CHECK(gtk_widget_get_sensitive(apply));
    gtk_editable_set_text(GTK_EDITABLE(contract), "999");
    gtk_editable_set_text(GTK_EDITABLE(exchange), "OLD");
    if (strcmp(test, "scanner-refresh-replaced") == 0) {
        ++scan.generation; scanRows[1].contractId = 555U;
        g_signal_emit_by_name(apply, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(contract)), "999") == 0);
        CHECK(gtk_drop_down_get_selected(results) == GTK_INVALID_LIST_POSITION && Contains(notice, "changed"));
    } else if (strcmp(test, "scanner-row-copy-failed") == 0) {
        ++scan.generation; scanRows[1].contractId = 555U; scanRowStatus = UMI_STATUS_BUSY;
        g_signal_emit_by_name(apply, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(contract)), "999") == 0);
        CHECK(Contains(state, "STALE") && Contains(table, "EXAMPLE-B"));
    } else if (strcmp(test, "scanner-stale") == 0 || strcmp(test, "scanner-failed") == 0) {
        scan.stale = true;
        if (strcmp(test, "scanner-failed") == 0) { scan.failed = true; scan.active = false; }
        g_signal_emit_by_name(apply, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(contract)), "999") == 0);
        CHECK(!gtk_widget_get_sensitive(apply) && gtk_widget_get_sensitive(stop));
    } else if (strcmp(test, "scanner-quote-busy") == 0) {
        quoteAvailable = TRUE; quote.subscribed = true;
        g_signal_emit_by_name(apply, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(contract)), "999") == 0);
        CHECK(!gtk_widget_get_sensitive(apply) && subscriptions == 0U);
    } else if (strcmp(test, "scanner-query-identity") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(code), "CHANGED");
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
        CHECK(Contains(state, "TOP_PERC_GAIN") && !Contains(state, "CHANGED"));
    } else if (strcmp(test, "scanner-reconnect") == 0) {
        gtk_window_destroy(inspector);
        CHECK(Contains(state, "STALE") && !gtk_widget_get_sensitive(apply));
        scanAvailable = FALSE;
        CHECK(UmiBrokerWorkspaceGtkOpenConnection(*workspace) == UMI_STATUS_OK);
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(results)) == 0U);
        CHECK(!Contains(table, "EXAMPLE-B"));
    } else if (strcmp(test, "scanner-utf8") == 0) {
        CHECK(Contains(table, "Broken \xef\xbf\xbd") && Contains(state, "Provider \xef\xbf\xbd"));
        CHECK((unsigned char)scanRows[0].symbol[7] == 0xffU);
    } else if (strcmp(test, "scanner-results") == 0) {
        g_signal_emit_by_name(apply, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(contract)), "101") == 0);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(exchange)), "") == 0);
        CHECK(subscriptions == 0U && scan.active);
    } else return 2;
    return 0;
}
