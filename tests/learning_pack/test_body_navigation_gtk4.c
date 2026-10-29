/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_body_navigation_gtk4.c
 *
 * PURPOSE:
 *   Verify native search-hit navigation preserves identities, selection and reader ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "text_fixture.h"
#include "../../adapters/gtk4/learning_catalogue_private.h"
#include "../support/gtk4_expanders.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result=1; goto cleanup; } } while (0)
static int Finish(void)
{
    gint64 deadline = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
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
static int Selected(GtkTextBuffer *buffer, const char *expected)
{
    GtkTextIter first, last;
    if (!gtk_text_buffer_get_selection_bounds(buffer, &first, &last)) return 0;
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, TRUE);
    int equal = strcmp(text, expected) == 0;
    g_free(text);
    return equal;
}
typedef struct Observer {
    GtkWidget *panel;
    GtkTextBuffer *buffer;
    unsigned calls;
    int retire, busy;
    UmiStatus nested;
} Observer;
static void SelectedNotice(GtkTextBuffer *buffer, const GtkTextIter *position, GtkTextMark *mark, gpointer data)
{
    (void)position; (void)mark;
    Observer *observer = data;
    if (!Selected(buffer, "ownership") || observer->calls != 0U) return;
    ++observer->calls;
    UmiLearningCatalogueGtkState state;
    observer->busy = UmiLearningCatalogueGtk4CopyState(observer->panel, &state) == UMI_STATUS_OK && state.busy;
    observer->nested = UmiLearningCatalogueGtk4ReadBodySearchResult(observer->panel, 1U);
    if (observer->retire) UmiLearningCatalogueGtk4Retire(observer->panel);
}
static void ChangeVisibleText(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    Observer *observer = data;
    gtk_text_buffer_set_text(observer->buffer, "external display change", -1);
}
static int Check(const char *name, const char *directory)
{
    int result = 0, known = 1;
    GtkWidget *panel = NULL;
    UmiLearningShelf *shelf = NULL;
    TextFixture fixture = TextFixtureDefault();
    static const char unicode_guide[] = "<p>\xc3\xa9 ownership \xce\xa9</p>";
    static const char unicode_source[] = "/* \xc3\xa9 ownership \xce\xa9 */";
    static const char at_end[] = "ownership";
    if (strcmp(name, "unicode-guide") == 0) {
        fixture.bytes[0] = unicode_guide; fixture.lengths[0] = sizeof(unicode_guide)-1U;
    } else if (strcmp(name, "unicode-source") == 0) {
        fixture.bytes[1] = unicode_source; fixture.lengths[1] = sizeof(unicode_source)-1U;
    } else if (strcmp(name, "last-byte") == 0) {
        fixture.bytes[1] = at_end; fixture.lengths[1] = sizeof(at_end)-1U;
    } else if (strcmp(name, "uppercase-html") == 0) fixture.paths[0] = "z-guide.HTML";
    CHECK(UmiLearningShelfCreate(&shelf) == UMI_STATUS_OK);
    CHECK(TextFixtureAdopt(shelf, directory, "navigation.pack", &fixture) == UMI_STATUS_OK);
    panel = UmiLearningCatalogueGtk4Create((UmiVersion){0U,9U,0U});
    CHECK(panel != NULL); g_object_ref_sink(panel); UmiTestExpandSections(panel);
    CHECK(UmiLearningCatalogueGtk4Load(panel, directory) == UMI_STATUS_OK && Finish());
    CHECK(UmiLearningCatalogueGtk4SearchBodies(panel, "ownership", 0, 0, 0) == UMI_STATUS_OK && Finish());
    GtkWidget *view = Find(panel, "learning.catalogue.content");
    CHECK(GTK_IS_TEXT_VIEW(view));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    Observer observer = {panel, buffer, 0U, strcmp(name, "retire-selection") == 0, 0, UMI_STATUS_OK};
    gulong selected_signal = 0U, heading_signal = 0U;
    GtkWidget *heading = NULL;
    if (strcmp(name, "retire-selection") == 0 || strcmp(name, "reentrant") == 0)
        selected_signal = g_signal_connect(buffer, "mark-set", G_CALLBACK(SelectedNotice), &observer);
    if (strcmp(name, "visible-mutated") == 0) {
        heading = Find(panel, "learning.catalogue.heading");
        CHECK(heading != NULL);
        heading_signal = g_signal_connect(heading, "notify::label", G_CALLBACK(ChangeVisibleText), &observer);
    }
    size_t position = strcmp(name, "source") == 0 || strcmp(name, "unicode-source") == 0 ||
        strcmp(name, "last-byte") == 0 ? 1U : strcmp(name, "note") == 0 ? 2U : 0U;
    CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel, position) == UMI_STATUS_OK);
    if (strcmp(name, "retire-pending") == 0 || strcmp(name, "destroy-pending") == 0) {
        UmiLearningCatalogueGtk4Retire(panel);
        if (strcmp(name, "destroy-pending") == 0) g_clear_object(&panel);
        CHECK(Finish()); goto cleanup;
    }
    CHECK(Finish());
    if (selected_signal != 0U) g_signal_handler_disconnect(buffer, selected_signal);
    if (heading_signal != 0U) g_signal_handler_disconnect(heading, heading_signal);
    UmiLearningCatalogueGtkState state;
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &state) == UMI_STATUS_OK);
    if (strcmp(name, "visible-mutated") == 0) {
        CHECK(state.last_status == UMI_STATUS_INVALID_STATE && !Selected(buffer, "ownership"));
        goto cleanup;
    }
    CHECK(state.last_status == UMI_STATUS_OK && state.reader.has_document);
    CHECK(Selected(buffer, position == 2U ? "Ownership" : "ownership"));
    if (strcmp(name, "retire-selection") == 0 || strcmp(name, "reentrant") == 0) {
        CHECK(observer.calls == 1U && observer.busy && observer.nested == UMI_STATUS_BUSY);
    } else if (strcmp(name, "changed-source") == 0 || strcmp(name, "missing-source") == 0 ||
        strcmp(name, "invalid-location") == 0 || strcmp(name, "wrong-identity") == 0) {
        if (strcmp(name, "changed-source") == 0)
            CHECK(UmiRootedFileWrite(directory, fixture.paths[1], "changed", 7U) == UMI_STATUS_OK);
        if (strcmp(name, "missing-source") == 0)
            CHECK(UmiRootedFileRemove(directory, fixture.paths[1]) == UMI_STATUS_OK);
        if (strcmp(name, "invalid-location") == 0 || strcmp(name, "wrong-identity") == 0) {
            UmiLearningTextHit hit;
            CHECK(UmiLearningCatalogueGtk4BodySearchResultAt(panel, 1U, &hit) == UMI_STATUS_OK);
            if (strcmp(name, "invalid-location") == 0) hit.byte_end = SIZE_MAX;
            else hit.resource.sha256[0] = hit.resource.sha256[0] == 'a' ? 'b' : 'a';
            CHECK(LearningCatalogueOpenTextHit(LearningCatalogueOwner(panel), &hit) == UMI_STATUS_OK);
        } else CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel, 1U) == UMI_STATUS_OK);
        CHECK(Finish() && UmiLearningCatalogueGtk4CopyState(panel, &state) == UMI_STATUS_OK);
        CHECK(state.last_status != UMI_STATUS_OK && strcmp(state.reader.selection.resource.id, "guide") == 0);
        CHECK(Selected(buffer, "ownership"));
    } else if (strcmp(name, "history") == 0 || strcmp(name, "reload") == 0) {
        CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel, 1U) == UMI_STATUS_OK && Finish());
        CHECK(Selected(buffer, "ownership"));
        if (strcmp(name, "history") == 0) {
            CHECK(UmiLearningCatalogueGtk4Back(panel) == UMI_STATUS_OK && Finish());
            CHECK(UmiLearningCatalogueGtk4CopyState(panel, &state) == UMI_STATUS_OK &&
                strcmp(state.reader.selection.resource.id, "guide") == 0);
            CHECK(UmiLearningCatalogueGtk4Forward(panel) == UMI_STATUS_OK && Finish());
        } else CHECK(UmiLearningCatalogueGtk4Reload(panel) == UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &state) == UMI_STATUS_OK &&
            state.last_status == UMI_STATUS_OK && strcmp(state.reader.selection.resource.id, "source") == 0);
        /* A reading history entry stores the resource, not a transient search
         * query. An explicit search-result open selects its location again. */
        CHECK(UmiLearningCatalogueGtk4ReadBodySearchResult(panel, 1U) == UMI_STATUS_OK && Finish());
        CHECK(Selected(buffer, "ownership"));
    } else if (strcmp(name, "guide") != 0 && strcmp(name, "source") != 0 &&
        strcmp(name, "note") != 0 && strcmp(name, "unicode-guide") != 0 &&
        strcmp(name, "unicode-source") != 0 && strcmp(name, "last-byte") != 0 &&
        strcmp(name, "uppercase-html") != 0) known = 0;
    CHECK(known);
cleanup:
    if (panel != NULL) UmiLearningCatalogueGtk4Retire(panel);
    g_clear_object(&panel);
    if (!Finish()) result = 1;
    UmiLearningShelfDestroy(shelf);
    return result;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) return 77;
    return Check(argv[1], argv[2]);
}
