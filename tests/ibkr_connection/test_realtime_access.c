/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_realtime_access.c
 *
 * PURPOSE:
 *   Exercise real stream controls with in-memory transport, explicit request identity and reentrant retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/broker_connectivity/connection_gtk4.h"
static Fixture transport;
static UmiStatus NativeCreate(const UmiIbkrConnectionOptions *options, UmiIbkrConnection **out)
{
    UmiIbkrIo io = {&transport, FOpen, FReady, FRead, FWrite, FClose};
    return UmiIbkrConnectionCreateWithIo(options, &io, out);
}
#define UmiIbkrConnectionCreate NativeCreate
#include "../../adapters/gtk4/ibkr_connection.c"
#undef UmiIbkrConnectionCreate

typedef struct StreamProbe {
    GtkWindow *window;
    UmiIbkrRealtimeQuery query;
    uint32_t request;
    unsigned calls;
    gboolean close;
    UmiStatus startStatus, stopStatus;
} StreamProbe;
static void StreamNotify(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    StreamProbe *probe = data;
    if (++probe->calls != 1U) return;
    if (probe->close) gtk_window_destroy(probe->window);
    else {
        probe->startStatus = UmiIbkrGtkStartRealtime(probe->window, &probe->query);
        probe->stopStatus = UmiIbkrGtkStopRealtime(probe->window, probe->request);
    }
}
static int RunStreamAccess(const char *test)
{
    GtkWindow *window = UmiIbkrGtkCreate(NULL);
    CHECK(window != NULL); g_object_ref(window);
    ConnectionUi *ui = Owner(window); CHECK(ui != NULL);
    UmiIbkrRealtimeQuery query = {0};
    query.contract.contractId = 123U; strcpy(query.contract.exchange, "SMART");
    query.dataKind = UMI_IBKR_HISTORY_TRADES; query.regularHours = true;
    if (!strcmp(test, "guards") || !strcmp(test, "retired")) {
        UmiIbkrRealtimeSnapshot snapshot, before;
        UmiIbkrRealtimeBar bar, priorBar;
        memset(&snapshot, 0x5a, sizeof(snapshot)); before = snapshot;
        memset(&bar, 0x5a, sizeof(bar)); priorBar = bar;
        UmiChartRenderScene *scene = (UmiChartRenderScene *)(void *)&bar;
        UmiChartRenderScene *priorScene = scene;
        CHECK(UmiIbkrGtkStartRealtime(NULL, &query) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrGtkStartRealtime(window, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrGtkStartRealtime(window, &query) == UMI_STATUS_UNAVAILABLE);
        CHECK(UmiIbkrGtkStopRealtime(window, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrGtkStopRealtime(window, 1U) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiIbkrGtkCopyRealtime(window, 0U, &snapshot) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrGtkCopyRealtime(window, 15000U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrGtkCopyRealtime(window, 15000U, &snapshot) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiIbkrGtkCopyRealtimeBar(window, 0U, 0U, &bar) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrGtkCopyRealtimeBar(window, 1U, 0U, &bar) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiIbkrGtkCreateRealtimeScene(window, 1U, 0U, 0U, 15000U, &scene) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrGtkCreateRealtimeScene(window, 1U, 0U, 513U, 15000U, &scene) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiIbkrGtkCreateRealtimeScene(window, 1U, 0U, 1U, 15000U, &scene) == UMI_STATUS_NOT_FOUND);
        if (!strcmp(test, "retired")) {
            gtk_window_destroy(window);
            CHECK(UmiIbkrGtkStartRealtime(window, &query) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkStopRealtime(window, 1U) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyRealtime(window, 15000U, &snapshot) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCopyRealtimeBar(window, 1U, 0U, &bar) == UMI_STATUS_CANCELLED);
            CHECK(UmiIbkrGtkCreateRealtimeScene(window, 1U, 0U, 1U, 15000U, &scene) == UMI_STATUS_CANCELLED);
        }
        CHECK(!memcmp(&snapshot, &before, sizeof(snapshot)) && !memcmp(&bar, &priorBar, sizeof(bar)));
        CHECK(scene == priorScene && transport.opens == 0U);
    } else {
        UmiIbkrConnectionOptions options = UmiIbkrConnectionOptionsDefault();
        CHECK(NativeCreate(&options, &ui->connection) == UMI_STATUS_OK);
        CHECK(UmiIbkrConnectionOpen(ui->connection, 0U) == UMI_STATUS_OK);
        /* Negotiation is covered by protocol fixtures. This synthetic ready
         * session isolates public GTK ownership, pacing and notification paths. */
        ui->connection->snapshot.state = UMI_IBKR_READY;
        ui->connection->snapshot.stale = false;
        ui->connection->snapshot.protocolVersion = 176;
        if (!strcmp(test, "busy")) {
            ui->fillBusy = TRUE;
            CHECK(UmiIbkrGtkStartRealtime(window, &query) == UMI_STATUS_BUSY);
            CHECK(ui->streamRequest == 0U); ui->fillBusy = FALSE;
        } else {
            if (!strcmp(test, "unsupported")) ui->connection->snapshot.protocolVersion = 177;
            StreamProbe probe = {0}; probe.window = window; probe.query = query;
            if (!strcmp(test, "start-close")) {
                probe.close = TRUE;
                g_signal_connect(ui->streamOutput, "notify::label", G_CALLBACK(StreamNotify), &probe);
            }
            UmiStatus started = UmiIbkrGtkStartRealtime(window, &query);
            if (!strcmp(test, "start-close")) {
                CHECK(started == UMI_STATUS_CANCELLED && probe.calls == 1U && Owner(window) == NULL);
            } else if (!strcmp(test, "unsupported")) {
                CHECK(started == UMI_STATUS_NOT_IMPLEMENTED && ui->streamRequest == 0U);
            } else {
                CHECK(started == UMI_STATUS_OK && ui->streamRequest != 0U);
                uint32_t request = ui->streamRequest;
                UmiIbkrRealtimeSnapshot capture;
                CHECK(UmiIbkrGtkCopyRealtime(window, 15000U, &capture) == UMI_STATUS_OK);
                CHECK(capture.active && capture.needsCancel && capture.stale && capture.count == 0U);
                CHECK(capture.query.contract.contractId == 123U);
                if (!strcmp(test, "replacement")) {
                    query.contract.contractId = 456U;
                    /* Remove pacing as a confounder: the native single-slot
                     * ownership guard must still reject a different contract. */
                    ui->connection->barRequested = false;
                    size_t queued = ui->connection->txSize;
                    CHECK(UmiIbkrGtkStartRealtime(window, &query) == UMI_STATUS_BUSY);
                    CHECK(ui->streamRequest == request && ui->connection->txSize == queued);
                } else if (!strcmp(test, "wrong-request")) {
                    size_t queued = ui->connection->txSize;
                    CHECK(UmiIbkrGtkStopRealtime(window, request + 1U) == UMI_STATUS_NOT_FOUND);
                    CHECK(ui->connection->txSize == queued);
                    UmiIbkrRealtimeBar bar = {0};
                    UmiChartRenderScene *scene = NULL;
                    CHECK(UmiIbkrGtkCopyRealtimeBar(window, request + 1U, 0U, &bar) == UMI_STATUS_NOT_FOUND);
                    CHECK(UmiIbkrGtkCreateRealtimeScene(window, request + 1U, 0U, 1U, 15000U, &scene) == UMI_STATUS_NOT_FOUND);
                } else {
                    probe.request = request;
                    if (!strcmp(test, "stop-close") || !strcmp(test, "reentry")) {
                        probe.close = !strcmp(test, "stop-close");
                        g_signal_connect(ui->streamOutput, "notify::label", G_CALLBACK(StreamNotify), &probe);
                    }
                    UmiStatus stopped = UmiIbkrGtkStopRealtime(window, request);
                    if (!strcmp(test, "stop-close")) {
                        CHECK(stopped == UMI_STATUS_CANCELLED && probe.calls == 1U && Owner(window) == NULL);
                    } else {
                        CHECK(stopped == UMI_STATUS_OK);
                        CHECK(UmiIbkrGtkCopyRealtime(window, 15000U, &capture) == UMI_STATUS_OK);
                        CHECK(capture.cancelled && capture.stale && !capture.needsCancel && !capture.active);
                        if (!strcmp(test, "reentry")) {
                            CHECK(probe.calls == 1U && probe.startStatus == UMI_STATUS_BUSY && probe.stopStatus == UMI_STATUS_BUSY);
                            g_signal_handlers_disconnect_by_data(ui->streamOutput, &probe);
                        }
                        if (!strcmp(test, "pacing")) {
                            CHECK(UmiIbkrGtkStartRealtime(window, &query) == UMI_STATUS_BUSY);
                            CHECK(ui->streamRequest == request);
                        }
                    }
                }
            }
        }
    }
    gtk_window_destroy(window); g_object_unref(window);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    return RunStreamAccess(argv[1]);
}
