/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_libraries_gtk4.c
 *
 * PURPOSE:
 *   Check native remembered-library jobs, lifetime, explicit admission and result ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "library_choices_fixture.h"
#include "umicom/ui/gtk4/learning_catalogue.h"
#include "../support/gtk4_expanders.h"
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); result = 1; goto cleanup; \
} } while (0)
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
static int Finish(void)
{
    gint64 deadline = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
typedef struct Reentry { GtkWidget *root; unsigned calls; UmiStatus status; } Reentry;
static void Reenter(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    Reentry *entry = data;
    ++entry->calls;
    entry->status = UmiLearningCatalogueGtk4ForgetLibrary(entry->root, 0U);
}
static void RetireOnNotify(GObject *object, GParamSpec *property, gpointer data)
{ (void)object; (void)property; UmiLearningCatalogueGtk4Retire(GTK_WIDGET(data)); }
static void Gone(gpointer data, GObject *object) { (void)object; *(int *)data = 1; }
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) { fprintf(stderr, "SKIP: GTK display unavailable.\n"); return 77; }
    const char *name = argv[1], *directory = argv[2];
    int result = 0, gone = 0;
    GtkWidget *root = NULL, *other = NULL;
    UmiLearningPack *fixture = NULL;
    UmiLearningLibraryChoices *saved = NULL;
    UmiLearningCatalogueGtkState state;
    UmiLearningCatalogueLibraryReceipt receipt;
    UmiLearningLibraryChoice choice;
    char filename[UMI_PATH_CAPACITY], basename[96];
    char *token = g_uuid_string_random();
    snprintf(basename, sizeof(basename), "libraries-%s.json", token); g_free(token);
    CHECK(umi_path_join(directory, basename, filename, sizeof(filename)) == UMI_STATUS_OK);
    CHECK(ChoiceFixturePack(directory, "native.library", "framework", "en-GB", 3U, 1, &fixture) == UMI_STATUS_OK);
    UmiLearningPackDestroy(fixture); fixture = NULL;
    root = UmiLearningCatalogueGtk4Create((UmiVersion){0,9,0}); CHECK(root != NULL);
    g_object_ref_sink(root); UmiTestExpandSections(root);
    if (strcmp(name, "controls") == 0) {
        CHECK(GTK_IS_ENTRY(Find(root, "learning.catalogue.libraries-path")));
        CHECK(GTK_IS_DROP_DOWN(Find(root, "learning.catalogue.libraries-choices")));
        CHECK(GTK_IS_BUTTON(Find(root, "learning.catalogue.libraries-remember")));
        CHECK(GTK_IS_BUTTON(Find(root, "learning.catalogue.libraries-forget")));
        CHECK(GTK_IS_BUTTON(Find(root, "learning.catalogue.libraries-reopen")));
        CHECK(GTK_IS_BUTTON(Find(root, "learning.catalogue.libraries-load")));
        CHECK(GTK_IS_BUTTON(Find(root, "learning.catalogue.libraries-save")));
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(root, "learning.catalogue.libraries-path"))), "") == 0);
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U); goto cleanup;
    }
    if (strcmp(name, "no-selection") == 0) {
        CHECK(UmiLearningCatalogueGtk4RememberLibrary(root) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4ForgetLibrary(root, 0U) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4ReopenLibrary(root, 0U) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiLearningCatalogueGtk4CopyLibraryReceipt(root, &receipt) == UMI_STATUS_NOT_FOUND);
        goto cleanup;
    }
    CHECK(UmiLearningCatalogueGtk4Load(root, directory) == UMI_STATUS_OK && Finish());
    CHECK(UmiLearningCatalogueGtk4Open(root, 0U) == UMI_STATUS_OK && Finish());
    CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK && state.reader.has_document);
    CHECK(UmiLearningCatalogueGtk4RememberLibrary(root) == UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4LibraryChoiceAt(root, 0U, &choice) == UMI_STATUS_OK);
    CHECK(strcmp(choice.pack_id, "native.library") == 0 && choice.revision == 3U);
    if (strcmp(name, "remember") == 0 || strcmp(name, "forget") == 0 || strcmp(name, "duplicate") == 0) {
        CHECK(UmiLearningCatalogueGtk4RememberLibrary(root) == UMI_STATUS_ALREADY_EXISTS);
        if (strcmp(name, "forget") == 0) {
            CHECK(UmiLearningCatalogueGtk4ForgetLibrary(root, 0U) == UMI_STATUS_OK);
            CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
            CHECK(state.library_choice_count == 0U && state.pack_count == 1U && state.reader.has_document);
        }
        goto cleanup;
    }
    if (strcmp(name, "arguments") == 0) {
        CHECK(UmiLearningCatalogueGtk4LoadLibraries(root, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4SaveLibrariesNew(root, "relative.json") == UMI_STATUS_INVALID_ARGUMENT);
        char ambiguous[UMI_PATH_CAPACITY];
        CHECK(snprintf(ambiguous, sizeof(ambiguous), "%s/../choices.json", directory) > 0);
        CHECK(UmiLearningCatalogueGtk4SaveLibrariesNew(root, ambiguous) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyLibraryReceipt(NULL, &receipt) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U); goto cleanup;
    }
    if (strcmp(name, "dispatch-retirement") == 0) {
        g_signal_connect(Find(root, "learning.catalogue.status"), "notify::label", G_CALLBACK(RetireOnNotify), root);
        CHECK(UmiLearningCatalogueGtk4SaveLibrariesNew(root, filename) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U);
        int exists = 1; CHECK(UmiRootedFileExists(directory, basename, &exists) == UMI_STATUS_OK && !exists);
        goto cleanup;
    }
    CHECK(UmiLearningCatalogueGtk4SaveLibrariesNew(root, filename) == UMI_STATUS_OK);
    CHECK(UmiLearningCatalogueGtk4ForgetLibrary(root, 0U) == UMI_STATUS_BUSY);
    if (strcmp(name, "destroy-pending") == 0) {
        g_object_weak_ref(G_OBJECT(root), Gone, &gone);
        g_object_unref(root); root = NULL; CHECK(gone);
    } else if (strcmp(name, "retire-pending") == 0) {
        UmiLearningCatalogueGtk4Retire(root);
        CHECK(UmiLearningCatalogueGtk4RememberLibrary(root) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "captured-path") == 0) {
        memset(filename, 'x', 8U);
        gtk_editable_set_text(GTK_EDITABLE(Find(root, "learning.catalogue.libraries-path")), "not-the-accepted-file");
    }
    CHECK(Finish());
    CHECK(UmiLearningLibraryChoicesLoad(directory, basename, &saved) == UMI_STATUS_OK);
    CHECK(UmiLearningLibraryChoicesCount(saved) == 1U);
    if (strcmp(name, "destroy-pending") == 0 || strcmp(name, "retire-pending") == 0 ||
        strcmp(name, "captured-path") == 0 || strcmp(name, "busy") == 0) goto cleanup;
    CHECK(UmiLearningCatalogueGtk4CopyLibraryReceipt(root, &receipt) == UMI_STATUS_OK);
    CHECK(receipt.status == UMI_STATUS_OK && receipt.creation.complete && receipt.creation.created);
    if (strcmp(name, "existing-save") == 0) {
        CHECK(UmiLearningCatalogueGtk4SaveLibrariesNew(root, filename) == UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyLibraryReceipt(root, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.status == UMI_STATUS_ALREADY_EXISTS && !receipt.creation.created); goto cleanup;
    }
    if (strcmp(name, "reader-retained") == 0 || strcmp(name, "duplicate-shelf") == 0) {
        UmiLearningReaderSnapshot before = state.reader;
        CHECK(UmiLearningCatalogueGtk4ReopenLibrary(root, 0U) == UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyLibraryReceipt(root, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.status == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(state.reader.has_document && state.reader.pack.revision == before.pack.revision &&
            strcmp(state.reader.selection.resource.sha256, before.selection.resource.sha256) == 0);
        goto cleanup;
    }
    if (strcmp(name, "invalid-load-preserves") == 0) {
        CHECK(UmiRootedFileWrite(directory, basename, "{}", 2U) == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4LoadLibraries(root, filename) == UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(root, &state) == UMI_STATUS_OK);
        CHECK(state.last_status != UMI_STATUS_OK && state.library_choice_count == 1U &&
            state.pack_count == 1U && state.reader.has_document);
        CHECK(UmiLearningCatalogueGtk4LibraryChoiceAt(root, 0U, &choice) == UMI_STATUS_OK && choice.revision == 3U);
        goto cleanup;
    }
    other = UmiLearningCatalogueGtk4Create((UmiVersion){0,9,0}); CHECK(other != NULL);
    g_object_ref_sink(other); UmiTestExpandSections(other);
    if (strcmp(name, "invalid-settings") == 0)
        CHECK(UmiRootedFileWrite(directory, basename, "{}", 2U) == UMI_STATUS_OK);
    else if (strcmp(name, "missing-settings") == 0)
        CHECK(UmiRootedFileRemove(directory, basename) == UMI_STATUS_OK);
    Reentry reentry = {other, 0U, UMI_STATUS_OK};
    gulong handler = 0U;
    if (strcmp(name, "reentrant") == 0)
        handler = g_signal_connect(Find(other, "learning.catalogue.libraries-choices"),
            "notify::model", G_CALLBACK(Reenter), &reentry);
    CHECK(UmiLearningCatalogueGtk4LoadLibraries(other, filename) == UMI_STATUS_OK && Finish());
    if (handler != 0U) {
        g_signal_handler_disconnect(Find(other, "learning.catalogue.libraries-choices"), handler);
        CHECK(reentry.calls != 0U && reentry.status == UMI_STATUS_BUSY);
    }
    CHECK(UmiLearningCatalogueGtk4CopyState(other, &state) == UMI_STATUS_OK);
    CHECK(state.pack_count == 0U && !state.reader.has_document);
    if (strcmp(name, "invalid-settings") == 0 || strcmp(name, "missing-settings") == 0) {
        CHECK(state.last_status != UMI_STATUS_OK && state.library_choice_count == 0U); goto cleanup;
    }
    CHECK(state.last_status == UMI_STATUS_OK && state.library_choice_count == 1U);
    if (strcmp(name, "load-only") == 0 || strcmp(name, "reentrant") == 0) goto cleanup;
    if (strcmp(name, "changed-identity") == 0 || strcmp(name, "changed-revision") == 0)
        CHECK(ChoiceFixturePack(directory, strcmp(name, "changed-identity") == 0 ? "another" : "native.library",
            "framework", "en-GB", strcmp(name, "changed-revision") == 0 ? 4U : 3U, 1, &fixture) == UMI_STATUS_OK);
    if (strcmp(name, "invalid-query") == 0)
        gtk_editable_set_text(GTK_EDITABLE(Find(other, "learning.catalogue.query")), "\001");
    CHECK(UmiLearningCatalogueGtk4ReopenLibrary(other, 0U) == UMI_STATUS_OK && Finish());
    CHECK(UmiLearningCatalogueGtk4CopyState(other, &state) == UMI_STATUS_OK);
    CHECK(!state.reader.has_document && state.library_choice_count == 1U);
    if (strcmp(name, "changed-identity") == 0 || strcmp(name, "changed-revision") == 0 ||
        strcmp(name, "invalid-query") == 0) CHECK(state.last_status != UMI_STATUS_OK && state.pack_count == 0U);
    else if (strcmp(name, "reopen") == 0) {
        CHECK(state.last_status == UMI_STATUS_OK && state.pack_count == 1U);
        CHECK(UmiLearningCatalogueGtk4Open(other, 0U) == UMI_STATUS_OK && Finish());
        CHECK(UmiLearningCatalogueGtk4CopyState(other, &state) == UMI_STATUS_OK && state.reader.has_document);
    } else { fprintf(stderr, "Unknown case: %s\n", name); result = 2; }
cleanup:
    if (root != NULL) { UmiLearningCatalogueGtk4Retire(root); g_object_unref(root); }
    if (other != NULL) { UmiLearningCatalogueGtk4Retire(other); g_object_unref(other); }
    if (!Finish()) result = 1;
    UmiLearningPackDestroy(fixture); UmiLearningLibraryChoicesDestroy(saved);
    return result;
}
