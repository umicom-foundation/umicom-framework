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
#include "../support/gtk4_expanders.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/path.h"
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


static void RetireDuringCopy(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    UmiLearningCatalogueGtk4Retire(GTK_WIDGET(data));
}

/* Exercise the actual GTK destination action as well as the copied service
 * contract. Every output name is unique to this run, avoiding destructive
 * setup and ensuring stale fixture files cannot manufacture a copy result. */
static int CopyCase(GtkWidget *root, const char *name, const char *directory)
{
    int result = 0;
    char *uuid = g_uuid_string_random();
    char *relative = g_strdup_printf("practice-%s.c", uuid);
    char *absolute = g_build_filename(directory, relative, NULL);
    char *before_text = NULL, *after_text = NULL;
    unsigned char *written = NULL;
    size_t written_size = 0U;
    UmiLearningCatalogueGtkState before, after;
    UmiLearningCatalogueCopyReceipt receipt;
    UmiLearningPackResource example;
    gulong observer = 0U;
    CHECK(uuid != NULL && relative != NULL && absolute != NULL);
    CHECK(UmiLearningCatalogueGtk4Open(root, strcmp(name, "copy-no-example") == 0 ? 0U : 1U) == UMI_STATUS_OK);
    CHECK(FinishReads());
    CHECK(UmiLearningCatalogueGtk4CopyState(root, &before) == UMI_STATUS_OK);
    CHECK(before.last_status == UMI_STATUS_OK && before.reader.has_document && !before.has_copy_result);
    before_text = Content(root);
    CHECK(before_text != NULL);
    CHECK(UmiLearningCatalogueGtk4CopyLastReceipt(root, &receipt) == UMI_STATUS_NOT_FOUND);
    if (strcmp(name, "copy-no-example") == 0) {
        CHECK(UmiLearningCatalogueGtk4PreviewExample(root, &example) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4CopyExample(root, directory, relative) == UMI_STATUS_NOT_FOUND);
        goto inspect;
    }
    CHECK(UmiLearningCatalogueGtk4PreviewExample(root, &example) == UMI_STATUS_OK);
    CHECK(strcmp(example.id, "next") == 0);
    if (strcmp(name, "copy-arguments") == 0) {
        CHECK(UmiLearningCatalogueGtk4CopyExample(root, "relative", relative) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyExample(root, directory, "../outside.c") != UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4CopyExample(root, NULL, relative) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4PreviewExample(root, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyLastReceipt(root, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        goto inspect;
    }
    /* Exercise admission through the visible entry and action. Construct the
     * original spelling without path helpers, which would hide the defect by
     * normalizing the very components this boundary is required to reject. */
    if (g_str_has_prefix(name, "copy-path-")) {
        const char *component = strcmp(name, "copy-path-dot") == 0 ? "/./" :
            strcmp(name, "copy-path-parent") == 0 ? "/unused/../" :
            strcmp(name, "copy-path-empty") == 0 ? "//" : "/";
        char *spelling = strcmp(name, "copy-path-relative") == 0 ?
            g_strdup(relative) : g_strdup_printf("%s%s%s%s", directory,
                component, relative, strcmp(name, "copy-path-trailing") == 0 ? "/" : "");
        CHECK(spelling != NULL);
        GtkWidget *entry = Find(root, "learning.catalogue.destination");
        GtkWidget *button = Find(root, "learning.catalogue.copy");
        CHECK(entry != NULL && button != NULL);
        gtk_editable_set_text(GTK_EDITABLE(entry), spelling);
        g_free(spelling);
        spelling = NULL;
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U);
        g_signal_emit_by_name(button, "clicked");
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &after) == UMI_STATUS_OK);
        CHECK(after.last_status == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!after.busy && !after.has_copy_result);
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U);
        CHECK(UmiLearningCatalogueGtk4CopyLastReceipt(root, &receipt) == UMI_STATUS_NOT_FOUND);
        CHECK(!g_file_test(absolute, G_FILE_TEST_EXISTS));
        goto inspect;
    }
    if (strcmp(name, "copy-existing") == 0)
        CHECK(UmiRootedFileWrite(directory, relative, "keep", 4U) == UMI_STATUS_OK);
    if (strcmp(name, "copy-changed") == 0)
        CHECK(UmiRootedFileWrite(directory, "next.c", "changed", 7U) == UMI_STATUS_OK);
    if (strcmp(name, "copy-reentrant") == 0)
        observer = g_signal_connect(Find(root, "learning.catalogue.status"), "notify::label",
            G_CALLBACK(RetireDuringCopy), root);
    if (strcmp(name, "copy") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(Find(root, "learning.catalogue.destination")), absolute);
        g_signal_emit_by_name(Find(root, "learning.catalogue.copy"), "clicked");
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &after) == UMI_STATUS_OK && after.busy);
    } else {
        UmiStatus status = UmiLearningCatalogueGtk4CopyExample(root, directory, relative);
        CHECK(status == (strcmp(name, "copy-reentrant") == 0 ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK));
    }
    if (observer != 0U) {
        g_signal_handler_disconnect(Find(root, "learning.catalogue.status"), observer);
        observer = 0U;
    }
    if (strcmp(name, "copy-reentrant") != 0) {
        CHECK(UmiLearningCatalogueGtk4CopyExample(root, directory, relative) == UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4Back(root) == UMI_STATUS_BUSY);
    }
    if (strcmp(name, "copy-destination-captured") == 0)
        gtk_editable_set_text(GTK_EDITABLE(Find(root, "learning.catalogue.destination")), "different.c");
    if (strcmp(name, "copy-retired") == 0) UmiLearningCatalogueGtk4Retire(root);
    CHECK(FinishReads());
    CHECK(UmiLearningCatalogueGtk4CopyState(root, &after) == UMI_STATUS_OK && !after.busy);
    if (strcmp(name, "copy-reentrant") == 0) {
        CHECK(after.retired && !after.has_copy_result);
        CHECK(UmiRootedFileRead(directory, relative, 1024U, &written, &written_size) == UMI_STATUS_NOT_FOUND);
        goto inspect;
    }
    if (strcmp(name, "copy-retired") == 0) {
        CHECK(after.retired && !after.has_copy_result);
        CHECK(UmiRootedFileRead(directory, relative, 1024U, &written, &written_size) == UMI_STATUS_OK);
        CHECK(written_size == sizeof(next) - 1U && memcmp(written, next, written_size) == 0);
        goto inspect;
    }
    CHECK(UmiLearningCatalogueGtk4CopyLastReceipt(root, &receipt) == UMI_STATUS_OK);
    /* The picker splits a normalized absolute path. Windows uses native
     * separators even when CMake supplied forward slashes. Compare normalized
     * roots for the picker, and retain exact capture checks for the direct API. */
#if 0
    CHECK(after.has_copy_result && strcmp(receipt.destination_root, directory) == 0);
#endif
    CHECK(after.has_copy_result);
    if (strcmp(name, "copy") == 0) {
        char normalized[2048];
        CHECK(umi_path_normalise(directory, normalized, sizeof(normalized)) == UMI_STATUS_OK);
        CHECK(strcmp(receipt.destination_root, normalized) == 0);
    } else {
        CHECK(strcmp(receipt.destination_root, directory) == 0);
    }
    CHECK(strcmp(receipt.destination_path, relative) == 0 && strcmp(receipt.selection.id, "next") == 0);
    if (strcmp(name, "copy-existing") == 0) {
        CHECK(receipt.result.status == UMI_STATUS_ALREADY_EXISTS && !receipt.result.destination.created);
        CHECK(UmiRootedFileRead(directory, relative, 1024U, &written, &written_size) == UMI_STATUS_OK);
        CHECK(written_size == 4U && memcmp(written, "keep", 4U) == 0);
    } else if (strcmp(name, "copy-changed") == 0) {
        CHECK(receipt.result.status == UMI_STATUS_INVALID_STATE && !receipt.result.destination_attempted);
        CHECK(UmiRootedFileRead(directory, relative, 1024U, &written, &written_size) == UMI_STATUS_NOT_FOUND);
    } else {
        CHECK(receipt.result.status == UMI_STATUS_OK && receipt.result.source_verified);
        CHECK(receipt.result.destination.created && receipt.result.destination.complete);
        CHECK(UmiRootedFileRead(directory, relative, 1024U, &written, &written_size) == UMI_STATUS_OK);
        CHECK(written_size == sizeof(next) - 1U && memcmp(written, next, written_size) == 0);
        CHECK(UmiLearningCatalogueGtk4Search(root, "") == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4CopyLastReceipt(root, &receipt) == UMI_STATUS_OK);
        CHECK(strcmp(receipt.destination_path, relative) == 0);
    }
inspect:
    CHECK(UmiLearningCatalogueGtk4CopyState(root, &after) == UMI_STATUS_OK);
    CHECK(after.reader.history_count == before.reader.history_count);
    CHECK(after.reader.revision == before.reader.revision);
    CHECK(strcmp(after.reader.selection.resource.id, before.reader.selection.resource.id) == 0);
    after_text = Content(root);
    CHECK(after_text != NULL && strcmp(before_text, after_text) == 0);
cleanup:
    if (observer != 0U) g_signal_handler_disconnect(Find(root, "learning.catalogue.status"), observer);
    if (!FinishReads()) result = 1;
    UmiRootedFileFree(written);
    g_free(before_text);
    g_free(after_text);
    g_free(absolute);
    g_free(relative);
    g_free(uuid);
    return result;
}

/* Contextual help does not depend on visible search rows. Its route owner may
 * be released immediately after admission; the job owns a captured selection. */
static int HelpCase(GtkWidget *root, const char *name, const char *directory)
{
    int result = 0;
    UmiLearningHelpMap *map = NULL;
    UmiLearningHelpRoute route = {0};
    strcpy(route.request.product, "studio");
    strcpy(route.request.locale, "en");
    strcpy(route.request.key, "document.save");
    route.request.kind = UMI_LEARNING_HELP_COMMAND;
    strcpy(route.pack_id, "catalogue.fixture");
    strcpy(route.resource_id, strcmp(name, "help-wrong-kind") == 0 ? "next" : "first");
    UmiLearningHelpRequest request = route.request;
    UmiLearningCatalogueGtkState before, after;
    gulong observer = 0U;
    CHECK(UmiLearningCatalogueGtk4Open(root, 1U) == UMI_STATUS_OK && FinishReads());
    CHECK(UmiLearningCatalogueGtk4CopyState(root, &before) == UMI_STATUS_OK);
    CHECK(before.reader.has_document && strcmp(before.reader.selection.resource.id, "next") == 0);
    CHECK(UmiLearningCatalogueGtk4Search(root, "no-visible-results") == UMI_STATUS_OK);
    CHECK(UmiLearningHelpMapCreate(&route, 1U, &map) == UMI_STATUS_OK);
    if (strcmp(name, "help-missing") == 0) strcpy(request.key, "missing");
    if (strcmp(name, "help-changed") == 0)
        CHECK(UmiRootedFileWrite(directory, "first.html", "changed", 7U) == UMI_STATUS_OK);
    if (strcmp(name, "help-reentrant") == 0)
        observer = g_signal_connect(Find(root, "learning.catalogue.status"), "notify::label",
            G_CALLBACK(RetireDuringCopy), root);
    UmiStatus expected = strcmp(name, "help-missing") == 0 ? UMI_STATUS_NOT_FOUND :
        strcmp(name, "help-wrong-kind") == 0 || strcmp(name, "help-reentrant") == 0 ?
        UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
    CHECK(UmiLearningCatalogueGtk4OpenHelp(root, map, &request) == expected);
    if (observer != 0U) {
        g_signal_handler_disconnect(Find(root, "learning.catalogue.status"), observer); observer = 0U;
    }
    if (expected == UMI_STATUS_OK) {
        CHECK(UmiLearningCatalogueGtk4OpenHelp(root, map, &request) == UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4Search(root, "") == UMI_STATUS_BUSY);
    }
    UmiLearningHelpMapDestroy(map); map = NULL;
    memset(&route, 0, sizeof(route)); memset(&request, 0, sizeof(request));
    if (strcmp(name, "help-retired") == 0) UmiLearningCatalogueGtk4Retire(root);
    CHECK(FinishReads());
    CHECK(UmiLearningCatalogueGtk4CopyState(root, &after) == UMI_STATUS_OK && !after.busy);
    CHECK(after.search.result_count == 0U);
    if (expected != UMI_STATUS_OK || strcmp(name, "help-changed") == 0 || strcmp(name, "help-retired") == 0) {
        CHECK(strcmp(after.reader.selection.resource.id, "next") == 0);
        CHECK(after.reader.history_count == before.reader.history_count);
        if (strcmp(name, "help-changed") == 0) CHECK(after.last_status == UMI_STATUS_INVALID_STATE);
    } else {
        CHECK(after.last_status == UMI_STATUS_OK && strcmp(after.reader.selection.resource.id, "first") == 0);
        CHECK(after.reader.history_count == before.reader.history_count + 1U);
        CHECK(UmiLearningCatalogueGtk4Back(root) == UMI_STATUS_OK && FinishReads());
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &after) == UMI_STATUS_OK);
        CHECK(strcmp(after.reader.selection.resource.id, "next") == 0);
    }
cleanup:
    if (observer != 0U) g_signal_handler_disconnect(Find(root, "learning.catalogue.status"), observer);
    if (!FinishReads()) result = 1;
    UmiLearningHelpMapDestroy(map);
    return result;
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
    /* Open fixture sections before lookup and before installing observers. */
    UmiTestExpandSections(root);
    if (strcmp(name, "controls") == 0) {
        const char *ids[] = {"path", "query", "results", "load", "search", "open", "back",
            "forward", "reload", "content", "heading", "status", "example", "copy", "destination", "copy-result"};
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
    if (g_str_has_prefix(name, "help-")) {
        result = HelpCase(root, name, directory);
        goto cleanup;
    }
    if (g_str_has_prefix(name, "copy")) {
        result = CopyCase(root, name, directory);
        goto cleanup;
    }
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
