/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Shared native record editing keeps worker inputs, draft state and host
 * lifetime separate. Applications compose this surface without duplicating
 * database calls, revision checks or notification-sensitive callbacks. */
#include "umicom/ui/gtk4/record_editor.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/data/record_history_page.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

typedef enum EditorAction { EDITOR_CONNECT, EDITOR_NEW, EDITOR_OPEN, EDITOR_SAVE, EDITOR_DISCARD, EDITOR_COMPARE, EDITOR_ADOPT, EDITOR_HISTORY, EDITOR_OLDER, EDITOR_REVISION } EditorAction;
typedef enum EditorJobKind { JOB_CONNECT, JOB_OPEN, JOB_SAVE, JOB_COMPARE, JOB_HISTORY, JOB_OLDER, JOB_REVISION } EditorJobKind;
typedef struct RecordEditor {
    gatomicrefcount references;
    GMainContext *context;
    GSource *refresh_source;
    bool retired, busy, setting_text, text_valid, epoch_exhausted;
    unsigned int applying;
    uint64_t edit_epoch;
    EditorJobKind operation;
    UmiRecordDraft *draft;
    UmiDataServer *server;
    UmiRecordDocument *reviewed;
    uint64_t reviewed_base_revision;
    bool reviewed_latest;
    UmiRecordHistoryPage *history;
    char *prefix, *source;
    GtkWidget *path, *key, *actor, *reason, *text, *summary, *feedback;
    GtkWidget *connect, *new_record, *open, *save, *discard;
    GtkWidget *compare, *adopt, *review_text, *review_summary;
    GtkWidget *history_refresh, *history_older, *history_view, *history_revision, *history_text, *history_summary;
} RecordEditor;

typedef struct RecordJob {
    RecordEditor *owner;
    EditorJobKind kind;
    uint64_t edit_epoch;
    uint64_t reviewed_base_revision, requested_revision, history_head, history_before;
    UmiRecordHistoryPage *page;
    char *path, *entity;
    UmiDataServer *server;
    UmiDataServer *opened_server;
    UmiRecordDocument *document;
    UmiRecordSaveRequest *request;
    UmiRecordRevision saved;
    UmiStatus status;
} RecordJob;

typedef struct UmiRecordEditorRoot { GtkBox parent; RecordEditor *editor; } UmiRecordEditorRoot;
typedef struct UmiRecordEditorRootClass { GtkBoxClass parent; } UmiRecordEditorRootClass;
G_DEFINE_TYPE(UmiRecordEditorRoot, umi_record_editor_root, GTK_TYPE_BOX)

static RecordEditor *editor_ref(RecordEditor *editor)
{
    g_atomic_ref_count_inc(&editor->references);
    return editor;
}
static void editor_unref(RecordEditor *editor)
{
    if (g_atomic_ref_count_dec(&editor->references)) {
        if (editor->context != NULL) g_main_context_unref(editor->context);
        umi_record_draft_destroy(editor->draft);
        umi_data_server_destroy(editor->server);
        g_free(editor->prefix);
        g_free(editor->source);
        g_free(editor->reviewed);
        g_free(editor->history);
        g_free(editor);
    }
}
static void signal_release(gpointer data, GClosure *closure)
{
    (void)closure;
    editor_unref(data);
}
static void connect_owned(RecordEditor *editor, GObject *object, const char *signal, GCallback callback)
{
    g_signal_connect_data(object, signal, callback, editor_ref(editor), signal_release, 0);
}
static void retire_editor(RecordEditor *editor)
{
    editor->retired = true;
    if (editor->refresh_source != NULL) {
        GSource *source = editor->refresh_source;
        editor->refresh_source = NULL;
        g_source_destroy(source);
        g_source_unref(source);
    }
}
static void root_dispose(GObject *object)
{
    UmiRecordEditorRoot *root = (UmiRecordEditorRoot *)object;
    if (root->editor != NULL) retire_editor(root->editor);
    G_OBJECT_CLASS(umi_record_editor_root_parent_class)->dispose(object);
}
static void root_finalize(GObject *object)
{
    UmiRecordEditorRoot *root = (UmiRecordEditorRoot *)object;
    if (root->editor != NULL) editor_unref(root->editor);
    root->editor = NULL;
    G_OBJECT_CLASS(umi_record_editor_root_parent_class)->finalize(object);
}
static void root_unroot(GtkWidget *widget)
{
    UmiRecordEditorRoot *root = (UmiRecordEditorRoot *)widget;
    if (root->editor != NULL) retire_editor(root->editor);
    GTK_WIDGET_CLASS(umi_record_editor_root_parent_class)->unroot(widget);
}
static void umi_record_editor_root_class_init(UmiRecordEditorRootClass *klass)
{
    G_OBJECT_CLASS(klass)->dispose = root_dispose;
    G_OBJECT_CLASS(klass)->finalize = root_finalize;
    GTK_WIDGET_CLASS(klass)->unroot = root_unroot;
}
static void umi_record_editor_root_init(UmiRecordEditorRoot *root) { root->editor = NULL; }

static RecordEditor *get_editor(GtkWidget *widget)
{
    return widget != NULL && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_record_editor_root_get_type())
        ? ((UmiRecordEditorRoot *)widget)->editor : NULL;
}

/* Each setter can synchronously notify a host which removes or disposes the
 * editor. Retain the particular control during the call; check retirement
 * before touching the next control. Callers retain editor throughout. */
static void label_text(RecordEditor *editor, GtkWidget *label, const char *text)
{
    if (editor->retired) return;
    g_object_ref(label);
    /* Imported provenance is byte-oriented; truncation can also split UTF-8.
     * Render invalid bytes as replacements without altering stored metadata. */
    char *valid = g_utf8_make_valid(text, -1);
    gtk_label_set_text(GTK_LABEL(label), valid);
    g_free(valid);
    g_object_unref(label);
}
static void sensitive(RecordEditor *editor, GtkWidget *widget, bool enabled)
{
    if (editor->retired) return;
    g_object_ref(widget);
    gtk_widget_set_sensitive(widget, enabled);
    g_object_unref(widget);
}
/* A preview is tied to the exact document and baseline observed when its read
 * began. Changing the draft text is safe; opening or saving another revision
 * requires another comparison before its baseline can be adopted. */
static bool review_current(const RecordEditor *editor, const UmiRecordDraftState *state)
{
    return editor->reviewed_latest && editor->reviewed != NULL && state->ready &&
        state->expected_revision == editor->reviewed_base_revision &&
        strcmp(state->entity_id, editor->reviewed->record.amendment.entity_id) == 0;
}
static void refresh(RecordEditor *editor)
{
    if (editor->retired) return;
    ++editor->applying;
    UmiRecordDraftState state = {0};
    (void)umi_record_draft_state(editor->draft, &state);
    bool connected = editor->server != NULL;
    bool editable = state.ready && (!editor->busy || (editor->operation != JOB_CONNECT && editor->operation != JOB_OPEN));
    sensitive(editor, editor->path, !connected && !editor->busy);
    sensitive(editor, editor->connect, !connected && !editor->busy);
    sensitive(editor, editor->new_record, connected && !editor->busy);
    sensitive(editor, editor->open, connected && !editor->busy);
    sensitive(editor, editor->discard, !editor->busy && (state.ready || !editor->text_valid));
    sensitive(editor, editor->save, connected && !editor->busy && state.dirty && editor->text_valid);
    sensitive(editor, editor->text, editable);
    sensitive(editor, editor->compare, connected && state.ready && !editor->busy);
    sensitive(editor, editor->adopt, connected && !editor->busy && editor->text_valid && review_current(editor, &state));
    bool history_matches = editor->history != NULL && state.ready && strcmp(state.entity_id, editor->history->entity_id) == 0;
    sensitive(editor, editor->history_refresh, connected && state.ready && !editor->busy);
    sensitive(editor, editor->history_older, connected && !editor->busy && history_matches && editor->history->next_before_revision != 0U);
    sensitive(editor, editor->history_view, connected && state.ready && !editor->busy);
    char text[256];
    if (!state.ready) {
        (void)snprintf(text, sizeof text, "%s", connected ? "Store ready. Create or open a record." : "Choose a local database to begin.");
    } else {
        (void)snprintf(text, sizeof text, "%s | revision %" PRIu64 " | %zu bytes | %s%s",
            state.entity_id, state.expected_revision, state.length,
            !editor->text_valid ? "Text exceeds the record limit" : state.dirty ? "Unsaved changes" : "Saved",
            editor->busy ? " | Working" : "");
    }
    label_text(editor, editor->summary, text);
    char review[640];
    if (editor->reviewed == NULL) {
        (void)snprintf(review, sizeof review, "Compare the stored revision without replacing your draft.");
    } else {
        (void)snprintf(review, sizeof review, "%s revision %" PRIu64 " by %s | %s%s",
            editor->reviewed_latest ? "Reviewed" : "Historical (read only)", editor->reviewed->record.revision, editor->reviewed->record.amendment.actor,
            editor->reviewed->record.amendment.reason,
            review_current(editor, &state) ? "" : " | Compare again before adopting");
    }
    label_text(editor, editor->review_summary, review);
    char history[224];
    if (!history_matches) (void)snprintf(history, sizeof history, "Refresh history for the active record.");
    else (void)snprintf(history, sizeof history, "%zu amendment rows from stored head %" PRIu64 "%s",
        editor->history->count, editor->history->head_revision,
        editor->history->next_before_revision != 0U ? " | Older rows available" : " | End of history");
    label_text(editor, editor->history_summary, history);
    --editor->applying;
}
static gboolean refresh_idle(gpointer data)
{
    RecordEditor *editor = data;
    GSource *source = editor->refresh_source;
    editor->refresh_source = NULL;
    if (source != NULL) g_source_unref(source);
    refresh(editor);
    return G_SOURCE_REMOVE;
}
static void refresh_release(gpointer data) { editor_unref(data); }
static void schedule_refresh(RecordEditor *editor)
{
    if (editor->retired || editor->refresh_source != NULL) return;
    /* Observers can edit during a label/sensitivity notification. Recompute
     * controls after that notification stack unwinds rather than recursively
     * publishing another snapshot while the current one is still applying. */
    editor->refresh_source = g_idle_source_new();
    g_source_set_callback(editor->refresh_source, refresh_idle, editor_ref(editor), refresh_release);
    (void)g_source_attach(editor->refresh_source, editor->context);
}
static void feedback(RecordEditor *editor, const char *operation, UmiStatus status)
{
    char message[320];
    (void)snprintf(message, sizeof message, "%s: %s", operation, umi_status_text(status));
    label_text(editor, editor->feedback, message);
}

static UmiStatus sync_text(RecordEditor *editor)
{
    if (editor->retired) return UMI_STATUS_UNAVAILABLE;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->text));
    if (gtk_text_buffer_get_char_count(buffer) > (int)UMI_RECORD_DOCUMENT_MAX_BYTES) {
        editor->text_valid = false;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    UmiStatus status = umi_record_draft_set_content(editor->draft, text, strlen(text));
    g_free(text);
    editor->text_valid = status == UMI_STATUS_OK;
    return status;
}
static void text_changed(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    RecordEditor *editor = editor_ref(data);
    if (!editor->retired && !editor->setting_text) {
        if (editor->edit_epoch == UINT64_MAX) editor->epoch_exhausted = true;
        else ++editor->edit_epoch;
        (void)sync_text(editor);
        if (editor->applying == 0U) refresh(editor);
        else schedule_refresh(editor);
    }
    editor_unref(editor);
}
static UmiStatus show_content(RecordEditor *editor)
{
    if (editor->retired) return UMI_STATUS_UNAVAILABLE;
    UmiRecordDraftState state;
    UmiStatus status = umi_record_draft_state(editor->draft, &state);
    if (status != UMI_STATUS_OK) return status;
    unsigned char text[UMI_RECORD_DOCUMENT_MAX_BYTES + 1U];
    size_t length = 0U;
    if (state.ready) status = umi_record_draft_copy_content(editor->draft, text, sizeof text - 1U, &length);
    if (status != UMI_STATUS_OK) return status;
    text[length] = '\0';
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->text)));
    editor->setting_text = true;
    gtk_text_buffer_set_text(buffer, (const char *)text, (int)length);
    editor->setting_text = false;
    g_object_unref(buffer);
    if (editor->retired) return UMI_STATUS_UNAVAILABLE;
    /* An observer can edit during set_text. Read the final visible bytes rather
     * than assuming the setter's initial input is still the displayed draft. */
    if (state.ready) return sync_text(editor);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->text));
    editor->text_valid = gtk_text_buffer_get_char_count(buffer) == 0;
    return editor->text_valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}

static void job_free(gpointer data)
{
    RecordJob *job = data;
    umi_data_server_destroy(job->opened_server);
    editor_unref(job->owner);
    g_free(job->path);
    g_free(job->entity);
    g_free(job->document);
    g_free(job->page);
    g_free(job->request);
    g_free(job);
}
static void worker(GTask *task, gpointer source, gpointer data, GCancellable *cancellable)
{
    (void)source; (void)cancellable;
    RecordJob *job = data;
    if (job->kind == JOB_CONNECT) {
        job->status = umi_data_server_create_sqlite(job->path, &job->opened_server);
    } else if (job->kind == JOB_HISTORY || job->kind == JOB_OLDER) {
        job->status = umi_record_history_page(job->server, job->entity, job->history_head,
            job->history_before, UMI_RECORD_HISTORY_PAGE_CAPACITY, job->page);
    } else if (job->kind == JOB_OPEN || job->kind == JOB_COMPARE || job->kind == JOB_REVISION) {
        job->status = job->kind == JOB_REVISION
            ? umi_record_document_load(job->server, job->entity, job->requested_revision, job->document)
            : umi_record_document_latest(job->server, job->entity, job->document);
        if (job->status == UMI_STATUS_OK &&
            (memchr(job->document->content, '\0', job->document->length) != NULL ||
             !g_utf8_validate((const char *)job->document->content, (gssize)job->document->length, NULL))) {
            job->status = UMI_STATUS_PARSE_ERROR;
        }
    } else {
        job->status = umi_record_save_request_commit(job->server, job->request, &job->saved);
    }
    g_task_return_boolean(task, TRUE);
}
static UmiStatus show_review(RecordEditor *editor, RecordJob *job)
{
    UmiRecordDraftState state;
    UmiStatus status = umi_record_draft_state(editor->draft, &state);
    if (status != UMI_STATUS_OK) return status;
    if (!state.ready || state.expected_revision != job->reviewed_base_revision ||
        strcmp(state.entity_id, job->entity) != 0) return UMI_STATUS_CANCELLED;
    /* Keep the validated snapshot independently of the read task. The visible
     * draft is never replaced by a comparison completion. */
    g_free(editor->reviewed);
    editor->reviewed = job->document;
    job->document = NULL;
    editor->reviewed_base_revision = job->reviewed_base_revision;
    editor->reviewed_latest = job->kind == JOB_COMPARE;
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->review_text)));
    gtk_text_buffer_set_text(buffer, (const char *)editor->reviewed->content, (int)editor->reviewed->length);
    g_object_unref(buffer);
    return editor->retired ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
}
static UmiStatus show_history(RecordEditor *editor, RecordJob *job)
{
    UmiRecordDraftState state;
    UmiStatus status = umi_record_draft_state(editor->draft, &state);
    if (status != UMI_STATUS_OK) return status;
    if (!state.ready || strcmp(state.entity_id, job->entity) != 0) return UMI_STATUS_CANCELLED;
    g_free(editor->history);
    editor->history = job->page;
    job->page = NULL;
    /* Render plain text, never markup, because actor and reason are data.
     * History contains metadata only; selected content has its own checked read. */
    GString *text = g_string_new(NULL);
    for (size_t i = 0U; i < editor->history->count; ++i) {
        const UmiRecordRevision *row = &editor->history->rows[i];
        g_string_append_printf(text, "Revision %" PRIu64 " | %s | %s\nReason: %s\nRecorded: %" PRIu64 " ns since epoch\n\n",
            row->revision, row->amendment.actor, row->amendment.source, row->amendment.reason,
            row->amendment.recorded_at_nanoseconds);
    }
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->history_text)));
    /* Legacy metadata is byte-oriented. Make invalid sequences visible as
     * replacements rather than passing invalid UTF-8 to a GTK text buffer. */
    char *valid = g_utf8_make_valid(text->str, (gssize)text->len);
    gtk_text_buffer_set_text(buffer, valid, -1);
    g_free(valid); g_string_free(text, TRUE); g_object_unref(buffer);
    return editor->retired ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
}
static void completed(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    GTask *task = G_TASK(result);
    RecordJob *job = g_task_get_task_data(task);
    RecordEditor *editor = job->owner;
    (void)g_task_propagate_boolean(task, NULL);
    ++editor->applying;
    UmiStatus status = job->status;
    if (!editor->retired) {
        if (job->kind == JOB_SAVE) {
            status = umi_record_draft_finish_save(editor->draft, job->request->request_id,
                status, status == UMI_STATUS_OK ? &job->saved : NULL);
        } else if (job->kind == JOB_CONNECT && status == UMI_STATUS_OK) {
            editor->server = job->opened_server;
            job->opened_server = NULL;
        } else if ((job->kind == JOB_HISTORY || job->kind == JOB_OLDER) && status == UMI_STATUS_OK) {
            status = show_history(editor, job);
        } else if ((job->kind == JOB_COMPARE || job->kind == JOB_REVISION) && status == UMI_STATUS_OK) {
            status = show_review(editor, job);
        } else if (job->kind == JOB_OPEN && status == UMI_STATUS_OK) {
            if (editor->epoch_exhausted || editor->edit_epoch != job->edit_epoch || !editor->text_valid) {
                status = UMI_STATUS_CANCELLED;
            } else {
                status = umi_record_draft_open(editor->draft, job->document);
                if (status == UMI_STATUS_OK) status = show_content(editor);
            }
        }
    }
    editor->busy = false;
    refresh(editor);
    if (!editor->retired) {
        if (status == UMI_STATUS_OK) label_text(editor, editor->feedback,
            job->kind == JOB_CONNECT ? "Database ready. This widget keeps using this path until closed." :
            job->kind == JOB_OPEN ? "Record opened. Earlier revisions remain in the store." :
            (job->kind == JOB_HISTORY || job->kind == JOB_OLDER) ? "History page loaded. Reading it does not change your draft or save baseline." :
            job->kind == JOB_REVISION ? "Historical content shown read-only. Compare stored text to choose a current save baseline." :
            job->kind == JOB_COMPARE ? "Stored text shown below. Your draft is unchanged; review both before choosing a save baseline." :
            "Captured revision saved. Any later edits remain in the editor.");
        else if (job->kind == JOB_SAVE && status == UMI_STATUS_INVALID_STATE)
            label_text(editor, editor->feedback, "Save refused: the stored revision changed. Compare the stored text, then explicitly choose the revision for your next save.");
        else if ((job->kind == JOB_HISTORY || job->kind == JOB_OLDER) && status == UMI_STATUS_INVALID_STATE)
            label_text(editor, editor->feedback, "History advanced since this page. Refresh history to restart from the new head; your draft is unchanged.");
        else if (job->kind == JOB_HISTORY || job->kind == JOB_OLDER || job->kind == JOB_REVISION)
            feedback(editor, "History read failed; draft unchanged", status);
        else feedback(editor, job->kind == JOB_CONNECT ? "Database unavailable" : job->kind == JOB_OPEN ? "Open left the draft unchanged" : job->kind == JOB_COMPARE ? "Comparison failed; draft unchanged" : "Save failed; keep this draft", status);
    }
    --editor->applying;
}
static UmiStatus attribution(RecordEditor *editor, UmiRecordDraftAttribution *output)
{
    const char *actor = gtk_editable_get_text(GTK_EDITABLE(editor->actor));
    const char *reason = gtk_editable_get_text(GTK_EDITABLE(editor->reason));
    if (strlen(actor) >= sizeof output->actor || strlen(reason) >= sizeof output->reason)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memset(output, 0, sizeof *output);
    (void)g_strlcpy(output->source, editor->source, sizeof output->source);
    (void)g_strlcpy(output->actor, actor, sizeof output->actor);
    (void)g_strlcpy(output->reason, reason, sizeof output->reason);
    gint64 now = g_get_real_time();
    if (now <= 0 || (uint64_t)now > UINT64_MAX / 1000U) return UMI_STATUS_INVALID_STATE;
    output->recorded_at_nanoseconds = (uint64_t)now * 1000U;
    return UMI_STATUS_OK;
}
static bool dirty(RecordEditor *editor)
{
    UmiRecordDraftState state = {0};
    (void)umi_record_draft_state(editor->draft, &state);
    return state.dirty || !editor->text_valid;
}
static UmiStatus selected_revision(RecordEditor *editor, uint64_t *output)
{
    const unsigned char *text = (const unsigned char *)gtk_editable_get_text(GTK_EDITABLE(editor->history_revision));
    uint64_t value = 0U;
    if (*text == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (; *text != '\0'; ++text) {
        if (*text < '0' || *text > '9') return UMI_STATUS_INVALID_ARGUMENT;
        unsigned int digit = (unsigned int)(*text - '0');
        if (value > (UINT64_MAX - digit) / 10U) return UMI_STATUS_CAPACITY_EXCEEDED;
        value = value * 10U + digit;
    }
    if (value == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    *output = value;
    return UMI_STATUS_OK;
}
static UmiStatus begin_job(RecordEditor *editor, EditorJobKind kind, const char *entity)
{
    RecordJob *job = g_try_new0(RecordJob, 1);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    job->owner = editor_ref(editor);
    job->kind = kind;
    job->server = editor->server;
    job->edit_epoch = editor->edit_epoch;
    UmiStatus status = UMI_STATUS_OK;
    if (kind == JOB_CONNECT) {
        job->path = g_strdup(gtk_editable_get_text(GTK_EDITABLE(editor->path)));
        if (!g_path_is_absolute(job->path) || strlen(job->path) > 4096U)
            status = UMI_STATUS_INVALID_ARGUMENT;
    } else if (kind == JOB_HISTORY || kind == JOB_OLDER) {
        job->entity = g_strdup(entity);
        job->page = g_try_new0(UmiRecordHistoryPage, 1);
        if (job->page == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        if (kind == JOB_OLDER) {
            if (editor->history == NULL || strcmp(editor->history->entity_id, entity) != 0 || editor->history->next_before_revision == 0U) status = UMI_STATUS_INVALID_STATE;
            else { job->history_head = editor->history->head_revision; job->history_before = editor->history->next_before_revision; }
        }
    } else if (kind == JOB_OPEN || kind == JOB_COMPARE || kind == JOB_REVISION) {
        job->entity = g_strdup(entity);
        if (kind == JOB_COMPARE || kind == JOB_REVISION) {
            UmiRecordDraftState state;
            status = umi_record_draft_state(editor->draft, &state);
            if (status == UMI_STATUS_OK && !state.ready) status = UMI_STATUS_INVALID_STATE;
            if (status == UMI_STATUS_OK) job->reviewed_base_revision = state.expected_revision;
        }
        if (kind == JOB_REVISION && status == UMI_STATUS_OK) status = selected_revision(editor, &job->requested_revision);
        job->document = g_try_new0(UmiRecordDocument, 1);
        if (job->document == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    } else {
        UmiRecordDraftAttribution author;
        status = attribution(editor, &author);
        job->request = g_try_new0(UmiRecordSaveRequest, 1);
        if (status == UMI_STATUS_OK && job->request == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        if (status == UMI_STATUS_OK) status = umi_record_draft_prepare_save(editor->draft, &author, job->request);
    }
    if (status != UMI_STATUS_OK) { job_free(job); return status; }
    editor->busy = true;
    editor->operation = kind;
    refresh(editor);
    label_text(editor, editor->feedback, kind == JOB_CONNECT ? "Opening or creating the selected local database…" :
        kind == JOB_OPEN ? "Reading the selected record…" :
        (kind == JOB_HISTORY || kind == JOB_OLDER) ? "Reading a bounded history page…" :
        kind == JOB_REVISION ? "Reading selected historical content…" :
        kind == JOB_COMPARE ? "Reading the stored text for comparison; you can continue editing…" :
        "Saving the captured revision; you can continue editing…");
    if (editor->retired || (kind == JOB_OPEN && (editor->epoch_exhausted || editor->edit_epoch != job->edit_epoch))) {
        if (kind == JOB_SAVE) (void)umi_record_draft_finish_save(editor->draft,
            job->request->request_id, UMI_STATUS_CANCELLED, NULL);
        editor->busy = false;
        job_free(job);
        return UMI_STATUS_CANCELLED;
    }
    /* No task references a widget. Its context reference owns the storage
     * handle until completion, even if root removal retires the editor. */
    GTask *task = g_task_new(NULL, NULL, completed, NULL);
    g_task_set_task_data(task, job, job_free);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, worker);
    g_object_unref(task);
    return UMI_STATUS_OK;
}

static void action(GtkButton *button, gpointer data)
{
    RecordEditor *editor = editor_ref(data);
    if (editor->retired || editor->applying != 0U || editor->busy) { editor_unref(editor); return; }
    ++editor->applying;
    EditorAction which = (EditorAction)GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "record-action"));
    UmiStatus status = UMI_STATUS_OK;
    char *entity = NULL;
    if (which == EDITOR_NEW || which == EDITOR_OPEN) {
        const char *key = gtk_editable_get_text(GTK_EDITABLE(editor->key));
        entity = g_strconcat(editor->prefix, key, NULL);
        if (*key == '\0' || strlen(entity) >= 64U) status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (status == UMI_STATUS_OK && which != EDITOR_CONNECT && editor->server == NULL)
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK) {
        switch (which) {
        case EDITOR_CONNECT:
            status = editor->server != NULL ? UMI_STATUS_INVALID_STATE : begin_job(editor, JOB_CONNECT, NULL);
            break;
        case EDITOR_NEW:
            status = dirty(editor) ? UMI_STATUS_INVALID_STATE : umi_record_draft_begin(editor->draft, entity);
            if (status == UMI_STATUS_OK) status = show_content(editor);
            break;
        case EDITOR_OPEN:
            status = dirty(editor) ? UMI_STATUS_INVALID_STATE : begin_job(editor, JOB_OPEN, entity);
            break;
        case EDITOR_SAVE:
            status = sync_text(editor);
            if (status == UMI_STATUS_OK) status = begin_job(editor, JOB_SAVE, NULL);
            break;
        case EDITOR_COMPARE: {
            UmiRecordDraftState state;
            status = umi_record_draft_state(editor->draft, &state);
            if (status == UMI_STATUS_OK && !state.ready) status = UMI_STATUS_INVALID_STATE;
            if (status == UMI_STATUS_OK) status = begin_job(editor, JOB_COMPARE, state.entity_id);
            break;
        }
        case EDITOR_HISTORY:
        case EDITOR_OLDER:
        case EDITOR_REVISION: {
            UmiRecordDraftState state;
            status = umi_record_draft_state(editor->draft, &state);
            if (status == UMI_STATUS_OK && !state.ready) status = UMI_STATUS_INVALID_STATE;
            EditorJobKind kind = which == EDITOR_HISTORY ? JOB_HISTORY : which == EDITOR_OLDER ? JOB_OLDER : JOB_REVISION;
            if (status == UMI_STATUS_OK) status = begin_job(editor, kind, state.entity_id);
            break;
        }
        case EDITOR_ADOPT: {
            UmiRecordDraftState state;
            status = umi_record_draft_state(editor->draft, &state);
            if (status == UMI_STATUS_OK && (!editor->text_valid || !review_current(editor, &state)))
                status = UMI_STATUS_INVALID_STATE;
            if (status == UMI_STATUS_OK) status = umi_record_draft_adopt_reviewed(editor->draft,
                editor->reviewed, editor->reviewed_base_revision);
            break;
        }
        case EDITOR_DISCARD: {
            UmiRecordDraftState state;
            status = umi_record_draft_state(editor->draft, &state);
            if (status == UMI_STATUS_OK && state.ready) status = umi_record_draft_discard(editor->draft);
            if (status == UMI_STATUS_OK) status = show_content(editor);
            break;
        }
        }
    }
    g_free(entity);
    refresh(editor);
    if (status != UMI_STATUS_OK) feedback(editor, "Action refused; existing content retained", status);
    else if (which == EDITOR_NEW) label_text(editor, editor->feedback, "New unsaved record. Enter text, reviewer and amendment reason before saving.");
    else if (which == EDITOR_ADOPT) label_text(editor, editor->feedback, "Reviewed baseline chosen; your text is unchanged. Save revision to append it, or discard to restore that reviewed text.");
    else if (which == EDITOR_DISCARD) label_text(editor, editor->feedback, "Unsaved edits discarded by your request.");
    --editor->applying;
    editor_unref(editor);
}

static GtkWidget *entry(GtkWidget *parent, const char *label, const char *name, int limit)
{
    GtkWidget *text = gtk_label_new(label);
    gtk_label_set_xalign(GTK_LABEL(text), 0.0F);
    gtk_box_append(GTK_BOX(parent), text);
    GtkWidget *widget = gtk_entry_new();
    gtk_widget_set_name(widget, name);
    gtk_entry_set_max_length(GTK_ENTRY(widget), limit);
    gtk_box_append(GTK_BOX(parent), widget);
    return widget;
}
static GtkWidget *button(RecordEditor *editor, GtkWidget *row, const char *label, const char *name, EditorAction which)
{
    GtkWidget *widget = gtk_button_new_with_label(label);
    gtk_widget_set_name(widget, name);
    g_object_set_data(G_OBJECT(widget), "record-action", GINT_TO_POINTER((int)which));
    connect_owned(editor, G_OBJECT(widget), "clicked", G_CALLBACK(action));
    gtk_box_append(GTK_BOX(row), widget);
    return widget;
}
static bool valid_option(const char *text, size_t limit, bool metadata)
{
    if (text == NULL || *text == '\0' || strlen(text) > limit || !g_utf8_validate(text, -1, NULL)) return false;
    if (metadata) for (const unsigned char *p = (const unsigned char *)text; *p; ++p)
        if (*p < 32U || *p == 127U) return false;
    return true;
}
UmiStatus UmiRecordEditorGtk4New(const UmiRecordEditorGtk4Options *options, GtkWidget **output)
{
    if (options == NULL || output == NULL || !valid_option(options->title, 256U, false) ||
        !valid_option(options->description, 1024U, false) || !valid_option(options->entity_prefix, 48U, true) ||
        !valid_option(options->source, 159U, true)) return UMI_STATUS_INVALID_ARGUMENT;
    RecordEditor *editor = g_try_new0(RecordEditor, 1);
    if (editor == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    g_atomic_ref_count_init(&editor->references);
    editor->context = g_main_context_ref_thread_default();
    editor->text_valid = true;
    UmiStatus status = umi_record_draft_create(&editor->draft);
    if (status != UMI_STATUS_OK) { editor_unref(editor); return status; }
    editor->prefix = g_strdup(options->entity_prefix);
    editor->source = g_strdup(options->source);
    UmiRecordEditorRoot *root = g_object_new(umi_record_editor_root_get_type(), "orientation", GTK_ORIENTATION_VERTICAL, "spacing", 8, NULL);
    root->editor = editor;
    GtkWidget *box = GTK_WIDGET(root);
    gtk_widget_set_name(box, "umicom-record-editor");
    GtkWidget *title = gtk_label_new(options->title);
    gtk_label_set_xalign(GTK_LABEL(title), 0.0F);
    gtk_widget_add_css_class(title, "title-2");
    gtk_box_append(GTK_BOX(box), title);
    GtkWidget *description = gtk_label_new(options->description);
    gtk_label_set_xalign(GTK_LABEL(description), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(description), TRUE);
    gtk_box_append(GTK_BOX(box), description);
    editor->path = entry(box, "Absolute path to a local database (opened or created explicitly)", "record-editor-path", 4096);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(box), row);
    GtkWidget *browse = UmiPathPickerGtk4Create(GTK_ENTRY(editor->path), UMI_PATH_PICKER_NEW_FILE,
        "Choose a local record database", "record-notes.sqlite3");
    if (browse != NULL) gtk_box_append(GTK_BOX(row), browse);
    editor->connect = button(editor, row, "Open or create database", "record-editor-connect", EDITOR_CONNECT);
    editor->key = entry(box, "Record key (within this application's namespace)", "record-editor-key", 63);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(box), row);
    editor->new_record = button(editor, row, "New draft", "record-editor-new", EDITOR_NEW);
    editor->open = button(editor, row, "Open record", "record-editor-open", EDITOR_OPEN);
    editor->actor = entry(box, "Reviewer name (recorded label, not a sign-in)", "record-editor-actor", 127);
    editor->reason = entry(box, "Reason for this revision", "record-editor-reason", 255);
    editor->text = gtk_text_view_new();
    gtk_widget_set_name(editor->text, "record-editor-text");
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor->text), GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 220);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), editor->text);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_box_append(GTK_BOX(box), scroll);
    connect_owned(editor, G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(editor->text))), "changed", G_CALLBACK(text_changed));
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(box), row);
    editor->save = button(editor, row, "Save revision", "record-editor-save", EDITOR_SAVE);
    editor->discard = button(editor, row, "Discard local edits", "record-editor-discard", EDITOR_DISCARD);
    editor->compare = button(editor, row, "Compare stored text", "record-editor-compare", EDITOR_COMPARE);
    GtkWidget *review = gtk_expander_new("Stored text for review");
    gtk_expander_set_expanded(GTK_EXPANDER(review), TRUE);
    gtk_box_append(GTK_BOX(box), review);
    GtkWidget *review_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_expander_set_child(GTK_EXPANDER(review), review_box);
    editor->review_summary = gtk_label_new("");
    gtk_widget_set_name(editor->review_summary, "record-editor-review-summary");
    gtk_label_set_xalign(GTK_LABEL(editor->review_summary), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(editor->review_summary), TRUE);
    gtk_box_append(GTK_BOX(review_box), editor->review_summary);
    editor->review_text = gtk_text_view_new();
    gtk_widget_set_name(editor->review_text, "record-editor-review-text");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(editor->review_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor->review_text), GTK_WRAP_WORD_CHAR);
    GtkWidget *review_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(review_scroll), 120);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(review_scroll), editor->review_text);
    gtk_box_append(GTK_BOX(review_box), review_scroll);
    editor->adopt = button(editor, review_box, "Keep my draft against this revision", "record-editor-adopt", EDITOR_ADOPT);
    GtkWidget *explanation = gtk_label_new("Review the stored text and your draft before choosing a baseline. This action does not merge text or save; the next save checks the store again.");
    gtk_label_set_wrap(GTK_LABEL(explanation), TRUE);
    gtk_label_set_xalign(GTK_LABEL(explanation), 0.0F);
    gtk_box_append(GTK_BOX(review_box), explanation);
    GtkWidget *history_expander = gtk_expander_new("Amendment history");
    gtk_box_append(GTK_BOX(box), history_expander);
    GtkWidget *history_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_expander_set_child(GTK_EXPANDER(history_expander), history_box);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(history_box), row);
    editor->history_refresh = button(editor, row, "Refresh history", "record-editor-history", EDITOR_HISTORY);
    editor->history_older = button(editor, row, "Older revisions", "record-editor-older", EDITOR_OLDER);
    editor->history_summary = gtk_label_new("");
    gtk_widget_set_name(editor->history_summary, "record-editor-history-summary");
    gtk_label_set_xalign(GTK_LABEL(editor->history_summary), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(editor->history_summary), TRUE);
    gtk_box_append(GTK_BOX(history_box), editor->history_summary);
    editor->history_text = gtk_text_view_new();
    gtk_widget_set_name(editor->history_text, "record-editor-history-text");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(editor->history_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(editor->history_text), GTK_WRAP_WORD_CHAR);
    GtkWidget *history_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(history_scroll), 180);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(history_scroll), editor->history_text);
    gtk_box_append(GTK_BOX(history_box), history_scroll);
    editor->history_revision = entry(history_box, "Revision number to inspect", "record-editor-history-revision", 20);
    editor->history_view = button(editor, history_box, "View historical text", "record-editor-history-view", EDITOR_REVISION);
    editor->summary = gtk_label_new("");
    gtk_widget_set_name(editor->summary, "record-editor-summary");
    gtk_label_set_xalign(GTK_LABEL(editor->summary), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(editor->summary), TRUE);
    gtk_box_append(GTK_BOX(box), editor->summary);
    editor->feedback = gtk_label_new("Local notes are not encrypted. Keep real patient information and account secrets out of synthetic exercises.");
    gtk_widget_set_name(editor->feedback, "record-editor-feedback");
    gtk_label_set_xalign(GTK_LABEL(editor->feedback), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(editor->feedback), TRUE);
    gtk_box_append(GTK_BOX(box), editor->feedback);
    refresh(editor);
    *output = box;
    return UMI_STATUS_OK;
}

bool UmiRecordEditorGtk4Busy(GtkWidget *widget)
{
    RecordEditor *editor = get_editor(widget);
    return editor != NULL && editor->busy;
}
UmiStatus UmiRecordEditorGtk4State(GtkWidget *widget, UmiRecordDraftState *output)
{
    RecordEditor *editor = get_editor(widget);
    if (editor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (editor->retired) return UMI_STATUS_UNAVAILABLE;
    return umi_record_draft_state(editor->draft, output);
}
bool UmiRecordEditorGtk4CanClose(GtkWidget *widget)
{
    RecordEditor *editor = get_editor(widget);
    return editor == NULL || editor->retired || (!editor->busy && !dirty(editor));
}
bool UmiRecordEditorGtk4RequestClose(GtkWidget *widget)
{
    RecordEditor *editor = get_editor(widget);
    if (editor == NULL || editor->retired) return true;
    editor_ref(editor);
    bool allowed = !editor->busy && !dirty(editor);
    if (!allowed) {
        ++editor->applying;
        label_text(editor, editor->feedback, editor->busy ? "Wait for the accepted storage operation before closing." :
            "Save or explicitly discard the local draft before closing.");
        --editor->applying;
    }
    editor_unref(editor);
    return allowed;
}
void UmiRecordEditorGtk4Retire(GtkWidget *widget)
{
    RecordEditor *editor = get_editor(widget);
    if (editor != NULL) retire_editor(editor);
}
