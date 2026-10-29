/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_progress_gtk4.c
 *
 * PURPOSE:
 *   Verify explicit native checkpoints, captured identities and uncertain-write recovery.
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
#include "umicom/platform/filesystem.h"
#include "../../adapters/gtk4/learning_catalogue_private.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

/* Inject an ambiguous storage result after a real durable write. The actual
 * worker, public request admission and native completion remain in use. This
 * checks recovery rather than replacing persistence with a successful mock. */
static gint uncertain_write;
static void ProgressWorkerObservation(LearningCatalogueJob *job)
{
    LearningCatalogueProgressWorker(job);
    if (job->status == UMI_STATUS_OK &&
        job->progress_receipt.request.action == UMI_LEARNING_PROGRESS_RECORD &&
        g_atomic_int_compare_and_exchange(&uncertain_write, 1, 0)) {
        job->status = UMI_STATUS_IO_ERROR;
        job->progress_receipt.status = job->status;
        job->progress_receipt.journal.recovery_required = 1;
    }
}
#define LearningCatalogueProgressWorker ProgressWorkerObservation
#include "../../adapters/gtk4/learning_catalogue_jobs.c"
#undef LearningCatalogueProgressWorker

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    result = 1; goto cleanup; } } while (0)
static int Drain(void)
{
    gint64 end = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < end) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
static GtkWidget *Control(GtkWidget *widget, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL;
        child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Control(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
static int Fixture(const char *directory, uint64_t revision, const char *kind)
{
    const char body[] = "<h1>A local lesson</h1><p>Review the example deliberately.</p>";
    char hash[UMI_SHA256_HEX_CAPACITY], json[1600];
    if (g_mkdir_with_parents(directory, 0700) != 0 ||
        UmiSha256Buffer(body, sizeof(body)-1U, hash) != UMI_STATUS_OK) return 0;
    int size = snprintf(json, sizeof(json),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"native.progress\","
        "\"title\":\"Private checkpoints\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":%" PRIu64 ","
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"lesson\",\"title\":\"Lesson\",\"topic\":\"learning\",\"kind\":\"%s\","
        "\"path\":\"lesson.html\",\"sha256\":\"%s\"}]}", revision, kind, hash);
    return size > 0 && (size_t)size < sizeof(json) &&
        UmiRootedFileWrite(directory, "lesson.html", body, sizeof(body)-1U) == UMI_STATUS_OK &&
        UmiRootedFileWrite(directory, "learning-pack.json", json, (size_t)size) == UMI_STATUS_OK;
}
static void RetireOnLabel(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    UmiLearningCatalogueGtk4Retire(GTK_WIDGET(data));
}
typedef struct Reentrant {
    GtkWidget *panel;
    UmiLearningProgressLocalRequest request;
    UmiStatus status;
    int calls;
} Reentrant;
static void RepeatOnResult(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    Reentrant *call = data;
    ++call->calls;
    call->status = UmiLearningCatalogueGtk4Progress(call->panel, &call->request);
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) { fprintf(stderr, "SKIP: GTK display unavailable.\n"); return 77; }
    int result = 0;
    const char *name = argv[1];
    GtkWidget *panel = NULL, *signal_widget = NULL;
    gulong signal_id = 0U;
    UmiDataServer *database = NULL;
    UmiLearningProgressLocalRequest request = {0}, captured, inspect;
    UmiLearningProgressLocalReceipt receipt, previous;
    UmiLearningCatalogueGtkState before, after;
    UmiLearningCatalogueReplacement replacement;
    Reentrant reentrant = {0};
    char *token = g_uuid_string_random();
    char *directory = g_build_filename(argv[2], token, NULL);
    char *pack = g_build_filename(directory, "pack", NULL);
    char *next = g_build_filename(directory, "next", NULL);
    char *probe = g_build_filename(directory, "probe.sqlite3", NULL);
    char *other = g_build_filename(directory, "other.sqlite3", NULL);
    g_free(token);
    CHECK(Fixture(pack, 1U, strcmp(name, "wrong-kind") == 0 ? "example" : "lesson"));
    CHECK(umi_path_join(directory, "progress.sqlite3", request.database_path,
        sizeof(request.database_path)) == UMI_STATUS_OK);
    request.action = UMI_LEARNING_PROGRESS_RECORD;
    memcpy(request.learner_id, "learner", 8U);
    memcpy(request.checkpoint, "reviewed", 9U);
    memcpy(request.evidence_reference, "practice-1", 11U);
    captured = request;
    inspect = request; inspect.action = UMI_LEARNING_PROGRESS_INSPECT;
    inspect.evidence_reference[0] = '\0';
    panel = UmiLearningCatalogueGtk4Create((UmiVersion){0U, 9U, 0U});
    CHECK(panel != NULL); g_object_ref_sink(panel);
    /* Open fixture sections before lookup and before installing observers. */
    UmiTestExpandSections(panel);
    CHECK(UmiLearningCatalogueGtk4ProgressAvailable());
    CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiLearningCatalogueGtk4Load(panel, pack) == UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4Open(panel, 0U) == UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &before) == UMI_STATUS_OK);
    CHECK(before.last_status == UMI_STATUS_OK && before.reader.has_document);
    CHECK(!before.has_progress_result && !umi_fs_exists(request.database_path));
    if (strcmp(name, "no-automatic-write") == 0) {
        CHECK(UmiLearningCatalogueGtk4Reload(panel) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4Search(panel, "") == UMI_STATUS_OK);
        CHECK(!umi_fs_exists(request.database_path));
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_NOT_FOUND);
        goto cleanup;
    }
    if (strcmp(name, "arguments") == 0) {
        CHECK(UmiLearningCatalogueGtk4Progress(NULL, &request) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4Progress(panel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        memcpy(request.database_path, "relative.sqlite3", 17U);
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!umi_fs_exists(captured.database_path));
        goto cleanup;
    }
    if (strcmp(name, "wrong-kind") == 0) {
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_INVALID_STATE);
        CHECK(!umi_fs_exists(request.database_path));
        goto cleanup;
    }
    if (strcmp(name, "retired") == 0) {
        UmiLearningCatalogueGtk4Retire(panel);
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_INVALID_STATE);
        CHECK(!umi_fs_exists(request.database_path));
        goto cleanup;
    }
    if (strcmp(name, "dispatch-retirement") == 0) {
        signal_widget = Control(panel, "learning.catalogue.status");
        signal_id = g_signal_connect(signal_widget, "notify::label", G_CALLBACK(RetireOnLabel), panel);
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 0U);
        CHECK(!umi_fs_exists(request.database_path));
        goto cleanup;
    }
    UmiStatus supported = umi_data_server_create_sqlite(probe, &database);
    if (supported == UMI_STATUS_UNAVAILABLE) {
        fprintf(stderr, "SKIP: SQLite Data Server backend unavailable.\n"); result = 77; goto cleanup;
    }
    CHECK(supported == UMI_STATUS_OK);
    umi_data_server_destroy(database); database = NULL;
    if (strcmp(name, "inspect-missing") == 0) {
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &inspect) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.status != UMI_STATUS_OK && !receipt.storage_opened && !receipt.write_attempted);
        CHECK(!umi_fs_exists(request.database_path));
        goto unchanged;
    }
    if (strcmp(name, "uncertain-recovery") == 0 || strcmp(name, "other-learner-recovery") == 0 ||
        strcmp(name, "other-database-recovery") == 0) g_atomic_int_set(&uncertain_write, 1);
    if (strcmp(name, "reentrant-result") == 0) {
        reentrant.panel = panel; reentrant.request = request;
        signal_widget = Control(panel, "learning.catalogue.progress-result");
        signal_id = g_signal_connect(signal_widget, "notify::label", G_CALLBACK(RepeatOnResult), &reentrant);
    }
    if (strcmp(name, "native-controls") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(Control(panel, "learning.catalogue.progress-path")), request.database_path);
        gtk_editable_set_text(GTK_EDITABLE(Control(panel, "learning.catalogue.progress-learner")), request.learner_id);
        gtk_editable_set_text(GTK_EDITABLE(Control(panel, "learning.catalogue.progress-checkpoint")), request.checkpoint);
        gtk_editable_set_text(GTK_EDITABLE(Control(panel, "learning.catalogue.progress-evidence")), request.evidence_reference);
        GtkWidget *button = Control(panel, "learning.catalogue.progress-record");
        CHECK(GTK_IS_BUTTON(button)); g_signal_emit_by_name(button, "clicked");
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 1U);
        gtk_editable_set_text(GTK_EDITABLE(Control(panel, "learning.catalogue.progress-learner")), "changed");
    } else CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_OK);
    if (strcmp(name, "busy") == 0) {
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_BUSY);
        CHECK(UmiLearningCatalogueGtk4Reload(panel) == UMI_STATUS_BUSY);
    }
    if (strcmp(name, "captured-request") == 0) {
        memset(&request, 0, sizeof(request));
        CHECK(UmiRootedFileWrite(pack, "lesson.html", "changed", 7U) == UMI_STATUS_OK);
    }
    if (strcmp(name, "destroy-pending") == 0 || strcmp(name, "retire-pending") == 0) {
        UmiLearningCatalogueGtk4Retire(panel);
        if (strcmp(name, "destroy-pending") == 0) { g_object_unref(panel); panel = NULL; }
        CHECK(Drain());
        /* Accepted writes outlive presentation; inspect durable storage using a
         * new reader rather than assuming retirement cancelled the transaction. */
        UmiLearningProgress *journal = NULL;
        CHECK(umi_data_server_open_sqlite_readonly(captured.database_path, &database) == UMI_STATUS_OK);
        CHECK(UmiLearningProgressOpen(database, captured.learner_id, &journal) == UMI_STATUS_OK);
        UmiLearningProgressSnapshot snapshot;
        UmiStatus copied = UmiLearningProgressCopySnapshot(journal, &snapshot);
        UmiLearningProgressClose(journal);
        CHECK(copied == UMI_STATUS_OK && snapshot.event_count == 1U);
        if (panel != NULL) {
            CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
            CHECK(after.retired && !after.busy && !after.has_progress_result);
        }
        goto cleanup;
    }
    CHECK(Drain());
    CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_OK);
    CHECK(strcmp(receipt.request.database_path, captured.database_path) == 0);
    CHECK(strcmp(receipt.request.learner_id, captured.learner_id) == 0);
    CHECK(strcmp(receipt.event.sha256, before.reader.selection.resource.sha256) == 0);
    CHECK(receipt.selection.pack.revision == 1U && receipt.journal.event_count == 1U);
    request = captured;
    if (strcmp(name, "uncertain-recovery") == 0 || strcmp(name, "other-learner-recovery") == 0 ||
        strcmp(name, "other-database-recovery") == 0) {
        CHECK(receipt.status == UMI_STATUS_IO_ERROR && receipt.journal.recovery_required);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK && after.progress_write_blocked);
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_INVALID_STATE);
        if (strcmp(name, "other-learner-recovery") == 0 || strcmp(name, "other-database-recovery") == 0) {
            UmiLearningProgressLocalRequest wrong = inspect;
            if (strcmp(name, "other-learner-recovery") == 0) memcpy(wrong.learner_id, "other", 6U);
            else {
                CHECK(umi_data_server_create_sqlite(other, &database) == UMI_STATUS_OK);
                umi_data_server_destroy(database); database = NULL;
                CHECK(umi_path_copy(wrong.database_path, sizeof(wrong.database_path), other) == UMI_STATUS_OK);
            }
            CHECK(UmiLearningCatalogueGtk4Progress(panel, &wrong) == UMI_STATUS_OK && Drain());
            CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK && after.progress_write_blocked);
            CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_INVALID_STATE);
        }
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &inspect) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.status == UMI_STATUS_OK && receipt.has_event && receipt.event.sequence == 1U);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK && !after.progress_write_blocked);
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.status == UMI_STATUS_ALREADY_EXISTS && receipt.journal.event_count == 1U);
        goto unchanged;
    }
    CHECK(receipt.status == UMI_STATUS_OK && receipt.has_event && receipt.event.sequence == 1U);
    previous = receipt;
    if (strcmp(name, "duplicate") == 0) {
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &request) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.status == UMI_STATUS_ALREADY_EXISTS && !receipt.has_event && receipt.journal.event_count == 1U);
    } else if (strcmp(name, "revision-binding") == 0) {
        CHECK(Fixture(next, 2U, "lesson"));
        CHECK(UmiLearningCatalogueGtk4PrepareReplacement(panel, next) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyReplacement(panel, &replacement) == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4AcceptReplacement(panel, replacement.review_serial) == UMI_STATUS_OK);
        CHECK(UmiLearningCatalogueGtk4Open(panel, 0U) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.selection.pack.revision == 1U && receipt.event.pack_revision == 1U);
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &inspect) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.status == UMI_STATUS_NOT_FOUND && receipt.selection.pack.revision == 2U);
        CHECK(receipt.journal.event_count == 1U && !receipt.has_event);
        goto cleanup;
    } else if (strcmp(name, "receipt-retained") == 0) {
        CHECK(UmiLearningCatalogueGtk4Reload(panel) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_OK);
        /* Compare public values, not representation padding: CopyProgressReceipt
         * uses ordinary C structure assignment and promises a value snapshot. */
#if 0
        CHECK(memcmp(&receipt, &previous, sizeof(receipt)) == 0);
#endif
#define SAME(member) CHECK(receipt.member == previous.member)
#define TEXT_SAME(member) CHECK(strcmp(receipt.member, previous.member) == 0)
        SAME(status); SAME(storage_opened); SAME(journal_loaded); SAME(write_attempted); SAME(has_event);
        SAME(request.action); TEXT_SAME(request.database_path); TEXT_SAME(request.learner_id);
        TEXT_SAME(request.checkpoint); TEXT_SAME(request.evidence_reference);
        TEXT_SAME(journal.learner_id); SAME(journal.event_count); SAME(journal.backend); SAME(journal.recovery_required);
        SAME(event.sequence); TEXT_SAME(event.pack_id); SAME(event.pack_revision); TEXT_SAME(event.resource_id);
        TEXT_SAME(event.sha256); TEXT_SAME(event.checkpoint); TEXT_SAME(event.evidence_reference);
        SAME(selection.content_size); SAME(selection.history_count); SAME(selection.history_index);
        SAME(selection.revision); SAME(selection.has_document); SAME(selection.can_go_back); SAME(selection.can_go_forward);
        TEXT_SAME(selection.pack.id); TEXT_SAME(selection.pack.title); TEXT_SAME(selection.pack.product);
        TEXT_SAME(selection.pack.locale); SAME(selection.pack.revision); SAME(selection.pack.resource_count);
        SAME(selection.pack.minimum_version.major); SAME(selection.pack.minimum_version.minor); SAME(selection.pack.minimum_version.patch);
        SAME(selection.pack.maximum_version.major); SAME(selection.pack.maximum_version.minor); SAME(selection.pack.maximum_version.patch);
        TEXT_SAME(selection.selection.resource.id); TEXT_SAME(selection.selection.resource.title);
        TEXT_SAME(selection.selection.resource.topic); SAME(selection.selection.resource.kind);
        TEXT_SAME(selection.selection.resource.path); TEXT_SAME(selection.selection.resource.sha256);
        TEXT_SAME(selection.selection.resource.example_id); TEXT_SAME(selection.selection.fragment);
#undef TEXT_SAME
#undef SAME
        goto cleanup;
    } else {
        CHECK(strcmp(name, "record-inspect") == 0 || strcmp(name, "native-controls") == 0 ||
            strcmp(name, "busy") == 0 || strcmp(name, "captured-request") == 0 ||
            strcmp(name, "reentrant-result") == 0);
        if (strcmp(name, "reentrant-result") == 0) {
            CHECK(reentrant.calls == 1 && reentrant.status == UMI_STATUS_BUSY);
            g_signal_handler_disconnect(signal_widget, signal_id); signal_id = 0U;
        }
        CHECK(UmiLearningCatalogueGtk4Progress(panel, &inspect) == UMI_STATUS_OK && Drain());
        CHECK(UmiLearningCatalogueGtk4CopyProgressReceipt(panel, &receipt) == UMI_STATUS_OK);
        CHECK(receipt.status == UMI_STATUS_OK && receipt.has_event && !receipt.write_attempted);
    }
unchanged:
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
    CHECK(!after.busy && after.has_progress_result && after.reader.revision == before.reader.revision);
    CHECK(after.reader.history_count == before.reader.history_count);
cleanup:
    if (signal_id != 0U && signal_widget != NULL) g_signal_handler_disconnect(signal_widget, signal_id);
    if (panel != NULL) { UmiLearningCatalogueGtk4Retire(panel); g_object_unref(panel); }
    if (!Drain()) result = 1;
    umi_data_server_destroy(database);
    g_free(directory); g_free(pack); g_free(next); g_free(probe); g_free(other);
    return result;
}
