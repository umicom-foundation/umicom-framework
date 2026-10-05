/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_library_exchange_gtk4.c
 * PURPOSE: Verify native import confirmation, frozen bytes, stale review and retained controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/library_exchange.h"
#include "umicom/test_runtime/check.h"
#include <stdlib.h>
#include <string.h>
typedef struct Fixture {
    UmiGtk4WorkspaceLibraryExchange *view;
    UmiUiWorkspaceCustomisation *model;
    bool destroy_review, destroy_apply;
    size_t applies;
} Fixture;
static const UmiUiWorkspaceCheckpointScope scope = {"test.exchange", "desktop", "test.exchange."};
static GtkWidget *find(GtkWidget *root, const char *id)
{
    if (g_strcmp0(g_object_get_data(G_OBJECT(root), "umicom-automation-id"), id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *result = find(child, id); if (result != NULL) return result;
    }
    return NULL;
}
static UmiStatus export_layouts(char *bytes, size_t capacity, size_t *size, void *context)
{ return umi_ui_workspace_library_export(&scope, ((Fixture *)context)->model, 0U, bytes, capacity, size); }
static UmiStatus review_layouts(const void *bytes, size_t size, UmiUiWorkspaceLibraryImport **review, void *context)
{
    Fixture *fixture = context;
    if (fixture->destroy_review) {
        umi_gtk4_ws_library_exchange_destroy(fixture->view); fixture->view = NULL;
        return UMI_STATUS_CANCELLED;
    }
    return umi_ui_workspace_library_import_review(&scope, fixture->model, bytes, size, review);
}
static UmiStatus apply_layouts(const UmiUiWorkspaceLibraryImport *review, void *context)
{
    Fixture *fixture = context; ++fixture->applies;
    if (fixture->destroy_apply) {
        umi_gtk4_ws_library_exchange_destroy(fixture->view); fixture->view = NULL;
        return UMI_STATUS_CANCELLED;
    }
    UmiUiWorkspaceCustomisation *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    const UmiStatus status = umi_ui_workspace_library_import_candidate(review, &scope, fixture->model, candidate);
    if (status == UMI_STATUS_OK) *fixture->model = *candidate;
    free(candidate); return status;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    Fixture fixture = {0}; fixture.model = calloc(1U, sizeof(*fixture.model));
    UMI_TEST_REQUIRE(fixture.model != NULL);
    umi_ui_workspace_customisation_init(fixture.model);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(fixture.model, "test.exchange.main", "Main workspace") == UMI_STATUS_OK);
    /* Use a real panel so geometry-only edits appear even when layout names
     * and summary counts remain identical. */
    UmiUiWindowDescriptor tool = {0};
    strcpy(tool.tool_id, "editor"); strcpy(tool.title, "Editor");
    tool.category = UMI_UI_WINDOW_CATEGORY_DEVELOPMENT; tool.default_width = 0.5; tool.default_height = 0.5;
    UMI_TEST_REQUIRE(umi_ui_window_catalogue_register(&fixture.model->windows, &tool) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(fixture.model) == UMI_STATUS_OK);
    char panel_id[UMI_UI_WORKSPACE_LAYOUT_ID_CAPACITY];
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_open_window(fixture.model, "editor", "canvas", false,
        1U, panel_id, sizeof panel_id) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(fixture.model, panel_id, 0.125, 0.25, 0.5, 0.5) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(fixture.model) == UMI_STATUS_OK);
    size_t size = 0U;
    UMI_TEST_REQUIRE(export_layouts(NULL, 0U, &size, &fixture) == UMI_STATUS_OK);
    char *bytes = malloc(size + 1U); UMI_TEST_REQUIRE(bytes != NULL);
    UMI_TEST_REQUIRE(export_layouts(bytes, size + 1U, &size, &fixture) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_gtk4_ws_library_exchange_create(export_layouts, review_layouts, apply_layouts,
        &fixture, &fixture.view) == UMI_STATUS_OK);
    if (!strcmp(argv[1], "geometry")) {
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_begin_edit(fixture.model) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_place_canvas_window(fixture.model, panel_id, 0.25, 0.25, 0.5, 0.5) == UMI_STATUS_OK);
        UMI_TEST_REQUIRE(umi_ui_workspace_customisation_commit_edit(fixture.model) == UMI_STATUS_OK);
    }
    GtkWidget *root = g_object_ref(umi_gtk4_ws_library_exchange_widget(fixture.view));
    GtkWidget *confirm = find(root, "workstation.layout-library.confirm-import");
    GtkWidget *apply = find(root, "workstation.layout-library.apply-import");
    UMI_TEST_REQUIRE(confirm != NULL && apply != NULL && !gtk_widget_get_sensitive(apply));
    const uint64_t revision = fixture.model->revision;
    fixture.destroy_review = strcmp(argv[1], "destroy-review") == 0;
    fixture.destroy_apply = strcmp(argv[1], "destroy-apply") == 0;
    UmiStatus status = umi_gtk4_ws_library_exchange_review_bytes(fixture.view, bytes, size);
    if (fixture.destroy_review) UMI_TEST_REQUIRE(status == UMI_STATUS_CANCELLED && fixture.view == NULL);
    else {
        UMI_TEST_REQUIRE(status == UMI_STATUS_OK && fixture.model->revision == revision);
        GtkWidget *details = find(root, "workstation.layout-library.import-details");
        UMI_TEST_REQUIRE(details != NULL);
        GtkWidget *left = find(details, "umicom.comparison.left");
        GtkWidget *right = find(details, "umicom.comparison.right");
        UMI_TEST_REQUIRE(GTK_IS_TEXT_VIEW(left) && GTK_IS_TEXT_VIEW(right));
        UMI_TEST_REQUIRE(!gtk_text_view_get_editable(GTK_TEXT_VIEW(left)) && !gtk_text_view_get_editable(GTK_TEXT_VIEW(right)));
        GtkTextIter first, last;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(right));
        gtk_text_buffer_get_bounds(buffer, &first, &last);
        char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
        UMI_TEST_REQUIRE(strstr(text, "Left: 0.125") != NULL && strstr(text, "Tool: editor") != NULL); g_free(text);
        if (!strcmp(argv[1], "geometry")) {
            buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(left)); gtk_text_buffer_get_bounds(buffer, &first, &last);
            text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
            UMI_TEST_REQUIRE(strstr(text, "Left: 0.25") != NULL); g_free(text);
        }
        bytes[0] = '!'; /* Frozen review must survive edits to the source buffer. */
        g_signal_emit_by_name(apply, "clicked"); UMI_TEST_REQUIRE(fixture.applies == 0U);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
        UMI_TEST_REQUIRE(gtk_widget_get_sensitive(apply));
        if (strcmp(argv[1], "invalid-replacement") == 0) {
            UMI_TEST_REQUIRE(umi_gtk4_ws_library_exchange_review_bytes(fixture.view, bytes, size) != UMI_STATUS_OK);
            UMI_TEST_REQUIRE(!gtk_widget_get_sensitive(apply));
        } else {
            if (strcmp(argv[1], "stale") == 0) ++fixture.model->revision;
            g_signal_emit_by_name(apply, "clicked"); UMI_TEST_REQUIRE(fixture.applies == 1U);
            if (fixture.destroy_apply) UMI_TEST_REQUIRE(fixture.view == NULL && fixture.model->revision == revision);
            else UMI_TEST_REQUIRE(fixture.model->revision == revision + 1U && !gtk_widget_get_sensitive(apply));
        }
    }
    if (fixture.view != NULL) umi_gtk4_ws_library_exchange_destroy(fixture.view);
    const size_t calls = fixture.applies;
    g_signal_emit_by_name(apply, "clicked"); UMI_TEST_REQUIRE(fixture.applies == calls);
    g_object_unref(root); free(bytes); free(fixture.model); return EXIT_SUCCESS;
}
