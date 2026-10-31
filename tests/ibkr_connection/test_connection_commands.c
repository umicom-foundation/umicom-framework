/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_connection_commands.c
 *
 * PURPOSE:
 *   Exercise real connection notifications with an in-memory transport and retained native owners.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Compile the actual native adapter with only construction redirected to the
 * existing in-memory transport seam. GTK signals, controls and core state stay
 * real; no socket, provider login or production test hook is involved. */
#include "fixture.h"
#include "umicom/broker_connectivity/connection_gtk4.h"
static Fixture transport;
static unsigned creations;
static UmiIbkrConnectionOptions captured;
static gboolean rejectCreation;
static UmiStatus NativeCreate(const UmiIbkrConnectionOptions *options, UmiIbkrConnection **out)
{
    ++creations; captured = *options;
    if (rejectCreation) return UMI_STATUS_OUT_OF_MEMORY;
    UmiIbkrIo io = {&transport, FOpen, FReady, FRead, FWrite, FClose};
    return UmiIbkrConnectionCreateWithIo(options, &io, out);
}
#define UmiIbkrConnectionCreate NativeCreate
#include "../../adapters/gtk4/ibkr_connection.c"
#undef UmiIbkrConnectionCreate

typedef struct Probe {
    GtkWindow *window;
    ConnectionUi *ui;
    const char *action;
    unsigned calls;
    gboolean release;
    UmiStatus nested;
} Probe;
static void OnNotify(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    Probe *probe = data;
    if (++probe->calls != 1U) return;
    if (!strcmp(probe->action, "close")) {
        gtk_window_destroy(probe->window);
        if (probe->release) g_object_unref(probe->window);
    } else if (!strcmp(probe->action, "reenter")) {
        g_signal_emit_by_name(probe->ui->connect, "clicked");
        g_signal_emit_by_name(probe->ui->disconnect, "clicked");
        g_signal_emit_by_name(probe->ui->read, "clicked");
        probe->nested = UmiIbkrGtkReadAccount(probe->window, "DU123");
    } else if (!strcmp(probe->action, "mode"))
        gtk_drop_down_set_selected(GTK_DROP_DOWN(probe->ui->mode), 1U);
    else if (!strcmp(probe->action, "port"))
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(probe->ui->port), 7501);
    else if (!strcmp(probe->action, "client"))
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(probe->ui->client), 96);
    else if (!strcmp(probe->action, "ack"))
        gtk_check_button_set_active(GTK_CHECK_BUTTON(probe->ui->ack), TRUE);
}
static void OnBufferChanged(GtkTextBuffer *buffer, gpointer data)
{
    OnNotify(G_OBJECT(buffer), NULL, data);
}
static int Ready(ConnectionUi *ui)
{
    UmiIbkrConnectionOptions options = UmiIbkrConnectionOptionsDefault();
    CHECK(NativeCreate(&options, &ui->connection) == UMI_STATUS_OK);
    CHECK(UmiIbkrConnectionOpen(ui->connection, 0U) == UMI_STATUS_OK);
    /* A received snapshot isolates GTK lifetime from protocol negotiation,
     * which has separate framing tests. This is a synthetic account identity. */
    ui->connection->snapshot.state = UMI_IBKR_READY;
    ui->connection->snapshot.stale = false;
    ui->connection->snapshot.accountCount = 1U;
    ui->connection->snapshot.protocolVersion = 176;
    strcpy(ui->connection->snapshot.accounts[0], "DU123");
    Render(ui, UmiIbkrMonotonicMilliseconds());
    CHECK(ui->snapshot.state == UMI_IBKR_READY);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->accounts), 0U);
    return 0;
}

/* Drive completed capture markers through the real decoder. Controls must
 * reopen only after both independent responses finish, without a new socket. */
static int FinishAccount(ConnectionUi *ui)
{
    char summary[24], positions[24];
    (void)snprintf(summary, sizeof summary, "%u", (unsigned)ui->connection->accountSummaryRequest);
    (void)snprintf(positions, sizeof positions, "%u", (unsigned)ui->connection->accountPositionsRequest);
    FEED(&transport, "64", "1", summary);
    FEED(&transport, "72", "1", positions);
    CHECK(UmiIbkrConnectionPump(ui->connection, UmiIbkrMonotonicMilliseconds()) == UMI_STATUS_OK);
    CHECK(UmiIbkrConnectionPump(ui->connection, UmiIbkrMonotonicMilliseconds()) == UMI_STATUS_OK);
    Render(ui, UmiIbkrMonotonicMilliseconds());
    CHECK(ui->snapshot.summaryComplete && ui->snapshot.positionsComplete);
    return 0;
}
static int RunRefresh(const char *test)
{
    GtkWindow *window = UmiIbkrGtkCreate(NULL);
    CHECK(window != NULL); g_object_ref(window);
    ConnectionUi *ui = Owner(window); CHECK(ui != NULL && Ready(ui) == 0);
    /* Provider negotiation is covered by the transport fixture. Supply two
     * synthetic advertised accounts to exercise the native selection path. */
    ui->connection->snapshot.accountCount = 2U;
    strcpy(ui->connection->snapshot.accounts[1], "DU456");
    ui->populated = FALSE;
    Render(ui, UmiIbkrMonotonicMilliseconds());
    gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->accounts), 0U);
    g_signal_emit_by_name(ui->read, "clicked");
    CHECK(ui->snapshot.requestIssued && !gtk_widget_get_sensitive(ui->accounts));
    CHECK(!gtk_widget_get_sensitive(ui->read));
    const uint32_t first = ui->connection->accountSummaryRequest;
    if (!strcmp(test, "read-refresh-pending")) {
        size_t queued = ui->connection->txSize;
        g_signal_emit_by_name(ui->read, "clicked");
        CHECK(UmiIbkrGtkReadAccount(window, "DU456") == UMI_STATUS_BUSY);
        CHECK(ui->connection->txSize == queued && ui->connection->accountSummaryRequest == first);
    } else {
        CHECK(FinishAccount(ui) == 0);
        CHECK(gtk_widget_get_sensitive(ui->read) && gtk_widget_get_sensitive(ui->accounts));
        CHECK(gtk_widget_get_sensitive(ui->accountReportKind));
        CHECK(!strcmp(gtk_button_get_label(GTK_BUTTON(ui->read)), "Refresh account"));
        if (!strcmp(test, "read-refresh-switch"))
            gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->accounts), 1U);
        if (!strcmp(test, "read-refresh-report")) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->accountReportKind), UMI_IBKR_ACCOUNT_CURRENCY);
            gtk_editable_set_text(GTK_EDITABLE(ui->accountReportCurrency), "GBP");
        }
        Probe probe = {window, ui, strstr(test, "reenter") ? "reenter" : "close", 0U, FALSE, UMI_STATUS_OK};
        gboolean observe = strstr(test, "label") != NULL;
        if (observe) {
            gtk_button_set_label(GTK_BUTTON(ui->read), "Read account");
            g_signal_connect(ui->read, "notify::label", G_CALLBACK(OnNotify), &probe);
        }
        if (!strcmp(test, "read-refresh-public"))
            CHECK(UmiIbkrGtkReadAccount(window, "DU456") == UMI_STATUS_OK);
        else g_signal_emit_by_name(ui->read, "clicked");
        if (observe) CHECK(probe.calls > 0U);
        if (!strcmp(test, "read-refresh-close-label")) {
            CHECK(ui->closed && ui->connection == NULL && transport.closes == 1U);
        } else {
            CHECK(ui->connection->accountSummaryRequest == first + 2U);
            CHECK(!ui->snapshot.summaryComplete && !ui->snapshot.positionsComplete);
            CHECK(!strcmp(ui->snapshot.selectedAccount,
                (!strcmp(test, "read-refresh-switch") || !strcmp(test, "read-refresh-public")) ? "DU456" : "DU123"));
            if (!strcmp(test, "read-refresh-report")) {
                CHECK(ui->connection->txSize > 0U);
                /* The first pump wrote cancellation frames already; inspect
                 * the newly queued request, not the preceding wire buffer. */
                const char *field = (const char *)ui->connection->tx + 4U;
                CHECK(!strcmp(field, "62"));
                for (unsigned i = 0U; i < 4U; ++i) field += strlen(field) + 1U;
                CHECK(!strcmp(field, "$LEDGER:GBP"));
                CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(ui->accountReportCurrency)), "GBP"));
            }
            if (!strcmp(test, "read-refresh-reenter-label")) CHECK(probe.nested == UMI_STATUS_BUSY);
            CHECK(!ui->commandBusy);
        }
    }
    CHECK(creations == 1U && transport.opens == 1U);
    gtk_window_destroy(window); g_object_unref(window);
    return 0;
}

static int Run(const char *test)
{
    if (g_str_has_prefix(test, "read-refresh-")) return RunRefresh(test);
    GtkWindow *window = UmiIbkrGtkCreate(NULL);
    CHECK(window != NULL); g_object_ref(window);
    ConnectionUi *ui = Owner(window); CHECK(ui != NULL);
    Probe probe = {window, ui, "close", 0U, FALSE, UMI_STATUS_OK};
    gpointer weakWindow = window;
    g_object_add_weak_pointer(G_OBJECT(window), &weakWindow);
    const gboolean reading = g_str_has_prefix(test, "read-") || g_str_has_prefix(test, "public-read-");
    const gboolean disconnecting = g_str_has_prefix(test, "disconnect-");
    if (reading || disconnecting || !strcmp(test, "poll-reserved")) CHECK(Ready(ui) == 0);
    if (!strcmp(test, "retained-controls")) {
        GtkWidget *buttons[] = {ui->connect, ui->disconnect, ui->read};
        for (size_t i = 0U; i < G_N_ELEMENTS(buttons); ++i) g_object_ref(buttons[i]);
        gtk_window_destroy(window);
        for (size_t i = 0U; i < G_N_ELEMENTS(buttons); ++i) {
            g_signal_emit_by_name(buttons[i], "clicked"); g_object_unref(buttons[i]);
        }
        CHECK(creations == 0U && transport.opens == 0U);
    } else if (!strcmp(test, "poll-reserved")) {
        ui->commandBusy = TRUE;
        CHECK(Poll(window) == G_SOURCE_CONTINUE);
        CHECK(transport.outSize == 0U && ui->connection->txSize == 0U);
        ui->commandBusy = FALSE;
    } else if (!strcmp(test, "public-read-busy")) {
        ui->commandBusy = TRUE;
        CHECK(UmiIbkrGtkReadAccount(window, "DU123") == UMI_STATUS_BUSY);
        CHECK(!ui->connection->snapshot.requestIssued);
        ui->commandBusy = FALSE;
    } else {
        if (!reading && !disconnecting) {
            gtk_label_set_text(GTK_LABEL(ui->quoteOutput), "Previous connection");
            gtk_label_set_text(GTK_LABEL(ui->quoteNotice), "Previous notice");
            GtkStringList *items = gtk_string_list_new((const char *[]){"DU123", NULL});
            gtk_drop_down_set_model(GTK_DROP_DOWN(ui->accounts), G_LIST_MODEL(items));
            g_object_unref(items);
        }
        if (!strcmp(test, "connect-live") || !strcmp(test, "connect-no-consent")) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->mode), 1U);
            if (!strcmp(test, "connect-live")) gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->ack), TRUE);
        }
        if (!strcmp(test, "connect-invalid-mode")) gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->mode), GTK_INVALID_LIST_POSITION);
        if (!strcmp(test, "connect-invalid-program")) gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->program), GTK_INVALID_LIST_POSITION);
        if (!strcmp(test, "connect-profile-busy")) ui->profileBusy = TRUE;
        if (!strcmp(test, "connect-fill-busy")) ui->fillBusy = TRUE;
        if (!strcmp(test, "connect-quote-busy")) ui->quoteAccessBusy = TRUE;
        rejectCreation = !strcmp(test, "connect-create-failure") || !strcmp(test, "connect-failure-close");
        if (!strcmp(test, "read-invalid-close")) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->accountReportKind), UMI_IBKR_ACCOUNT_CURRENCY);
            gtk_editable_set_text(GTK_EDITABLE(ui->accountReportCurrency), "bad");
        }
        GObject *watched = NULL; const char *signal = "notify::label";
        if (strstr(test, "close-output")) {
            watched = G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->output))); signal = "changed";
        } else if (strstr(test, "close-controls")) {
            watched = G_OBJECT(reading ? ui->read : ui->connect); signal = "notify::sensitive";
        } else if (!strcmp(test, "connect-close-accounts")) {
            watched = G_OBJECT(ui->accounts); signal = "notify::model";
        } else if (!strcmp(test, "connect-close-notice")) watched = G_OBJECT(ui->quoteNotice);
        else if (!strcmp(test, "connect-close-quote") || !strcmp(test, "connect-close-release"))
            watched = G_OBJECT(ui->quoteOutput);
        else if (strstr(test, "close") || !strcmp(test, "disconnect-reenter") || !strcmp(test, "read-reenter"))
            watched = G_OBJECT(ui->status);
        else if (!strcmp(test, "connect-reenter") || g_str_has_prefix(test, "connect-edit-"))
            watched = G_OBJECT(ui->quoteOutput);
        if (strstr(test, "reenter")) probe.action = "reenter";
        if (g_str_has_prefix(test, "connect-edit-")) probe.action = test + strlen("connect-edit-");
        if (!strcmp(test, "connect-close-release")) probe.release = TRUE;
        if (watched != NULL) {
            if (!strcmp(signal, "changed"))
                g_signal_connect(watched, signal, G_CALLBACK(OnBufferChanged), &probe);
            else g_signal_connect(watched, signal, G_CALLBACK(OnNotify), &probe);
        }
        UmiStatus accountStatus = UMI_STATUS_OK;
        if (!strcmp(test, "public-read-close")) accountStatus = UmiIbkrGtkReadAccount(window, "DU123");
        else g_signal_emit_by_name(reading ? ui->read : disconnecting ? ui->disconnect : ui->connect, "clicked");
        if (probe.release) {
            CHECK(weakWindow == NULL && creations == 0U && transport.opens == 0U); return 0;
        }
        if (watched != NULL) CHECK(probe.calls > 0U);
        if (watched != NULL && !strcmp(probe.action, "close")) {
            CHECK(ui->closed && ui->connection == NULL && ui->source == 0U);
            if (reading || disconnecting || strstr(test, "close-output") || strstr(test, "close-status") ||
                strstr(test, "close-controls")) CHECK(transport.closes == 1U);
            else CHECK(transport.opens == 0U);
            if (!strcmp(test, "public-read-close")) CHECK(accountStatus == UMI_STATUS_CANCELLED);
        } else if (g_str_has_prefix(test, "connect-edit-")) {
            CHECK(creations == 0U && transport.opens == 0U && ui->connection == NULL);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(ui->status)), "Settings changed") != NULL);
        } else if (!strcmp(probe.action, "reenter")) {
            CHECK(creations == 1U && transport.opens == 1U && probe.nested == UMI_STATUS_BUSY);
            if (disconnecting) CHECK(ui->snapshot.state == UMI_IBKR_DISCONNECTED);
            else CHECK(ui->snapshot.state == (reading ? UMI_IBKR_READY : UMI_IBKR_CONNECTING));
        } else if (!strcmp(test, "connect-paper") || !strcmp(test, "connect-live") || !strcmp(test, "connect-active")) {
            CHECK(creations == 1U && transport.opens == 1U && ui->source != 0U);
            CHECK(captured.environment == (!strcmp(test, "connect-live") ? UMI_TRADING_LIVE : UMI_TRADING_PAPER));
            CHECK(captured.adapter.port == (!strcmp(test, "connect-live") ? 7496U : 7497U));
            if (!strcmp(test, "connect-active")) {
                g_signal_emit_by_name(ui->connect, "clicked"); CHECK(creations == 1U && transport.opens == 1U);
            }
        } else if (!strcmp(test, "connect-create-failure")) CHECK(creations == 1U && transport.opens == 0U);
        else CHECK(creations == 0U && transport.opens == 0U);
        CHECK(!ui->commandBusy);
    }
    gtk_window_destroy(window); g_object_unref(window);
    CHECK(weakWindow == NULL); return 0;
}

/* A timer publication is a command boundary too. The fixture retains real GTK
 * notifications while the in-memory transport prevents any broker contact. */
static int RunPollPublication(const char *test)
{
    GtkWindow *window = UmiIbkrGtkCreate(NULL);
    CHECK(window != NULL); g_object_ref(window);
    ConnectionUi *ui = Owner(window);
    CHECK(ui != NULL && Ready(ui) == 0);
    gpointer weakWindow = window;
    g_object_add_weak_pointer(G_OBJECT(window), &weakWindow);
    const gboolean failed = strstr(test, "failed") != NULL;
    const gboolean closing = strstr(test, "close") != NULL;
    const gboolean release = strstr(test, "release") != NULL;
    const uint64_t now = UmiIbkrMonotonicMilliseconds();
    ui->connection->snapshot.clockReceivedAtMilliseconds = now;
    ui->lastPaint = now >= 250U ? now - 250U : 0U;
    if (failed) transport.eof = true;
    GObject *watched;
    const char *signal = "notify::label";
    if (strstr(test, "output")) {
        g_clear_pointer(&ui->lastOutput, g_free);
        watched = G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->output)));
        signal = "changed";
    } else if (strstr(test, "accounts")) {
        ui->populated = FALSE;
        watched = G_OBJECT(ui->accounts);
        signal = "notify::model";
    } else if (strstr(test, "controls")) {
        gtk_widget_set_sensitive(ui->connect, failed ? FALSE : TRUE);
        watched = G_OBJECT(ui->connect);
        signal = "notify::sensitive";
    } else {
        gtk_label_set_text(GTK_LABEL(ui->status), "Previous status");
        watched = G_OBJECT(ui->status);
    }
    Probe probe = {window, ui, closing ? "close" : "reenter", 0U, release, UMI_STATUS_OK};
    gulong handler = !strcmp(signal, "changed")
        ? g_signal_connect(watched, signal, G_CALLBACK(OnBufferChanged), &probe)
        : g_signal_connect(watched, signal, G_CALLBACK(OnNotify), &probe);
    gboolean keep = Poll(window);
    CHECK(probe.calls > 0U);
    CHECK(creations == 1U && transport.opens == 1U);
    if (release) {
        CHECK(weakWindow == NULL && keep == G_SOURCE_REMOVE && transport.closes == 1U);
        return 0;
    }
    CHECK(!ui->commandBusy);
    if (closing) {
        CHECK(keep == G_SOURCE_REMOVE && ui->closed && ui->connection == NULL);
        CHECK(transport.closes == 1U);
    } else {
        CHECK(probe.nested == UMI_STATUS_BUSY);
        CHECK(ui->snapshot.state == (failed ? UMI_IBKR_FAILED : UMI_IBKR_READY));
        CHECK(keep == (failed ? G_SOURCE_REMOVE : G_SOURCE_CONTINUE));
        CHECK(!ui->snapshot.requestIssued && ui->source == 0U);
        CHECK(transport.closes == (failed ? 1U : 0U));
        g_signal_handler_disconnect(watched, handler);
        if (!strcmp(test, "poll-ready-followup")) {
            /* Reentrancy is blocked only during publication, not permanently.
             * An explicit action after Poll returns retains its normal authority. */
            CHECK(UmiIbkrGtkReadAccount(window, "DU123") == UMI_STATUS_OK);
            CHECK(ui->snapshot.requestIssued && !ui->commandBusy);
        } else if (!strcmp(test, "poll-failed-followup")) {
            transport.eof = false;
            g_signal_emit_by_name(ui->connect, "clicked");
            CHECK(creations == 2U && transport.opens == 2U);
            CHECK(ui->snapshot.state == UMI_IBKR_CONNECTING && ui->source != 0U);
        }
    }
    gtk_window_destroy(window); g_object_unref(window);
    CHECK(weakWindow == NULL);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    if (g_str_has_prefix(argv[1], "poll-ready-") ||
        g_str_has_prefix(argv[1], "poll-failed-") ||
        g_str_has_prefix(argv[1], "poll-close-")) return RunPollPublication(argv[1]);
    return Run(argv[1]);
}
