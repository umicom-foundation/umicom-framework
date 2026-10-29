/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_reading_gtk4.c
 *
 * PURPOSE:
 *   Verify native reading structure, local navigation, Unicode offsets and callback lifetime boundaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/learning_catalogue.h"
#include "umicom/platform/rooted_files.h"
#include "../support/gtk4_expanders.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); failed = 1; goto cleanup; } } while (0)
static const char guide[] =
    "<h1 id='start'>\xce\xa9 Guide</h1><p>Read the source.</p>"
    "<pre><code>int answer(void)\n{\n    return 42;\n}</code></pre>"
    "<h2 id='details'>Details</h2><p><a href='#details'>Section</a> "
    "<a href='example.c'>Source</a> <a href='https://invalid.example/'>Remote</a></p>"
    "<img src='not-loaded.png' alt='Ownership flow'>";
static int Fixture(const char *root)
{
    const char *contents[] = {guide, "<h1>Unclosed heading", "int answer(void) { return 42; }\n"};
    const char *paths[] = {"guide.html","fallback.html","example.c"};
    char hashes[3][UMI_SHA256_HEX_CAPACITY], json[3000];
    for (size_t i = 0U; i < 3U; ++i)
        if (UmiSha256Buffer(contents[i], strlen(contents[i]), hashes[i]) != UMI_STATUS_OK ||
            UmiRootedFileWrite(root, paths[i], contents[i], strlen(contents[i])) != UMI_STATUS_OK) return 0;
    int n = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"reading.fixture\","
        "\"title\":\"Reading fixture\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"structured-first\",\"title\":\"First guide\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"guide.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"fallback\",\"title\":\"Fallback guide\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"fallback.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"source\",\"title\":\"Source\",\"topic\":\"reading\",\"kind\":\"example\","
        "\"path\":\"example.c\",\"sha256\":\"%s\"}]}", hashes[0],hashes[1],hashes[2]);
    return n > 0 && (size_t)n < sizeof(json) &&
        UmiRootedFileWrite(root, "learning-pack.json", json, (size_t)n) == UMI_STATUS_OK;
}
static int Finish(void)
{
    gint64 deadline = g_get_monotonic_time() + 15*G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
    }
    return NULL;
}
static char *Text(GtkTextBuffer *buffer)
{
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
}
static int Cursor(GtkTextBuffer *buffer)
{
    GtkTextIter iter; gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
    return gtk_text_iter_get_offset(&iter);
}
typedef struct Observer { GtkWidget *root; unsigned calls; UmiStatus mutation, metadata; int retire; } Observer;
static void Changed(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    Observer *observer = data; ++observer->calls;
    UmiBrowserReadingSummary summary;
    observer->metadata = UmiLearningCatalogueGtk4CopyReadingSummary(observer->root, &summary);
    observer->mutation = UmiLearningCatalogueGtk4SetReadingZoom(observer->root, 150U);
    if (observer->retire) UmiLearningCatalogueGtk4Retire(observer->root);
}
static void TagAdded(GtkTextTagTable *table, GtkTextTag *tag, gpointer data)
{
    (void)table; (void)tag; UmiLearningCatalogueGtk4Retire(GTK_WIDGET(data));
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) return 77;
    const char *name = argv[1];
    int failed = 0, matched = 0;
    GtkWidget *panel = NULL, *other = NULL;
    GtkTextBuffer *buffer = NULL;
    char *text = NULL, *after_text = NULL;
    gulong signal = 0U;
    GObject *signal_owner = NULL;
    UmiLearningCatalogueGtkState before, after;
    UmiBrowserReadingSummary summary;
    CHECK(Fixture(argv[2]));
    panel = UmiLearningCatalogueGtk4Create((UmiVersion){0,9,0}); CHECK(panel != NULL); g_object_ref_sink(panel);
    UmiTestExpandSections(panel);
    GtkWidget *view = Find(panel, "learning.catalogue.content"); CHECK(GTK_IS_TEXT_VIEW(view));
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    if (strcmp(name,"empty") == 0) {
        matched = 1;
        CHECK(UmiLearningCatalogueGtk4CopyReadingSummary(panel,&summary) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4GoToAnchor(panel,"start") == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4OpenReadingLink(panel,0U) == UMI_STATUS_NOT_FOUND);
        goto cleanup;
    }
    CHECK(UmiLearningCatalogueGtk4Load(panel,argv[2]) == UMI_STATUS_OK && Finish());
    CHECK(UmiLearningCatalogueGtk4Search(panel,"structured-first") == UMI_STATUS_OK);
    Observer observer = {panel,0U,UMI_STATUS_OK,UMI_STATUS_NOT_FOUND,strcmp(name,"retire-publication")==0};
    if (strcmp(name,"reentrant") == 0 || strcmp(name,"retire-publication") == 0)
    {
        signal_owner = G_OBJECT(buffer);
        signal = g_signal_connect(buffer,"changed",G_CALLBACK(Changed),&observer);
    }
    if (strcmp(name,"retire-tag-creation") == 0) {
        signal_owner = G_OBJECT(gtk_text_buffer_get_tag_table(buffer));
        signal = g_signal_connect(signal_owner,"tag-added",G_CALLBACK(TagAdded),panel);
    }
    CHECK(UmiLearningCatalogueGtk4Open(panel,0U) == UMI_STATUS_OK);
    if (strcmp(name,"busy") == 0) {
        matched = 1;
        CHECK(UmiLearningCatalogueGtk4GoToAnchor(panel,"start") == UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4OpenReadingLink(panel,0U) == UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4SetReadingZoom(panel,150U) == UMI_STATUS_BUSY);
    }
    if (strcmp(name,"destroy-pending") == 0) {
        matched = 1; UmiLearningCatalogueGtk4Retire(panel); g_clear_object(&panel); CHECK(Finish()); goto cleanup;
    }
    CHECK(Finish());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel,&before) == UMI_STATUS_OK);
    if (strcmp(name,"retire-tag-creation") == 0) {
        matched = 1;
        CHECK(before.retired && !before.busy && before.has_reading_structure);
        GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);
        CHECK(gtk_text_tag_table_lookup(table,"umicom-reading-body") != NULL);
        CHECK(gtk_text_tag_table_lookup(table,"umicom-reading-link") == NULL);
        CHECK(gtk_text_tag_table_lookup(table,"umicom-reading-heading-1") == NULL);
        goto cleanup;
    }
    if (strcmp(name,"reentrant") == 0 || strcmp(name,"retire-publication") == 0) {
        matched = 1;
        CHECK(observer.calls > 0U && observer.mutation == UMI_STATUS_BUSY && observer.metadata == UMI_STATUS_OK);
        CHECK(!before.busy && before.has_reading_structure);
        CHECK(before.retired == observer.retire);
        goto cleanup;
    }
    CHECK(before.last_status == UMI_STATUS_OK && before.has_reading_structure && !before.busy);
    CHECK(UmiLearningCatalogueGtk4CopyReadingSummary(panel,&summary) == UMI_STATUS_OK);
    CHECK(summary.links == 3U && summary.anchors == 2U && summary.image_descriptions == 1U);
    text = Text(buffer); CHECK(text != NULL);
    if (strcmp(name,"formatted") == 0 || strcmp(name,"unicode") == 0) {
        matched = 1;
        CHECK(strstr(text,"{\n    return 42;\n}") != NULL && strstr(text,"[Image: Ownership flow]") != NULL);
        GtkTextIter iter; gtk_text_buffer_get_start_iter(buffer,&iter);
        GtkTextTag *heading = gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(buffer),"umicom-reading-heading-1");
        CHECK(heading != NULL && gtk_text_iter_has_tag(&iter,heading));
        CHECK(UmiLearningCatalogueGtk4GoToAnchor(panel,"details") == UMI_STATUS_OK);
        const char *details = strstr(text,"Details"); CHECK(details != NULL);
        CHECK(Cursor(buffer) == (int)g_utf8_pointer_to_offset(text,details));
    } else if (strcmp(name,"controls") == 0) {
        matched = 1;
        CHECK(GTK_IS_DROP_DOWN(Find(panel,"learning.catalogue.reading-links")));
        CHECK(GTK_IS_SPIN_BUTTON(Find(panel,"learning.catalogue.reading-zoom")));
        GtkWidget *entry = Find(panel,"learning.catalogue.reading-anchor");
        CHECK(GTK_IS_ENTRY(entry)); gtk_editable_set_text(GTK_EDITABLE(entry),"details");
        g_signal_emit_by_name(Find(panel,"learning.catalogue.reading-go-anchor"),"clicked");
        CHECK(Cursor(buffer) == (int)g_utf8_pointer_to_offset(text,strstr(text,"Details")));
    } else if (strcmp(name,"missing-anchor") == 0) {
        matched = 1;
        CHECK(UmiLearningCatalogueGtk4GoToAnchor(panel,"details") == UMI_STATUS_OK);
        int before_cursor = Cursor(buffer);
        CHECK(UmiLearningCatalogueGtk4GoToAnchor(panel,"missing") == UMI_STATUS_NOT_FOUND);
        CHECK(Cursor(buffer) == before_cursor);
    } else if (strcmp(name,"link-anchor") == 0 || strcmp(name,"link-source") == 0 ||
        strcmp(name,"link-control") == 0 || strcmp(name,"history") == 0) {
        matched = 1;
        size_t index = strcmp(name,"link-anchor") == 0 ? 0U : 1U;
        if (strcmp(name,"link-control") == 0) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(panel,"learning.catalogue.reading-links")),(guint)index);
            g_signal_emit_by_name(Find(panel,"learning.catalogue.reading-open-link"),"clicked");
        } else CHECK(UmiLearningCatalogueGtk4OpenReadingLink(panel,index) == UMI_STATUS_OK);
        CHECK(Finish() && UmiLearningCatalogueGtk4CopyState(panel,&after) == UMI_STATUS_OK && after.last_status == UMI_STATUS_OK);
        CHECK(strcmp(after.reader.selection.resource.id,index == 0U ? "structured-first" : "source") == 0);
        if (index == 0U) {
            CHECK(strcmp(after.reader.selection.fragment,"details") == 0);
            CHECK(Cursor(buffer) == (int)g_utf8_pointer_to_offset(text,strstr(text,"Details")));
        } else CHECK(!after.has_reading_structure);
        if (strcmp(name,"history") == 0) {
            CHECK(UmiLearningCatalogueGtk4Back(panel) == UMI_STATUS_OK && Finish());
            CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after) == UMI_STATUS_OK && after.has_reading_structure);
            CHECK(UmiLearningCatalogueGtk4CopyReadingSummary(panel,&summary) == UMI_STATUS_OK && summary.links == 3U);
        }
    } else if (strcmp(name,"remote") == 0 || strcmp(name,"invalid-index") == 0) {
        matched = 1;
        CHECK(UmiLearningCatalogueGtk4OpenReadingLink(panel,strcmp(name,"remote") == 0 ? 2U : 99U) != UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after) == UMI_STATUS_OK && after.reader.revision == before.reader.revision);
    } else if (strcmp(name,"fallback") == 0) {
        matched = 1;
        CHECK(UmiLearningCatalogueGtk4Search(panel,"fallback") == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4Open(panel,0U) == UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after) == UMI_STATUS_OK && !after.has_reading_structure && after.last_status == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4CopyReadingSummary(panel,&summary) == UMI_STATUS_NOT_FOUND);
        after_text = Text(buffer); CHECK(strstr(after_text,"Unclosed heading") != NULL);
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(Find(panel,"learning.catalogue.reading-links")))) == 0U);
    } else if (strcmp(name,"changed-resource") == 0) {
        matched = 1;
        CHECK(UmiRootedFileWrite(argv[2],"guide.html","changed",7U) == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4Reload(panel) == UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after) == UMI_STATUS_OK && after.last_status != UMI_STATUS_OK && after.has_reading_structure);
        CHECK(after.reader.revision == before.reader.revision);
        after_text = Text(buffer); CHECK(strcmp(text,after_text) == 0);
        CHECK(UmiLearningCatalogueGtk4CopyReadingSummary(panel,&summary) == UMI_STATUS_OK && summary.links == 3U);
    } else if (strcmp(name,"zoom") == 0 || strcmp(name,"zoom-controls") == 0 || strcmp(name,"independent-zoom") == 0) {
        matched = 1;
        if (strcmp(name,"zoom-controls") == 0) gtk_spin_button_set_value(GTK_SPIN_BUTTON(Find(panel,"learning.catalogue.reading-zoom")),150.0);
        else CHECK(UmiLearningCatalogueGtk4SetReadingZoom(panel,150U) == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after) == UMI_STATUS_OK && after.reading_zoom_percent == 150U);
        GtkTextTag *body = gtk_text_tag_table_lookup(gtk_text_buffer_get_tag_table(buffer),"umicom-reading-body");
        double points = 0.0; CHECK(body != NULL); g_object_get(body,"size-points",&points,NULL); CHECK(points == 18.0);
        CHECK(UmiLearningCatalogueGtk4SetReadingZoom(panel,49U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4SetReadingZoom(panel,201U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel,&after) == UMI_STATUS_OK && after.reading_zoom_percent == 150U);
        if (strcmp(name,"independent-zoom") == 0) {
            other = UmiLearningCatalogueGtk4Create((UmiVersion){0,9,0}); CHECK(other != NULL); g_object_ref_sink(other);
            CHECK(UmiLearningCatalogueGtk4CopyState(other,&after) == UMI_STATUS_OK && after.reading_zoom_percent == 100U);
        }
    } else if (strcmp(name,"retired") == 0) {
        matched = 1; UmiLearningCatalogueGtk4Retire(panel);
        CHECK(UmiLearningCatalogueGtk4OpenReadingLink(panel,0U) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4GoToAnchor(panel,"start") == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4SetReadingZoom(panel,150U) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name,"missing-requested-anchor") == 0) {
        matched = 1;
        CHECK(UmiLearningCatalogueGtk4Follow(panel,"#unknown") == UMI_STATUS_OK && Finish());
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Find(panel,"learning.catalogue.status"))),"anchor was not found") != NULL);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
        CHECK(after.last_status == UMI_STATUS_OK && after.reader.has_document && after.has_reading_structure);
        CHECK(strcmp(after.reader.selection.fragment, "unknown") == 0);
        CHECK(Cursor(buffer) == 0);
        after_text = Text(buffer); CHECK(after_text != NULL && strcmp(after_text, text) == 0);
    }
    CHECK(matched);
cleanup:
    if (signal != 0U && signal_owner != NULL) g_signal_handler_disconnect(signal_owner,signal);
    if (panel != NULL) UmiLearningCatalogueGtk4Retire(panel);
    if (other != NULL) UmiLearningCatalogueGtk4Retire(other);
    if (!Finish()) failed = 1;
    g_clear_object(&panel); g_clear_object(&other); g_free(text); g_free(after_text);
    return failed;
}
