/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_library/test_history_gtk4.c
 * PURPOSE: Verify recent-library persistence, explicit opening and late retirement.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workspace_library.h"
#include "umicom/ui/gtk4/automation.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "Line %d: %s\n", __LINE__, #c); result = 1; goto cleanup; } } while (0)
typedef struct Probe { unsigned calls; UmiStatus status; } Probe;
static UmiStatus Open(const char *path, bool create, void *context)
{
    (void)path; (void)create; Probe *probe = context; ++probe->calls; return probe->status;
}
static GtkWidget *Find(UmiWorkspaceLibraryGtk *library, const char *id)
{ return umi_gtk4_automation_find_tagged_widget(UmiWorkspaceLibraryGtkWidget(library), id); }
/* The owner-run regression pumps completions with a deadline; it does not rely
 * on an arbitrary sleep to decide whether a worker finished its file operation. */
static bool Wait(UmiWorkspaceLibraryGtk *library)
{
    gint64 end = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (UmiWorkspaceLibraryGtkHistoryBusy(library) && g_get_monotonic_time() < end) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return !UmiWorkspaceLibraryGtkHistoryBusy(library);
}
static guint Count(UmiWorkspaceLibraryGtk *library)
{
    GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(Find(library, "workspace.library.history.list")));
    return model ? g_list_model_get_n_items(model) : 0U;
}
int main(void)
{
    if (!gtk_init_check()) return 77;
    int result = 0;
    Probe probe = {0U, UMI_STATUS_OK};
    UmiWorkspaceLibraryGtk *library = NULL;
    char *directory = g_dir_make_tmp("umicom-history-ui-XXXXXX", NULL);
    char *preferences = directory ? g_build_filename(directory, "preferences", NULL) : NULL;
    char *settings = preferences ? g_build_filename(preferences, "recent.txt", NULL) : NULL;
    char *first = directory ? g_build_filename(directory, "caf\xc3\xa9.sqlite3", NULL) : NULL;
    char *second = directory ? g_build_filename(directory, "another.sqlite3", NULL) : NULL;
    char *text = NULL; gsize length = 0U;
    GtkWidget *retained = NULL;
    CHECK(directory && preferences && settings && first && second);
    CHECK(g_file_set_contents(first, "saved project", -1, NULL));
    CHECK(UmiWorkspaceLibraryGtkCreate(Open, &probe, &library) == UMI_STATUS_OK);
    CHECK(UmiWorkspaceLibraryGtkEnableHistory(library, settings) == UMI_STATUS_OK);
    CHECK(Wait(library) && Count(library) == 0U && probe.calls == 0U);
    CHECK(!g_file_test(preferences, G_FILE_TEST_EXISTS));
    UmiWorkspaceLibraryGtkSetCurrentPath(library, first);
    CHECK(!g_file_test(preferences, G_FILE_TEST_EXISTS));
    CHECK(UmiWorkspaceLibraryGtkRememberCurrent(library) == UMI_STATUS_OK && Wait(library));
    CHECK(Count(library) == 1U && probe.calls == 0U);
    UmiWorkspaceLibraryGtkDestroy(library); library = NULL;
    /* A new controller loads locations without opening a database or replacing
     * an environment-prefilled draft while its worker is still completing. */
    CHECK(UmiWorkspaceLibraryGtkCreate(Open, &probe, &library) == UMI_STATUS_OK);
    CHECK(UmiWorkspaceLibraryGtkEnableHistory(library, settings) == UMI_STATUS_OK);
    UmiWorkspaceLibraryGtkSetPath(library, second);
    CHECK(Wait(library) && Count(library) == 1U && probe.calls == 0U);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(library, "workspace.library.path"))), second) == 0);
    g_signal_emit_by_name(Find(library, "workspace.library.history.use"), "clicked");
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(library, "workspace.library.path"))), first) == 0);
    CHECK(probe.calls == 0U);
    probe.status = UMI_STATUS_IO_ERROR;
    g_signal_emit_by_name(Find(library, "workspace.library.open"), "clicked");
    CHECK(probe.calls == 1U && Wait(library) && Count(library) == 1U);
    /* Rejected opening does not remember a new location. Successful opening
     * and multiple queued changes are applied in their accepted order. */
    UmiWorkspaceLibraryGtkSetPath(library, second);
    g_signal_emit_by_name(Find(library, "workspace.library.open"), "clicked");
    CHECK(Wait(library) && Count(library) == 1U);
    probe.status = UMI_STATUS_OK;
    g_signal_emit_by_name(Find(library, "workspace.library.open"), "clicked");
    CHECK(Wait(library) && Count(library) == 2U);
    CHECK(!g_file_test(second, G_FILE_TEST_EXISTS));
    g_signal_emit_by_name(Find(library, "workspace.library.history.forget"), "clicked");
    CHECK(Wait(library) && Count(library) == 1U);
    g_signal_emit_by_name(Find(library, "workspace.library.history.clear"), "clicked");
    CHECK(Wait(library) && Count(library) == 0U);
    CHECK(g_file_get_contents(first, &text, &length, NULL));
    CHECK(length == strlen("saved project") && strcmp(text, "saved project") == 0);
    g_clear_pointer(&text, g_free);
    /* Corrupt metadata is not overwritten by a later successful database open. */
    CHECK(g_file_set_contents(settings, "damaged preferences", -1, NULL));
    CHECK(UmiWorkspaceLibraryGtkRememberCurrent(library) == UMI_STATUS_OK && Wait(library));
    CHECK(g_file_get_contents(settings, &text, &length, NULL) && strcmp(text, "damaged preferences") == 0);
    g_clear_pointer(&text, g_free);
    retained = g_object_ref(Find(library, "workspace.library.history.clear"));
    CHECK(UmiWorkspaceLibraryGtkRememberCurrent(library) == UMI_STATUS_OK);
    /* Retained controls cannot enqueue after Destroy; the in-flight worker owns
     * only metadata and the root reference, never the released host context. */
    GtkWidget *root = UmiWorkspaceLibraryGtkWidget(library);
    GWeakRef weak; g_weak_ref_init(&weak, root);
    UmiWorkspaceLibraryGtkDestroy(library); library = NULL;
    g_signal_emit_by_name(retained, "clicked");
    gint64 end = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    bool retired = false;
    while (g_get_monotonic_time() < end) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        GObject *alive = g_weak_ref_get(&weak);
        if (alive == NULL) { retired = true; break; }
        g_object_unref(alive); g_usleep(1000U);
    }
    g_weak_ref_clear(&weak); CHECK(retired);
cleanup:
    UmiWorkspaceLibraryGtkDestroy(library); g_clear_object(&retained); g_free(text);
    if (settings) (void)g_remove(settings); if (preferences) (void)g_rmdir(preferences);
    if (first) (void)g_remove(first); if (directory) (void)g_rmdir(directory);
    g_free(settings); g_free(preferences); g_free(first); g_free(second); g_free(directory);
    return result;
}
