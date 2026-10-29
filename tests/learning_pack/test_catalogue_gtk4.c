/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_gtk4.c
 *
 * PURPOSE:
 *   Check asynchronous native catalogue ownership, history and late-result refusal.
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
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)
static const char first[] = "<h1>First guide</h1><p>Read the source.</p>";
static const char next[] = "int answer(void) { return 42; }\n";
static const unsigned char binary[] = {0U, 0xffU, 'C'};

static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
        child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
static UmiStatus Fixture(const char *root, int binary_content)
{
    const void *bytes = binary_content ? (const void *)binary : (const void *)first;
    size_t size = binary_content ? sizeof(binary) : sizeof(first) - 1U;
    char hash[UMI_SHA256_HEX_CAPACITY], next_hash[UMI_SHA256_HEX_CAPACITY];
    UmiStatus status = UmiSha256Buffer(bytes, size, hash);
    if (status == UMI_STATUS_OK) status = UmiSha256Buffer(next, sizeof(next) - 1U, next_hash);
    if (status == UMI_STATUS_OK) status = UmiRootedFileWrite(root, "first.html", bytes, size);
    if (status == UMI_STATUS_OK) status = UmiRootedFileWrite(root, "next.c", next, sizeof(next) - 1U);
    if (status != UMI_STATUS_OK) return status;
    char manifest[2048];
    int n = snprintf(manifest, sizeof(manifest),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"catalogue.fixture\","
        "\"title\":\"Native learning\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"first\",\"title\":\"First guide\",\"topic\":\"reading\",\"kind\":\"guide\","
        "\"path\":\"first.html\",\"sha256\":\"%s\"},"
        "{\"id\":\"next\",\"title\":\"Next source\",\"topic\":\"reading\",\"kind\":\"example\","
        "\"path\":\"next.c\",\"sha256\":\"%s\"}]}", hash, next_hash);
    if (n < 0 || (size_t)n >= sizeof(manifest)) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiRootedFileWrite(root, "learning-pack.json", manifest, (size_t)n);
}
/* Completion accounting covers requests whose weak widget has already died.
 * This is a bounded GTK-context drain, not a sleep used as proof of completion. */
static int FinishReads(void)
{
    gint64 deadline = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
static char *Content(GtkWidget *root)
{
    GtkWidget *view = Find(root, "learning.catalogue.content");
    if (view == NULL) return NULL;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter begin, end;
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    return gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
}
static void WeakGone(gpointer data, GObject *object)
{ (void)object; *(int *)data = 1; }
typedef struct Reentry {
    GtkWidget *root;
    const char *directory;
    unsigned calls;
    UmiStatus observed;
} Reentry;
static void Reenter(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    Reentry *request = data;
    ++request->calls;
    request->observed = UmiLearningCatalogueGtk4Load(request->root, request->directory);
}

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) return 77;
    const char *name = argv[1], *directory = argv[2];
    int result = 0, gone = 0;
    GtkWidget *root = NULL, *retained = NULL;
    char *text = NULL;
    UmiLearningCatalogueGtkState state;
    CHECK(Fixture(directory, strcmp(name, "binary") == 0) == UMI_STATUS_OK);
    root = UmiLearningCatalogueGtk4Create((UmiVersion){0U, 9U, 0U});
    CHECK(root != NULL);
    g_object_ref_sink(root);
    if (strcmp(name, "controls") == 0) {
        const char *ids[] = {"path", "query", "results", "load", "search", "open", "back",
            "forward", "reload", "content", "heading", "status"};
        for (size_t i = 0U; i < sizeof(ids) / sizeof(ids[0]); ++i) {
            char id[80];
            int n = snprintf(id, sizeof(id), "learning.catalogue.%s", ids[i]);
            CHECK(n > 0 && (size_t)n < sizeof(id) && Find(root, id) != NULL);
        }
        CHECK(UmiLearningCatalogueGtk4Load(root, "relative") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4Open(root, 0U) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4Back(root) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(!state.busy && !state.reader.has_document && state.pack_count == 0U);
        goto cleanup;
    }
    if (strcmp(name, "retire-idle") == 0) {
        retained = Find(root, "learning.catalogue.load");
        CHECK(retained != NULL);
        g_object_ref(retained);
        UmiLearningCatalogueGtk4Retire(root);
        UmiLearningCatalogueGtk4Retire(root);
        CHECK(UmiLearningCatalogueGtk4Load(root, directory) == UMI_STATUS_INVALID_STATE);
        g_signal_emit_by_name(retained, "clicked");
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U);
        g_object_unref(root);
        root = NULL;
        g_signal_emit_by_name(retained, "clicked");
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U);
        goto cleanup;
    }
    if (strcmp(name, "missing-pack") == 0)
        CHECK(UmiRootedFileRemove(directory, "learning-pack.json") == UMI_STATUS_OK);
    if (strcmp(name, "invalid-query-load") == 0)
        gtk_editable_set_text(GTK_EDITABLE(Find(root, "learning.catalogue.query")),
            "one two three four five six seven eight nine");

    Reentry reentry = {root, directory, 0U, UMI_STATUS_OK};
    GtkWidget *status_label = Find(root, "learning.catalogue.status");
    gulong handler = 0U;
    if (strcmp(name, "reentrant-busy") == 0)
        handler = g_signal_connect(status_label, "notify::label", G_CALLBACK(Reenter), &reentry);
    CHECK(UmiLearningCatalogueGtk4Load(root, directory) == UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4Load(root, directory) == UMI_STATUS_BUSY);
    CHECK(UmiLearningCatalogueGtk4Search(root, "") == UMI_STATUS_BUSY);
    if (strcmp(name, "retire-pending") == 0) UmiLearningCatalogueGtk4Retire(root);
    if (strcmp(name, "destroy-pending") == 0) {
        g_object_weak_ref(G_OBJECT(root), WeakGone, &gone);
        g_object_unref(root);
        root = NULL;
        CHECK(gone);
        CHECK(FinishReads());
        goto cleanup;
    }
    CHECK(FinishReads());
    if (handler != 0U) {
        g_signal_handler_disconnect(status_label, handler);
        CHECK(reentry.calls > 0U && reentry.observed == UMI_STATUS_BUSY);
    }
    CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK && !state.busy);
    if (strcmp(name, "retire-pending") == 0) {
        CHECK(state.retired && state.pack_count == 0U && !state.reader.has_document);
        goto cleanup;
    }
    if (strcmp(name, "missing-pack") == 0 || strcmp(name, "invalid-query-load") == 0) {
        CHECK(state.last_status != UMI_STATUS_OK && state.pack_count == 0U);
        CHECK(state.search.result_count == 0U);
        goto cleanup;
    }
    CHECK(state.last_status == UMI_STATUS_OK && state.pack_count == 1U && state.search.result_count == 2U);
    if (strcmp(name, "load") == 0 || strcmp(name, "reentrant-busy") == 0) goto cleanup;
    if (strcmp(name, "duplicate") == 0) {
        CHECK(UmiLearningCatalogueGtk4Load(root, directory) == UMI_STATUS_OK);
        CHECK(FinishReads());
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(state.last_status == UMI_STATUS_ALREADY_EXISTS && state.pack_count == 1U &&
            state.search.result_count == 2U);
        goto cleanup;
    }
    if (strcmp(name, "search") == 0) {
        CHECK(UmiLearningCatalogueGtk4Search(root, "Next") == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(state.search.result_count == 1U);
        CHECK(UmiLearningCatalogueGtk4Search(root, "absent") == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(state.search.result_count == 0U);
        CHECK(UmiLearningCatalogueGtk4Open(root, 0U) == UMI_STATUS_NOT_FOUND);
        goto cleanup;
    }
    CHECK(strcmp(name, "open") == 0 || strcmp(name, "history") == 0 ||
        strcmp(name, "changed-content") == 0 || strcmp(name, "binary") == 0);
    CHECK(UmiLearningCatalogueGtk4Open(root, 0U) == UMI_STATUS_OK);
    CHECK(FinishReads());
    CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
    CHECK(state.last_status == UMI_STATUS_OK && state.reader.has_document);
    CHECK(strcmp(state.reader.selection.resource.id, "first") == 0);
    text = Content(root);
    CHECK(text != NULL && strstr(text, strcmp(name, "binary") == 0 ?
        "not valid UTF-8 text" : "First guide") != NULL);
    if (strcmp(name, "changed-content") == 0) {
        CHECK(UmiRootedFileWrite(directory, "first.html", "changed", 7U) == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4Reload(root) == UMI_STATUS_OK);
        CHECK(FinishReads());
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(state.last_status == UMI_STATUS_INVALID_STATE && state.reader.history_count == 1U);
        char *after = Content(root);
        int same = after != NULL && strcmp(text, after) == 0;
        g_free(after);
        CHECK(same);
    }
    if (strcmp(name, "history") == 0) {
        CHECK(UmiLearningCatalogueGtk4Open(root, 1U) == UMI_STATUS_OK);
        CHECK(FinishReads());
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(state.reader.history_count == 2U && state.reader.can_go_back);
        CHECK(UmiLearningCatalogueGtk4Back(root) == UMI_STATUS_OK);
        CHECK(FinishReads());
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(strcmp(state.reader.selection.resource.id, "first") == 0 && state.reader.can_go_forward);
        CHECK(UmiLearningCatalogueGtk4Forward(root) == UMI_STATUS_OK);
        CHECK(FinishReads());
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(strcmp(state.reader.selection.resource.id, "next") == 0);
    }
cleanup:
    if (root != NULL) UmiLearningCatalogueGtk4Retire(root);
    if (!FinishReads()) result = 1;
    g_free(text);
    g_clear_object(&root);
    g_clear_object(&retained);
    return result;
}
