/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_saved_library_review_gtk4.c
 * PURPOSE: Exercise saved-review confirmation, replacement failures and callback lifetime.
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
    size_t applies, reads;
    UmiDataServer *server;
    bool missing;
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

static UmiStatus saved_layouts(UmiUiWorkspaceLibraryImport **review, void *context)
{
    Fixture *fixture = context; ++fixture->reads;
    if (fixture->missing) return UMI_STATUS_NOT_FOUND;
    const UmiStatus status = umi_ui_workspace_library_checkpoint_review(fixture->server, &scope, fixture->model, review);
    if (fixture->destroy_review) {
        umi_gtk4_ws_library_exchange_destroy(fixture->view); fixture->view = NULL;
    } else if (umi_gtk4_ws_library_exchange_set_saved_review_handler(fixture->view, NULL, NULL) != UMI_STATUS_BUSY)
        return UMI_STATUS_INVALID_STATE;
    return status;
}
int main(int argc, char **argv)
{
    const char *cases[] = {"apply", "stale", "missing", "unbind", "destroy-read", "destroy-apply", "retained"};
    if (argc != 2) return 2;
    bool known = false; for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) if (!strcmp(argv[1], cases[i])) known = true;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    Fixture fixture = {0}; fixture.model = calloc(1U, sizeof(*fixture.model));
    UMI_TEST_REQUIRE(fixture.model != NULL);
    UMI_TEST_REQUIRE(umi_data_server_create_memory(&fixture.server) == UMI_STATUS_OK);
    umi_ui_workspace_customisation_init(fixture.model);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(fixture.model, "test.exchange.main", "Stored arrangement") == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(fixture.server, &scope, fixture.model, 42U, 0U, NULL) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_ui_workspace_customisation_create_blank_layout(fixture.model, "test.exchange.draft", "Unsaved arrangement") == UMI_STATUS_OK);
    const uint64_t revision = fixture.model->revision;
    UMI_TEST_REQUIRE(umi_gtk4_ws_library_exchange_create(export_layouts, review_layouts, apply_layouts, &fixture, &fixture.view) == UMI_STATUS_OK);
    GtkWidget *root = g_object_ref(umi_gtk4_ws_library_exchange_widget(fixture.view));
    GtkWidget *saved = find(root, "workstation.layout-library.review-saved");
    GtkWidget *apply = find(root, "workstation.layout-library.apply-import");
    GtkWidget *confirm = find(root, "workstation.layout-library.confirm-import");
    UMI_TEST_REQUIRE(saved && apply && confirm && !gtk_widget_get_sensitive(saved));
    UMI_TEST_REQUIRE(umi_gtk4_ws_library_exchange_set_saved_review_handler(fixture.view, saved_layouts, &fixture) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(fixture.reads == 0U && gtk_widget_get_sensitive(saved));
    fixture.destroy_review = !strcmp(argv[1], "destroy-read");
    fixture.destroy_apply = !strcmp(argv[1], "destroy-apply");
    g_signal_emit_by_name(saved, "clicked");
    UMI_TEST_REQUIRE(fixture.reads == 1U && fixture.model->revision == revision);
    if (!fixture.destroy_review) {
        UMI_TEST_REQUIRE(!gtk_widget_get_sensitive(apply));
        UMI_TEST_REQUIRE(find(root, "umicom.comparison.left") && find(root, "umicom.comparison.right"));
        gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
        UMI_TEST_REQUIRE(gtk_widget_get_sensitive(apply));
        if (!strcmp(argv[1], "missing")) {
            fixture.missing = true; g_signal_emit_by_name(saved, "clicked");
            UMI_TEST_REQUIRE(!gtk_widget_get_sensitive(apply));
            g_signal_emit_by_name(apply, "clicked"); UMI_TEST_REQUIRE(fixture.applies == 0U);
        } else if (!strcmp(argv[1], "unbind")) {
            UMI_TEST_REQUIRE(umi_gtk4_ws_library_exchange_set_saved_review_handler(fixture.view, NULL, NULL) == UMI_STATUS_OK);
            UMI_TEST_REQUIRE(!gtk_widget_get_sensitive(saved) && !gtk_widget_get_sensitive(apply));
        } else if (strcmp(argv[1], "retained")) {
            if (!strcmp(argv[1], "stale")) ++fixture.model->revision;
            /* The next stored library contains the unsaved arrangement, but
             * acceptance must still restore the one-layout reviewed content. */
            UMI_TEST_REQUIRE(umi_ui_workspace_library_checkpoint_save(fixture.server, &scope, fixture.model, 100U, 1U, NULL) == UMI_STATUS_OK);
            g_signal_emit_by_name(apply, "clicked"); UMI_TEST_REQUIRE(fixture.applies == 1U);
            UMI_TEST_REQUIRE(fixture.model->layout_count == ((!strcmp(argv[1], "stale") || fixture.destroy_apply) ? 2U : 1U));
            if (fixture.view) UMI_TEST_REQUIRE(!gtk_widget_get_sensitive(apply));
        }
    } else UMI_TEST_REQUIRE(fixture.view == NULL);
    if (fixture.view) umi_gtk4_ws_library_exchange_destroy(fixture.view);
    const size_t reads = fixture.reads, applies = fixture.applies;
    g_signal_emit_by_name(saved, "clicked"); g_signal_emit_by_name(apply, "clicked");
    UMI_TEST_REQUIRE(reads == fixture.reads && applies == fixture.applies);
    g_object_unref(root); umi_data_server_destroy(fixture.server); free(fixture.model); return 0;
}
