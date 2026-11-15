/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_reviewed_panel_gtk4.c
 *
 * PURPOSE:
 *   Exercise the existing GTK reviewed-stream controls with real loopback transport and retained teardown failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/ai_workspace.h"
#include "umicom/platform/threading.h"
#include "local_http_peer.h"
#include "local_chat_stream_fixture.h"
#include <glib/gstdio.h>

static bool fail_finish, fail_release, fail_poll, fail_copy, fail_try, fail_start;
static UmiStatus CheckedStart(UmiAiWorkspaceStream *stream)
{
    if (fail_start) { fail_start = false; return UMI_STATUS_IO_ERROR; }
    return UmiAiWorkspaceStreamStart(stream);
}
static UmiStatus CheckedFinish(UmiAiWorkspaceStream *stream)
{
    if (fail_finish) { fail_finish = false; return UMI_STATUS_IO_ERROR; }
    return UmiAiWorkspaceStreamFinish(stream);
}
static UmiStatus CheckedRelease(UmiAiWorkspaceStream **stream)
{
    if (fail_release) { fail_release = false; return UMI_STATUS_IO_ERROR; }
    return UmiAiWorkspaceStreamDestroy(stream);
}
static UmiStatus CheckedPoll(UmiAiWorkspaceStream *stream, UmiAiWorkspaceStreamSnapshot *snapshot)
{
    if (fail_poll) { fail_poll = false; return UMI_STATUS_IO_ERROR; }
    return UmiAiWorkspaceStreamPoll(stream, snapshot);
}
static UmiStatus CheckedCopy(UmiAiWorkspaceStream *stream, char **text, UmiAiResponseStreamSnapshot *snapshot)
{
    if (fail_copy) { fail_copy = false; return UMI_STATUS_OUT_OF_MEMORY; }
    return UmiAiWorkspaceStreamCopyText(stream, text, snapshot);
}

static size_t range_expected_offset;
static unsigned range_calls;
static bool range_contract_failed;
static UmiStatus CheckedCopyRange(UmiAiWorkspaceStream *stream, size_t offset,
    size_t maximum, char **text, size_t *copied, UmiAiResponseStreamSnapshot *snapshot)
{
    ++range_calls;
    if (offset != range_expected_offset || maximum != 4096U) range_contract_failed = true;
    if (fail_copy) { fail_copy = false; return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status = UmiAiWorkspaceStreamCopyTextRange(stream, offset, maximum, text, copied, snapshot);
    if (status == UMI_STATUS_OK) {
        if (*copied > maximum || strlen(*text) != *copied ||
            offset + *copied > snapshot->text_bytes) range_contract_failed = true;
        range_expected_offset += *copied;
    }
    return status;
}
static void CountDraftChange(GtkTextBuffer *buffer, gpointer context)
{
    (void)buffer;
    ++*(unsigned *)context;
}

static UmiStatus CheckedTry(UmiAiWorkspaceStream *stream)
{
    if (fail_try) { fail_try = false; return UMI_STATUS_IO_ERROR; }
    return UmiAiWorkspaceStreamTryFinish(stream);
}
/* Only controller failure returns are injected. Compile the actual panel and
 * its fragments; workspace, reviewed controller, HTTP peer and native job run
 * through their real implementation. No model process is involved. */
#define UmiAiWorkspaceStreamStart CheckedStart
#define UmiAiWorkspaceStreamFinish CheckedFinish
#define UmiAiWorkspaceStreamDestroy CheckedRelease
#define UmiAiWorkspaceStreamPoll CheckedPoll
#define UmiAiWorkspaceStreamCopyText CheckedCopy
#define UmiAiWorkspaceStreamCopyTextRange CheckedCopyRange
#define UmiAiWorkspaceStreamTryFinish CheckedTry
#include "../../adapters/gtk4/ai_workspace_panel_gtk4.c"
#undef UmiAiWorkspaceStreamTryFinish
#undef UmiAiWorkspaceStreamCopyTextRange
#undef UmiAiWorkspaceStreamCopyText
#undef UmiAiWorkspaceStreamPoll
#undef UmiAiWorkspaceStreamDestroy
#undef UmiAiWorkspaceStreamFinish
#undef UmiAiWorkspaceStreamStart

static GtkWidget *FindControl(GtkWidget *root, const char *id)
{
    if (root == NULL) return NULL;
    const char *actual = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (actual != NULL && strcmp(actual, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = FindControl(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
typedef struct Reentry {
    UmiAiWorkspaceGtkPanel *panel;
    unsigned calls;
    bool retained;
} Reentry;
static void ReentrantClose(GtkTextBuffer *buffer, gpointer context)
{
    (void)buffer;
    Reentry *check = context;
    UmiAiWorkspaceGtkPanel *alias = check->panel;
    UmiStatus status = UmiAiWorkspaceGtkPanelRelease(&alias);
    ++check->calls;
    check->retained = check->retained && status == UMI_STATUS_BUSY && alias == check->panel;
}
typedef struct WrongOwner {
    UmiAiWorkspaceGtkPanel *panel;
    UmiAiLocalTransport *transport;
    UmiStatus attach, release;
    bool retained;
} WrongOwner;
static int WrongThread(void *context)
{
    WrongOwner *check = context;
    UmiAiWorkspaceGtkPanel *alias = check->panel;
    check->attach = UmiAiWorkspaceGtkPanelEnableLocalStreaming(check->panel, check->transport);
    check->release = UmiAiWorkspaceGtkPanelRelease(&alias);
    check->retained = alias == check->panel;
    return 0;
}
static bool AwaitRequest(LocalHttpPeer *peer)
{
    gint64 deadline = g_get_monotonic_time() + 3000000;
    while (!LocalHttpPeerHasRequest(peer) && g_get_monotonic_time() < deadline) g_usleep(1000U);
    return LocalHttpPeerHasRequest(peer);
}
static bool AwaitIdle(UmiAiWorkspaceGtkPanel *panel)
{
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (UmiAiWorkspaceGtkPanelBusy(panel) && g_get_monotonic_time() < deadline) {
        /* Bound iterations so another idle source cannot monopolize the fixture. */
        (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return !UmiAiWorkspaceGtkPanelBusy(panel);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    const char *const cases[] = {"arguments", "wrong-owner", "success", "cancel", "close-active",
        "join-failure", "release-failure", "poll-failure", "copy-failure", "try-failure", "reentrant-close", "publication-failure", "start-refusal", "range-append"};
    size_t selected = sizeof(cases) / sizeof(cases[0]);
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0) selected = i;
    if (selected == sizeof(cases) / sizeof(cases[0])) return 2;
    bool start_refusal = strcmp(mode, "start-refusal") == 0;
    bool peer_case = selected >= 2U && !start_refusal;
    bool sql = strcmp(mode, "publication-failure") == 0;
    UmiAiLocalTransport *transport = NULL;
    UmiStatus status = UmiAiLocalTransportCreate(&transport);
    if (status == UMI_STATUS_UNAVAILABLE) return 77;
    CHECK(status == UMI_STATUS_OK);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) { CHECK(UmiAiLocalTransportDestroy(&transport) == UMI_STATUS_OK); return 77; }
    char *directory = sql ? g_dir_make_tmp("umicom-reviewed-stream-XXXXXX", NULL) : NULL;
    char *database = sql && directory != NULL ? g_build_filename(directory, "jobs.sqlite", NULL) : NULL;
    CHECK(!sql || database != NULL);
    UmiDataServer *data = NULL;
    status = sql ? umi_data_server_create_sqlite(database, &data) : umi_data_server_create_memory(&data);
    if (status == UMI_STATUS_UNAVAILABLE) {
        g_free(database); if (directory != NULL) { (void)g_rmdir(directory); g_free(directory); }
        CHECK(UmiAiLocalTransportDestroy(&transport) == UMI_STATUS_OK); return 77;
    }
    CHECK(status == UMI_STATUS_OK);
    UmiAiWorkspaceCancellation *token = NULL;
    CHECK(UmiAiWorkspaceCancellationCreate(&token) == UMI_STATUS_OK);
    LocalHttpPeer *peer = NULL;
    uint16_t port = 8080U;
    const char *body = TEXT_HELLO STOP DONE;
    char response[1024];
    if (peer_case) {
        int length = snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\nContent-Type: text/event-stream\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n%s", strlen(body), body);
        CHECK(length > 0 && (size_t)length < sizeof(response));
        LocalHttpPeerOptions peer_options = {.response=response, .response_bytes=(size_t)length,
            .fragment_bytes=16384U, .delay_millis=200U, .allow_disconnect=true};
        CHECK(LocalHttpPeerStart(&peer_options, &peer, &port) == UMI_STATUS_OK);
    }
    UmiAiRuntime runtime;
    umi_ai_runtime_init(&runtime);
    UmiAiProvider provider = {0};
    CHECK(UmiAiWorkspaceLocalProviderCreate(port, 2000U, token, &provider) == UMI_STATUS_OK);
    CHECK(umi_ai_provider_registry_add(&runtime.providers, &provider) == UMI_STATUS_OK);
    UmiAiWorkspace *workspace = NULL;
    CHECK(UmiAiWorkspaceCreate(data, &runtime, "panel.streaming", &workspace) == UMI_STATUS_OK);
    UmiAiWorkspaceGtkPanel *panel = NULL;
    CHECK(UmiAiWorkspaceGtkPanelCreate(workspace, &runtime, token, "Isolated native loopback fixture",
        "org.umicom.llm", &panel) == UMI_STATUS_OK);
    GtkWidget *root = UmiAiWorkspaceGtkPanelWidget(panel);
    CHECK(!gtk_widget_get_realized(root));
    CHECK(UmiAiWorkspaceGtkPanelEnableLocalStreaming(NULL, transport) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiWorkspaceGtkPanelEnableLocalStreaming(panel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiWorkspaceGtkPanelEnableLocalStreaming(panel, transport) == UMI_STATUS_OK);
    size_t count = 99U;
    CHECK(UmiAiLocalTransportRequestCount(transport, &count) == UMI_STATUS_OK && count == 0U);
    if (strcmp(mode, "wrong-owner") == 0) {
        WrongOwner check = {.panel=panel, .transport=transport};
        UmiThread *thread = NULL; int result = 0;
        CHECK(umi_thread_start(WrongThread, &check, &thread) == UMI_STATUS_OK);
        CHECK(umi_thread_join(thread, &result) == UMI_STATUS_OK && result == 0);
        CHECK(UmiThreadRelease(&thread) == UMI_STATUS_OK);
        CHECK(check.attach == UMI_STATUS_INVALID_STATE && check.release == UMI_STATUS_INVALID_STATE && check.retained);
    }
    GtkWidget *retained_run = g_object_ref(FindControl(root, "ai.job.run"));
    CHECK(retained_run != NULL);
    if (start_refusal) {
        CHECK(UmiAiWorkspacePrepare(workspace, "unstarted.job", UMI_AI_WORKSPACE_DRAFT,
            "umicom.local-chat", "test-model", NULL, "Do not contact a peer.", "writer", 64U) == UMI_STATUS_OK);
        CHECK(UmiAiWorkspaceReview(workspace, "unstarted.job", "reviewer", true) == UMI_STATUS_OK);
        gtk_editable_set_text(GTK_EDITABLE(FindControl(root, "ai.job.id")), "unstarted.job");
        g_signal_emit_by_name(FindControl(root, "ai.job.load"), "clicked");
        fail_start = true;
        g_signal_emit_by_name(retained_run, "clicked");
        CHECK(panel->stream != NULL && panel->worker == NULL);
        CHECK(AwaitIdle(panel));
        CHECK(strstr(gtk_label_get_text(panel->message), "no generated result was saved") != NULL);
        UmiAiWorkspaceJob *job = calloc(1U, sizeof(*job)); CHECK(job != NULL);
        CHECK(UmiAiWorkspaceJobFind(workspace, "unstarted.job", job) == UMI_STATUS_OK &&
            job->state == UMI_AI_WORKSPACE_APPROVED && job->response.text[0] == '\0');
        free(job);
        CHECK(UmiAiLocalTransportRequestCount(transport, &count) == UMI_STATUS_OK && count == 0U);
    }
    if (peer_case) {
        CHECK(UmiAiWorkspacePrepare(workspace, "panel.job", UMI_AI_WORKSPACE_DRAFT,
            "umicom.local-chat", "test-model", NULL, "Introduce native ownership.", "writer", 64U) == UMI_STATUS_OK);
        CHECK(UmiAiWorkspaceReview(workspace, "panel.job", "reviewer", true) == UMI_STATUS_OK);
        gtk_editable_set_text(GTK_EDITABLE(FindControl(root, "ai.job.id")), "panel.job");
        g_signal_emit_by_name(FindControl(root, "ai.job.load"), "clicked");
        Reentry reentry = {.panel=panel, .retained=true};
        gulong reentry_handler = strcmp(mode, "reentrant-close") == 0
            ? g_signal_connect(panel->review, "changed", G_CALLBACK(ReentrantClose), &reentry) : 0U;
        g_signal_emit_by_name(retained_run, "clicked");
        CHECK(panel->stream != NULL && panel->worker == NULL);
        CHECK(UmiAiWorkspaceGtkPanelBusy(panel) && !gtk_widget_get_sensitive(panel->forms));
        CHECK(UmiAiWorkspaceGtkPanelEnableLocalStreaming(panel, transport) == UMI_STATUS_BUSY);
        UmiAiWorkspace *alias = workspace;
        CHECK(UmiAiWorkspaceRelease(&alias) == UMI_STATUS_BUSY && alias == workspace);
        CHECK(AwaitRequest(peer));
        if (strcmp(mode, "range-append") == 0) {
            /* Join and settle the real bounded fixture before checking append
             * behavior, so the assertions do not depend on timer scheduling. */
            CHECK(UmiAiWorkspaceStreamFinish(panel->stream) == UMI_STATUS_OK);
            panel->streamCallbackActive = true;
            CHECK(AiStreamAppend(panel) == UMI_STATUS_OK && panel->streamDisplayedBytes == 5U);
            unsigned changed = 0U;
            gulong handler = g_signal_connect(panel->review, "changed", G_CALLBACK(CountDraftChange), &changed);
            CHECK(AiStreamAppend(panel) == UMI_STATUS_OK && panel->streamDisplayedBytes == 5U);
            g_signal_handler_disconnect(panel->review, handler);
            CHECK(changed == 0U);
            GtkTextIter first, last;
            gtk_text_buffer_get_bounds(panel->review, &first, &last);
            char *draft = gtk_text_buffer_get_text(panel->review, &first, &last, FALSE);
            CHECK(g_str_has_prefix(draft, "Streaming draft") && g_str_has_suffix(draft, "Hello"));
            g_free(draft);
            panel->streamCallbackActive = false;
        }
        if (sql) CHECK(umi_data_server_execute(data,
            "CREATE TRIGGER reject_panel_result BEFORE INSERT ON umicom_kv BEGIN SELECT RAISE(ABORT, 'injected'); END;") == UMI_STATUS_OK);
        bool closing = strcmp(mode, "close-active") == 0 || strcmp(mode, "join-failure") == 0 || strcmp(mode, "release-failure") == 0;
        if (closing) {
            fail_finish = strcmp(mode, "join-failure") == 0;
            fail_release = strcmp(mode, "release-failure") == 0;
            status = UmiAiWorkspaceGtkPanelRelease(&panel);
            if (strcmp(mode, "close-active") != 0) {
                CHECK(status == UMI_STATUS_IO_ERROR && panel != NULL && panel->closed);
                CHECK(UmiAiWorkspaceGtkPanelBusy(panel));
                CHECK(UmiAiWorkspaceRelease(&alias) == UMI_STATUS_BUSY);
                CHECK(UmiAiLocalTransportRequestCount(transport, &count) == UMI_STATUS_OK && count == 1U);
                g_signal_emit_by_name(retained_run, "clicked"); /* Disconnected, no second job. */
                CHECK(UmiAiWorkspaceGtkPanelRelease(&panel) == UMI_STATUS_OK);
            } else CHECK(status == UMI_STATUS_OK);
            CHECK(panel == NULL);
        } else {
            if (strcmp(mode, "cancel") == 0) g_signal_emit_by_name(FindControl(root, "ai.job.cancel"), "clicked");
            fail_poll = strcmp(mode, "poll-failure") == 0;
            fail_copy = strcmp(mode, "copy-failure") == 0;
            fail_try = strcmp(mode, "try-failure") == 0;
            CHECK(AwaitIdle(panel));
            CHECK(panel->stream == NULL && gtk_widget_get_sensitive(panel->forms));
            CHECK(strstr(gtk_label_get_text(panel->message), "Save:") != NULL);
            CHECK(range_calls > 0U && !range_contract_failed);
            if (reentry_handler != 0U) {
                g_signal_handler_disconnect(panel->review, reentry_handler);
                CHECK(reentry.calls >= 2U && reentry.retained);
            }
            CHECK(UmiAiWorkspaceGtkPanelRelease(&panel) == UMI_STATUS_OK);
        }
        UmiWebRequest *request = calloc(1U, sizeof(*request)); CHECK(request != NULL);
        CHECK(LocalHttpPeerFinish(&peer, request) == UMI_STATUS_OK);
        CHECK(request->method == UMI_HTTP_METHOD_POST && strcmp(request->path, "/v1/chat/completions") == 0);
        free(request);
        UmiAiWorkspaceJob *job = calloc(1U, sizeof(*job)); CHECK(job != NULL);
        if (sql) {
            UmiAiWorkspaceSnapshot snapshot;
            CHECK(UmiAiWorkspaceSnapshotRead(workspace, &snapshot) == UMI_STATUS_OK && snapshot.recoveryRequired);
            CHECK(umi_data_server_execute(data, "DROP TRIGGER reject_panel_result;") == UMI_STATUS_OK);
            CHECK(UmiAiWorkspaceReload(workspace) == UMI_STATUS_OK);
            CHECK(UmiAiWorkspaceJobFind(workspace, "panel.job", job) == UMI_STATUS_OK && job->state == UMI_AI_WORKSPACE_INTERRUPTED);
        } else {
            CHECK(UmiAiWorkspaceJobFind(workspace, "panel.job", job) == UMI_STATUS_OK);
            bool cancelled = closing || strcmp(mode, "cancel") == 0 || strcmp(mode, "poll-failure") == 0 ||
                strcmp(mode, "copy-failure") == 0 || strcmp(mode, "try-failure") == 0;
            CHECK(job->state == (cancelled ? UMI_AI_WORKSPACE_CANCELLED : UMI_AI_WORKSPACE_SUCCEEDED));
            CHECK(cancelled ? job->response.text[0] == '\0' : strcmp(job->response.text, "Hello") == 0);
        }
        free(job);
    }
    CHECK(UmiAiWorkspaceGtkPanelRelease(&panel) == UMI_STATUS_OK);
    CHECK(UmiAiWorkspaceRelease(&workspace) == UMI_STATUS_OK);
    g_signal_emit_by_name(retained_run, "clicked");
    (void)g_main_context_iteration(NULL, FALSE);
    g_object_unref(retained_run);
    CHECK(UmiAiRuntimeRelease(&runtime) == UMI_STATUS_OK);
    UmiAiWorkspaceCancellationDestroy(token);
    umi_data_server_destroy(data);
    CHECK(UmiAiLocalTransportDestroy(&transport) == UMI_STATUS_OK);
    if (sql) { CHECK(g_remove(database) == 0); CHECK(g_rmdir(directory) == 0); }
    g_free(database); g_free(directory);
    return 0;
}
