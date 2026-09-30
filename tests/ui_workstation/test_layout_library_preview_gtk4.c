/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_layout_library_preview_gtk4.c
 * PURPOSE: Verify explicit preview dispatch, stale reads and controller lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/layout_library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); failed = 1; goto cleanup; } } while (0)
typedef struct Fixture {
    UmiGtk4WorkspaceLayoutLibrary *library;
    UmiUiWorkspaceLibrarySnapshot current, saved;
    uint64_t generation;
    size_t reads, previews, writes;
    UmiStatus failure, nested;
    bool malformed, destroy;
} Fixture;
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    if (g_strcmp0(g_object_get_data(G_OBJECT(root), "umicom-automation-id"), id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static void Drain(void)
{
    for (size_t i = 0U; i < 128U && g_main_context_pending(NULL); ++i) (void)g_main_context_iteration(NULL, FALSE);
}
static UmiStatus Read(UmiUiWorkspaceLibrarySnapshot *out, void *data)
{
    Fixture *f = data; ++f->reads; *out = f->current; return UMI_STATUS_OK;
}
static UmiStatus Apply(const UmiUiWorkspaceLibraryRequest *request, void *data)
{
    (void)request; ++((Fixture *)data)->writes; return UMI_STATUS_NOT_IMPLEMENTED;
}
static UmiStatus StorageRead(UmiGtk4WorkspaceLayoutLibraryStorageState *out, void *data)
{
    Fixture *f = data;
    memset(out, 0, sizeof(*out)); out->supported = true; out->revision_known = true;
    out->save_enabled = true; out->restore_enabled = true; out->storage_generation = f->generation;
    return UMI_STATUS_OK;
}
static UmiStatus StorageWrite(const UmiGtk4WorkspaceLayoutLibraryStorageRequest *request, void *data)
{
    (void)request; ++((Fixture *)data)->writes; return UMI_STATUS_NOT_IMPLEMENTED;
}
static UmiStatus Preview(const UmiGtk4WorkspaceLayoutLibraryPreviewRequest *request,
    UmiUiWorkspaceLibraryPreview *out, void *data)
{
    Fixture *f = data; ++f->previews;
    f->nested = umi_gtk4_ws_layout_library_refresh(f->library);
    if (request->expected_customisation_revision != f->current.customisation_revision ||
        request->expected_storage_generation != f->generation) return UMI_STATUS_INVALID_STATE;
    if (f->destroy) { umi_gtk4_ws_layout_library_destroy(f->library); f->library = NULL; return UMI_STATUS_OK; }
    if (f->failure != UMI_STATUS_OK) return f->failure;
    memset(out, 0, sizeof(*out)); out->saved = f->saved;
    out->report.checkpoint.recovered_last_good = true;
    if (f->malformed) memset(out->saved.rows[0].name, 'x', sizeof(out->saved.rows[0].name));
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    Fixture *f = calloc(1U, sizeof(*f));
    GtkWidget *root = NULL, *button, *text, *confirm, *status;
    int failed = 0;
    CHECK(f != NULL);
    f->current.layout_count = 1U; f->current.customisation_revision = 5U; f->generation = 3U;
    strcpy(f->current.rows[0].layout_id, "test.preview"); strcpy(f->current.rows[0].name, "Current");
    f->current.rows[0].active = true; f->current.rows[0].locked = true;
    f->saved = f->current; strcpy(f->saved.rows[0].name, "Saved <name>");
    CHECK(umi_gtk4_ws_layout_library_create(Read, Apply, f, &f->library) == UMI_STATUS_OK);
    root = g_object_ref(umi_gtk4_ws_layout_library_popover(f->library));
    button = Find(root, "workstation.layout-library.preview-saved");
    text = Find(root, "workstation.layout-library.preview-text");
    confirm = Find(root, "workstation.layout-library.confirm-restore");
    status = Find(root, "workstation.layout-library.status");
    CHECK(GTK_IS_BUTTON(button) && GTK_IS_LABEL(text) && GTK_IS_CHECK_BUTTON(confirm) && GTK_IS_LABEL(status));
    CHECK(!gtk_widget_get_sensitive(button));
    CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(f->library, StorageRead, StorageWrite, f) == UMI_STATUS_OK);
    CHECK(umi_gtk4_ws_layout_library_set_preview_handler(f->library, Preview, f) == UMI_STATUS_OK);
    CHECK(f->previews == 0U && f->writes == 0U && gtk_widget_get_sensitive(button));
    CHECK(umi_gtk4_ws_layout_library_refresh(f->library) == UMI_STATUS_OK && f->previews == 0U);
    size_t reads = f->reads;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
    g_signal_emit_by_name(button, "clicked");
    CHECK(f->previews == 0U && !gtk_check_button_get_active(GTK_CHECK_BUTTON(confirm)));
    CHECK(umi_gtk4_ws_layout_library_set_preview_handler(f->library, NULL, NULL) == UMI_STATUS_BUSY);
    if (strcmp(argv[1], "stale-model") == 0) ++f->current.customisation_revision;
    else if (strcmp(argv[1], "stale-backend") == 0) ++f->generation;
    else if (strcmp(argv[1], "malformed") == 0) f->malformed = true;
    else if (strcmp(argv[1], "missing") == 0) f->failure = UMI_STATUS_NOT_FOUND;
    else if (strcmp(argv[1], "destroy") == 0) f->destroy = true;
    else if (strcmp(argv[1], "cancel") == 0) { umi_gtk4_ws_layout_library_destroy(f->library); f->library = NULL; }
    else CHECK(strcmp(argv[1], "display") == 0 || strcmp(argv[1], "rebind") == 0);
    Drain();
    CHECK(f->previews == (strcmp(argv[1], "cancel") == 0 ? 0U : 1U));
    CHECK(f->writes == 0U && f->reads == reads);
    if (f->previews != 0U) CHECK(f->nested == UMI_STATUS_BUSY);
    if (f->library == NULL) {
        size_t calls = f->previews; g_signal_emit_by_name(button, "clicked"); Drain(); CHECK(f->previews == calls);
    } else if (strcmp(argv[1], "display") == 0 || strcmp(argv[1], "rebind") == 0) {
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(text)), "Current → Saved <name>") != NULL);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(text)), "previous valid saved copy") != NULL);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(text)), "memory-only") != NULL);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(text)), "Restore reads storage again") != NULL);
        CHECK(f->current.customisation_revision == 5U && strcmp(f->current.rows[0].name, "Current") == 0);
        if (strcmp(argv[1], "rebind") == 0) {
            CHECK(umi_gtk4_ws_layout_library_set_storage_handlers(f->library, StorageRead, StorageWrite, f) == UMI_STATUS_OK);
            CHECK(!gtk_widget_get_sensitive(button));
            g_signal_emit_by_name(button, "clicked"); Drain(); CHECK(f->previews == 1U);
        } else {
            CHECK(umi_gtk4_ws_layout_library_refresh(f->library) == UMI_STATUS_OK);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(text)), "Saved <name>") == NULL);
            CHECK(f->previews == 1U);
        }
    } else {
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(text)), "Saved <name>") == NULL);
        CHECK(gtk_widget_has_css_class(status, "error"));
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(confirm)));
    }
cleanup:
    if (f != NULL) umi_gtk4_ws_layout_library_destroy(f->library);
    if (root != NULL) g_object_unref(root);
    free(f);
    return failed;
}
