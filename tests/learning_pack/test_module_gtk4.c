/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_module_gtk4.c
 *
 * PURPOSE:
 *   Exercise offered module buttons, failed reopen and retired native readers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/main_context.h"
#include "module_fixture.h"
#include "umicom/ui/gtk4/learning_catalogue.h"
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); result = 1; goto cleanup; } } while (0)
static GtkWidget *Find(GtkWidget *widget, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
static int FinishReads(void)
{
    gint64 deadline = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < deadline) {
/* An always-ready GLib source can keep the former drain inside one iteration forever. Reuse the shared finite slice so pending-read and deadline checks remain reachable; retain the old loop for review. */
#if 0
        while (g_main_context_iteration(NULL, FALSE)) {}
#endif
        (void)UmiTestDispatchSlice(NULL, deadline, 64U);
        g_usleep(1000U);
    }
    if (UmiLearningCatalogueGtk4PendingReads() != 0U) {
        fprintf(stderr, "Learning fixture wait exceeded 15 seconds; pending=%zu\n",
            (size_t)UmiLearningCatalogueGtk4PendingReads());
        fflush(stderr);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) return 77;
    int result = 0;
    GtkWidget *widget = NULL;
    const char *name = argv[1], *directory = argv[2];
    UmiLearningCatalogueGtkState before, after;
    CHECK(ModuleFixture(directory) == UMI_STATUS_OK);
    widget = UmiLearningCatalogueGtk4Create((UmiVersion){0U, 9U, 0U});
    CHECK(widget != NULL);
    g_object_ref_sink(widget);
    CHECK(UmiLearningCatalogueGtk4OfferModule(widget, "Read a module", "One local lesson", directory,
        "module.fixture", "framework", "lesson") == UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4CopyState(widget, &before) == UMI_STATUS_OK);
    CHECK(before.pack_count == 0U && !before.busy);
    CHECK(UmiLearningCatalogueGtk4OfferModule(widget, "Other title", "Other route", directory,
        "module.fixture", "studio", "absent") == UMI_STATUS_ALREADY_EXISTS);
    GtkWidget *button = Find(widget, "learning.module.module.fixture");
    CHECK(GTK_IS_BUTTON(button));
    g_signal_emit_by_name(button, "clicked");
    if (strcmp(name, "retire-pending") == 0 || strcmp(name, "destroy-pending") == 0) {
        UmiLearningCatalogueGtk4Retire(widget);
        if (strcmp(name, "destroy-pending") == 0) { g_object_unref(widget); widget = NULL; }
        CHECK(FinishReads());
        if (widget != NULL) {
            CHECK(UmiLearningCatalogueGtk4CopyState(widget, &after) == UMI_STATUS_OK);
            CHECK(after.retired && !after.busy && !after.reader.has_document);
        }
        goto cleanup;
    }
    CHECK(FinishReads());
    CHECK(UmiLearningCatalogueGtk4CopyState(widget, &before) == UMI_STATUS_OK);
    CHECK(before.last_status == UMI_STATUS_OK && before.pack_count == 1U && before.reader.has_document);
    CHECK(strcmp(before.reader.pack.id, "module.fixture") == 0);
    if (strcmp(name, "failed-reopen") == 0) {
        CHECK(UmiRootedFileWrite(directory, "lesson.html", "changed", 7U) == UMI_STATUS_OK);
        g_signal_emit_by_name(button, "clicked");
        CHECK(FinishReads());
        CHECK(UmiLearningCatalogueGtk4CopyState(widget, &after) == UMI_STATUS_OK);
        CHECK(after.last_status == UMI_STATUS_INVALID_STATE && after.pack_count == 1U);
        CHECK(after.reader.has_document && after.reader.history_count == before.reader.history_count);
        CHECK(strcmp(after.reader.selection.resource.sha256, before.reader.selection.resource.sha256) == 0);
    } else CHECK(strcmp(name, "click") == 0);
cleanup:
    if (widget != NULL) { UmiLearningCatalogueGtk4Retire(widget); g_object_unref(widget); }
    if (!FinishReads()) result = 1;
    return result;
}
