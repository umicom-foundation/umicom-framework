/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_workspace/test_import_file_gtk4.c
 * PURPOSE: Exercise the real file worker and retirement boundary with isolated native files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Compile the actual panel owner into this fixture to reach its private file
 * selection boundary without automating an operating-system file chooser.
 * The production reader, GTask and main-context completion are unchanged. */
#include "../../adapters/gtk4/ai_workspace_panel_gtk4.c"
#include "umicom/platform/output_file.h"
#include <glib/gstdio.h>
#include <stdio.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
static void Finalized(gpointer data, GObject *object)
{
    (void)object;
    *(bool *)data = true;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"load",         "invalid",       "nul",    "over-limit",
                           "edit-pending", "close-pending", "missing"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    UmiDataServer *data = NULL;
    UmiAiWorkspace *workspace = NULL;
    UmiAiWorkspaceCancellation *cancellation = NULL;
    UmiAiWorkspaceGtkPanel *panel = NULL;
    UmiAiRuntime runtime;
    umi_ai_runtime_init(&runtime);
    GtkWidget *section = NULL;
    bool finalized = false;
    bool weakAttached = false;
    UmiOutputFile *file = NULL;
    char *directory = NULL, *path = NULL, *current = NULL;
    unsigned char *large = NULL;
    directory = g_dir_make_tmp("umicom-import-file-XXXXXX", NULL);
    CHECK(directory != NULL);
    path = g_build_filename(directory, "caf\xc3\xa9.txt", NULL);
    if (strcmp(name, "missing") != 0)
    {
        CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
        const void *bytes = "Local workshop notice.\n";
        size_t count = strlen(bytes);
        if (strcmp(name, "invalid") == 0)
        {
            bytes = "\xc0\xaf";
            count = 2U;
        }
        if (strcmp(name, "nul") == 0)
        {
            bytes = "a\0b";
            count = 3U;
        }
        if (strcmp(name, "over-limit") == 0)
        {
            count = UMI_AI_WORKSPACE_IMPORT_MAX_BYTES + 1U;
            large = malloc(count);
            CHECK(large != NULL);
            memset(large, 'x', count);
            bytes = large;
        }
        CHECK(UmiOutputFileWrite(file, bytes, count) == UMI_STATUS_OK);
        CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
        UmiOutputFileDestroy(file);
        file = NULL;
    }
    CHECK(umi_data_server_create_memory(&data) == UMI_STATUS_OK);
    CHECK(UmiAiWorkspaceCancellationCreate(&cancellation) == UMI_STATUS_OK);
    CHECK(UmiAiWorkspaceCreate(data, &runtime, "file-import", &workspace) == UMI_STATUS_OK);
    CHECK(UmiAiWorkspaceGtkPanelCreate(workspace, &runtime, cancellation, "Memory fixture", "org.umicom.rag",
                                       &panel) == UMI_STATUS_OK);
    section = g_object_ref(panel->importPanel);
    AiImportPanel *import = g_object_get_data(G_OBJECT(section), "umicom-ai-import");
    CHECK(import != NULL);
    g_object_weak_ref(G_OBJECT(section), Finalized, &finalized);
    weakAttached = true;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(import->input));
    gtk_text_buffer_set_text(buffer, "Existing input", -1);
    CHECK(AiImportReadStart(import, path) == UMI_STATUS_OK && import->loading);
    CHECK(AiImportReadStart(import, path) == UMI_STATUS_BUSY);
    if (strcmp(name, "edit-pending") == 0)
        gtk_text_buffer_set_text(buffer, "My later edit", -1);
    if (strcmp(name, "close-pending") == 0)
    {
        UmiAiWorkspaceGtkPanelDestroy(panel);
        panel = NULL;
        UmiAiWorkspaceDestroy(workspace);
        workspace = NULL;
        umi_data_server_destroy(data);
        data = NULL;
        UmiAiWorkspaceCancellationDestroy(cancellation);
        cancellation = NULL;
    }
    gint64 deadline = g_get_monotonic_time() + 5000000;
    while (import->loading && g_get_monotonic_time() < deadline)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
    CHECK(!import->loading);
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    current = gtk_text_buffer_get_text(buffer, &begin, &end, TRUE);
    const char *expected = strcmp(name, "load") == 0            ? "Local workshop notice.\n"
                           : strcmp(name, "edit-pending") == 0  ? "My later edit"
                           : strcmp(name, "close-pending") == 0 ? ""
                                                                : "Existing input";
    CHECK(strcmp(current, expected) == 0);
    if (workspace != NULL)
    {
        UmiAiWorkspaceSnapshot snapshot;
        CHECK(UmiAiWorkspaceSnapshotRead(workspace, &snapshot) == UMI_STATUS_OK &&
              snapshot.sourceCount == 0U && snapshot.revision == 0U);
    }
    UmiAiWorkspaceGtkPanelDestroy(panel);
    panel = NULL;
    g_clear_object(&section);
    CHECK(finalized);
    weakAttached = false;
cleanup:
    /* A failed assertion may leave a read pending. Cancel/retire the host before
     * releasing services, then let its owning GTask finish independently. */
    UmiAiWorkspaceGtkPanelDestroy(panel);
    if (section != NULL && weakAttached)
        g_object_weak_unref(G_OBJECT(section), Finalized, &finalized);
    g_clear_object(&section);
    UmiOutputFileDestroy(file);
    UmiAiWorkspaceDestroy(workspace);
    UmiAiWorkspaceCancellationDestroy(cancellation);
    umi_data_server_destroy(data);
    umi_ai_runtime_destroy(&runtime);
    free(large);
    g_free(current);
    g_free(path);
    g_free(directory);
    return failed;
}
