/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_replacement_gtk4.c
 *
 * PURPOSE:
 *   Exercise explicit pack review, stale acceptance and independent native reader revisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/learning_catalogue.h"
#include "../support/gtk4_expanders.h"
#include "umicom/platform/rooted_files.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

static int fail_search, search_failures;
static UmiStatus ReplacementSearch(const UmiLearningShelf *shelf,
    const UmiLearningSearchQuery *query, size_t maximum, UmiLearningSearch **out)
{
    if (fail_search) { ++search_failures; *out = NULL; return UMI_STATUS_OUT_OF_MEMORY; }
    return UmiLearningSearchCreate(shelf, query, maximum, out);
}
/* The real GTK owner is compiled with a narrow search-allocation substitution.
 * Other catalogue objects come from the archive; no production hook is added. */
#define UmiLearningSearchCreate ReplacementSearch
#include "../../adapters/gtk4/learning_catalogue_gtk4.c"
#undef UmiLearningSearchCreate

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    failed = 1; goto cleanup; } } while (0)
static int Drain(void)
{
    gint64 end = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < end) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
static GtkWidget *FindControl(GtkWidget *widget, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
        child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = FindControl(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
static int Fixture(const char *directory, const char *id, const char *product,
    const char *locale, uint64_t revision)
{
    const char guide[] = "<h1>Guide</h1><p>Read the linked source example.</p>";
    const char *code = revision == 1U ? "int example(void) { return 1; }\n" : "int example(void) { return 2; }\n";
    char hash[UMI_SHA256_HEX_CAPACITY], code_hash[UMI_SHA256_HEX_CAPACITY], json[2400];
    if (g_mkdir_with_parents(directory, 0700) != 0 ||
        UmiSha256Buffer(guide, sizeof(guide)-1U, hash) != UMI_STATUS_OK ||
        UmiSha256Buffer(code, strlen(code), code_hash) != UMI_STATUS_OK) return 0;
    int size = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"%s\","
        "\"title\":\"Reviewed learning\",\"product\":\"%s\",\"locale\":\"%s\",\"revision\":%" PRIu64 ","
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"guide\",\"title\":\"Guide\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"guide.html\",\"sha256\":\"%s\",\"example\":\"source\"},"
        "{\"id\":\"source\",\"title\":\"Source\",\"topic\":\"reading\",\"kind\":\"example\","
        "\"path\":\"example.c\",\"sha256\":\"%s\"}]}", id, product, locale, revision, hash, code_hash);
    return size > 0 && (size_t)size < sizeof(json) &&
        UmiRootedFileWrite(directory, "guide.html", guide, sizeof(guide)-1U) == UMI_STATUS_OK &&
        UmiRootedFileWrite(directory, "example.c", code, strlen(code)) == UMI_STATUS_OK &&
        UmiRootedFileWrite(directory, "learning-pack.json", json, (size_t)size) == UMI_STATUS_OK;
}
static void RetireOnStatus(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    UmiLearningCatalogueGtk4Retire(GTK_WIDGET(data));
}
typedef struct AcceptCallback {
    GtkWidget *panel;
    uint64_t serial;
    UmiStatus nested;
    int calls;
} AcceptCallback;
static void AcceptDuringPublication(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    AcceptCallback *callback = data;
    ++callback->calls;
    callback->nested = UmiLearningCatalogueGtk4AcceptReplacement(callback->panel, callback->serial);
    UmiLearningCatalogueGtk4Retire(callback->panel);
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) return 77;
    const char *name = argv[1];
    int failed = 0;
    GtkWidget *panel = NULL, *signal_object = NULL;
    gulong handler = 0U;
    char *first = g_build_filename(argv[2], "original", NULL);
    char *next = g_build_filename(argv[2], "replacement", NULL);
    char *extra = g_build_filename(argv[2], "extra", NULL);
    UmiLearningCatalogueGtkState before, after;
    UmiLearningCatalogueReplacement review, observed;
    AcceptCallback callback = {0};
    CHECK(Fixture(first, "guide", "framework", "en", 1U));
    CHECK(Fixture(next, strcmp(name, "missing-id") == 0 ? "absent" : "guide",
        strcmp(name, "product-change") == 0 ? "studio" : "framework",
        strcmp(name, "locale-change") == 0 ? "fr" : "en",
        strcmp(name, "same-revision") == 0 ? 1U : 2U));
    panel = UmiLearningCatalogueGtk4Create((UmiVersion){0U, 9U, 0U});
    CHECK(panel != NULL); g_object_ref_sink(panel);
    /* Open fixture sections before lookup and before installing observers. */
    UmiTestExpandSections(panel);
    CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &review) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiLearningCatalogueGtk4Load(panel, first) == UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4Open(panel, 0U) == UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &before) == UMI_STATUS_OK && before.last_status == UMI_STATUS_OK);
    if (strcmp(name, "arguments") == 0) {
        CHECK(UmiLearningCatalogueGtk4PrepareReplacement(NULL, next) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4PrepareReplacement(panel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4PrepareReplacement(panel, "relative") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4AcceptReplacement(panel, 1U) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4DiscardReplacement(panel, 1U) == UMI_STATUS_NOT_FOUND);
        goto cleanup;
    }
    if (strcmp(name, "retired") == 0) {
        UmiLearningCatalogueGtk4Retire(panel);
        CHECK(UmiLearningCatalogueGtk4PrepareReplacement(panel, next) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4AcceptReplacement(panel, 1U) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4DiscardReplacement(panel, 1U) == UMI_STATUS_INVALID_STATE);
        goto cleanup;
    }
    if (strcmp(name, "dispatch-retirement") == 0) {
        signal_object = FindControl(panel, "learning.catalogue.status");
        handler = g_signal_connect(signal_object, "notify::label", G_CALLBACK(RetireOnStatus), panel);
        CHECK(UmiLearningCatalogueGtk4PrepareReplacement(panel, next) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK && !after.busy);
        goto cleanup;
    }
    if (strcmp(name, "native-controls") == 0) {
        GtkWidget *button = FindControl(panel, "learning.catalogue.review-replacement");
        CHECK(GTK_IS_BUTTON(button));
        gtk_editable_set_text(GTK_EDITABLE(FindControl(panel, "learning.catalogue.path")), next);
        g_signal_emit_by_name(button, "clicked");
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 1U);
        gtk_editable_set_text(GTK_EDITABLE(FindControl(panel, "learning.catalogue.path")), first);
    } else CHECK(UmiLearningCatalogueGtk4PrepareReplacement(panel, next) == UMI_STATUS_OK);
    if (strcmp(name, "busy") == 0) {
        CHECK(UmiLearningCatalogueGtk4PrepareReplacement(panel, next) == UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4Search(panel, "") == UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4AcceptReplacement(panel, 1U) == UMI_STATUS_BUSY);
    }
    if (strcmp(name, "destroy-pending") == 0) {
        UmiLearningCatalogueGtk4Retire(panel); g_object_unref(panel); panel = NULL;
        CHECK(Drain()); goto cleanup;
    }
    if (strcmp(name, "retire-pending") == 0) {
        UmiLearningCatalogueGtk4Retire(panel);
        CHECK(Drain());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
        CHECK(!after.busy && after.retired && after.reader.pack.revision == 1U);
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &review) == UMI_STATUS_INVALID_STATE);
        goto cleanup;
    }
    CHECK(Drain());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
    CHECK(after.pack_count == 1U && after.reader.revision == before.reader.revision);
    if (strcmp(name, "missing-id") == 0 || strcmp(name, "product-change") == 0 ||
        strcmp(name, "locale-change") == 0 || strcmp(name, "same-revision") == 0) {
        CHECK(after.last_status == (strcmp(name, "missing-id") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_INVALID_STATE));
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &review) == UMI_STATUS_NOT_FOUND);
        goto cleanup;
    }
    CHECK(after.last_status == UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &review) == UMI_STATUS_OK);
    CHECK(review.review_serial == 1U && review.shelf_generation == 1U);
    CHECK(review.previous.revision == 1U && review.proposed.revision == 2U);
    CHECK(review.changes.modified == 1U && review.changes.unchanged == 1U);
    CHECK(review.changes.changed_digests == 1U && review.changes.change_count == 1U);
    UmiLearningResourceChange change;
    memset(&change, 0, sizeof(change)); change.fields = 999U;
    CHECK(UmiLearningCatalogueGtk4ReplacementChangeAt(panel, review.review_serial + 1U,
        0U, &change) == UMI_STATUS_BUSY && change.fields == 999U);
    CHECK(UmiLearningCatalogueGtk4ReplacementChangeAt(panel, review.review_serial,
        1U, &change) == UMI_STATUS_NOT_FOUND && change.fields == 999U);
    CHECK(UmiLearningCatalogueGtk4ReplacementChangeAt(panel, review.review_serial, 0U, &change) == UMI_STATUS_OK);
    CHECK(change.kind == UMI_LEARNING_RESOURCE_MODIFIED && change.fields == UMI_LEARNING_CHANGE_DIGEST);
    CHECK(strcmp(change.proposed.id, "source") == 0 && strcmp(change.previous.sha256, change.proposed.sha256) != 0);
    const char *details = gtk_label_get_text(GTK_LABEL(FindControl(panel, "learning.catalogue.replacement-details")));
    CHECK(strstr(details, change.previous.sha256) != NULL && strstr(details, change.proposed.sha256) != NULL);
    CHECK(UmiLearningCatalogueGtk4AcceptReplacement(panel, review.review_serial + 1U) == UMI_STATUS_BUSY);
    CHECK(UmiLearningCatalogueGtk4DiscardReplacement(panel, review.review_serial + 1U) == UMI_STATUS_BUSY);
    if (strcmp(name, "discard") == 0) {
        CHECK(UmiLearningCatalogueGtk4DiscardReplacement(panel, review.review_serial) == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &observed) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4Open(panel, 0U) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK && after.reader.pack.revision == 1U);
        goto cleanup;
    }
    if (strcmp(name, "failed-prepare-keeps-review") == 0) {
        CHECK(UmiLearningCatalogueGtk4PrepareReplacement(panel, first) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK && after.last_status == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &observed) == UMI_STATUS_OK);
        CHECK(observed.review_serial == review.review_serial && observed.proposed.revision == 2U);
    } else if (strcmp(name, "stale-shelf") == 0) {
        CHECK(Fixture(extra, "other", "framework", "en", 1U));
        CHECK(UmiLearningCatalogueGtk4Load(panel, extra) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4AcceptReplacement(panel, review.review_serial) == UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &observed) == UMI_STATUS_OK);
        CHECK(observed.review_serial == review.review_serial);
        goto cleanup;
    } else if (strcmp(name, "superseded-review") == 0) {
        CHECK(Fixture(extra, "guide", "framework", "en", 3U));
        CHECK(UmiLearningCatalogueGtk4PrepareReplacement(panel, extra) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4AcceptReplacement(panel, review.review_serial) == UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &review) == UMI_STATUS_OK);
        CHECK(review.review_serial == 2U && review.proposed.revision == 3U);
    } else if (strcmp(name, "search-allocation") == 0 || strcmp(name, "query-refusal") == 0) {
        GtkWidget *query = FindControl(panel, "learning.catalogue.query");
        if (strcmp(name, "search-allocation") == 0) fail_search = 1;
        else gtk_editable_set_text(GTK_EDITABLE(query), "one two three four five six seven eight nine");
        CHECK(UmiLearningCatalogueGtk4AcceptReplacement(panel, review.review_serial) != UMI_STATUS_OK);
        fail_search = 0;
        if (strcmp(name, "search-allocation") == 0) CHECK(search_failures == 1);
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &observed) == UMI_STATUS_OK);
        CHECK(observed.review_serial == review.review_serial);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
        CHECK(after.reader.revision == before.reader.revision && after.search.result_count == before.search.result_count);
        gtk_editable_set_text(GTK_EDITABLE(query), "");
    } else if (strcmp(name, "accept-retirement") == 0) {
        signal_object = FindControl(panel, "learning.catalogue.results");
        callback.panel = panel; callback.serial = review.review_serial;
        handler = g_signal_connect(signal_object, "notify::model", G_CALLBACK(AcceptDuringPublication), &callback);
    } else CHECK(strcmp(name, "accept") == 0 || strcmp(name, "busy") == 0 ||
        strcmp(name, "reader-kept") == 0 || strcmp(name, "changed-resource") == 0 ||
        strcmp(name, "native-controls") == 0);
    if (strcmp(name, "native-controls") == 0)
        g_signal_emit_by_name(FindControl(panel, "learning.catalogue.accept-replacement"), "clicked");
    else CHECK(UmiLearningCatalogueGtk4AcceptReplacement(panel, review.review_serial) == UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK && !after.busy);
    CHECK(after.reader.revision == before.reader.revision && after.reader.pack.revision == 1U);
    if (strcmp(name, "accept-retirement") == 0) {
        CHECK(callback.calls == 1 && callback.nested == UMI_STATUS_BUSY && after.retired);
        goto cleanup;
    }
    CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &observed) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiLearningCatalogueGtk4ReplacementChangeAt(panel, review.review_serial, 0U, &change) == UMI_STATUS_NOT_FOUND);
    if (strcmp(name, "reader-kept") == 0) {
        CHECK(UmiLearningCatalogueGtk4Follow(panel, "example.c") == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
        CHECK(after.last_status == UMI_STATUS_OK && after.reader.pack.revision == 1U);
        CHECK(UmiLearningCatalogueGtk4Back(panel) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4OpenExample(panel) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
        CHECK(after.last_status == UMI_STATUS_OK && after.reader.pack.revision == 1U);
    }
    if (strcmp(name, "changed-resource") == 0)
        CHECK(UmiRootedFileWrite(next, "guide.html", "changed", 7U) == UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4Open(panel, 0U) == UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
    if (strcmp(name, "changed-resource") == 0) {
        CHECK(after.last_status == UMI_STATUS_INVALID_STATE && after.reader.pack.revision == 1U);
    } else CHECK(after.last_status == UMI_STATUS_OK && after.reader.pack.revision == review.proposed.revision);
cleanup:
    fail_search = 0;
    if (handler != 0U && signal_object != NULL) g_signal_handler_disconnect(signal_object, handler);
    if (panel != NULL) { UmiLearningCatalogueGtk4Retire(panel); g_object_unref(panel); }
    if (!Drain()) failed = 1;
    g_free(first); g_free(next); g_free(extra);
    return failed;
}
