/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_workspace/test_library_gtk4.c
 * PURPOSE: Reopen an edited Media project from its explicitly selected library.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/creative_workspace.h"
#include "umicom/ui/gtk4/workspace_library.h"
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
static void Click(GtkWidget *root, const char *id)
{
    GtkWidget *button = Find(root, id);
    if (button != NULL) g_signal_emit_by_name(button, "clicked");
}
int main(void)
{
    if (!gtk_init_check()) return 77;
    int result = 0;
    GtkWidget *reportExport = NULL;
    GWeakRef historyRoot; g_weak_ref_init(&historyRoot, NULL);
    char *directory = g_dir_make_tmp("umicom-creative-library-XXXXXX", NULL);
    char *path = directory != NULL ? g_build_filename(directory, "project library.sqlite3", NULL) : NULL;
    char *missing = directory != NULL ? g_build_filename(directory, "missing.sqlite3", NULL) : NULL;
    char *historyFile = directory != NULL ? g_build_filename(directory, "recent-libraries.txt", NULL) : NULL;
    char *previousHistory = g_strdup(g_getenv("UMICOM_LIBRARY_HISTORY_FILE"));
    UmiCreativeGtkPanel *panel = NULL;
    UmiDataServer *data = NULL;
    UmiCreativeWorkspace *inspector = NULL;
    UmiCreativeProject *project = g_new0(UmiCreativeProject, 1);
    CHECK(directory != NULL);
    /* A regression must never read or overwrite the owner's remembered work. */
    CHECK(g_setenv("UMICOM_LIBRARY_HISTORY_FILE", historyFile, TRUE));
    UmiStatus status = UmiWorkspaceLibraryOpenData(path, true, &data);
    if (status == UMI_STATUS_UNAVAILABLE) { result = 77; goto cleanup; }
    CHECK(status == UMI_STATUS_OK);
    umi_data_server_destroy(data); data = NULL;
    CHECK(UmiCreativeGtkPanelCreate(NULL, "media", &panel) == UMI_STATUS_OK);
    GtkWidget *root = UmiCreativeGtkPanelWidget(panel);
    g_weak_ref_set(&historyRoot, Find(root, "workspace.library"));
    CHECK(Find(root, "creative.export.browse") != NULL);
    CHECK(Find(root, "text.export") != NULL);
    reportExport = g_object_ref(Find(root, "text.export"));
    GtkWidget *entry = Find(root, "workspace.library.path");
    GtkWidget *active = Find(root, "workspace.library.active");
    GtkWidget *confirm = Find(root, "workspace.library.confirm");
    CHECK(entry != NULL && active != NULL && confirm != NULL);
    gtk_editable_set_text(GTK_EDITABLE(entry), path);
    Click(root, "workspace.library.open");
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(active)), path) == 0);
    Click(root, "creative.project.create");
    Click(root, "creative.project.starter");
    /* Failed library selection must leave the live project and its editor usable. */
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    gtk_editable_set_text(GTK_EDITABLE(entry), missing);
    Click(root, "workspace.library.open");
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(active)), path) == 0);
    CHECK(!g_file_test(missing, G_FILE_TEST_EXISTS));
    Click(root, "creative.project.reload");
    UmiCreativeGtkPanelDestroy(panel); panel = NULL;
    /* The source project can be released while automation keeps this control. */
    CHECK(!gtk_widget_get_sensitive(reportExport));
    CHECK(UmiWorkspaceLibraryOpenData(path, false, &data) == UMI_STATUS_OK);
    CHECK(UmiCreativeWorkspaceOpen(data, "workshop", &inspector) == UMI_STATUS_OK);
    CHECK(UmiCreativeWorkspaceCopy(inspector, project) == UMI_STATUS_OK);
    CHECK(project->revision == 2U && project->elementCount == 2U);
cleanup:
    g_clear_object(&reportExport);
    UmiCreativeGtkPanelDestroy(panel);
    UmiCreativeWorkspaceDestroy(inspector);
    umi_data_server_destroy(data);
    g_free(project);
    if (path != NULL) { (void)g_remove(path); g_free(path); }
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
