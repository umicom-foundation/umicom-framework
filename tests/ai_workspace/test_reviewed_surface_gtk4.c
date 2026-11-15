/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_reviewed_surface_gtk4.c
 *
 * PURPOSE:
 *   Verify complete library chains survive checked retirement and candidate-cleanup failures in the existing GTK surface.
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
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); return 1; } } while (0)
static bool fail_panel, fail_workspace, fail_runtime, fail_attach;
static UmiStatus CheckedPanel(UmiAiWorkspaceGtkPanel **panel)
{
    if (*panel != NULL && fail_panel) { fail_panel = false; return UMI_STATUS_IO_ERROR; }
    return UmiAiWorkspaceGtkPanelRelease(panel);
}
static UmiStatus CheckedWorkspace(UmiAiWorkspace **workspace)
{
    if (*workspace != NULL && fail_workspace) { fail_workspace = false; return UMI_STATUS_BUSY; }
    return UmiAiWorkspaceRelease(workspace);
}
static UmiStatus CheckedRuntime(UmiAiRuntime *runtime)
{
    if (fail_runtime) { fail_runtime = false; return UMI_STATUS_INVALID_STATE; }
    return UmiAiRuntimeRelease(runtime);
}
static UmiStatus CheckedAttach(UmiAiWorkspaceGtkPanel *panel, UmiAiLocalTransport *transport)
{
    if (fail_attach) { fail_attach = false; return UMI_STATUS_IO_ERROR; }
    return UmiAiWorkspaceGtkPanelEnableLocalStreaming(panel, transport);
}
/* Exercise the actual surface with real panels, workspace and SQLite. Only
 * refusal at a checked cleanup/attachment boundary is injected. */
#define UmiAiWorkspaceGtkPanelRelease CheckedPanel
#define UmiAiWorkspaceRelease CheckedWorkspace
#define UmiAiRuntimeRelease CheckedRuntime
#define UmiAiWorkspaceGtkPanelEnableLocalStreaming CheckedAttach
#include "../../adapters/gtk4/ai_workspace_surface_gtk4.c"
#undef UmiAiWorkspaceGtkPanelEnableLocalStreaming
#undef UmiAiRuntimeRelease
#undef UmiAiWorkspaceRelease
#undef UmiAiWorkspaceGtkPanelRelease
typedef struct WrongOwner {
    UmiAiWorkspaceGtkSurface *surface;
    UmiAiLocalTransport *transport;
    UmiStatus attach, release;
    bool retained;
} WrongOwner;
static int WrongThread(void *context)
{
    WrongOwner *check = context;
    UmiAiWorkspaceGtkSurface *alias = check->surface;
    check->attach = UmiAiWorkspaceGtkSurfaceEnableLocalStreaming(check->surface, check->transport);
    check->release = UmiAiWorkspaceGtkSurfaceRelease(&alias);
    check->retained = alias == check->surface;
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    const char *const names[] = {"attach", "wrong-owner", "switch-retain", "release-retain",
        "workspace-release", "runtime-release", "candidate-retain"};
    size_t selected = sizeof(names) / sizeof(names[0]);
    for (size_t i = 0U; i < sizeof(names) / sizeof(names[0]); ++i)
        if (strcmp(mode, names[i]) == 0) selected = i;
    if (selected == sizeof(names) / sizeof(names[0])) return 2;
    UmiAiLocalTransport *transport = NULL;
    UmiStatus status = UmiAiLocalTransportCreate(&transport);
    if (status == UMI_STATUS_UNAVAILABLE) return 77;
    CHECK(status == UMI_STATUS_OK);
    (void)g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    (void)g_setenv("GTK_A11Y", "test", TRUE);
    if (!gtk_init_check()) { CHECK(UmiAiLocalTransportDestroy(&transport) == UMI_STATUS_OK); return 77; }
    char *directory = g_dir_make_tmp("umicom-stream-surface-XXXXXX", NULL);
    CHECK(directory != NULL);
    char *first = g_build_filename(directory, "first.sqlite", NULL);
    char *second = g_build_filename(directory, "second.sqlite", NULL);
    (void)g_setenv("UMICOM_LIBRARY_HISTORY_FILE", "", TRUE);
    (void)g_setenv("UMICOM_AI_WORKSPACE_DB", first, TRUE);
    (void)g_setenv("UMICOM_AI_WORKSPACE_PORT", "8080", TRUE);
    GtkWidget *original = gtk_label_new("Existing product layout"); g_object_ref_sink(original);
    UmiAiWorkspaceGtkSurface *surface = NULL;
    CHECK(UmiAiWorkspaceGtkSurfaceCreate(original, "org.umicom.llm", &surface) == UMI_STATUS_OK);
    CHECK(UmiAiWorkspaceGtkSurfaceEnableLocalStreaming(NULL, transport) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiWorkspaceGtkSurfaceEnableLocalStreaming(surface, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiAiWorkspaceGtkSurfaceEnableLocalStreaming(surface, transport) == UMI_STATUS_OK);
    CHECK(surface->session == NULL && surface->pending == NULL);
    if (strcmp(mode, "wrong-owner") == 0) {
        WrongOwner check = {.surface=surface, .transport=transport};
        UmiThread *thread = NULL; int result = 0;
        CHECK(umi_thread_start(WrongThread, &check, &thread) == UMI_STATUS_OK);
        CHECK(umi_thread_join(thread, &result) == UMI_STATUS_OK && result == 0);
        CHECK(UmiThreadRelease(&thread) == UMI_STATUS_OK);
        CHECK(check.attach == UMI_STATUS_INVALID_STATE && check.release == UMI_STATUS_INVALID_STATE && check.retained);
    }
    if (selected >= 2U) {
        if (strcmp(mode, "candidate-retain") == 0) { fail_attach = true; fail_panel = true; }
        status = UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, first, true);
        if (status == UMI_STATUS_UNAVAILABLE) {
            CHECK(UmiAiWorkspaceGtkSurfaceRelease(&surface) == UMI_STATUS_OK);
            g_object_unref(original);
            if (g_file_test(first, G_FILE_TEST_EXISTS)) (void)g_remove(first);
            (void)g_rmdir(directory); g_free(first); g_free(second); g_free(directory);
            CHECK(UmiAiLocalTransportDestroy(&transport) == UMI_STATUS_OK); return 77;
        }
        if (strcmp(mode, "candidate-retain") == 0) {
            CHECK(status == UMI_STATUS_IO_ERROR && surface->session == NULL && surface->pending != NULL);
            CHECK(surface->pending->panel != NULL && surface->pending->workspace != NULL);
        } else {
            CHECK(status == UMI_STATUS_OK && surface->session != NULL);
            CHECK(UmiAiWorkspaceGtkSurfaceEnableLocalStreaming(surface, transport) == UMI_STATUS_BUSY);
            if (strcmp(mode, "switch-retain") == 0) {
                AiLibrarySession *old = surface->session;
                fail_panel = true;
                CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, second, true) == UMI_STATUS_IO_ERROR);
                CHECK(surface->session == old && surface->pending != NULL);
                CHECK(surface->pending->panel != NULL && surface->pending->workspace != NULL);
/* A failed old-session retirement requires checked shutdown before any reopen; test both same-path and different-path refusal. The former implementation is retained for owner review. */
#if 0
                /* Retry releases only retained owners; opening an existing second
                 * database does not recreate or overwrite the first library. */
                CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, second, false) == UMI_STATUS_OK);
                CHECK(surface->pending == NULL && strcmp(surface->session->path, second) == 0);
#endif
                CHECK(surface->retirementFailed);
                CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, first, false) == UMI_STATUS_INVALID_STATE);
                CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, second, false) == UMI_STATUS_INVALID_STATE);
                /* Finish checked retirement before creating another surface.
                 * The second file already exists and is opened, not recreated. */
                CHECK(UmiAiWorkspaceGtkSurfaceRelease(&surface) == UMI_STATUS_OK);
                CHECK(gtk_widget_get_parent(original) == NULL);
                CHECK(UmiAiWorkspaceGtkSurfaceCreate(original, "org.umicom.llm", &surface) == UMI_STATUS_OK);
                CHECK(UmiAiWorkspaceGtkSurfaceEnableLocalStreaming(surface, transport) == UMI_STATUS_OK);
                CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, second, false) == UMI_STATUS_OK);
                CHECK(surface->pending == NULL && strcmp(surface->session->path, second) == 0);
            } else {
                fail_panel = strcmp(mode, "release-retain") == 0;
                fail_workspace = strcmp(mode, "workspace-release") == 0;
                fail_runtime = strcmp(mode, "runtime-release") == 0;
                UmiStatus expected = fail_panel ? UMI_STATUS_IO_ERROR :
                    fail_workspace ? UMI_STATUS_BUSY : UMI_STATUS_INVALID_STATE;
                CHECK(UmiAiWorkspaceGtkSurfaceRelease(&surface) == expected);
                CHECK(surface != NULL && surface->closed && surface->session != NULL);
                CHECK(gtk_widget_get_parent(original) != NULL);
                CHECK(UmiAiWorkspaceGtkSurfaceOpenLibrary(surface, first, false) == UMI_STATUS_INVALID_STATE);
                if (strcmp(mode, "workspace-release") == 0)
                    CHECK(surface->session->panel == NULL && surface->session->workspace != NULL);
                if (strcmp(mode, "runtime-release") == 0)
                    CHECK(surface->session->panel == NULL && surface->session->workspace == NULL);
            }
        }
    }
    CHECK(UmiAiWorkspaceGtkSurfaceRelease(&surface) == UMI_STATUS_OK && surface == NULL);
    CHECK(gtk_widget_get_parent(original) == NULL);
    g_object_unref(original);
    size_t requests = 99U;
    CHECK(UmiAiLocalTransportRequestCount(transport, &requests) == UMI_STATUS_OK && requests == 0U);
    CHECK(UmiAiLocalTransportDestroy(&transport) == UMI_STATUS_OK);
    if (g_file_test(first, G_FILE_TEST_EXISTS)) CHECK(g_remove(first) == 0);
    if (g_file_test(second, G_FILE_TEST_EXISTS)) CHECK(g_remove(second) == 0);
    CHECK(g_rmdir(directory) == 0);
    g_free(first); g_free(second); g_free(directory);
    return 0;
}
