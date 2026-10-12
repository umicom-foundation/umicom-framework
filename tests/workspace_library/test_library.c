/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workspace_library/test_library.c
 * PURPOSE: Check explicit database selection, persistence and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
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
static int Files(void)
{
    int result = 0;
    char *directory = g_dir_make_tmp("umicom-library-XXXXXX", NULL);
    char *path = directory != NULL ? g_build_filename(directory, "caf\xc3\xa9 library.sqlite3", NULL) : NULL;
    char *missing = directory != NULL ? g_build_filename(directory, "missing.sqlite3", NULL) : NULL;
    UmiDataServer *data = NULL;
    CHECK(directory != NULL && path != NULL);
    CHECK(UmiWorkspaceLibraryOpenData(NULL, false, &data) == UMI_STATUS_INVALID_ARGUMENT && data == NULL);
    CHECK(UmiWorkspaceLibraryOpenData("relative.sqlite3", true, &data) == UMI_STATUS_INVALID_ARGUMENT && data == NULL);
    CHECK(UmiWorkspaceLibraryOpenData(directory, false, &data) == UMI_STATUS_INVALID_ARGUMENT && data == NULL);
    CHECK(UmiWorkspaceLibraryOpenData(missing, false, &data) == UMI_STATUS_NOT_FOUND && data == NULL);
    CHECK(!g_file_test(missing, G_FILE_TEST_EXISTS));
    UmiStatus status = UmiWorkspaceLibraryOpenData(path, true, &data);
    if (status == UMI_STATUS_UNAVAILABLE) { result = 77; goto cleanup; }
    CHECK(status == UMI_STATUS_OK);
    CHECK(umi_data_server_set(data, "saved-work", "A library opened by its owner") == UMI_STATUS_OK);
    umi_data_server_destroy(data); data = NULL;
    /* An existing file must survive a mistaken Create, including its contents. */
    CHECK(UmiWorkspaceLibraryOpenData(path, true, &data) == UMI_STATUS_ALREADY_EXISTS && data == NULL);
    CHECK(UmiWorkspaceLibraryOpenData(path, false, &data) == UMI_STATUS_OK);
    char value[96] = {0};
    CHECK(umi_data_server_get(data, "saved-work", value, sizeof(value)) == UMI_STATUS_OK);
    CHECK(strcmp(value, "A library opened by its owner") == 0);
cleanup:
    umi_data_server_destroy(data);
    if (path != NULL) { (void)g_remove(path); g_free(path); }
    g_free(missing);
    if (directory != NULL) { (void)g_rmdir(directory); g_free(directory); }
    return result;
}
typedef struct OpenProbe { size_t calls; UmiStatus status; bool create; } OpenProbe;
static UmiStatus Probe(const char *path, bool create, void *data)
{
    OpenProbe *probe = data;
    if (!g_path_is_absolute(path)) return UMI_STATUS_INVALID_ARGUMENT;
    ++probe->calls; probe->create = create; return probe->status;
}
static int Controls(void)
{
    if (!gtk_init_check()) return 77;
    int result = 0;
    UmiWorkspaceLibraryGtk *library = NULL;
    GtkWidget *retained = NULL;
    char *path = g_build_filename(g_get_tmp_dir(), "umicom-library-choice.sqlite3", NULL);
    char *other = g_build_filename(g_get_tmp_dir(), "umicom-library-other.sqlite3", NULL);
    char *canonical = g_canonicalize_filename(path, NULL);
    OpenProbe probe = {0};
    CHECK(UmiWorkspaceLibraryGtkCreate(Probe, &probe, &library) == UMI_STATUS_OK);
    GtkWidget *root = UmiWorkspaceLibraryGtkWidget(library);
    GtkWidget *open = Find(root, "workspace.library.open");
    GtkWidget *create = Find(root, "workspace.library.create");
    GtkWidget *active = Find(root, "workspace.library.active");
    GtkWidget *confirm = Find(root, "workspace.library.confirm");
    CHECK(open != NULL && create != NULL && active != NULL && confirm != NULL);
    UmiWorkspaceLibraryGtkSetPath(library, "relative.sqlite3");
    g_signal_emit_by_name(open, "clicked"); CHECK(probe.calls == 0U);
    UmiWorkspaceLibraryGtkSetPath(library, path); CHECK(probe.calls == 0U);
    g_signal_emit_by_name(create, "clicked");
    CHECK(probe.calls == 1U && probe.create);
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(active)), canonical) == 0);
    g_signal_emit_by_name(open, "clicked"); CHECK(probe.calls == 1U);
    UmiWorkspaceLibraryGtkSetPath(library, other);
    g_signal_emit_by_name(open, "clicked"); CHECK(probe.calls == 1U);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    probe.status = UMI_STATUS_IO_ERROR;
    g_signal_emit_by_name(open, "clicked"); CHECK(probe.calls == 2U && !probe.create);
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(active)), canonical) == 0);
    /* A retained button must be inert after its borrowed callback context retires. */
    retained = g_object_ref(open);
    UmiWorkspaceLibraryGtkDestroy(library); library = NULL;
    g_signal_emit_by_name(retained, "clicked"); CHECK(probe.calls == 2U);
cleanup:
    UmiWorkspaceLibraryGtkDestroy(library);
    g_clear_object(&retained);
    g_free(canonical); g_free(path); g_free(other);
    return result;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "files") == 0) return Files();
    if (strcmp(argv[1], "controls") == 0) return Controls();
    return 2;
}
