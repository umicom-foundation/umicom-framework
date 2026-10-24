/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Keep database work away from GTK and give every worker copied input. Typed
 * sessions own revision checks; products supply only schema and namespace. */
#include "umicom/ui/gtk4/form_record_editor.h"
#include "umicom/ui/gtk4/structured_form.h"
#include "umicom/application/experience/structured_form_snapshot.h"
#include "umicom/base/text.h"
#include "umicom/data/record_draft_files.h"
#include <string.h>

typedef struct UmiFormRecordRoot {
    GtkBox parent;
    const UmiStructuredFormSchema *schema;
    UmiStructuredFormSession *session;
    UmiDataServer *server;
    GtkWidget *body, *form, *feedback, *path, *key, *actor, *reason, *recovery;
    char prefix[49], source[160];
    UmiStatus (*key_validator)(const char *key);
    bool retired, busy, working;
    UmiStatus last;
} UmiFormRecordRoot;
typedef struct UmiFormRecordRootClass { GtkBoxClass parent; } UmiFormRecordRootClass;
GType umi_form_record_root_get_type(void) G_GNUC_CONST;
G_DEFINE_TYPE(UmiFormRecordRoot, umi_form_record_root, GTK_TYPE_BOX)
typedef enum FormRecordOperation { FORM_RECORD_CONNECT, FORM_RECORD_OPEN, FORM_RECORD_SAVE, FORM_RECORD_KEEP, FORM_RECORD_RECOVER } FormRecordOperation;
typedef struct FormRecordJob {
    FormRecordOperation operation;
    UmiDataServer *server, *opened;
    UmiRecordSaveRequest *request;
    UmiRecordDocument *document;
    UmiRecordDraftSnapshot *snapshot;
    UmiRecordRevision saved;
    uint64_t generation, native_revision;
    char path[1024], entity[64], prefix[49];
    UmiStatus status;
} FormRecordJob;
static UmiFormRecordRoot *root_from(GtkWidget *widget) {
    return widget != NULL && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_form_record_root_get_type())
        ? (UmiFormRecordRoot *)widget : NULL;
}
static void retire(UmiFormRecordRoot *self) {
    self->retired = true;
    if (self->form != NULL) UmiStructuredFormGtk4Retire(self->form);
}
static void unroot(GtkWidget *widget) {
    retire((UmiFormRecordRoot *)widget);
    GTK_WIDGET_CLASS(umi_form_record_root_parent_class)->unroot(widget);
}
static void dispose(GObject *object) {
    retire((UmiFormRecordRoot *)object);
    /* Child pointers cease to be usable after chaining to GtkBox disposal.
     * Keep the session/store until finalization: GTask retains its owner. */
    ((UmiFormRecordRoot *)object)->form = NULL;
    G_OBJECT_CLASS(umi_form_record_root_parent_class)->dispose(object);
}
static void finalize(GObject *object) {
    UmiFormRecordRoot *self = (UmiFormRecordRoot *)object;
    umi_structured_form_session_destroy(self->session);
    umi_data_server_destroy(self->server);
    G_OBJECT_CLASS(umi_form_record_root_parent_class)->finalize(object);
}
static void umi_form_record_root_class_init(UmiFormRecordRootClass *klass) {
    GTK_WIDGET_CLASS(klass)->unroot = unroot;
    G_OBJECT_CLASS(klass)->dispose = dispose;
    G_OBJECT_CLASS(klass)->finalize = finalize;
}
static void umi_form_record_root_init(UmiFormRecordRoot *self) {
    gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
    gtk_box_set_spacing(GTK_BOX(self), 10);
}
static void show_status(UmiFormRecordRoot *self, UmiStatus status, const char *success) {
    self->last = status;
    if (self->retired) return;
    GtkWidget *label = g_object_ref(self->feedback);
    gtk_label_set_text(GTK_LABEL(label), status == UMI_STATUS_OK ? success : umi_status_text(status));
    g_object_unref(label);
}
static UmiStatus enter(UmiFormRecordRoot *self) {
    if (self == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (self->retired) return UMI_STATUS_INVALID_STATE;
    if (self->working || self->busy) return UMI_STATUS_BUSY;
    g_object_ref(self);
    self->working = true;
    return UMI_STATUS_OK;
}
static UmiStatus leave(UmiFormRecordRoot *self, UmiStatus status, const char *message) {
    show_status(self, status, message);
    self->working = false;
    g_object_unref(self);
    return status;
}
static bool ascii_key_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || c == '_' || c == '-';
}
static UmiStatus entity_for(UmiFormRecordRoot *self, const char *key, char out[64]) {
    if (key == NULL || key[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0, prefix = strlen(self->prefix);
    while (length < 64 && key[length] != '\0') {
        if (!ascii_key_char(key[length])) return UMI_STATUS_INVALID_ARGUMENT;
        ++length;
    }
    if (length + prefix >= 64) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (self->key_validator != NULL) {
        UmiStatus status = self->key_validator(key);
        if (status != UMI_STATUS_OK) return status;
        if (self->retired) return UMI_STATUS_INVALID_STATE;
    }
    memcpy(out, self->prefix, prefix);
    memcpy(out + prefix, key, length + 1);
    return UMI_STATUS_OK;
}
static bool clean(UmiFormRecordRoot *self) {
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    if (view == NULL) return false;
    bool okay = umi_structured_form_session_snapshot(self->session, view) == UMI_STATUS_OK &&
        !view->record.dirty && !view->record.saving &&
        (self->form == NULL || UmiStructuredFormGtk4CanClose(self->form));
    g_free(view);
    return okay;
}
/* The new form is fully constructed before changing record selection. Holding
 * its reference through parent notifications also handles synchronous unroot. */
static void install_form(UmiFormRecordRoot *self, GtkWidget *form) {
    GtkWidget *old = self->form;
    self->form = NULL;
    if (old != NULL) {
        UmiStructuredFormGtk4Retire(old);
        gtk_box_remove(GTK_BOX(self->body), old);
    }
    if (!self->retired && form != NULL) {
        self->form = form;
        gtk_box_append(GTK_BOX(self->body), form);
    }
}
static void job_free(gpointer data) {
    FormRecordJob *job = data;
    umi_data_server_destroy(job->opened);
    g_free(job->request); g_free(job->document); g_free(job->snapshot); g_free(job);
}
/* Preserve the original three-operation worker for review. Recovery adds
 * exclusive private-file operations using the existing shared file service. */
#if 0
static void worker(GTask *task, gpointer source, gpointer data, GCancellable *cancellable) {
    (void)source; (void)cancellable;
    FormRecordJob *job = data;
    if (job->operation == FORM_RECORD_CONNECT)
        job->status = umi_data_server_create_sqlite(job->path, &job->opened);
    else if (job->operation == FORM_RECORD_OPEN)
        job->status = umi_record_document_latest(job->server, job->entity, job->document);
    else
        job->status = umi_record_save_request_commit(job->server, job->request, &job->saved);
    g_task_return_boolean(task, TRUE);
}
#endif
static void worker(GTask *task, gpointer source, gpointer data, GCancellable *cancellable) {
    (void)source; (void)cancellable;
    FormRecordJob *job = data;
    if (job->operation == FORM_RECORD_CONNECT)
        job->status = umi_data_server_create_sqlite(job->path, &job->opened);
    else if (job->operation == FORM_RECORD_OPEN)
        job->status = umi_record_document_latest(job->server, job->entity, job->document);
    else if (job->operation == FORM_RECORD_SAVE)
        job->status = umi_record_save_request_commit(job->server, job->request, &job->saved);
    else if (job->operation == FORM_RECORD_KEEP)
        job->status = umi_record_draft_snapshot_write_new(job->path, job->snapshot);
    else {
        char *directory = g_path_get_dirname(job->path), *file = g_path_get_basename(job->path);
        job->status = umi_record_draft_snapshot_read(directory, file, job->snapshot);
        g_free(directory); g_free(file);
        if (job->status == UMI_STATUS_OK && !g_str_has_prefix(job->snapshot->baseline.record.amendment.entity_id, job->prefix))
            job->status = UMI_STATUS_PERMISSION_DENIED;
        if (job->status == UMI_STATUS_OK)
            job->status = umi_record_draft_snapshot_check_store(job->server, job->snapshot);
    }
    g_task_return_boolean(task, TRUE);
}
static UmiStatus accept_document(UmiFormRecordRoot *self, FormRecordJob *job) {
    if (!clean(self)) return UMI_STATUS_INVALID_STATE;
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    UmiStructuredForm *form = g_try_new0(UmiStructuredForm, 1);
    if (view == NULL || form == NULL) { g_free(view); g_free(form); return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status = umi_structured_form_session_snapshot(self->session, view);
    if (status == UMI_STATUS_OK && view->generation != job->generation) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = umi_structured_form_snapshot_decode(self->schema, job->document->content,
            job->document->length, form);
    GtkWidget *native = NULL;
    if (status == UMI_STATUS_OK) {
        form->saved_revision = form->revision;
        native = UmiStructuredFormGtk4New(self->schema, form);
        if (native == NULL) status = UMI_STATUS_INVALID_STATE;
        else g_object_ref_sink(native);
    }
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_open(self->session, job->document);
    if (status == UMI_STATUS_OK) install_form(self, native);
    if (native != NULL) g_object_unref(native);
    g_free(form); g_free(view);
    return status;
}
static UmiStatus accept_recovery(UmiFormRecordRoot *self, FormRecordJob *job) {
    if (!clean(self)) return UMI_STATUS_INVALID_STATE;
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    UmiStructuredForm *form = g_try_new0(UmiStructuredForm, 1), *baseline = g_try_new0(UmiStructuredForm, 1);
    if (view == NULL || form == NULL || baseline == NULL) { g_free(view); g_free(form); g_free(baseline); return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status = umi_structured_form_session_snapshot(self->session, view);
    if (status == UMI_STATUS_OK && view->generation != job->generation) status = UMI_STATUS_INVALID_STATE;
    char entity[64];
    if (status == UMI_STATUS_OK) status = entity_for(self,
        job->snapshot->baseline.record.amendment.entity_id + strlen(self->prefix), entity);
    if (status == UMI_STATUS_OK) status = umi_structured_form_snapshot_decode(self->schema,
        job->snapshot->content, job->snapshot->length, form);
    if (status == UMI_STATUS_OK && job->snapshot->baseline.record.revision != 0)
        status = umi_structured_form_snapshot_decode(self->schema, job->snapshot->baseline.content,
            job->snapshot->baseline.length, baseline);
    GtkWidget *native = NULL;
    if (status == UMI_STATUS_OK) {
        form->saved_revision = baseline->revision;
        native = UmiStructuredFormGtk4New(self->schema, form);
        if (native == NULL) status = UMI_STATUS_INVALID_STATE;
        else g_object_ref_sink(native);
    }
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_restore(self->session, job->snapshot);
    if (status == UMI_STATUS_OK) install_form(self, native);
    if (native != NULL) g_object_unref(native);
    g_free(view); g_free(form); g_free(baseline);
    return status;
}
static void completed(GObject *source, GAsyncResult *result, gpointer data) {
    (void)data;
    UmiFormRecordRoot *self = (UmiFormRecordRoot *)source;
    GTask *task = G_TASK(result);
    FormRecordJob *job = g_task_get_task_data(task);
    (void)g_task_propagate_boolean(task, NULL);
    self->working = true;
    UmiStatus status = job->status;
    const char *message = "Database connected. Choose a record key and New or Open.";
    if (job->operation == FORM_RECORD_SAVE) {
        /* Finish storage ownership even after retirement; never claim a running
         * write was cancelled because its window disappeared. */
        status = umi_structured_form_session_finish_save(self->session, job->request->request_id,
            job->status, job->status == UMI_STATUS_OK ? &job->saved : NULL);
        if (status == UMI_STATUS_OK && !self->retired && self->form != NULL)
            status = UmiStructuredFormGtk4MarkSaved(self->form, job->native_revision);
        message = "Saved the captured answers. Later edits remain unsaved.";
    } else if (job->operation == FORM_RECORD_KEEP) {
        /* A completed recovery write is real even when its host retired. */
        message = "Kept a recovery copy. This does not save or clear changes in the database.";
    } else if (!self->retired && status == UMI_STATUS_OK) {
        if (job->operation == FORM_RECORD_CONNECT) {
            self->server = job->opened;
            job->opened = NULL;
        } else if (job->operation == FORM_RECORD_OPEN) {
            status = accept_document(self, job);
            message = "Opened the requested record. Review its answers before editing.";
        } else if (job->operation == FORM_RECORD_RECOVER) {
            status = accept_recovery(self, job);
            message = "Recovered the selected draft. Review it before saving; the stored head is checked again on save.";
        } else message = "Kept a recovery copy. This does not save or clear changes in the database.";
    } else if (self->retired && status == UMI_STATUS_OK) status = UMI_STATUS_CANCELLED;
    self->busy = false;
    show_status(self, status, message);
    self->working = false;
}
static void dispatch(UmiFormRecordRoot *self, FormRecordJob *job) {
    job->server = self->server;
    self->busy = true;
    GTask *task = g_task_new(self, NULL, completed, NULL);
    g_task_set_task_data(task, job, job_free);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, worker);
    g_object_unref(task);
}
UmiStatus UmiFormRecordEditorGtk4Connect(GtkWidget *editor, const char *path) {
    UmiFormRecordRoot *self = root_from(editor);
    UmiStatus status = enter(self);
    if (status != UMI_STATUS_OK) return status;
    if (self->server != NULL) return leave(self, UMI_STATUS_ALREADY_EXISTS, "");
    if (path == NULL || !g_path_is_absolute(path) || !g_utf8_validate(path, -1, NULL))
        return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
    FormRecordJob *job = g_try_new0(FormRecordJob, 1);
    if (job == NULL) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
    status = umi_text_replace(job->path, sizeof(job->path), path);
    if (status == UMI_STATUS_OK) { job->operation = FORM_RECORD_CONNECT; dispatch(self, job); }
    else job_free(job);
    return leave(self, status, "Opening the selected local database.");
}
UmiStatus UmiFormRecordEditorGtk4Begin(GtkWidget *editor, const char *key) {
    UmiFormRecordRoot *self = root_from(editor);
    UmiStatus status = enter(self);
    if (status != UMI_STATUS_OK) return status;
    char entity[64];
    status = self->server == NULL ? UMI_STATUS_INVALID_STATE : entity_for(self, key, entity);
    if (status == UMI_STATUS_OK && !clean(self)) status = UMI_STATUS_INVALID_STATE;
    GtkWidget *native = NULL;
    if (status == UMI_STATUS_OK) {
        native = UmiStructuredFormGtk4New(self->schema, NULL);
        if (native == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
        else g_object_ref_sink(native);
    }
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_begin(self->session, entity);
    if (status == UMI_STATUS_OK) install_form(self, native);
    if (native != NULL) g_object_unref(native);
    return leave(self, status, "New unsaved record. This key is checked for existing data when saved.");
}
UmiStatus UmiFormRecordEditorGtk4Open(GtkWidget *editor, const char *key) {
    UmiFormRecordRoot *self = root_from(editor);
    UmiStatus status = enter(self);
    if (status != UMI_STATUS_OK) return status;
    if (self->server == NULL || !clean(self)) return leave(self, UMI_STATUS_INVALID_STATE, "");
    FormRecordJob *job = g_try_new0(FormRecordJob, 1);
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    if (job == NULL || view == NULL) { g_free(job); g_free(view); return leave(self, UMI_STATUS_OUT_OF_MEMORY, ""); }
    job->document = g_try_new0(UmiRecordDocument, 1);
    status = job->document == NULL ? UMI_STATUS_OUT_OF_MEMORY : entity_for(self, key, job->entity);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_snapshot(self->session, view);
    if (status == UMI_STATUS_OK) {
        job->generation = view->generation; job->operation = FORM_RECORD_OPEN; dispatch(self, job);
    } else job_free(job);
    g_free(view);
    return leave(self, status, "Reading the requested record; typing will prevent replacement.");
}
/* Save and recovery now capture native input through one shared path. The
 * previous save implementation is retained to review its lifetime checks. */
#if 0
UmiStatus UmiFormRecordEditorGtk4Save(GtkWidget *editor,
    const UmiRecordDraftAttribution *attribution, int submitting) {
    UmiFormRecordRoot *self = root_from(editor);
    UmiStatus status = enter(self);
    if (status != UMI_STATUS_OK) return status;
    if (self->server == NULL || self->form == NULL || attribution == NULL || (submitting != 0 && submitting != 1))
        return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
    /* Allocate all worker-owned storage before applying visible values. */
    FormRecordJob *job = g_try_new0(FormRecordJob, 1);
    UmiStructuredForm *form = g_try_new0(UmiStructuredForm, 1);
    UmiStructuredFormEdit *edit = g_try_new0(UmiStructuredFormEdit, 1);
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    if (job != NULL) job->request = g_try_new0(UmiRecordSaveRequest, 1);
    if (job == NULL || job->request == NULL || form == NULL || edit == NULL || view == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    GtkWidget *native = g_object_ref(self->form);
    if (status == UMI_STATUS_OK) status = UmiStructuredFormGtk4Apply(native, submitting);
    if (status == UMI_STATUS_OK && self->retired) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiStructuredFormGtk4Snapshot(native, form);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_snapshot(self->session, view);
    if (status == UMI_STATUS_OK) status = umi_structured_form_edit_capture(self->schema, &view->form, form, edit);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_apply(self->session, view->generation, edit);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_prepare_save(self->session,
        view->generation, attribution, submitting, job->request);
    if (status == UMI_STATUS_OK) {
        job->operation = FORM_RECORD_SAVE; job->native_revision = form->revision; dispatch(self, job);
    } else if (job != NULL) job_free(job);
    g_object_unref(native); g_free(view); g_free(edit); g_free(form);
    return leave(self, status, "Saving the captured answers; you may continue editing.");
}
#endif
static UmiStatus accept_visible(UmiFormRecordRoot *self, int submitting,
    uint64_t *generation, uint64_t *native_revision) {
    UmiStructuredForm *form = g_try_new0(UmiStructuredForm, 1);
    UmiStructuredFormEdit *edit = g_try_new0(UmiStructuredFormEdit, 1);
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    UmiStatus status = form == NULL || edit == NULL || view == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    GtkWidget *native = g_object_ref(self->form);
    if (status == UMI_STATUS_OK) status = UmiStructuredFormGtk4Apply(native, submitting);
    if (status == UMI_STATUS_OK && self->retired) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK) status = UmiStructuredFormGtk4Snapshot(native, form);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_snapshot(self->session, view);
    if (status == UMI_STATUS_OK) status = umi_structured_form_edit_capture(self->schema, &view->form, form, edit);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_apply(self->session, view->generation, edit);
    if (status == UMI_STATUS_OK) { *generation = view->generation; *native_revision = form->revision; }
    g_object_unref(native); g_free(view); g_free(edit); g_free(form);
    return status;
}
UmiStatus UmiFormRecordEditorGtk4Save(GtkWidget *editor,
    const UmiRecordDraftAttribution *attribution, int submitting) {
    UmiFormRecordRoot *self = root_from(editor);
    UmiStatus status = enter(self);
    if (status != UMI_STATUS_OK) return status;
    if (self->server == NULL || self->form == NULL || attribution == NULL || (submitting != 0 && submitting != 1))
        return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
    /* Capture command metadata before native Apply can notify application code. */
    const UmiRecordDraftAttribution captured = *attribution;
    FormRecordJob *job = g_try_new0(FormRecordJob, 1);
    if (job == NULL) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
    job->request = g_try_new0(UmiRecordSaveRequest, 1);
    status = job->request == NULL ? UMI_STATUS_OUT_OF_MEMORY :
        accept_visible(self, submitting, &job->generation, &job->native_revision);
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_prepare_save(self->session,
        job->generation, &captured, submitting, job->request);
    if (status == UMI_STATUS_OK) { job->operation = FORM_RECORD_SAVE; dispatch(self, job); }
    else job_free(job);
    return leave(self, status, "Saving the captured answers; you may continue editing.");
}
static UmiStatus recovery_operation(GtkWidget *editor, const char *path, bool writing) {
    UmiFormRecordRoot *self = root_from(editor);
    UmiStatus status = enter(self);
    if (status != UMI_STATUS_OK) return status;
    if (self->server == NULL || (writing && self->form == NULL) || (!writing && !clean(self)))
        return leave(self, UMI_STATUS_INVALID_STATE, "");
    if (path == NULL || !g_path_is_absolute(path) || !g_utf8_validate(path, -1, NULL))
        return leave(self, UMI_STATUS_INVALID_ARGUMENT, "");
    FormRecordJob *job = g_try_new0(FormRecordJob, 1);
    if (job == NULL) return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
    job->snapshot = g_try_new0(UmiRecordDraftSnapshot, 1);
    status = job->snapshot == NULL ? UMI_STATUS_OUT_OF_MEMORY : umi_text_replace(job->path, sizeof(job->path), path);
    memcpy(job->prefix, self->prefix, sizeof(job->prefix));
    if (status == UMI_STATUS_OK && writing) {
        status = accept_visible(self, 0, &job->generation, &job->native_revision);
        if (status == UMI_STATUS_OK) status = umi_structured_form_session_capture(self->session, job->snapshot);
    } else if (status == UMI_STATUS_OK) {
        UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
        status = view == NULL ? UMI_STATUS_OUT_OF_MEMORY : umi_structured_form_session_snapshot(self->session, view);
        if (status == UMI_STATUS_OK) job->generation = view->generation;
        g_free(view);
    }
    if (status == UMI_STATUS_OK) {
        job->operation = writing ? FORM_RECORD_KEEP : FORM_RECORD_RECOVER; dispatch(self, job);
    } else job_free(job);
    return leave(self, status, writing ? "Writing a new recovery file; the database remains unchanged." : "Reading a recovery file and checking its stored baseline.");
}
UmiStatus UmiFormRecordEditorGtk4KeepRecovery(GtkWidget *editor, const char *path) {
    return recovery_operation(editor, path, true);
}
UmiStatus UmiFormRecordEditorGtk4Recover(GtkWidget *editor, const char *path) {
    return recovery_operation(editor, path, false);
}
UmiStatus UmiFormRecordEditorGtk4Discard(GtkWidget *editor) {
    UmiFormRecordRoot *self = root_from(editor);
    UmiStatus status = enter(self);
    if (status != UMI_STATUS_OK) return status;
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    UmiRecordDraftSnapshot *snapshot = g_try_new0(UmiRecordDraftSnapshot, 1);
    UmiStructuredForm *baseline = g_try_new0(UmiStructuredForm, 1);
    GtkWidget *native = NULL;
    if (view == NULL || snapshot == NULL || baseline == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_snapshot(self->session, view);
    if (status == UMI_STATUS_OK && !view->record.ready) status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK && !view->record.is_new) {
        status = umi_structured_form_session_capture(self->session, snapshot);
        if (status == UMI_STATUS_OK) status = umi_structured_form_snapshot_decode(self->schema,
            snapshot->baseline.content, snapshot->baseline.length, baseline);
        if (status == UMI_STATUS_OK) {
            baseline->saved_revision = baseline->revision;
            native = UmiStructuredFormGtk4New(self->schema, baseline);
            if (native == NULL) status = UMI_STATUS_OUT_OF_MEMORY;
            else g_object_ref_sink(native);
        }
    }
    if (status == UMI_STATUS_OK) status = umi_structured_form_session_discard(self->session, view->generation);
    if (status == UMI_STATUS_OK) install_form(self, native);
    if (native != NULL) g_object_unref(native);
    g_free(baseline); g_free(snapshot); g_free(view);
    return leave(self, status, "Discarded unsaved answers after the explicit discard action.");
}
UmiStatus UmiFormRecordEditorGtk4State(GtkWidget *editor, UmiStructuredFormSessionView *out) {
    UmiFormRecordRoot *self = root_from(editor);
    if (self == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_structured_form_session_snapshot(self->session, out);
}
UmiStatus UmiFormRecordEditorGtk4LastStatus(GtkWidget *editor) {
    UmiFormRecordRoot *self = root_from(editor);
    return self != NULL ? self->last : UMI_STATUS_INVALID_ARGUMENT;
}
bool UmiFormRecordEditorGtk4Busy(GtkWidget *editor) {
    UmiFormRecordRoot *self = root_from(editor); return self != NULL && self->busy;
}
bool UmiFormRecordEditorGtk4CanClose(GtkWidget *editor) {
    UmiFormRecordRoot *self = root_from(editor);
    return self != NULL && !self->busy && !self->working && !self->retired && clean(self);
}
bool UmiFormRecordEditorGtk4RequestClose(GtkWidget *editor) {
    UmiFormRecordRoot *self = root_from(editor);
    if (self == NULL || self->retired) return false;
    bool allowed = UmiFormRecordEditorGtk4CanClose(editor);
    if (!allowed) {
        g_object_ref(self);
        GtkWidget *label = g_object_ref(self->feedback);
        gtk_label_set_text(GTK_LABEL(label), self->busy
            ? "Wait for the current storage operation before closing."
            : "Save your answers or explicitly discard them before closing.");
        g_object_unref(label);
        g_object_unref(self);
    }
    return allowed;
}
void UmiFormRecordEditorGtk4Retire(GtkWidget *editor) {
    UmiFormRecordRoot *self = root_from(editor); if (self != NULL) retire(self);
}
static void connect_clicked(GtkButton *button, gpointer data) {
    (void)button; UmiFormRecordRoot *self = data;
    if (!self->retired) (void)UmiFormRecordEditorGtk4Connect(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->path)));
}
static void begin_clicked(GtkButton *button, gpointer data) {
    (void)button; UmiFormRecordRoot *self = data;
    if (!self->retired) (void)UmiFormRecordEditorGtk4Begin(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->key)));
}
static void open_clicked(GtkButton *button, gpointer data) {
    (void)button; UmiFormRecordRoot *self = data;
    if (!self->retired) (void)UmiFormRecordEditorGtk4Open(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->key)));
}
static void save_clicked(GtkButton *button, gpointer data) {
    UmiFormRecordRoot *self = data;
    if (self->retired) return;
    UmiRecordDraftAttribution attribution = {0};
    UmiStatus status = umi_text_replace(attribution.source, sizeof(attribution.source), self->source);
    if (status == UMI_STATUS_OK) status = umi_text_replace(attribution.actor, sizeof(attribution.actor), gtk_editable_get_text(GTK_EDITABLE(self->actor)));
    if (status == UMI_STATUS_OK) status = umi_text_replace(attribution.reason, sizeof(attribution.reason), gtk_editable_get_text(GTK_EDITABLE(self->reason)));
    if (status == UMI_STATUS_OK) (void)UmiFormRecordEditorGtk4Save(GTK_WIDGET(self), &attribution,
        GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "submit")));
    else { g_object_ref(self); show_status(self, status, ""); g_object_unref(self); }
}
/* Retain the original dialog composition for review. The context-bound GTK4
 * window below replaces it and avoids deprecated message-dialog interfaces. */
#if 0
static void discard_response(GtkDialog *dialog, int response, gpointer data) {
    UmiFormRecordRoot *self = data;
    if (response == GTK_RESPONSE_ACCEPT && !self->retired) (void)UmiFormRecordEditorGtk4Discard(GTK_WIDGET(self));
    gtk_window_destroy(GTK_WINDOW(dialog));
}
static void discard_clicked(GtkButton *button, gpointer data) {
    (void)button; UmiFormRecordRoot *self = data;
    if (self->retired || self->busy || self->working) return;
    GtkRoot *parent = gtk_widget_get_root(GTK_WIDGET(self));
    if (parent == NULL || !GTK_IS_WINDOW(parent)) return;
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "%s", "Discard unsaved answers?");
    gtk_dialog_add_buttons(GTK_DIALOG(dialog), "Keep editing", GTK_RESPONSE_CANCEL, "Discard", GTK_RESPONSE_ACCEPT, NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    g_signal_connect_object(dialog, "response", G_CALLBACK(discard_response), self, 0);
    gtk_window_present(GTK_WINDOW(dialog));
}
#endif
/* The modal decision carries the selected session generation. A stale dialog
 * cannot discard a record opened elsewhere while the prompt was visible. */
typedef struct FormDiscardPrompt { GWeakRef editor; uint64_t generation; } FormDiscardPrompt;
static void prompt_free(gpointer data) {
    FormDiscardPrompt *prompt = data; g_weak_ref_clear(&prompt->editor); g_free(prompt);
}
static void prompt_cancel(GtkButton *button, gpointer data) {
    (void)button; gtk_window_destroy(GTK_WINDOW(data));
}
static void prompt_accept(GtkButton *button, gpointer data) {
    (void)button;
    GtkWindow *dialog = GTK_WINDOW(data);
    FormDiscardPrompt *prompt = g_object_get_data(G_OBJECT(dialog), "form-record-discard-context");
    UmiFormRecordRoot *self = g_weak_ref_get(&prompt->editor);
    if (self != NULL) {
        UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
        UmiStatus status = view == NULL ? UMI_STATUS_OUT_OF_MEMORY :
            umi_structured_form_session_snapshot(self->session, view);
        if (status == UMI_STATUS_OK && view->generation != prompt->generation) status = UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK && !self->retired) status = UmiFormRecordEditorGtk4Discard(GTK_WIDGET(self));
        else if (!self->retired) show_status(self, status, "");
        g_free(view); g_object_unref(self);
    }
    gtk_window_destroy(dialog);
}
static void discard_clicked(GtkButton *button, gpointer data) {
    (void)button; UmiFormRecordRoot *self = data;
    if (self->retired || self->busy || self->working) return;
    GtkRoot *parent = gtk_widget_get_root(GTK_WIDGET(self));
    if (parent == NULL || !GTK_IS_WINDOW(parent)) return;
    UmiStructuredFormSessionView *view = g_try_new0(UmiStructuredFormSessionView, 1);
    FormDiscardPrompt *prompt = g_try_new0(FormDiscardPrompt, 1);
    if (view == NULL || prompt == NULL) { g_free(view); g_free(prompt); return; }
    if (umi_structured_form_session_snapshot(self->session, view) != UMI_STATUS_OK || !view->record.ready) {
        g_free(view); g_free(prompt); return;
    }
    prompt->generation = view->generation; g_free(view);
    g_weak_ref_init(&prompt->editor, self);
    GtkWidget *dialog = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(dialog), "Discard unsaved answers?");
    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(parent));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    g_object_set_data_full(G_OBJECT(dialog), "form-record-discard-context", prompt, prompt_free);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_start(box, 20); gtk_widget_set_margin_end(box, 20);
    gtk_widget_set_margin_top(box, 20); gtk_widget_set_margin_bottom(box, 20);
    gtk_box_append(GTK_BOX(box), gtk_label_new("Discard unsaved answers for the selected record?"));
    GtkWidget *cancel = gtk_button_new_with_label("Keep editing");
    GtkWidget *accept = gtk_button_new_with_label("Discard");
    gtk_widget_set_name(cancel, "form-record-discard-cancel");
    gtk_widget_set_name(accept, "form-record-discard-confirm");
    gtk_box_append(GTK_BOX(box), cancel); gtk_box_append(GTK_BOX(box), accept);
    gtk_window_set_child(GTK_WINDOW(dialog), box);
    gtk_window_set_default_widget(GTK_WINDOW(dialog), cancel);
    g_signal_connect_object(cancel, "clicked", G_CALLBACK(prompt_cancel), dialog, 0);
    g_signal_connect_object(accept, "clicked", G_CALLBACK(prompt_accept), dialog, 0);
    gtk_window_present(GTK_WINDOW(dialog));
}
static void keep_clicked(GtkButton *button, gpointer data) {
    (void)button; UmiFormRecordRoot *self = data;
    if (!self->retired) (void)UmiFormRecordEditorGtk4KeepRecovery(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->recovery)));
}
static void recover_clicked(GtkButton *button, gpointer data) {
    (void)button; UmiFormRecordRoot *self = data;
    if (!self->retired) (void)UmiFormRecordEditorGtk4Recover(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->recovery)));
}
static GtkWidget *entry(GtkWidget *parent, const char *name, const char *placeholder) {
    GtkWidget *result = gtk_entry_new(); gtk_widget_set_name(result, name);
    gtk_entry_set_placeholder_text(GTK_ENTRY(result), placeholder); gtk_box_append(GTK_BOX(parent), result); return result;
}
static GtkWidget *button(GtkWidget *parent, UmiFormRecordRoot *self, const char *name,
    const char *label, GCallback callback) {
    GtkWidget *result = gtk_button_new_with_label(label); gtk_widget_set_name(result, name);
    gtk_box_append(GTK_BOX(parent), result); g_signal_connect_object(result, "clicked", callback, self, 0); return result;
}
UmiStatus UmiFormRecordEditorGtk4New(const UmiFormRecordEditorGtk4Options *options, GtkWidget **out) {
    if (options == NULL || out == NULL || options->entity_prefix == NULL || options->source == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t prefix = strlen(options->entity_prefix), source = strlen(options->source);
    if (prefix == 0 || prefix > 48 || options->entity_prefix[prefix - 1] != '.' || source == 0 || source >= 160 ||
        !g_utf8_validate(options->source, -1, NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0; i < prefix; ++i)
        if (!ascii_key_char(options->entity_prefix[i]) && options->entity_prefix[i] != '.') return UMI_STATUS_INVALID_ARGUMENT;
    UmiStructuredFormSession *session = NULL;
    UmiStatus status = umi_structured_form_session_create(options->schema, &session);
    if (status != UMI_STATUS_OK) return status;
    /* Validate native rendering now, before accepting a schema that cannot be
     * displayed. No answers are kept or silently selected by this probe. */
    GtkWidget *probe = UmiStructuredFormGtk4New(options->schema, NULL);
    if (probe == NULL) { umi_structured_form_session_destroy(session); return UMI_STATUS_INVALID_ARGUMENT; }
    g_object_ref_sink(probe); g_object_unref(probe);
    UmiFormRecordRoot *self = g_object_new(umi_form_record_root_get_type(), NULL);
    self->schema = options->schema; self->session = session;
    self->key_validator = options->key_validator;
    memcpy(self->prefix, options->entity_prefix, prefix + 1); memcpy(self->source, options->source, source + 1);
    gtk_box_append(GTK_BOX(self), gtk_label_new(options->schema->base->title));
    self->path = entry(GTK_WIDGET(self), "form-record-path", "Absolute path to a local SQLite database");
    (void)button(GTK_WIDGET(self), self, "form-record-connect", "Open or create database", G_CALLBACK(connect_clicked));
    self->key = entry(GTK_WIDGET(self), "form-record-key", "Record key: letters, digits, underscore or hyphen");
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(self), actions);
    (void)button(actions, self, "form-record-new", "New record", G_CALLBACK(begin_clicked));
    (void)button(actions, self, "form-record-open", "Open record", G_CALLBACK(open_clicked));
    self->body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); gtk_box_append(GTK_BOX(self), self->body);
    self->actor = entry(GTK_WIDGET(self), "form-record-actor", "Attribution label (not authentication)");
    self->reason = entry(GTK_WIDGET(self), "form-record-reason", "Reason for this revision");
    GtkWidget *saves = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(self), saves);
    (void)button(saves, self, "form-record-save-draft", "Save draft", G_CALLBACK(save_clicked));
    GtkWidget *submit = button(saves, self, "form-record-save-complete", "Check required answers and save", G_CALLBACK(save_clicked));
    g_object_set_data(G_OBJECT(submit), "submit", GINT_TO_POINTER(1));
    (void)button(saves, self, "form-record-discard", "Discard unsaved answers...", G_CALLBACK(discard_clicked));
    self->recovery = entry(GTK_WIDGET(self), "form-record-recovery-path", "Absolute path for a private recovery file");
    (void)button(GTK_WIDGET(self), self, "form-record-keep-recovery", "Keep a new recovery copy", G_CALLBACK(keep_clicked));
    (void)button(GTK_WIDGET(self), self, "form-record-recover", "Recover a checked draft", G_CALLBACK(recover_clicked));
    self->feedback = gtk_label_new("Choose a local database. Stored records are not encrypted.");
    gtk_widget_set_name(self->feedback, "form-record-feedback"); gtk_label_set_wrap(GTK_LABEL(self->feedback), TRUE);
    gtk_box_append(GTK_BOX(self), self->feedback);
    *out = GTK_WIDGET(self); return UMI_STATUS_OK;
}
