/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/workspace_time_sales_projection.inc
 *
 * PURPOSE:
 *   Verify native tick controls, receipt evidence and reentrant window retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static UmiIbkrTimeSalesReport tape;
static gboolean tapeAvailable;
static unsigned tapeStarts, tapeStops;
static UmiStatus tapeStartStatus = UMI_STATUS_OK, tapeStopStatus = UMI_STATUS_OK, tapeCopyStatus = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkStartTimeSales(GtkWindow *window, const UmiIbkrTimeSalesQuery *query)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (tapeStartStatus != UMI_STATUS_OK) return tapeStartStatus;
    if (UmiIbkrTimeSalesQueryValidate(query) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    if (tapeAvailable && tape.capture.needsCancel) return UMI_STATUS_ALREADY_EXISTS;
    ++tapeStarts; memset(&tape, 0, sizeof(tape)); tapeAvailable = TRUE;
    tape.capture.requestId = 44000U + tapeStarts; tape.capture.query = *query;
    tape.capture.active = tape.capture.needsCancel = tape.capture.stale = true; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkStopTimeSales(GtkWindow *window, uint32_t expected)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!tapeAvailable || expected != tape.capture.requestId) return UMI_STATUS_NOT_FOUND;
    if (tapeStopStatus != UMI_STATUS_OK) return tapeStopStatus;
    ++tapeStops; tape.capture.active = tape.capture.needsCancel = false;
    tape.capture.cancelled = tape.capture.stale = true; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCopyTimeSales(GtkWindow *window, uint64_t age, UmiIbkrTimeSalesReport *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (age != 15000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (tapeCopyStatus != UMI_STATUS_OK) return tapeCopyStatus;
    if (!tapeAvailable) return UMI_STATUS_NOT_FOUND;
    *out = tape; return UMI_STATUS_OK;
}
static void TapeRows(void)
{
    tape.count = tape.capture.count = 2U; tape.capture.receivedEvents = 9U;
    tape.capture.droppedEvents = 7U; tape.capture.stale = false;
    for (size_t i = 0U; i < 2U; ++i) {
        UmiIbkrTimeSalesEvent *row = &tape.rows[i].event;
        row->sequence = 8U + i; row->kind = tape.capture.query.kind;
        row->epochSeconds = 1791363600U; row->flags = 3U;
        strcpy(row->price.reportedText, "9007199254740993.123456789"); strcpy(row->size.reportedText, "0");
        strcpy(row->bidPrice.reportedText, "12.000000001"); strcpy(row->bidSize.reportedText, "4.5");
        strcpy(row->askPrice.reportedText, "12.000000002"); strcpy(row->askSize.reportedText, "8");
        strcpy(row->exchange, "TEST"); strcpy(row->specialConditions, "SYNTHETIC");
    }
}
static void TapeChangeIntent(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec; gtk_editable_set_text(GTK_EDITABLE(data), "999");
}
static int TapeProjection(const char *test, GtkWidget *root, UmiBrokerWorkspaceGtk **workspace)
{
    GtkWidget *start = Find(root, "broker-workspace-tape-start"), *stop = Find(root, "broker-workspace-tape-stop");
    GtkWidget *kind = Find(root, "broker-workspace-tape-kind"), *ignore = Find(root, "broker-workspace-tape-ignore-size");
    GtkWidget *status = Find(root, "broker-workspace-tape-status"), *notice = Find(root, "broker-workspace-tape-notice");
    GtkWidget *contract = Find(root, "broker-workspace-contract"), *exchange = Find(root, "broker-workspace-exchange");
    GtkWidget *price = Find(root, "broker-workspace-tape-cell-0-3");
    CHECK(start != NULL && price != NULL && gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(stop));
    CHECK(!gtk_widget_get_visible(price));
    if (!strcmp(test, "tape-retained")) {
        UmiBrokerWorkspaceGtkDestroy(*workspace); *workspace = NULL;
        g_signal_emit_by_name(start, "clicked"); g_signal_emit_by_name(stop, "clicked");
        CHECK(tapeStarts == 0U && tapeStops == 0U); return 0;
    }
    gtk_editable_set_text(GTK_EDITABLE(contract), "123"); gtk_editable_set_text(GTK_EDITABLE(exchange), "SMART");
    if (!strcmp(test, "tape-invalid")) gtk_editable_set_text(GTK_EDITABLE(contract), "not-id");
    if (!strcmp(test, "tape-kind-invalid")) gtk_drop_down_set_selected(GTK_DROP_DOWN(kind), GTK_INVALID_LIST_POSITION);
    if (!strcmp(test, "tape-ignore-invalid")) gtk_check_button_set_active(GTK_CHECK_BUTTON(ignore), TRUE);
    if (!strcmp(test, "tape-bid-ask")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(kind), 2U); gtk_check_button_set_active(GTK_CHECK_BUTTON(ignore), TRUE);
    }
    if (!strcmp(test, "tape-midpoint")) gtk_drop_down_set_selected(GTK_DROP_DOWN(kind), 3U);
    if (!strcmp(test, "tape-all-last")) gtk_drop_down_set_selected(GTK_DROP_DOWN(kind), 1U);
    if (!strcmp(test, "tape-rejected")) tapeStartStatus = UMI_STATUS_NOT_IMPLEMENTED;
    if (!strcmp(test, "tape-pacing")) tapeStartStatus = UMI_STATUS_BUSY;
    if (!strcmp(test, "tape-session-stale")) observed.stale = true;
    if (!strcmp(test, "tape-external")) {
        tapeAvailable = TRUE; tape.capture.requestId = 44999U; tape.capture.needsCancel = true;
        tape.capture.query.contract.contractId = 456U;
    }
    if (!strcmp(test, "tape-intent")) g_signal_connect(start, "notify::sensitive", G_CALLBACK(TapeChangeIntent), contract);
    g_signal_emit_by_name(start, "clicked"); g_signal_handlers_disconnect_by_data(start, contract);
    if (!strcmp(test, "tape-invalid") || !strcmp(test, "tape-kind-invalid") || !strcmp(test, "tape-ignore-invalid") ||
        !strcmp(test, "tape-rejected") || !strcmp(test, "tape-pacing") || !strcmp(test, "tape-session-stale") ||
        !strcmp(test, "tape-external")) {
        CHECK(tapeStarts == 0U);
        if (!strcmp(test, "tape-pacing")) CHECK(Contains(notice, "15 seconds"));
        if (!strcmp(test, "tape-external")) CHECK(Contains(status, "456") && !gtk_widget_get_sensitive(start));
        return 0;
    }
    CHECK(tapeStarts == 1U && tape.capture.query.contract.contractId == 123U);
    CHECK(!strcmp(tape.capture.query.contract.exchange, "SMART"));
    CHECK(!gtk_widget_get_sensitive(start) && gtk_widget_get_sensitive(stop));
    g_signal_emit_by_name(start, "clicked"); CHECK(tapeStarts == 1U);
    TapeRows();
    if (!strcmp(test, "tape-empty")) tape.count = tape.capture.count = 0U;
    if (!strcmp(test, "tape-row-stale")) tape.rows[0].stale = true;
    if (!strcmp(test, "tape-stale")) tape.capture.stale = true;
    if (!strcmp(test, "tape-failed")) { tape.capture.failed = tape.capture.stale = true; tape.capture.active = false; }
    if (!strcmp(test, "tape-copy-failure")) tapeCopyStatus = UMI_STATUS_UNAVAILABLE;
    if (!strcmp(test, "tape-capacity")) tape.count = 65U;
    if (!strcmp(test, "tape-utf8")) memcpy(tape.rows[0].event.exchange, "TEST\xff", sizeof("TEST\xff"));
    if (!strcmp(test, "tape-reentrant")) g_signal_connect(status, "notify::label", G_CALLBACK(CloseDuringRender), workspace);
    if (!strcmp(test, "tape-cell-retired")) g_signal_connect(price, "notify::label", G_CALLBACK(CloseDuringRender), workspace);
    UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(*workspace);
    if (!strcmp(test, "tape-reentrant") || !strcmp(test, "tape-cell-retired")) {
        CHECK(refreshed == UMI_STATUS_CANCELLED && *workspace == NULL);
        g_signal_handlers_disconnect_by_data(status, workspace); g_signal_handlers_disconnect_by_data(price, workspace);
        g_signal_emit_by_name(stop, "clicked"); CHECK(tapeStops == 0U); return 0;
    }
    CHECK(refreshed == UMI_STATUS_OK);
    if (!strcmp(test, "tape-empty") || !strcmp(test, "tape-copy-failure") || !strcmp(test, "tape-capacity")) {
        CHECK(!gtk_widget_get_visible(price)); return 0;
    }
    CHECK(gtk_widget_get_visible(price));
    if (!strcmp(test, "tape-bid-ask")) {
        CHECK(Contains(Find(root, "broker-workspace-tape-cell-0-5"), "12.000000001"));
        CHECK(Contains(Find(root, "broker-workspace-tape-cell-0-10"), "bid past low")); CHECK(tape.capture.query.ignoreSize);
    } else CHECK(Contains(price, "9007199254740993.123456789"));
    if (!strcmp(test, "tape-row-stale") || !strcmp(test, "tape-stale") || !strcmp(test, "tape-failed"))
        CHECK(Contains(Find(root, "broker-workspace-tape-cell-0-12"), "STALE"));
    CHECK(Contains(status, "7 evicted"));
    if (!strcmp(test, "tape-utf8")) CHECK(Contains(Find(root, "broker-workspace-tape-cell-0-9"), "TEST\xef\xbf\xbd"));
    if (!strcmp(test, "tape-disconnect") || !strcmp(test, "tape-reconnect")) {
        gtk_window_destroy(inspector);
        CHECK(Contains(status, "STALE") && !gtk_widget_get_sensitive(stop) && Contains(price, "9007199254740993"));
        if (!strcmp(test, "tape-reconnect")) {
            tapeAvailable = FALSE; CHECK(UmiBrokerWorkspaceGtkOpenConnection(*workspace) == UMI_STATUS_OK);
            CHECK(Contains(status, "No tick stream") && !gtk_widget_get_visible(price));
        }
    } else if (!strcmp(test, "tape-stop") || !strcmp(test, "tape-stop-failure") || !strcmp(test, "tape-replaced-stop")) {
        if (!strcmp(test, "tape-stop-failure")) tapeStopStatus = UMI_STATUS_CAPACITY_EXCEEDED;
        if (!strcmp(test, "tape-replaced-stop")) ++tape.capture.requestId;
        g_signal_emit_by_name(stop, "clicked");
        CHECK(tapeStops == (!strcmp(test, "tape-stop") ? 1U : 0U));
        CHECK(gtk_widget_get_sensitive(stop) == (tapeStops == 0U));
        CHECK(Contains(price, "9007199254740993"));
    }
    CHECK(subscriptions == 0U && cancellations == 0U);
    return 0;
}
