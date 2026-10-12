/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_evidence/test_export_gtk.c
 * PURPOSE: Verify explicit AI report exports and retained controls without contacting a model service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ai_workspace/evidence_export.h"
#include "umicom/ui/gtk4/ai_workspace.h"
#include "umicom/ui/gtk4/automation.h"
#include <glib/gstdio.h>

static GtkWidget *Find(GtkWidget *root, const char *id)
{
    return umi_gtk4_automation_find_tagged_widget(root, id);
}
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id);
    if (button != NULL) g_signal_emit_by_name(button, "clicked");
}
static gboolean Wait(GtkWidget *status)
{
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        if (strstr(gtk_label_get_text(GTK_LABEL(status)), "Job job:") != NULL) return TRUE;
        g_usleep(1000U);
    }
    return FALSE;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    Fixture fixture;
    CHECK(OpenFixture(&fixture, NULL) == 0);
    CHECK(RunFixture(&fixture) == 0); /* Pure extractive fixture; no network call. */
    UmiAiWorkspaceCancellation *cancel = NULL;
    OK(UmiAiWorkspaceCancellationCreate(&cancel));
    UmiAiWorkspaceGtkPanel *panel = NULL;
    OK(UmiAiWorkspaceGtkPanelCreate(fixture.workspace, &fixture.runtime, cancel,
        "Memory-only export fixture", "org.umicom.rag", &panel));
    GtkWidget *root = UmiAiWorkspaceGtkPanelWidget(panel);
    GtkWidget *pathEntry = Find(root, "ai.evidence.export.path");
    GtkWidget *save = Find(root, "ai.evidence.export.save");
    GtkWidget *status = Find(root, "ai.evidence.export.status");
    CHECK(pathEntry != NULL && save != NULL && status != NULL);
    char *directory = g_dir_make_tmp("umicom-ai-export-XXXXXX", NULL);
    CHECK(directory != NULL);
    char *path = g_build_filename(directory, "inspection caf\xc3\xa9.txt", NULL);
    gtk_editable_set_text(GTK_EDITABLE(pathEntry), path);
    gtk_editable_set_text(GTK_EDITABLE(Find(root, "ai.job.id")), "job");
    Click(root, "ai.job.load");
    UmiAiWorkspaceSnapshot before, after;
    OK(UmiAiWorkspaceSnapshotRead(fixture.workspace, &before));
    if (strcmp(argv[1], "retained") == 0) {
        g_object_ref(save);
        UmiAiWorkspaceGtkPanelDestroy(panel); panel = NULL;
        g_signal_emit_by_name(save, "clicked");
        CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
        g_object_unref(save);
    } else if (strcmp(argv[1], "invalid") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(pathEntry), "relative.txt");
        g_signal_emit_by_name(save, "clicked");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(status)), "not started") != NULL);
        CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    } else {
        gboolean collision = strcmp(argv[1], "collision") == 0;
        CHECK(collision || strcmp(argv[1], "save") == 0);
        if (collision) CHECK(g_file_set_contents(path, "Existing work", -1, NULL));
        UmiAiEvidenceReview *review = NULL;
        OK(UmiAiEvidenceCapture(fixture.workspace, "job", &review));
        char *expected = g_malloc(UMI_AI_EVIDENCE_REPORT_CAPACITY);
        size_t length = 0U;
        OK(UmiAiEvidenceFormat(review, expected, UMI_AI_EVIDENCE_REPORT_CAPACITY, &length));
        g_signal_emit_by_name(save, "clicked");
        CHECK(Wait(status));
        char *actual = NULL;
        gsize bytes = 0U;
        CHECK(g_file_get_contents(path, &actual, &bytes, NULL));
        CHECK(collision ? strcmp(actual, "Existing work") == 0 :
            (bytes == length && memcmp(actual, expected, length) == 0));
        CHECK(collision ? strstr(gtk_label_get_text(GTK_LABEL(status)), "not replaced") != NULL :
            strstr(gtk_label_get_text(GTK_LABEL(status)), "Saved the captured") != NULL);
        g_free(actual); g_free(expected); UmiAiEvidenceDestroy(review);
        CHECK(g_remove(path) == 0);
    }
    OK(UmiAiWorkspaceSnapshotRead(fixture.workspace, &after));
    CHECK(before.revision == after.revision);
    UmiAiWorkspaceGtkPanelDestroy(panel);
    UmiAiWorkspaceCancellationDestroy(cancel);
    CloseFixture(&fixture);
    CHECK(g_rmdir(directory) == 0);
    g_free(path); g_free(directory);
    return 0;
}
