/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/workspace_streaming_projection.inc
 *
 * PURPOSE:
 *   Verify live chart subscription identity, rolling receipts, stale data and retained navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

static UmiIbkrRealtimeSnapshot streamCapture;
static UmiIbkrRealtimeBar streamBar;
static gboolean streamAvailable;
static unsigned streamStarts, streamStops, streamScenes;
static size_t streamFirst, streamCount;
static UmiStatus streamStartStatus = UMI_STATUS_OK, streamStopStatus = UMI_STATUS_OK;
static UmiStatus streamCopyStatus = UMI_STATUS_OK, streamSceneStatus = UMI_STATUS_OK;
UmiStatus UmiIbkrGtkStartRealtime(GtkWindow *window, const UmiIbkrRealtimeQuery *query)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (streamStartStatus != UMI_STATUS_OK) return streamStartStatus;
    if (UmiIbkrRealtimeQueryValidate(query) != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    if (streamAvailable && streamCapture.needsCancel) return UMI_STATUS_BUSY;
    ++streamStarts; memset(&streamCapture, 0, sizeof(streamCapture)); streamAvailable = TRUE;
    streamCapture.requestId = 45000U + streamStarts; streamCapture.query = *query;
    streamCapture.active = streamCapture.needsCancel = streamCapture.stale = true;
    return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkStopRealtime(GtkWindow *window, uint32_t expected)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!streamAvailable || streamCapture.requestId != expected) return UMI_STATUS_NOT_FOUND;
    if (streamStopStatus != UMI_STATUS_OK) return streamStopStatus;
    ++streamStops; streamCapture.active = streamCapture.needsCancel = false;
    streamCapture.cancelled = streamCapture.stale = true; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCopyRealtime(GtkWindow *window, uint64_t age, UmiIbkrRealtimeSnapshot *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (age != 15000U) return UMI_STATUS_INVALID_ARGUMENT;
    if (streamCopyStatus != UMI_STATUS_OK) return streamCopyStatus;
    if (!streamAvailable) return UMI_STATUS_NOT_FOUND;
    *out = streamCapture; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCopyRealtimeBar(GtkWindow *window, uint32_t expected, size_t index, UmiIbkrRealtimeBar *out)
{
    if (window != inspector) return UMI_STATUS_CANCELLED;
    if (!streamAvailable || expected != streamCapture.requestId || index >= streamCapture.count)
        return UMI_STATUS_NOT_FOUND;
    *out = streamBar; return UMI_STATUS_OK;
}
UmiStatus UmiIbkrGtkCreateRealtimeScene(GtkWindow *window, uint32_t expected, size_t first, size_t count,
    uint64_t age, UmiChartRenderScene **out)
{
    if (window != inspector || expected != streamCapture.requestId) return UMI_STATUS_NOT_FOUND;
    if (age != 15000U || count == 0U || first + count > streamCapture.count) return UMI_STATUS_INVALID_ARGUMENT;
    ++streamScenes; streamFirst = first; streamCount = count;
    /* Rendering geometry is covered by the portable scene suite. This boundary
     * records requested ranges without allocating a renderer in the UI fixture. */
    if (streamSceneStatus == UMI_STATUS_OK) *out = NULL;
    return streamSceneStatus;
}
static void StreamBars(void)
{
    streamCapture.stale = false; streamCapture.count = 100U; streamCapture.receivedBars = 100U;
    streamBar.timeMilliseconds = INT64_C(1791363600000); streamBar.volumeAvailable = true;
    strcpy(streamBar.open.reportedText, "9007199254740993.123456789");
    strcpy(streamBar.high.reportedText, "9007199254740994.123456789");
    strcpy(streamBar.low.reportedText, "9007199254740992.123456789");
    strcpy(streamBar.close.reportedText, "9007199254740993.999999999");
    strcpy(streamBar.volume.reportedText, "0");
}
static int StreamProjection(const char *test, GtkWidget *root, UmiBrokerWorkspaceGtk **workspace)
{
    GtkWidget *start = Find(root, "broker-workspace-stream-start"), *stop = Find(root, "broker-workspace-stream-stop");
    GtkWidget *kind = Find(root, "broker-workspace-stream-kind"), *rth = Find(root, "broker-workspace-stream-rth");
    GtkWidget *status = Find(root, "broker-workspace-stream-status"), *notice = Find(root, "broker-workspace-stream-notice");
    GtkWidget *details = Find(root, "broker-workspace-stream-details");
    GtkWidget *contract = Find(root, "broker-workspace-contract"), *exchange = Find(root, "broker-workspace-exchange");
    GtkWidget *visible = Find(root, "broker-workspace-stream-visible"), *offset = Find(root, "broker-workspace-stream-offset");
    GtkWidget *modes = Find(root, "broker-workspace-chart-modes");
    CHECK(start && stop && details && GTK_IS_STACK(modes));
    CHECK(gtk_widget_get_sensitive(start) && !gtk_widget_get_sensitive(stop));
    if (!strcmp(test, "stream-retained")) {
        UmiBrokerWorkspaceGtkDestroy(*workspace); *workspace = NULL;
        g_signal_emit_by_name(start, "clicked"); g_signal_emit_by_name(stop, "clicked");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(visible), 25);
        CHECK(streamStarts == 0U && streamStops == 0U); return 0;
    }
    gtk_editable_set_text(GTK_EDITABLE(contract), "123"); gtk_editable_set_text(GTK_EDITABLE(exchange), "SMART");
    if (!strcmp(test, "stream-invalid")) gtk_editable_set_text(GTK_EDITABLE(contract), "not-id");
    if (!strcmp(test, "stream-kind-invalid")) gtk_drop_down_set_selected(GTK_DROP_DOWN(kind), GTK_INVALID_LIST_POSITION);
    if (!strcmp(test, "stream-midpoint")) gtk_drop_down_set_selected(GTK_DROP_DOWN(kind), 1U);
    if (!strcmp(test, "stream-bid")) gtk_drop_down_set_selected(GTK_DROP_DOWN(kind), 2U);
    if (!strcmp(test, "stream-ask")) gtk_drop_down_set_selected(GTK_DROP_DOWN(kind), 3U);
    if (!strcmp(test, "stream-all-hours")) gtk_check_button_set_active(GTK_CHECK_BUTTON(rth), FALSE);
    if (!strcmp(test, "stream-rejected")) streamStartStatus = UMI_STATUS_NOT_IMPLEMENTED;
    if (!strcmp(test, "stream-pacing")) streamStartStatus = UMI_STATUS_BUSY;
    if (!strcmp(test, "stream-session-stale")) observed.stale = true;
    if (!strcmp(test, "stream-external")) {
        streamAvailable = TRUE; streamCapture.requestId = 45999U; streamCapture.needsCancel = true;
        streamCapture.query.contract.contractId = 456U;
    }
    if (!strcmp(test, "stream-intent")) g_signal_connect(start, "notify::sensitive", G_CALLBACK(TapeChangeIntent), contract);
    g_signal_emit_by_name(start, "clicked"); g_signal_handlers_disconnect_by_data(start, contract);
    if (!strcmp(test, "stream-invalid") || !strcmp(test, "stream-kind-invalid") ||
        !strcmp(test, "stream-rejected") || !strcmp(test, "stream-pacing") ||
        !strcmp(test, "stream-session-stale") || !strcmp(test, "stream-external")) {
        CHECK(streamStarts == 0U);
        CHECK(Contains(notice, !strcmp(test, "stream-pacing") ? "15 seconds" : "not started"));
        if (!strcmp(test, "stream-external")) CHECK(!gtk_widget_get_sensitive(start) && Contains(status, "456"));
        return 0;
    }
    CHECK(streamStarts == 1U && streamCapture.query.contract.contractId == 123U);
    CHECK(!strcmp(streamCapture.query.contract.exchange, "SMART"));
    CHECK(streamCapture.query.dataKind == (!strcmp(test, "stream-midpoint") ? UMI_IBKR_HISTORY_MIDPOINT :
        !strcmp(test, "stream-bid") ? UMI_IBKR_HISTORY_BID :
        !strcmp(test, "stream-ask") ? UMI_IBKR_HISTORY_ASK : UMI_IBKR_HISTORY_TRADES));
    CHECK(streamCapture.query.regularHours == (strcmp(test, "stream-all-hours") != 0));
    CHECK(!gtk_widget_get_sensitive(start) && gtk_widget_get_sensitive(stop) && !gtk_widget_get_sensitive(kind));
    g_signal_emit_by_name(start, "clicked"); CHECK(streamStarts == 1U);
    StreamBars();
    if (!strcmp(test, "stream-empty")) streamCapture.count = 0U;
    if (!strcmp(test, "stream-stale")) streamCapture.stale = true;
    if (!strcmp(test, "stream-failed")) { streamCapture.failed = streamCapture.stale = true; streamCapture.active = false; }
    if (!strcmp(test, "stream-copy-failure")) streamCopyStatus = UMI_STATUS_UNAVAILABLE;
    if (!strcmp(test, "stream-capacity")) streamCapture.count = 513U;
    if (!strcmp(test, "stream-volume-missing")) streamBar.volumeAvailable = false;
    if (!strcmp(test, "stream-render-failure")) streamSceneStatus = UMI_STATUS_UNAVAILABLE;
    if (!strcmp(test, "stream-utf8")) memcpy(streamCapture.message, "TEST\xff", sizeof("TEST\xff"));
    if (!strcmp(test, "stream-reentrant")) g_signal_connect(status, "notify::label", G_CALLBACK(CloseDuringRender), workspace);
    UmiStatus refreshed = UmiBrokerWorkspaceGtkRefresh(*workspace);
    if (!strcmp(test, "stream-reentrant")) {
        CHECK(refreshed == UMI_STATUS_CANCELLED && *workspace == NULL);
        g_signal_handlers_disconnect_by_data(status, workspace);
        g_signal_emit_by_name(stop, "clicked"); CHECK(streamStops == 0U); return 0;
    }
    CHECK(refreshed == UMI_STATUS_OK);
    if (!strcmp(test, "stream-empty") || !strcmp(test, "stream-stale") || !strcmp(test, "stream-failed") ||
        !strcmp(test, "stream-copy-failure") || !strcmp(test, "stream-capacity")) {
        CHECK(!Contains(details, "9007199254740993"));
        CHECK(streamScenes == 0U); return 0;
    }
    CHECK(Contains(details, "9007199254740993.123456789"));
    CHECK(Contains(details, !strcmp(test, "stream-volume-missing") ? "Volume Unavailable" : "Volume 0"));
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
    CHECK(streamScenes == 1U && streamFirst == 20U && streamCount == 80U);
    if (!strcmp(test, "stream-render-failure")) CHECK(Contains(details, "Chart unavailable"));
#endif
    if (!strcmp(test, "stream-utf8")) CHECK(Contains(status, "TEST\xef\xbf\xbd"));
    if (!strcmp(test, "stream-cache")) {
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
        CHECK(streamScenes == 1U);
#endif
    } else if (!strcmp(test, "stream-rollover")) {
        streamCapture.count = 512U; streamCapture.receivedBars = 512U;
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
        ++streamCapture.receivedBars; streamCapture.droppedBars = 1U;
        CHECK(UmiBrokerWorkspaceGtkRefresh(*workspace) == UMI_STATUS_OK);
        CHECK(Contains(status, "1 evicted"));
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
        CHECK(streamScenes == 3U && streamFirst == 432U && streamCount == 80U);
#endif
    } else if (!strcmp(test, "stream-range") || !strcmp(test, "stream-range-clamp")) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(visible), 25);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(offset), !strcmp(test, "stream-range") ? 10 : 511);
#ifdef UMI_IBKR_HAS_HISTORY_MODEL
        CHECK(streamFirst == (!strcmp(test, "stream-range") ? 65U : 0U));
        CHECK(streamCount == (!strcmp(test, "stream-range") ? 25U : 1U));
#endif
        CHECK(streamStarts == 1U && streamStops == 0U);
    } else if (!strcmp(test, "stream-modes")) {
        gtk_stack_set_visible_child_name(GTK_STACK(modes), "live");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(visible), 25);
        gtk_stack_set_visible_child_name(GTK_STACK(modes), "history");
        gtk_stack_set_visible_child_name(GTK_STACK(modes), "live");
        CHECK(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(visible)) == 25);
        CHECK(streamStarts == 1U && streamStops == 0U);
    } else if (!strcmp(test, "stream-disconnect") || !strcmp(test, "stream-reconnect")) {
        gtk_window_destroy(inspector);
        CHECK(Contains(status, "STALE") && !gtk_widget_get_sensitive(stop));
        CHECK(!Contains(details, "9007199254740993"));
        if (!strcmp(test, "stream-reconnect")) {
            streamAvailable = FALSE;
            CHECK(UmiBrokerWorkspaceGtkOpenConnection(*workspace) == UMI_STATUS_OK);
            CHECK(Contains(status, "No live bars requested"));
            CHECK(gtk_widget_get_sensitive(start) && streamStarts == 1U);
        }
    } else if (!strcmp(test, "stream-stop") || !strcmp(test, "stream-stop-failure") || !strcmp(test, "stream-replaced-stop")) {
        if (!strcmp(test, "stream-stop-failure")) streamStopStatus = UMI_STATUS_CAPACITY_EXCEEDED;
        if (!strcmp(test, "stream-replaced-stop")) ++streamCapture.requestId;
        g_signal_emit_by_name(stop, "clicked");
        CHECK(streamStops == (!strcmp(test, "stream-stop") ? 1U : 0U));
        CHECK(gtk_widget_get_sensitive(stop) == (streamStops == 0U));
        CHECK(Contains(details, "9007199254740993") == (streamStops == 0U));
    }
    CHECK(subscriptions == 0U && cancellations == 0U);
    return 0;
}
