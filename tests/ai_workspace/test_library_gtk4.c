/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_library_gtk4.c
 * PURPOSE: Check AI library switching without discarding a usable session on failure.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/ai_workspace.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "Line %d: %s\n", __LINE__, #condition); result = 1; goto cleanup; \
} } while (0)
/* Logical lookup includes collapsed expanders and inactive notebook pages;
 * widget rendering order is not part of the application's control contract. */
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    return umi_gtk4_automation_find_tagged_widget(root, id);
}
int main(void)
{
    (void)g_setenv("UMICOM_AI_WORKSPACE_PORT", "8080", TRUE);
    if (!gtk_init_check()) return 77;
    int result = 0;
    GtkWidget *reportExport = NULL;
    GWeakRef historyRoot; g_weak_ref_init(&historyRoot, NULL);
    char *directory = g_dir_make_tmp("umicom-ai-libraries-XXXXXX", NULL);
    char *first = directory != NULL ? g_build_filename(directory, "first.sqlite3", NULL) : NULL;
    char *second = directory != NULL ? g_build_filename(directory, "second.sqlite3", NULL) : NULL;
    char *missing = directory != NULL ? g_build_filename(directory, "missing.sqlite3", NULL) : NULL;
    char *historyFile = directory != NULL ? g_build_filename(directory, "recent-libraries.txt", NULL) : NULL;
    char *previousHistory = g_strdup(g_getenv("UMICOM_LIBRARY_HISTORY_FILE"));
    UmiAiWorkspaceGtkSurface *surface = NULL;
    GtkWidget *original = gtk_label_new("Original product layout"); g_object_ref_sink(original);
    CHECK(directory != NULL);
    /* A regression must never read or overwrite the owner's remembered work. */
    CHECK(g_setenv("UMICOM_LIBRARY_HISTORY_FILE", historyFile, TRUE));
    CHECK(UmiAiWorkspaceGtkSurfaceCreate(original, "org.umicom.llm", &surface) == UMI_STATUS_OK);
    GtkWidget *root = UmiAiWorkspaceGtkSurfaceWidget(surface);
    g_weak_ref_set(&historyRoot, Find(root, "workspace.library"));
    CHECK(Find(root, "ai.job.prompt") == NULL);
    CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, missing, false) == UMI_STATUS_NOT_FOUND);
    CHECK(!g_file_test(missing, G_FILE_TEST_EXISTS));
    UmiStatus status = UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, first, true);
    if (status == UMI_STATUS_UNAVAILABLE) { result = 77; goto cleanup; }
    CHECK(status == UMI_STATUS_OK);
    /* Save actual sources, then verify that changing libraries isolates them
     * and reopening the first library recovers them through the public panel. */
    CHECK(Find(root, "text.export") != NULL);
    reportExport = g_object_ref(Find(root, "text.export"));
    GtkWidget *saveCollection = Find(root, "ai.collection.save");
    GtkWidget *saveSource = Find(root, "ai.source.save");
    CHECK(saveCollection != NULL && saveSource != NULL);
    GtkWidget *passage = Find(root, "ai.source.text"); CHECK(passage != NULL);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(passage)),
        "The workshop opens at nine each weekday.", -1);
    g_signal_emit_by_name(saveCollection, "clicked");
    g_signal_emit_by_name(saveSource, "clicked");
    GtkWidget *prompt = Find(root, "ai.job.prompt");
    CHECK(prompt != NULL);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(prompt)), "Keep this unsaved question.", -1);
    CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, missing, false) == UMI_STATUS_NOT_FOUND);
    CHECK(Find(root, "ai.job.prompt") == prompt);
    CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, first, false) == UMI_STATUS_OK);
    CHECK(Find(root, "ai.job.prompt") == prompt);
    CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, first, true) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, second, true) == UMI_STATUS_OK);
    /* Switching databases retires the old draft exporter instead of borrowing
     * the newly selected library through a retained widget. */
    CHECK(!gtk_widget_get_sensitive(reportExport));
    GtkWidget *catalogue = Find(root, "ai.source.catalogue");
    CHECK(catalogue != NULL);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(catalogue));
    GtkTextIter firstLine, lastLine;
    gtk_text_buffer_get_bounds(buffer, &firstLine, &lastLine);
    char *contents = gtk_text_buffer_get_text(buffer, &firstLine, &lastLine, FALSE);
    bool empty = strstr(contents, "Opening notice") == NULL;
    g_free(contents);
    CHECK(empty);
    CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, first, false) == UMI_STATUS_OK);
    catalogue = Find(root, "ai.source.catalogue"); CHECK(catalogue != NULL);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(catalogue));
    gtk_text_buffer_get_bounds(buffer, &firstLine, &lastLine);
    contents = gtk_text_buffer_get_text(buffer, &firstLine, &lastLine, FALSE);
    bool restored = strstr(contents, "Opening notice") != NULL;
    g_free(contents);
    CHECK(restored);
    /* Destroying the extension must leave the product's layout reusable. */
    UmiAiWorkspaceGtkSurfaceDestroy(surface); surface = NULL;
    CHECK(gtk_widget_get_parent(original) == NULL);
cleanup:
    g_clear_object(&reportExport);
    UmiAiWorkspaceGtkSurfaceDestroy(surface);
    g_object_unref(original);
    if (first != NULL) { (void)g_remove(first); g_free(first); }
    if (second != NULL) { (void)g_remove(second); g_free(second); }
    g_free(missing);
    /* A closed selector may still own one accepted metadata operation. Wait
     * for its root to retire before removing this test's private preferences. */
    gint64 retirementEnd = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    bool historyRetired = false;
    while (g_get_monotonic_time() < retirementEnd) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        GObject *alive = g_weak_ref_get(&historyRoot);
        if (alive == NULL) { historyRetired = true; break; }
        g_object_unref(alive); g_usleep(1000U);
    }
    g_weak_ref_clear(&historyRoot);
    if (!historyRetired) { fprintf(stderr, "History worker did not retire.\n"); result = 1; }
    if (historyRetired && historyFile != NULL) (void)g_remove(historyFile);
    if (previousHistory != NULL) (void)g_setenv("UMICOM_LIBRARY_HISTORY_FILE", previousHistory, TRUE);
    else g_unsetenv("UMICOM_LIBRARY_HISTORY_FILE");
    g_free(previousHistory); g_free(historyFile);
    if (directory != NULL) { if (historyRetired) (void)g_rmdir(directory); g_free(directory); }
    return result;
}
