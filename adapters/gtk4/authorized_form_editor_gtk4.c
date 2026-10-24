/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reuse typed controls and an owned authorization host; never run storage on GTK. */
#include "umicom/ui/gtk4/authorized_form_editor.h"
#include "umicom/ui/gtk4/structured_form.h"
#include <string.h>
typedef struct UmiAuthorizedFormEditorRoot {
  GtkBox parent;
  UmiRecordHost *host;
  const UmiStructuredFormSchema *schema;
  UmiRecordAccessRequest binding;
  UmiStructuredForm *baseline;
  UmiRecordRevision record;
  GtkWidget *body, *form; /* Strong references survive explicit root disposal. */
  GtkWidget *entity_entry, *reason_entry, *feedback;
  char entity[64];
  uint64_t generation;
  bool ready, has_baseline, busy, working, retired;
  UmiStatus last;
} UmiAuthorizedFormEditorRoot;
typedef struct UmiAuthorizedFormEditorRootClass {
  GtkBoxClass parent;
} UmiAuthorizedFormEditorRootClass;
GType umi_authorized_form_editor_root_get_type(void) G_GNUC_CONST;
G_DEFINE_TYPE(UmiAuthorizedFormEditorRoot, umi_authorized_form_editor_root, GTK_TYPE_BOX)
typedef struct AuthorizedFormJob {
  bool save, was_ready;
  UmiRecordHost *host; /* The GTask source retains the widget's host reference. */
  const UmiStructuredFormSchema *schema;
  UmiRecordAccessRequest request;
  uint64_t generation, native_revision, expected_revision, observed;
  char reason[256];
  UmiStructuredForm form;
  UmiAuthorizedFormRecord loaded;
  UmiAuthorizedRecordRevision saved;
  UmiStatus status;
} AuthorizedFormJob;
static UmiAuthorizedFormEditorRoot *root_from(GtkWidget *widget) {
  return widget && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_authorized_form_editor_root_get_type())
             ? (UmiAuthorizedFormEditorRoot *)widget
             : NULL;
}
static void retire(UmiAuthorizedFormEditorRoot *self) {
  self->retired = true;
  if (self->form)
    UmiStructuredFormGtk4Retire(self->form);
}
static void unroot(GtkWidget *widget) {
  retire((UmiAuthorizedFormEditorRoot *)widget);
  GTK_WIDGET_CLASS(umi_authorized_form_editor_root_parent_class)->unroot(widget);
}
static void dispose(GObject *object) {
  UmiAuthorizedFormEditorRoot *self = (UmiAuthorizedFormEditorRoot *)object;
  retire(self);
  self->entity_entry = self->reason_entry = self->feedback = NULL;
  G_OBJECT_CLASS(umi_authorized_form_editor_root_parent_class)->dispose(object);
}
static void finalize(GObject *object) {
  UmiAuthorizedFormEditorRoot *self = (UmiAuthorizedFormEditorRoot *)object;
  g_clear_object(&self->form);
  g_clear_object(&self->body);
  g_free(self->baseline);
  umi_record_host_release(self->host);
  G_OBJECT_CLASS(umi_authorized_form_editor_root_parent_class)->finalize(object);
}
static void umi_authorized_form_editor_root_class_init(UmiAuthorizedFormEditorRootClass *klass) {
  GTK_WIDGET_CLASS(klass)->unroot = unroot;
  G_OBJECT_CLASS(klass)->dispose = dispose;
  G_OBJECT_CLASS(klass)->finalize = finalize;
}
static void umi_authorized_form_editor_root_init(UmiAuthorizedFormEditorRoot *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 8);
}
static UmiStatus copy_text(char *out, size_t capacity, const char *text, bool identifier) {
  if (!text)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t n = 0;
  while (n < capacity && text[n]) {
    unsigned char c = (unsigned char)text[n];
    if (c < 32 || c == 127)
      return UMI_STATUS_INVALID_ARGUMENT;
    if (identifier && !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                        (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '-'))
      return UMI_STATUS_INVALID_ARGUMENT;
    ++n;
  }
  if (!n || n == capacity)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!g_utf8_validate(text, (gssize)n, NULL))
    return UMI_STATUS_PARSE_ERROR;
  memcpy(out, text, n + 1);
  return UMI_STATUS_OK;
}
static bool can_close(UmiAuthorizedFormEditorRoot *self) {
  return !self->retired && !self->busy && !self->working &&
         (!self->ready ||
          (self->has_baseline && self->form && UmiStructuredFormGtk4CanClose(self->form)));
}
static bool replaceable(UmiAuthorizedFormEditorRoot *self) {
  return !self->ready ||
         (self->has_baseline && self->form && UmiStructuredFormGtk4CanClose(self->form));
}
static UmiStatus enter(UmiAuthorizedFormEditorRoot *self) {
  if (!self)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired)
    return UMI_STATUS_INVALID_STATE;
  if (self->busy || self->working)
    return UMI_STATUS_BUSY;
  g_object_ref(self);
  self->working = true;
  return UMI_STATUS_OK;
}
static void feedback(UmiAuthorizedFormEditorRoot *self, UmiStatus status, const char *message) {
  self->last = status;
  if (self->retired || !self->feedback)
    return;
  GtkWidget *label = g_object_ref(self->feedback);
  gtk_label_set_text(GTK_LABEL(label), status == UMI_STATUS_OK ? message : umi_status_text(status));
  g_object_unref(label);
}
static UmiStatus leave(UmiAuthorizedFormEditorRoot *self, UmiStatus status, const char *message) {
  feedback(self, status, message);
  self->working = false;
  g_object_unref(self);
  return status;
}
static UmiStatus make_body(UmiAuthorizedFormEditorRoot *self, const char *entity,
                           const UmiStructuredForm *form, GtkWidget **body_out,
                           GtkWidget **form_out) {
  GtkWidget *editor = UmiStructuredFormGtk4New(self->schema, form);
  if (!editor)
    return UMI_STATUS_INVALID_ARGUMENT;
  g_object_ref_sink(editor);
  gtk_widget_set_name(editor, "authorized-form-values");
  GtkWidget *body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  g_object_ref_sink(body);
  GtkWidget *label = gtk_label_new(entity);
  gtk_label_set_xalign(GTK_LABEL(label), 0);
  gtk_widget_set_name(label, "authorized-form-current-record");
  gtk_box_append(GTK_BOX(body), label);
  gtk_box_append(GTK_BOX(body), editor);
  *body_out = body;
  *form_out = editor;
  return UMI_STATUS_OK;
}
static UmiStatus install_body(UmiAuthorizedFormEditorRoot *self, GtkWidget *body, GtkWidget *form) {
  GtkWidget *old_body = self->body, *old_form = self->form;
  /* Publish new pointers before GTK notifications. Old retained controls are
   * retired explicitly even when they were never rooted in a native window. */
  if (old_form)
    UmiStructuredFormGtk4Retire(old_form);
  self->body = body ? g_object_ref(body) : NULL;
  self->form = form ? g_object_ref(form) : NULL;
  if (body)
    gtk_box_append(GTK_BOX(self), body);
  if (!self->retired && old_body && gtk_widget_get_parent(old_body) == GTK_WIDGET(self))
    gtk_box_remove(GTK_BOX(self), old_body);
  if (old_form)
    g_object_unref(old_form);
  if (old_body)
    g_object_unref(old_body);
  return self->retired ? UMI_STATUS_CANCELLED : UMI_STATUS_OK;
}
static void worker(GTask *task, gpointer source, gpointer data, GCancellable *cancellable) {
  (void)source;
  (void)cancellable;
  AuthorizedFormJob *job = data;
  if (job->save)
    job->status = umi_record_host_form_save_current(job->host, &job->request, job->schema,
                                                    &job->form, job->reason, job->observed,
                                                    job->expected_revision, &job->saved);
  else
    job->status =
        umi_record_host_form_load_current(job->host, &job->request, 0, job->schema, &job->loaded);
  g_task_return_boolean(task, TRUE);
}
static void completed(GObject *source, GAsyncResult *result, gpointer data) {
  (void)data;
  UmiAuthorizedFormEditorRoot *self = (UmiAuthorizedFormEditorRoot *)source;
  GTask *task = G_TASK(result);
  AuthorizedFormJob *job = g_task_get_task_data(task);
  (void)g_task_propagate_boolean(task, NULL);
  self->working = true;
  UmiStatus status = job->status;
  const char *message = "Record opened.";
  if (job->save) {
    if (status == UMI_STATUS_OK) {
      /* An actual committed result is recorded even after retirement.
       * Later visible edits never become part of this saved baseline. */
      *self->baseline = job->form;
      self->baseline->saved_revision = job->form.revision;
      self->record = job->saved.record;
      self->has_baseline = true;
      if (!self->retired && self->form &&
          UmiStructuredFormGtk4MatchesRevision(self->form, job->form.revision))
        (void)UmiStructuredFormGtk4MarkSaved(self->form, job->form.revision);
      message = "Captured answers saved. Later edits remain local.";
    } else
      message = "Save refused; local answers retained.";
  } else if (status == UMI_STATUS_OK) {
    if (self->retired)
      status = UMI_STATUS_CANCELLED;
    else if (self->generation != job->generation || self->ready != job->was_ready ||
             (job->was_ready && (!self->form || !UmiStructuredFormGtk4MatchesRevision(
                                                    self->form, job->native_revision))))
      status = UMI_STATUS_INVALID_STATE;
    else {
      GtkWidget *body = NULL, *form = NULL;
      status = make_body(self, job->request.entity_id, &job->loaded.form, &body, &form);
      if (status == UMI_STATUS_OK) {
        *self->baseline = job->loaded.form;
        self->record = job->loaded.record;
        self->ready = self->has_baseline = true;
        ++self->generation;
        strcpy(self->entity, job->request.entity_id);
        status = install_body(self, body, form);
      }
      if (form)
        g_object_unref(form);
      if (body)
        g_object_unref(body);
    }
  }
  self->busy = false;
  feedback(self, status, message);
  self->working = false;
}
static void dispatch(UmiAuthorizedFormEditorRoot *self, AuthorizedFormJob *job) {
  job->host = self->host;
  job->schema = self->schema;
  self->busy = true;
  GTask *task = g_task_new(self, NULL, completed, NULL);
  g_task_set_task_data(task, job, g_free);
  g_task_set_check_cancellable(task, FALSE);
  g_task_run_in_thread(task, worker);
  g_object_unref(task);
}
static void begin_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiAuthorizedFormEditorRoot *self = data;
  if (!self->retired && self->entity_entry)
    (void)UmiAuthorizedFormEditorGtk4Begin(GTK_WIDGET(self),
                                           gtk_editable_get_text(GTK_EDITABLE(self->entity_entry)));
}
static void open_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiAuthorizedFormEditorRoot *self = data;
  if (!self->retired && self->entity_entry)
    (void)UmiAuthorizedFormEditorGtk4Open(GTK_WIDGET(self),
                                          gtk_editable_get_text(GTK_EDITABLE(self->entity_entry)));
}
static void save_clicked(GtkButton *button, gpointer data) {
  (void)button;
  UmiAuthorizedFormEditorRoot *self = data;
  if (!self->retired && self->reason_entry)
    (void)UmiAuthorizedFormEditorGtk4Save(
        GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->reason_entry)), 0, false);
}
static void discard_clicked(GtkButton *button, gpointer data) {
  (void)button;
  (void)UmiAuthorizedFormEditorGtk4Discard(GTK_WIDGET(data));
}
UmiStatus UmiAuthorizedFormEditorGtk4New(const UmiAuthorizedFormEditorGtk4Options *options,
                                         GtkWidget **out) {
  if (!options || !options->host || !options->schema || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiRecordAccessRequest binding = {
      .entity_id = "probe.record", .operation = UMI_RECORD_ACCESS_READ_HEAD, .now_nanoseconds = 1};
  UmiStatus status =
      copy_text(binding.session_id, sizeof binding.session_id, options->session_id, false);
  if (status == UMI_STATUS_OK)
    status = copy_text(binding.purpose, sizeof binding.purpose, options->purpose, true);
  if (status == UMI_STATUS_OK)
    status = umi_record_access_request_validate(&binding);
  if (status != UMI_STATUS_OK)
    return status;
  GtkWidget *probe = UmiStructuredFormGtk4New(options->schema, NULL);
  if (!probe)
    return UMI_STATUS_INVALID_ARGUMENT;
  g_object_ref_sink(probe);
  g_object_unref(probe);
  UmiStructuredForm *baseline = g_try_new0(UmiStructuredForm, 1);
  if (!baseline)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_record_host_retain(options->host);
  if (status != UMI_STATUS_OK) {
    g_free(baseline);
    return status;
  }
  UmiAuthorizedFormEditorRoot *self =
      g_object_new(umi_authorized_form_editor_root_get_type(), NULL);
  self->host = options->host;
  self->schema = options->schema;
  self->binding = binding;
  self->baseline = baseline;
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  self->entity_entry = gtk_entry_new();
  gtk_widget_set_name(self->entity_entry, "authorized-form-record-id");
  gtk_entry_set_placeholder_text(GTK_ENTRY(self->entity_entry), "Record identifier");
  gtk_widget_set_hexpand(self->entity_entry, TRUE);
  gtk_box_append(GTK_BOX(row), self->entity_entry);
  GtkWidget *begin = gtk_button_new_with_label("New draft"),
            *open = gtk_button_new_with_label("Open record");
  gtk_widget_set_name(begin, "authorized-form-new");
  gtk_widget_set_name(open, "authorized-form-open");
  gtk_box_append(GTK_BOX(row), begin);
  gtk_box_append(GTK_BOX(row), open);
  gtk_box_append(GTK_BOX(self), row);
  self->reason_entry = gtk_entry_new();
  gtk_widget_set_name(self->reason_entry, "authorized-form-reason");
  gtk_entry_set_placeholder_text(GTK_ENTRY(self->reason_entry), "Reason for saving");
  gtk_box_append(GTK_BOX(self), self->reason_entry);
  row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  GtkWidget *save = gtk_button_new_with_label("Save draft"),
            *discard = gtk_button_new_with_label("Discard local edits");
  gtk_widget_set_name(save, "authorized-form-save");
  gtk_widget_set_name(discard, "authorized-form-discard");
  gtk_box_append(GTK_BOX(row), save);
  gtk_box_append(GTK_BOX(row), discard);
  gtk_box_append(GTK_BOX(self), row);
  self->feedback =
      gtk_label_new("Open a record or begin a draft. Saving checks current permissions.");
  gtk_label_set_wrap(GTK_LABEL(self->feedback), TRUE);
  gtk_widget_set_name(self->feedback, "authorized-form-feedback");
  gtk_box_append(GTK_BOX(self), self->feedback);
  g_signal_connect_object(begin, "clicked", G_CALLBACK(begin_clicked), self, 0);
  g_signal_connect_object(open, "clicked", G_CALLBACK(open_clicked), self, 0);
  g_signal_connect_object(save, "clicked", G_CALLBACK(save_clicked), self, 0);
  g_signal_connect_object(discard, "clicked", G_CALLBACK(discard_clicked), self, 0);
  *out = GTK_WIDGET(self);
  return UMI_STATUS_OK;
}
UmiStatus UmiAuthorizedFormEditorGtk4Begin(GtkWidget *widget, const char *entity_id) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  char entity[64];
  status = copy_text(entity, sizeof entity, entity_id, true);
  if (status == UMI_STATUS_OK && !replaceable(self))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK && self->generation == UINT64_MAX)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStructuredForm *initial = g_try_new0(UmiStructuredForm, 1);
  GtkWidget *body = NULL, *form = NULL;
  if (status == UMI_STATUS_OK && !initial)
    status = UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_init(self->schema, initial);
  if (status == UMI_STATUS_OK) {
    initial->saved_revision = 0;
    status = make_body(self, entity, initial, &body, &form);
  }
  if (status == UMI_STATUS_OK) {
    self->ready = true;
    self->has_baseline = false;
    ++self->generation;
    memset(&self->record, 0, sizeof self->record);
    strcpy(self->entity, entity);
    status = install_body(self, body, form);
  }
  if (form)
    g_object_unref(form);
  if (body)
    g_object_unref(body);
  g_free(initial);
  return leave(self, status, "New unsaved draft.");
}
UmiStatus UmiAuthorizedFormEditorGtk4Open(GtkWidget *widget, const char *entity_id) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  AuthorizedFormJob *job = g_try_new0(AuthorizedFormJob, 1);
  if (!job)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  job->request = self->binding;
  status = copy_text(job->request.entity_id, sizeof job->request.entity_id, entity_id, true);
  if (status == UMI_STATUS_OK && !replaceable(self))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK && self->generation == UINT64_MAX)
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  job->was_ready = self->ready;
  job->generation = self->generation;
  if (status == UMI_STATUS_OK && self->ready) {
    status = UmiStructuredFormGtk4Snapshot(self->form, &job->form);
    job->native_revision = job->form.revision;
  }
  if (status == UMI_STATUS_OK)
    dispatch(self, job);
  else
    g_free(job);
  return leave(self, status, "Opening record with current permissions.");
}
UmiStatus UmiAuthorizedFormEditorGtk4Save(GtkWidget *widget, const char *reason,
                                          uint64_t observed_at_nanoseconds, bool submitting) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (!self->ready || !self->form)
    return leave(self, UMI_STATUS_INVALID_STATE, "");
  AuthorizedFormJob *job = g_try_new0(AuthorizedFormJob, 1);
  if (!job)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  status = copy_text(job->reason, sizeof job->reason, reason, false);
  if (status == UMI_STATUS_OK)
    status = UmiStructuredFormGtk4Apply(self->form, submitting ? 1 : 0);
  if (status == UMI_STATUS_OK && self->retired)
    status = UMI_STATUS_CANCELLED;
  if (status == UMI_STATUS_OK)
    status = UmiStructuredFormGtk4Snapshot(self->form, &job->form);
  /* A clean saved form needs no duplicate amendment. New drafts still save
   * because they have no durable baseline, including unanswered drafts. */
  if (status == UMI_STATUS_OK && self->has_baseline &&
      job->form.revision == job->form.saved_revision)
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK) {
    job->save = true;
    job->request = self->binding;
    strcpy(job->request.entity_id, self->entity);
    job->expected_revision = self->has_baseline ? self->record.revision : 0;
    job->request.operation =
        job->expected_revision ? UMI_RECORD_ACCESS_AMEND : UMI_RECORD_ACCESS_CREATE;
    job->generation = self->generation;
    job->observed = observed_at_nanoseconds;
    dispatch(self, job);
  } else
    g_free(job);
  return leave(self, status, "Saving captured answers with current permissions.");
}
UmiStatus UmiAuthorizedFormEditorGtk4Discard(GtkWidget *widget) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  if (!self->ready)
    return leave(self, UMI_STATUS_INVALID_STATE, "");
  if (self->generation == UINT64_MAX)
    return leave(self, UMI_STATUS_CAPACITY_EXCEEDED, "");
  GtkWidget *body = NULL, *form = NULL;
  if (self->has_baseline)
    status = make_body(self, self->entity, self->baseline, &body, &form);
  if (status == UMI_STATUS_OK) {
    ++self->generation;
    if (!self->has_baseline) {
      self->ready = false;
      self->entity[0] = 0;
      memset(&self->record, 0, sizeof self->record);
    }
    status = install_body(self, body, form);
  }
  if (form)
    g_object_unref(form);
  if (body)
    g_object_unref(body);
  return leave(self, status, "Local edits discarded.");
}
UmiStatus UmiAuthorizedFormEditorGtk4Snapshot(GtkWidget *widget, UmiStructuredForm *out) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  if (!self || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->retired || !self->ready || !self->form)
    return UMI_STATUS_INVALID_STATE;
  if (self->working)
    return UMI_STATUS_BUSY;
  return UmiStructuredFormGtk4Snapshot(self->form, out);
}
/* The ReadState name avoids colliding with the public state typedef in C.
 * Retain the original implementation for review; behavior is unchanged. */
#if 0
UmiStatus UmiAuthorizedFormEditorGtk4State(GtkWidget *widget,
                                           UmiAuthorizedFormEditorGtk4State *out) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  if (!self || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->working)
    return UMI_STATUS_BUSY;
  UmiAuthorizedFormEditorGtk4State state = {.ready = self->ready,
                                            .has_baseline = self->has_baseline,
                                            .busy = self->busy,
                                            .retired = self->retired,
                                            .can_close = can_close(self),
                                            .generation = self->generation,
                                            .record_revision =
                                                self->has_baseline ? self->record.revision : 0,
                                            .last_status = self->last};
  strcpy(state.entity_id, self->entity);
  *out = state;
  return UMI_STATUS_OK;
}
#endif
UmiStatus UmiAuthorizedFormEditorGtk4ReadState(GtkWidget *widget,
                                           UmiAuthorizedFormEditorGtk4State *out) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  if (!self || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->working)
    return UMI_STATUS_BUSY;
  UmiAuthorizedFormEditorGtk4State state = {.ready = self->ready,
                                            .has_baseline = self->has_baseline,
                                            .busy = self->busy,
                                            .retired = self->retired,
                                            .can_close = can_close(self),
                                            .generation = self->generation,
                                            .record_revision =
                                                self->has_baseline ? self->record.revision : 0,
                                            .last_status = self->last};
  strcpy(state.entity_id, self->entity);
  *out = state;
  return UMI_STATUS_OK;
}
bool UmiAuthorizedFormEditorGtk4CanClose(GtkWidget *widget) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  return self && can_close(self);
}
bool UmiAuthorizedFormEditorGtk4Busy(GtkWidget *widget) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  return self && (self->busy || self->working);
}
void UmiAuthorizedFormEditorGtk4Retire(GtkWidget *widget) {
  UmiAuthorizedFormEditorRoot *self = root_from(widget);
  if (self)
    retire(self);
}
