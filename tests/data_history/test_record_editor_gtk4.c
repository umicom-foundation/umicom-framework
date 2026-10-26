/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_editor_gtk4.c
 *
 * PURPOSE:
 *   Real GTK and SQLite fixtures exercise visible drafts and retained controls. Headless display absence is reported as skipped coverage, never a pass.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/record_editor.h"
#include "umicom/data/record_draft_files.h"
#include "../support/gtk4_expanders.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto cleanup; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static GtkWidget *find(GtkWidget *root, const char *name)
{
    if (strcmp(gtk_widget_get_name(root), name) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = find(child, name);
        if (found != NULL) return found;
    }
    return NULL;
}
static bool set_entry(GtkWidget *root, const char *name, const char *text)
{
    GtkWidget *widget = find(root, name);
    if (widget == NULL) return false;
    gtk_editable_set_text(GTK_EDITABLE(widget), text);
    return true;
}
static bool click(GtkWidget *root, const char *name)
{
    GtkWidget *widget = find(root, name);
    if (widget == NULL) return false;
    g_signal_emit_by_name(widget, "clicked");
    return true;
}
static bool set_text(GtkWidget *root, const char *text)
{
    GtkWidget *view = find(root, "record-editor-text");
    if (view == NULL) return false;
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), text, -1);
    return true;
}
static bool named_text_is(GtkWidget *root, const char *name, const char *expected)
{
    GtkWidget *view = find(root, name);
    if (view == NULL) return false;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter start, end; gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    bool same = strcmp(text, expected) == 0; g_free(text); return same;
}
static bool text_is(GtkWidget *root, const char *expected)
{
    return named_text_is(root, "record-editor-text", expected);
}
static bool wait_idle(GtkWidget *root)
{
    gint64 deadline = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
    while (UmiRecordEditorGtk4Busy(root) && g_get_monotonic_time() < deadline) {
        (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return !UmiRecordEditorGtk4Busy(root);
}
static UmiStatus create(GtkWidget **root)
{
    const UmiRecordEditorGtk4Options options = {.title = "Test notes", .description = "Fictional data only",
        .entity_prefix = "test.review.", .source = "Native fixture"};
    UmiStatus status = UmiRecordEditorGtk4New(&options, root);
    if (status == UMI_STATUS_OK) g_object_ref_sink(*root);
    if (status == UMI_STATUS_OK) UmiTestExpandSections(*root);
    return status;
}
typedef struct Observer { GtkWidget *root; const char *prefix; bool close; bool fired; } Observer;
static void observe(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)spec;
    Observer *observer = data;
    if (observer->fired || !g_str_has_prefix(gtk_label_get_text(GTK_LABEL(object)), observer->prefix)) return;
    observer->fired = true;
    if (observer->close) g_object_run_dispose(G_OBJECT(observer->root));
    else (void)set_text(observer->root, "observer edit");
}
static void retire_on_model(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec;
    Observer *observer = data;
    if (!observer->fired) { observer->fired = true; g_object_run_dispose(G_OBJECT(observer->root)); }
}
int main(int argc, char **argv)
{
    int result = 0;
    GtkWidget *root = NULL, *observed = NULL, *retained = NULL, *window = NULL;
    UmiDataServer *reader = NULL;
    UmiRecordDraftState state;
    UmiRecordDocument document;
    UmiRecordRevision revision;
    gulong signal = 0U;
    char *directory = NULL, *database = NULL, *oversized = NULL;
    char *recovery_path = NULL, *alternate_path = NULL;
    UmiRecordDraftSnapshot *recovery_snapshot = NULL;
    Observer observer = {0};
    const char *name = argc == 3 ? argv[1] : "";
    if (!gtk_init_check()) return 77;
    CHECK(argc == 3 && g_path_is_absolute(argv[2]));
    CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
    directory = g_build_filename(argv[2], "run-XXXXXX", NULL);
    CHECK(g_mkdtemp(directory) != NULL);
    database = g_build_filename(directory, "notes.sqlite3", NULL);
    OK(create(&root));
    CHECK(UmiRecordEditorGtk4CanClose(root));
    if (strcmp(name, "options") == 0) {
        UmiRecordEditorGtk4Options options = {.title = "", .description = "test", .entity_prefix = "test.", .source = "test"};
        GtkWidget *unchanged = root;
        CHECK(UmiRecordEditorGtk4New(&options, &unchanged) == UMI_STATUS_INVALID_ARGUMENT && unchanged == root);
        options.title = "test"; options.entity_prefix = "bad\nnamespace";
        CHECK(UmiRecordEditorGtk4New(&options, &unchanged) == UMI_STATUS_INVALID_ARGUMENT && unchanged == root);
        options.entity_prefix = "test."; options.source = "bad\tlabel";
        CHECK(UmiRecordEditorGtk4New(&options, &unchanged) == UMI_STATUS_INVALID_ARGUMENT && unchanged == root);
        goto cleanup;
    }
    if (strcmp(name, "relative-path") == 0) {
        CHECK(set_entry(root, "record-editor-path", "relative.sqlite3"));
        CHECK(click(root, "record-editor-connect"));
        CHECK(!UmiRecordEditorGtk4Busy(root));
        OK(UmiRecordEditorGtk4State(root, &state)); CHECK(!state.ready);
        goto cleanup;
    }
    CHECK(set_entry(root, "record-editor-path", database));
    if (strcmp(name, "retired-actions") == 0) {
        retained = g_object_ref(find(root, "record-editor-connect"));
        UmiRecordEditorGtk4Retire(root);
        g_signal_emit_by_name(retained, "clicked");
        CHECK(!UmiRecordEditorGtk4Busy(root) && !g_file_test(database, G_FILE_TEST_EXISTS));
        CHECK(UmiRecordEditorGtk4State(root, &state) == UMI_STATUS_UNAVAILABLE);
        goto cleanup;
    }
    if (strcmp(name, "acceptance-close") == 0) {
        observer = (Observer){.root = root, .prefix = "Opening or creating", .close = true};
        observed = g_object_ref(find(root, "record-editor-feedback"));
        retained = g_object_ref(find(root, "record-editor-connect"));
        signal = g_signal_connect(observed, "notify::label", G_CALLBACK(observe), &observer);
        CHECK(click(root, "record-editor-connect"));
        CHECK(observer.fired && !UmiRecordEditorGtk4Busy(root));
        g_signal_emit_by_name(retained, "clicked");
        CHECK(!g_file_test(database, G_FILE_TEST_EXISTS));
        goto cleanup;
    }
    CHECK(click(root, "record-editor-connect")); CHECK(wait_idle(root));
    CHECK(g_file_test(database, G_FILE_TEST_EXISTS));
    CHECK(set_entry(root, "record-editor-key", "one"));
    CHECK(set_entry(root, "record-editor-actor", "Fixture reviewer"));
    CHECK(set_entry(root, "record-editor-reason", "Save synthetic text"));
    CHECK(click(root, "record-editor-new"));
    OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.ready && state.dirty);
    CHECK(set_text(root, "first"));
    CHECK(click(root, "record-editor-save")); CHECK(wait_idle(root));
    OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.expected_revision == 1U && !state.dirty);
    OK(umi_data_server_create_sqlite(database, &reader));
    if (g_str_has_prefix(name, "catalogue-")) {
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        UmiRecordAmendment author = document.record.amendment;
        strcpy(author.entity_id, "test.review.two");
        if (strcmp(name, "catalogue-utf8") == 0) { author.actor[0] = (char)0xff; author.actor[1] = '\0'; }
        OK(umi_record_document_append(reader, &author, "second", 6U, 0U, &revision));
        strcpy(author.entity_id, "another.product.hidden");
        OK(umi_record_document_append(reader, &author, "outside", 7U, 0U, &revision));
        if (strcmp(name, "catalogue-pages") == 0) {
            for (unsigned i = 0U; i < 18U; ++i) {
                (void)snprintf(author.entity_id, sizeof author.entity_id, "test.review.page.%02u", i);
                OK(umi_record_document_append(reader, &author, "page", 4U, 0U, &revision));
            }
        }
        if (strcmp(name, "catalogue-search") == 0) CHECK(set_entry(root, "record-editor-search", "two"));
        if (strcmp(name, "catalogue-empty") == 0) CHECK(set_entry(root, "record-editor-search", "not present"));
        if (strcmp(name, "catalogue-dirty") == 0) CHECK(set_text(root, "unsaved work"));
        if (strcmp(name, "catalogue-completion-edit") == 0) {
            observer = (Observer){.root=root, .prefix="Results loaded", .close=false};
            observed = g_object_ref(find(root, "record-editor-feedback"));
            signal = g_signal_connect(observed, "notify::label", G_CALLBACK(observe), &observer);
        }
        if (strcmp(name, "catalogue-model-close") == 0) {
            observer = (Observer){.root=root, .close=true};
            /* Report missing controls as a fixture assertion before retaining
             * a GTK object. This keeps a collapsed-section regression from
             * becoming an unrelated fatal GObject critical. */
#if 0
            observed = g_object_ref(find(root, "record-editor-results"));
#endif
            GtkWidget *results = find(root, "record-editor-results");
            CHECK(GTK_IS_DROP_DOWN(results));
            observed = g_object_ref(results);
            signal = g_signal_connect(observed, "notify::model", G_CALLBACK(retire_on_model), &observer);
        }
        CHECK(click(root, "record-editor-find"));
        if (strcmp(name, "catalogue-query-edit") == 0) CHECK(set_entry(root, "record-editor-search", "changed during read"));
        if (strcmp(name, "catalogue-retire") == 0) UmiRecordEditorGtk4Retire(root);
        CHECK(wait_idle(root));
        if (strcmp(name, "catalogue-retire") == 0 || strcmp(name, "catalogue-model-close") == 0) { CHECK(UmiRecordEditorGtk4State(root, &state) == UMI_STATUS_UNAVAILABLE); goto cleanup; }
        if (strcmp(name, "catalogue-completion-edit") == 0) {
            CHECK(observer.fired && text_is(root, "observer edit"));
            OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.dirty); goto cleanup;
        }
        GtkWidget *results = find(root, "record-editor-results"); CHECK(results != NULL);
        GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(results));
        if (strcmp(name, "catalogue-query-edit") == 0) {
            CHECK(model == NULL || g_list_model_get_n_items(model) == 0U);
            CHECK(!gtk_widget_get_sensitive(find(root, "record-editor-open-listed"))); CHECK(text_is(root, "first")); goto cleanup;
        }
        CHECK(model != NULL);
        if (strcmp(name, "catalogue-empty") == 0) {
            CHECK(g_list_model_get_n_items(model) == 0U);
            CHECK(click(root, "record-editor-open-listed")); CHECK(!UmiRecordEditorGtk4Busy(root)); CHECK(text_is(root, "first")); goto cleanup;
        }
        if (strcmp(name, "catalogue-pages") == 0) {
            CHECK(g_list_model_get_n_items(model) == 16U);
            CHECK(click(root, "record-editor-find-next")); CHECK(wait_idle(root));
            model = gtk_drop_down_get_model(GTK_DROP_DOWN(results)); CHECK(g_list_model_get_n_items(model) == 4U);
            CHECK(!gtk_widget_get_sensitive(find(root, "record-editor-find-next"))); CHECK(text_is(root, "first")); goto cleanup;
        }
        CHECK(g_list_model_get_n_items(model) == (strcmp(name, "catalogue-search") == 0 ? 1U : 2U));
        gtk_drop_down_set_selected(GTK_DROP_DOWN(results), strcmp(name, "catalogue-search") == 0 ? 0U : 1U);
        if (strcmp(name, "catalogue-dirty") == 0) {
            CHECK(click(root, "record-editor-open-listed")); CHECK(!UmiRecordEditorGtk4Busy(root)); CHECK(text_is(root, "unsaved work")); goto cleanup;
        }
        if (strcmp(name, "catalogue-stale") == 0) {
            strcpy(author.entity_id, "test.review.two");
            OK(umi_record_document_append(reader, &author, "changed", 7U, 1U, &revision));
        }
        CHECK(click(root, "record-editor-open-listed"));
        if (strcmp(name, "catalogue-selection-change") == 0) gtk_drop_down_set_selected(GTK_DROP_DOWN(results), 0U);
        if (strcmp(name, "catalogue-later-edit") == 0) CHECK(set_text(root, "typed during open"));
        CHECK(wait_idle(root));
        if (strcmp(name, "catalogue-stale") == 0) { CHECK(text_is(root, "first")); }
        else if (strcmp(name, "catalogue-later-edit") == 0) { CHECK(text_is(root, "typed during open")); }
        else {
            CHECK(strcmp(name, "catalogue-open") == 0 || strcmp(name, "catalogue-search") == 0 || strcmp(name, "catalogue-utf8") == 0 || strcmp(name, "catalogue-selection-change") == 0);
            CHECK(text_is(root, "second")); OK(UmiRecordEditorGtk4State(root, &state));
            CHECK(strcmp(state.entity_id, "test.review.two") == 0 && !state.dirty);
        }
    } else if (g_str_has_prefix(name, "recovery-")) {
        recovery_path = g_build_filename(directory, "kept.umird", NULL);
        alternate_path = g_build_filename(directory, "alternate.umird", NULL);
        recovery_snapshot = g_try_new0(UmiRecordDraftSnapshot, 1); CHECK(recovery_snapshot != NULL);
        CHECK(set_entry(root, "record-editor-recovery-path", recovery_path));
        CHECK(set_text(root, "unsaved"));
        CHECK(click(root, "record-editor-keep-draft")); CHECK(UmiRecordEditorGtk4Busy(root));
        if (strcmp(name, "recovery-later-edit") == 0) CHECK(set_text(root, "later typing"));
        if (strcmp(name, "recovery-retire-write") == 0) UmiRecordEditorGtk4Retire(root);
        CHECK(wait_idle(root));
        OK(umi_record_draft_snapshot_read(directory, "kept.umird", recovery_snapshot));
        CHECK(recovery_snapshot->length == 7U && memcmp(recovery_snapshot->content, "unsaved", 7U) == 0);
        OK(umi_record_document_latest(reader, "test.review.one", &document)); CHECK(document.record.revision == 1U);
        if (strcmp(name, "recovery-retire-write") == 0) { CHECK(UmiRecordEditorGtk4State(root, &state) == UMI_STATUS_UNAVAILABLE); goto cleanup; }
        OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.dirty && state.expected_revision == 1U);
        CHECK(!UmiRecordEditorGtk4CanClose(root));
        if (strcmp(name, "recovery-later-edit") == 0) { CHECK(text_is(root, "later typing")); goto cleanup; }
        if (strcmp(name, "recovery-dirty") == 0) {
            CHECK(set_text(root, "keep this")); CHECK(click(root, "record-editor-recover-draft"));
            CHECK(!UmiRecordEditorGtk4Busy(root) && text_is(root, "keep this")); goto cleanup;
        }
        if (strcmp(name, "recovery-exclusive") == 0) {
            CHECK(set_text(root, "second copy")); CHECK(click(root, "record-editor-keep-draft")); CHECK(wait_idle(root));
            OK(umi_record_draft_snapshot_read(directory, "kept.umird", recovery_snapshot));
            CHECK(recovery_snapshot->length == 7U && memcmp(recovery_snapshot->content, "unsaved", 7U) == 0);
            CHECK(text_is(root, "second copy")); goto cleanup;
        }
        CHECK(click(root, "record-editor-discard")); CHECK(text_is(root, "first"));
        if (strcmp(name, "recovery-stale") == 0) {
            UmiRecordAmendment author = document.record.amendment; author.recorded_at_nanoseconds += 1U;
            OK(umi_record_document_append(reader, &author, "external", 8U, 1U, &revision));
        }
        bool rejected = strcmp(name, "recovery-wrong-store") == 0 || strcmp(name, "recovery-namespace") == 0 || strcmp(name, "recovery-binary") == 0;
        if (rejected) {
            if (strcmp(name, "recovery-wrong-store") == 0) strcpy(recovery_snapshot->baseline.record.amendment.actor, "Different baseline actor");
            else if (strcmp(name, "recovery-namespace") == 0) strcpy(recovery_snapshot->baseline.record.amendment.entity_id, "another.product.one");
            else recovery_snapshot->content[0] = 0U;
            OK(umi_record_draft_snapshot_write_new(alternate_path, recovery_snapshot));
            CHECK(set_entry(root, "record-editor-recovery-path", alternate_path));
        }
        if (strcmp(name, "recovery-acceptance-edit") == 0 || strcmp(name, "recovery-completion-edit") == 0) {
            observer = (Observer){.root = root, .prefix = strcmp(name, "recovery-acceptance-edit") == 0 ? "Reading and checking" : "Draft recovered", .close = false};
            observed = g_object_ref(find(root, "record-editor-feedback"));
            signal = g_signal_connect(observed, "notify::label", G_CALLBACK(observe), &observer);
        }
        CHECK(click(root, "record-editor-recover-draft"));
        if (strcmp(name, "recovery-retire-read") == 0) UmiRecordEditorGtk4Retire(root);
        CHECK(wait_idle(root));
        if (strcmp(name, "recovery-retire-read") == 0) { CHECK(UmiRecordEditorGtk4State(root, &state) == UMI_STATUS_UNAVAILABLE); goto cleanup; }
        if (rejected) { CHECK(text_is(root, "first")); OK(UmiRecordEditorGtk4State(root, &state)); CHECK(!state.dirty); goto cleanup; }
        if (observer.prefix != NULL) {
            CHECK(observer.fired && text_is(root, "observer edit"));
            for (int pass = 0; pass < 8; ++pass) (void)g_main_context_iteration(NULL, FALSE);
            CHECK(gtk_widget_get_sensitive(find(root, "record-editor-save"))); goto cleanup;
        }
        CHECK(text_is(root, "unsaved")); OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.dirty && state.expected_revision == 1U);
        CHECK(click(root, "record-editor-save")); CHECK(wait_idle(root));
        OK(UmiRecordEditorGtk4State(root, &state));
        if (strcmp(name, "recovery-stale") == 0) CHECK(state.dirty && state.expected_revision == 1U);
        else { CHECK(strcmp(name, "recovery-roundtrip") == 0); CHECK(!state.dirty && state.expected_revision == 2U); }
    } else if (strcmp(name, "roundtrip") == 0) {
        CHECK(set_text(root, "second")); CHECK(click(root, "record-editor-save")); CHECK(wait_idle(root));
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        CHECK(document.record.revision == 2U && document.length == 6U && memcmp(document.content, "second", 6U) == 0);
        OK(umi_record_document_load(reader, "test.review.one", 1U, &document));
        CHECK(document.length == 5U && memcmp(document.content, "first", 5U) == 0);
    } else if (strcmp(name, "reopen") == 0) {
        g_object_unref(root); root = NULL;
        OK(create(&root)); CHECK(set_entry(root, "record-editor-path", database));
        CHECK(click(root, "record-editor-connect")); CHECK(wait_idle(root));
        CHECK(set_entry(root, "record-editor-key", "one")); CHECK(click(root, "record-editor-open")); CHECK(wait_idle(root));
        CHECK(text_is(root, "first"));
        OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.expected_revision == 1U && !state.dirty);
    } else if (strcmp(name, "stale") == 0) {
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        OK(umi_record_document_append(reader, &document.record.amendment, "external", 8U, 1U, &revision));
        CHECK(set_text(root, "local edit")); CHECK(click(root, "record-editor-save")); CHECK(wait_idle(root));
        OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.dirty && state.expected_revision == 1U && !state.saving);
        CHECK(text_is(root, "local edit"));
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        CHECK(document.record.revision == 2U && document.length == 8U && memcmp(document.content, "external", 8U) == 0);
    } else if (g_str_has_prefix(name, "history-") || strcmp(name, "metadata-utf8") == 0) {
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        for (uint64_t i = 1U; i < 19U; ++i)
            OK(umi_record_document_append(reader, &document.record.amendment, "archived text", 13U, i, &revision));
        CHECK(set_text(root, "keep my draft"));
        CHECK(click(root, "record-editor-history")); CHECK(wait_idle(root));
        CHECK(text_is(root, "keep my draft"));
        GtkWidget *history_label = find(root, "record-editor-history-summary");
        CHECK(history_label != NULL && strstr(gtk_label_get_text(GTK_LABEL(history_label)), "16 amendment rows") != NULL);
        if (strcmp(name, "history-pages") == 0) {
            CHECK(click(root, "record-editor-older")); CHECK(wait_idle(root));
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(history_label)), "3 amendment rows") != NULL);
            CHECK(!gtk_widget_get_sensitive(find(root, "record-editor-older")));
        } else if (strcmp(name, "history-stale") == 0) {
            OK(umi_record_document_append(reader, &document.record.amendment, "newest text", 11U, 19U, &revision));
            CHECK(click(root, "record-editor-older")); CHECK(wait_idle(root));
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(find(root, "record-editor-feedback"))), "History advanced") != NULL);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(history_label)), "stored head 19") != NULL);
            CHECK(click(root, "record-editor-history")); CHECK(wait_idle(root));
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(history_label)), "stored head 20") != NULL);
        } else if (strcmp(name, "history-content") == 0) {
            CHECK(set_entry(root, "record-editor-history-revision", "1"));
            CHECK(click(root, "record-editor-history-view")); CHECK(wait_idle(root));
            CHECK(named_text_is(root, "record-editor-review-text", "first"));
            CHECK(!gtk_widget_get_sensitive(find(root, "record-editor-adopt")));
            CHECK(click(root, "record-editor-adopt"));
        } else if (strcmp(name, "history-invalid") == 0) {
            const char *invalid[] = {"0", "-1", "abc", "18446744073709551616"};
            for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i) {
                CHECK(set_entry(root, "record-editor-history-revision", invalid[i]));
                CHECK(click(root, "record-editor-history-view")); CHECK(!UmiRecordEditorGtk4Busy(root));
            }
        } else if (strcmp(name, "history-close") == 0) {
            observer = (Observer){.root = root, .prefix = "History page loaded", .close = true};
            observed = g_object_ref(find(root, "record-editor-feedback"));
            retained = g_object_ref(find(root, "record-editor-history"));
            signal = g_signal_connect(observed, "notify::label", G_CALLBACK(observe), &observer);
            CHECK(click(root, "record-editor-older")); CHECK(wait_idle(root)); CHECK(observer.fired);
            g_signal_emit_by_name(retained, "clicked");
            CHECK(UmiRecordEditorGtk4State(root, &state) == UMI_STATUS_UNAVAILABLE);
            goto cleanup;
        } else if (strcmp(name, "metadata-utf8") == 0) {
            document.record.amendment.actor[0] = (char)0xff; document.record.amendment.actor[1] = '\0';
            OK(umi_record_document_append(reader, &document.record.amendment, "legacy note", 11U, 19U, &revision));
            CHECK(click(root, "record-editor-compare")); CHECK(wait_idle(root));
            CHECK(named_text_is(root, "record-editor-review-text", "legacy note"));
            const char *label = gtk_label_get_text(GTK_LABEL(find(root, "record-editor-review-summary")));
            CHECK(g_utf8_validate(label, -1, NULL));
            CHECK(click(root, "record-editor-history")); CHECK(wait_idle(root));
        } else CHECK(0 && "Unknown history case");
        CHECK(text_is(root, "keep my draft"));
        OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.expected_revision == 1U && state.dirty);
    } else if (g_str_has_prefix(name, "compare-")) {
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        OK(umi_record_document_append(reader, &document.record.amendment, "external", 8U, 1U, &revision));
        CHECK(set_text(root, "local edit"));
        if (strcmp(name, "compare-close") == 0) {
            observer = (Observer){.root = root, .prefix = "Stored text shown", .close = true};
            observed = g_object_ref(find(root, "record-editor-feedback"));
            retained = g_object_ref(find(root, "record-editor-adopt"));
            signal = g_signal_connect(observed, "notify::label", G_CALLBACK(observe), &observer);
        }
        CHECK(click(root, "record-editor-compare")); CHECK(UmiRecordEditorGtk4Busy(root));
        CHECK(set_text(root, "later local edit"));
        CHECK(wait_idle(root));
        if (strcmp(name, "compare-close") == 0) {
            CHECK(observer.fired);
            g_signal_emit_by_name(retained, "clicked");
            CHECK(UmiRecordEditorGtk4State(root, &state) == UMI_STATUS_UNAVAILABLE);
        } else {
            CHECK(text_is(root, "later local edit"));
            CHECK(named_text_is(root, "record-editor-review-text", "external"));
            OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.expected_revision == 1U && state.dirty);
            if (strcmp(name, "compare-preserves") == 0) {
                CHECK(click(root, "record-editor-save")); CHECK(wait_idle(root));
                OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.expected_revision == 1U && state.dirty);
            } else if (strcmp(name, "compare-adopt") == 0 || strcmp(name, "compare-stale") == 0 || strcmp(name, "compare-discard") == 0) {
                if (strcmp(name, "compare-stale") == 0) {
                    OK(umi_record_document_append(reader, &document.record.amendment, "changed again", 13U, 2U, &revision));
                }
                CHECK(click(root, "record-editor-adopt"));
                OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.expected_revision == 2U && state.dirty);
                CHECK(text_is(root, "later local edit"));
                if (strcmp(name, "compare-discard") == 0) {
                    CHECK(click(root, "record-editor-discard")); CHECK(text_is(root, "external"));
                    OK(UmiRecordEditorGtk4State(root, &state)); CHECK(!state.dirty);
                } else {
                    CHECK(click(root, "record-editor-save")); CHECK(wait_idle(root));
                    OK(UmiRecordEditorGtk4State(root, &state));
                    CHECK(state.expected_revision == (strcmp(name, "compare-adopt") == 0 ? 3U : 2U));
                    CHECK(state.dirty == (strcmp(name, "compare-stale") == 0));
                    OK(umi_record_document_latest(reader, "test.review.one", &document));
                    CHECK(document.record.revision == 3U);
                    const char *expected = strcmp(name, "compare-adopt") == 0 ? "later local edit" : "changed again";
                    CHECK(document.length == strlen(expected) && memcmp(document.content, expected, document.length) == 0);
                }
            } else CHECK(0 && "Unknown comparison case");
        }
    } else if (strcmp(name, "later-edit") == 0 || strcmp(name, "busy") == 0) {
        CHECK(set_text(root, "captured")); CHECK(click(root, "record-editor-save"));
        CHECK(UmiRecordEditorGtk4Busy(root));
        CHECK(set_text(root, "later"));
        if (strcmp(name, "busy") == 0) {
            CHECK(click(root, "record-editor-save")); CHECK(click(root, "record-editor-discard"));
            CHECK(set_entry(root, "record-editor-key", "other")); CHECK(click(root, "record-editor-new"));
        }
        CHECK(wait_idle(root)); CHECK(text_is(root, "later"));
        OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.dirty && state.expected_revision == 2U);
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        CHECK(document.length == 8U && memcmp(document.content, "captured", 8U) == 0);
    } else if (strcmp(name, "oversize") == 0) {
        oversized = g_malloc(UMI_RECORD_DOCUMENT_MAX_BYTES + 2U);
        memset(oversized, 'x', UMI_RECORD_DOCUMENT_MAX_BYTES + 1U); oversized[UMI_RECORD_DOCUMENT_MAX_BYTES + 1U] = '\0';
        CHECK(set_text(root, oversized)); CHECK(!UmiRecordEditorGtk4CanClose(root));
        CHECK(click(root, "record-editor-save")); CHECK(!UmiRecordEditorGtk4Busy(root));
        CHECK(text_is(root, oversized));
        OK(umi_record_document_latest(reader, "test.review.one", &document)); CHECK(document.record.revision == 1U);
        CHECK(click(root, "record-editor-discard")); CHECK(text_is(root, "first")); CHECK(UmiRecordEditorGtk4CanClose(root));
    } else if (strcmp(name, "invalid-attribution") == 0) {
        CHECK(set_entry(root, "record-editor-actor", "")); CHECK(set_text(root, "not committed"));
        CHECK(click(root, "record-editor-save")); CHECK(!UmiRecordEditorGtk4Busy(root));
        CHECK(text_is(root, "not committed"));
        OK(umi_record_document_latest(reader, "test.review.one", &document)); CHECK(document.record.revision == 1U);
    } else if (strcmp(name, "dirty-open") == 0) {
        CHECK(set_text(root, "keep this")); CHECK(click(root, "record-editor-open"));
        CHECK(!UmiRecordEditorGtk4Busy(root) && text_is(root, "keep this"));
    } else if (strcmp(name, "binary") == 0 || strcmp(name, "invalid-utf8") == 0) {
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        strcpy(document.record.amendment.entity_id, "test.review.binary");
        const unsigned char bytes[] = {'a', 0U, 0xffU};
        const bool utf8 = strcmp(name, "invalid-utf8") == 0;
        OK(umi_record_document_append(reader, &document.record.amendment, utf8 ? bytes + 2U : bytes,
            utf8 ? 1U : sizeof bytes, 0U, &revision));
        CHECK(set_entry(root, "record-editor-key", "binary")); CHECK(click(root, "record-editor-open")); CHECK(wait_idle(root));
        CHECK(text_is(root, "first"));
        OK(UmiRecordEditorGtk4State(root, &state)); CHECK(strcmp(state.entity_id, "test.review.one") == 0);
    } else if (strcmp(name, "acceptance-edit") == 0 || strcmp(name, "completion-close") == 0 || strcmp(name, "completion-edit") == 0) {
        observer = (Observer){.root = root, .prefix = strcmp(name, "acceptance-edit") == 0 ? "Saving the captured" : "Captured revision saved",
            .close = strcmp(name, "completion-close") == 0};
        observed = g_object_ref(find(root, "record-editor-feedback"));
        retained = g_object_ref(find(root, "record-editor-save"));
        signal = g_signal_connect(observed, "notify::label", G_CALLBACK(observe), &observer);
        CHECK(set_text(root, "captured")); CHECK(click(root, "record-editor-save")); CHECK(wait_idle(root));
        CHECK(observer.fired);
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        CHECK(document.length == 8U && memcmp(document.content, "captured", 8U) == 0);
        if (observer.close) {
            g_signal_emit_by_name(retained, "clicked");
            CHECK(UmiRecordEditorGtk4State(root, &state) == UMI_STATUS_UNAVAILABLE);
        } else {
            CHECK(text_is(root, "observer edit"));
            OK(UmiRecordEditorGtk4State(root, &state)); CHECK(state.dirty && state.expected_revision == 2U);
            /* The observer edit occurs inside completion notification. Its
             * deferred presentation pass must re-enable Save after unwinding. */
            for (int pass = 0; pass < 8; ++pass) (void)g_main_context_iteration(NULL, FALSE);
            CHECK(gtk_widget_get_sensitive(find(root, "record-editor-save")));
        }
    } else if (strcmp(name, "unroot") == 0) {
        window = gtk_window_new();
        gtk_window_set_child(GTK_WINDOW(window), root);
        retained = g_object_ref(find(root, "record-editor-save"));
        gtk_window_set_child(GTK_WINDOW(window), NULL);
        g_signal_emit_by_name(retained, "clicked");
        CHECK(UmiRecordEditorGtk4State(root, &state) == UMI_STATUS_UNAVAILABLE);
        CHECK(!UmiRecordEditorGtk4Busy(root));
    } else if (strcmp(name, "close-guard") == 0) {
        CHECK(UmiRecordEditorGtk4RequestClose(root));
        CHECK(set_text(root, "pending")); CHECK(!UmiRecordEditorGtk4RequestClose(root));
        CHECK(click(root, "record-editor-save")); CHECK(!UmiRecordEditorGtk4RequestClose(root));
        CHECK(wait_idle(root)); CHECK(UmiRecordEditorGtk4RequestClose(root));
        CHECK(set_text(root, "discard")); CHECK(!UmiRecordEditorGtk4RequestClose(root));
        CHECK(click(root, "record-editor-discard")); CHECK(UmiRecordEditorGtk4RequestClose(root));
    } else if (strcmp(name, "retire-save") == 0 || strcmp(name, "retire-read") == 0) {
        bool save = strcmp(name, "retire-save") == 0;
        if (save) CHECK(set_text(root, "accepted before close"));
        CHECK(click(root, save ? "record-editor-save" : "record-editor-open"));
        CHECK(UmiRecordEditorGtk4Busy(root)); UmiRecordEditorGtk4Retire(root);
        CHECK(wait_idle(root)); CHECK(UmiRecordEditorGtk4State(root, &state) == UMI_STATUS_UNAVAILABLE);
        OK(umi_record_document_latest(reader, "test.review.one", &document));
        CHECK(document.record.revision == (save ? 2U : 1U));
        if (save) CHECK(document.length == 21U && memcmp(document.content, "accepted before close", 21U) == 0);
    } else CHECK(0 && "Unknown native record editor case");
cleanup:
    if (observed != NULL && signal != 0U) g_signal_handler_disconnect(observed, signal);
    if (window != NULL) gtk_window_destroy(GTK_WINDOW(window));
    if (root != NULL) { (void)wait_idle(root); g_object_unref(root); }
    g_clear_object(&observed); g_clear_object(&retained);
    umi_data_server_destroy(reader);
    g_free(oversized);
    /* All removable paths belong to this test's unique supplied fixture directory. */
    if (database != NULL) {
        (void)g_remove(database);
        char *wal = g_strconcat(database, "-wal", NULL), *shm = g_strconcat(database, "-shm", NULL);
        (void)g_remove(wal); (void)g_remove(shm); g_free(wal); g_free(shm);
    }
    if (recovery_path != NULL) (void)g_remove(recovery_path);
    if (alternate_path != NULL) (void)g_remove(alternate_path);
    g_free(recovery_path); g_free(alternate_path); g_free(recovery_snapshot);
    if (directory != NULL) (void)g_rmdir(directory);
    g_free(database); g_free(directory);
    return result;
}
