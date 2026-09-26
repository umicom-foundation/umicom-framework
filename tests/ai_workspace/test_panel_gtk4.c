/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_panel_gtk4.c
 *
 * PURPOSE:
 *   Exercise the real GTK source/job form and join-before-release lifecycle without any external model or process.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/ai_workspace.h"
#include <stdio.h>
#include <string.h>

#define REQUIRE(expression) do { if (!(expression)) { \
    fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); failed = 1; goto cleanup; \
} } while (0)

static GtkWidget *Find(GtkWidget *root, const char *id)
{
    if (root == NULL) return NULL;
    const char *actual = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (actual != NULL && strcmp(actual, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static bool SetEntry(GtkWidget *root, const char *id, const char *text)
{
    GtkWidget *entry = Find(root, id); if (!GTK_IS_ENTRY(entry)) return false;
    gtk_editable_set_text(GTK_EDITABLE(entry), text); return true;
}
static bool SetText(GtkWidget *root, const char *id, const char *text)
{
    GtkWidget *view = Find(root, id); if (!GTK_IS_TEXT_VIEW(view)) return false;
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), text, -1); return true;
}
static bool Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id); if (!GTK_IS_BUTTON(button)) return false;
    g_signal_emit_by_name(button, "clicked"); return true;
}
static bool WaitForIdle(GtkWidget *forms)
{
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (!gtk_widget_get_sensitive(forms) && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return gtk_widget_get_sensitive(forms);
}
typedef struct WaitingProvider {
    UmiAiWorkspaceCancellation *cancellation;
    gint entered, returned;
} WaitingProvider;
/* Deliberately inert provider fixture. It proves the GUI's own worker release
 * ordering; it neither contacts a service nor pretends to run a language model. */
static UmiStatus WaitGenerate(void *context, const UmiAiRequest *request, UmiAiResponse *response)
{
    WaitingProvider *provider = context; (void)request; (void)response;
    g_atomic_int_set(&provider->entered, 1);
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (!UmiAiWorkspaceCancellationIsRequested(provider->cancellation) && g_get_monotonic_time() < deadline)
        g_usleep(1000U);
    g_atomic_int_set(&provider->returned, 1);
    return UmiAiWorkspaceCancellationIsRequested(provider->cancellation) ? UMI_STATUS_CANCELLED : UMI_STATUS_TIMEOUT;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    bool closeDuring = strcmp(argv[1], "close_during_request") == 0;
    if (!closeDuring && strcmp(argv[1], "workflow") != 0) return 2;
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) return 77;
    UmiDataServer *data = NULL; UmiAiWorkspace *workspace = NULL; UmiAiWorkspaceGtkPanel *panel = NULL;
    UmiAiWorkspaceCancellation *cancellation = NULL; UmiAiRuntime runtime; UmiAiProvider provider = {0};
    UmiAiWorkspaceJob *job = g_new0(UmiAiWorkspaceJob, 1U);
    GtkWidget *root = NULL, *window = NULL, *forms = NULL, *retained = NULL;
    WaitingProvider waiting = {0}; UmiAiWorkspaceSnapshot snapshot = {0}; int failed = 0;
    umi_ai_runtime_init(&runtime);
    REQUIRE(UmiAiWorkspaceCancellationCreate(&cancellation) == UMI_STATUS_OK);
    REQUIRE(UmiAiWorkspaceExtractiveProviderCreate(&provider) == UMI_STATUS_OK);
    if (closeDuring) { waiting.cancellation = cancellation; provider.instance = &waiting; provider.generate = WaitGenerate; }
    REQUIRE(umi_ai_provider_registry_add(&runtime.providers, &provider) == UMI_STATUS_OK);
    REQUIRE(umi_data_server_create_memory(&data) == UMI_STATUS_OK);
    REQUIRE(UmiAiWorkspaceCreate(data, &runtime, "gui-test", &workspace) == UMI_STATUS_OK);
    REQUIRE(UmiAiWorkspaceGtkPanelCreate(workspace, &runtime, cancellation, "Memory-only graphical test",
        "org.umicom.rag", &panel) == UMI_STATUS_OK);
    root = UmiAiWorkspaceGtkPanelWidget(panel); REQUIRE(root != NULL);
    window = gtk_window_new(); g_object_ref(window); gtk_window_set_child(GTK_WINDOW(window), root);
    /* No presentation or GTK application loop is required to test form signals. */
    REQUIRE(!gtk_widget_get_realized(window));
    REQUIRE(umi_data_server_count(data) == 0U);
    forms = Find(root, "ai.workspace.forms"); REQUIRE(forms != NULL);
    REQUIRE(Click(root, "ai.collection.save"));
    REQUIRE(SetEntry(root, "ai.source.line", "7"));
    REQUIRE(SetText(root, "ai.source.text", "The community workshop opens at 10:00 on Saturday.\nBring a notebook."));
    REQUIRE(Click(root, "ai.source.save"));
    REQUIRE(Click(root, "ai.search.run"));
    REQUIRE(SetEntry(root, "ai.job.id", "gui.draft"));
    REQUIRE(Click(root, "ai.job.prepare"));
    REQUIRE(UmiAiWorkspaceJobFind(workspace, "gui.draft", job) == UMI_STATUS_OK);
    REQUIRE(job->state == UMI_AI_WORKSPACE_REVIEW && job->evidenceCount == 1U);
    REQUIRE(job->evidence[0].source.firstLine == 7U && job->evidence[0].source.lastLine == 8U);
    REQUIRE(!gtk_widget_get_sensitive(Find(root, "ai.job.run")));
    /* Editing another ID does not silently redirect approval to a different
     * record: the selected frozen job remains the explicit review target. */
    REQUIRE(SetEntry(root, "ai.job.id", "unprepared.other"));
    REQUIRE(Click(root, "ai.job.approve"));
    REQUIRE(UmiAiWorkspaceJobFind(workspace, "gui.draft", job) == UMI_STATUS_OK);
    REQUIRE(job->state == UMI_AI_WORKSPACE_APPROVED);
    retained = g_object_ref(Find(root, "ai.job.run"));
    REQUIRE(Click(root, "ai.job.run"));
    REQUIRE(!gtk_widget_get_sensitive(forms));
    if (closeDuring) {
        gint64 deadline = g_get_monotonic_time() + 1000000;
        while (!g_atomic_int_get(&waiting.entered) && g_get_monotonic_time() < deadline) g_usleep(1000U);
        REQUIRE(g_atomic_int_get(&waiting.entered));
        gtk_window_set_child(GTK_WINDOW(window), NULL);
        UmiAiWorkspaceGtkPanelDestroy(panel); panel = NULL; root = NULL;
        REQUIRE(g_atomic_int_get(&waiting.returned));
        REQUIRE(UmiAiWorkspaceJobFind(workspace, "gui.draft", job) == UMI_STATUS_OK);
        REQUIRE(job->state == UMI_AI_WORKSPACE_CANCELLED);
    } else {
        REQUIRE(WaitForIdle(forms));
        REQUIRE(UmiAiWorkspaceJobFind(workspace, "gui.draft", job) == UMI_STATUS_OK);
        REQUIRE(job->state == UMI_AI_WORKSPACE_SUCCEEDED && strstr(job->response.text, "10:00") != NULL);
        REQUIRE(strstr(job->response.text, "not a language-model answer") != NULL);
        REQUIRE(UmiAiWorkspaceSnapshotRead(workspace, &snapshot) == UMI_STATUS_OK);
        uint64_t revision = snapshot.revision;
        REQUIRE(Click(root, "ai.job.run")); REQUIRE(WaitForIdle(forms));
        REQUIRE(UmiAiWorkspaceSnapshotRead(workspace, &snapshot) == UMI_STATUS_OK && snapshot.revision == revision);
        gtk_window_set_child(GTK_WINDOW(window), NULL);
        UmiAiWorkspaceGtkPanelDestroy(panel); panel = NULL; root = NULL;
    }
    /* The button deliberately outlives its controller. Its closure must have
     * disconnected before the borrowed workspace is freed. */
    UmiAiWorkspaceDestroy(workspace); workspace = NULL;
    g_signal_emit_by_name(retained, "clicked");
    while (g_main_context_iteration(NULL, FALSE)) {}
cleanup:
    if (window != NULL) gtk_window_set_child(GTK_WINDOW(window), NULL);
    UmiAiWorkspaceGtkPanelDestroy(panel);
    if (retained != NULL) g_object_unref(retained);
    if (window != NULL) { gtk_window_destroy(GTK_WINDOW(window)); g_object_unref(window); }
    UmiAiWorkspaceDestroy(workspace); umi_ai_runtime_destroy(&runtime);
    UmiAiWorkspaceCancellationDestroy(cancellation); umi_data_server_destroy(data); g_free(job);
    return failed;
}
