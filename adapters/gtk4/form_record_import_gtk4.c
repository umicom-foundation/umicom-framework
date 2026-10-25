/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/form_record_import_gtk4.c
 *
 * PURPOSE:
 *   Workers own immutable requests. GTK owns intent and confirmation, while the
 *   common data services own framing, schema checks and atomic record restore.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Workers own immutable requests. GTK owns intent and confirmation, while the
 * common data services own framing, schema checks and atomic record restore. */
#include "umicom/base/text.h"
#include "umicom/data/record_bundle_files.h"
#include "umicom/platform/output_file.h"
#include "umicom/ui/gtk4/form_record_import.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/ui/gtk4/record_transfer_receipt.h"
#include <string.h>
typedef struct UmiFormImportRoot {
  GtkBox parent;
  UmiFormRecordImportGtk4Options options;
  char form_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY], prefix[64];
  UmiFormRecordImportGtk4State state;
  UmiStructuredFormBundleReview *review;
  GtkWidget *file, *database, *preview_box, *preview, *feedback;
  GtkWidget *receipt_box;
  char receipt_source[160];
  GtkWidget *receipt_id, *receipt_actor, *receipt_reason;
  uint64_t generation;
  bool working;
} UmiFormImportRoot;
typedef struct UmiFormImportRootClass {
  GtkBoxClass parent_class;
} UmiFormImportRootClass;
GType umi_form_import_root_get_type(void);
G_DEFINE_TYPE(UmiFormImportRoot, umi_form_import_root, GTK_TYPE_BOX)
static guint finished_signal;
typedef struct ImportJob {
  UmiFormRecordImportOperation operation;
  UmiStatus status;
  uint64_t generation;
  char path[UMI_PATH_CAPACITY], form_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY], prefix[64],
      digest[65];
  const UmiStructuredFormCatalogue *catalogue;
  UmiStructuredFormBundleKeyCheck key_check;
  void *key_context;
  UmiStructuredFormBundleReview *review;
  UmiRecordRevision restored;
  bool receipted;
  UmiRecordTransferRequest receipt_request;
  UmiRecordTransferReceipt receipt;
} ImportJob;
static UmiFormImportRoot *instance(GtkWidget *widget) {
  return widget && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_form_import_root_get_type())
             ? (UmiFormImportRoot *)widget
             : NULL;
}
static void retire(UmiFormImportRoot *self) { self->state.retired = true; }
static void import_unroot(GtkWidget *widget) {
  retire((UmiFormImportRoot *)widget);
  GTK_WIDGET_CLASS(umi_form_import_root_parent_class)->unroot(widget);
}
static void import_dispose(GObject *object) {
  UmiFormImportRoot *self = (UmiFormImportRoot *)object;
  retire(self);
  self->preview = NULL;
  /* Accepted jobs retain the root, but disposed children cannot be touched. */
  G_OBJECT_CLASS(umi_form_import_root_parent_class)->dispose(object);
}
static void import_finalize(GObject *object) {
  umi_structured_form_bundle_review_destroy(((UmiFormImportRoot *)object)->review);
  G_OBJECT_CLASS(umi_form_import_root_parent_class)->finalize(object);
}
static void umi_form_import_root_class_init(UmiFormImportRootClass *klass) {
  GTK_WIDGET_CLASS(klass)->unroot = import_unroot;
  G_OBJECT_CLASS(klass)->dispose = import_dispose;
  G_OBJECT_CLASS(klass)->finalize = import_finalize;
  finished_signal = g_signal_new("operation-finished", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
                                 0, NULL, NULL, NULL, G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);
}
static void umi_form_import_root_init(UmiFormImportRoot *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 10);
  self->generation = 1;
}
static void feedback(UmiFormImportRoot *self, const char *message) {
  if (self->state.retired)
    return;
  GtkWidget *label = g_object_ref(self->feedback);
  gtk_label_set_text(GTK_LABEL(label), message);
  g_object_unref(label);
}
static UmiStatus enter(UmiFormImportRoot *self) {
  if (!self)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->state.retired)
    return UMI_STATUS_INVALID_STATE;
  if (self->working || self->state.busy)
    return UMI_STATUS_BUSY;
  g_object_ref(self);
  self->working = true;
  return UMI_STATUS_OK;
}
static UmiStatus leave(UmiFormImportRoot *self, UmiStatus status, const char *message) {
  self->state.status = status;
  feedback(self, status == UMI_STATUS_OK ? message : umi_status_text(status));
  self->working = false;
  g_object_unref(self);
  return status;
}
static UmiStatus copy_path(const char *path, char *out) {
  if (!path || !g_utf8_validate(path, -1, NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t length = strlen(path);
  if (length >= UMI_PATH_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStatus status = UmiOutputFileValidatePath(path);
  if (status == UMI_STATUS_OK)
    memcpy(out, path, length + 1);
  return status;
}
static void changed(GtkEditable *entry, gpointer context) {
  (void)entry;
  UmiFormImportRoot *self = context;
  if (self->state.retired)
    return;
  g_object_ref(self);
  if (self->generation == UINT64_MAX)
    retire(self);
  else
    ++self->generation;
  self->state.has_review = false;
  memset(&self->state.reviewed, 0, sizeof(self->state.reviewed));
  umi_structured_form_bundle_review_destroy(self->review);
  self->review = NULL;
  if (!self->state.retired && self->preview)
    UmiFormBundleReviewGtk4Retire(self->preview);
  /* Attribution edits invalidate the same review as destination edits. Keep
   * the earlier path-only wording for review of the expanded input contract. */
#if 0
  feedback(self, self->state.busy
                     ? "Inputs changed. An admitted restore still uses its captured destination; "
                       "an older read result will be discarded."
                     : "Path changed. Review the selected file again before importing.");
#endif
  feedback(self, self->state.busy
                     ? "Inputs changed. An admitted restore still uses its captured destination; "
                       "an older read result will be discarded."
                     : "Inputs changed. Review the selected file again before importing.");
  g_object_unref(self);
}
static void job_free(gpointer data) {
  ImportJob *job = data;
  umi_structured_form_bundle_review_destroy(job->review);
  g_free(job);
}
static void worker(GTask *task, gpointer source, gpointer task_data, GCancellable *cancel) {
  (void)source;
  (void)cancel;
  ImportJob *job = task_data;
  if (job->operation == UMI_FORM_IMPORT_REVIEW) {
    char *directory = g_path_get_dirname(job->path), *leaf = g_path_get_basename(job->path);
    UmiRecordBundle *bundle = NULL;
    job->status = umi_record_bundle_read(directory, leaf, &bundle);
    if (job->status == UMI_STATUS_OK)
      job->status =
          umi_structured_form_bundle_review(bundle, job->catalogue, job->form_id, job->prefix,
                                            job->key_check, job->key_context, &job->review);
    umi_record_bundle_destroy(bundle);
    g_free(leaf);
    g_free(directory);
  } else {
    UmiDataServer *server = NULL;
    job->status = umi_data_server_create_sqlite(job->path, &server);
    /* Preserve the plain restore route. Receipt-enabled hosts add provenance
     * inside the same portable transaction rather than after a successful write. */
    if (job->status == UMI_STATUS_OK && job->receipted) {
      job->status = umi_structured_form_bundle_restore_receipted(
          server, job->review, job->digest, &job->receipt_request, &job->receipt);
      if (job->status == UMI_STATUS_OK)
        job->restored = job->receipt.imported_head;
    } else if (job->status == UMI_STATUS_OK)
      job->status =
          umi_structured_form_bundle_restore(server, job->review, job->digest, &job->restored);
    umi_data_server_destroy(server);
  }
  g_task_return_boolean(task, TRUE);
}
static UmiStatus accept_review(UmiFormImportRoot *self, ImportJob *job) {
  if (self->state.retired || self->generation != job->generation)
    return UMI_STATUS_CANCELLED;
  GtkWidget *candidate = NULL;
  UmiStatus status = UmiFormBundleReviewGtk4New(job->review, &candidate);
  if (status != UMI_STATUS_OK)
    return status;
  g_object_ref_sink(candidate);
  GtkWidget *box = g_object_ref(self->preview_box), *old = self->preview;
  self->preview = NULL;
  if (old)
    gtk_box_remove(GTK_BOX(box), old);
  if (self->state.retired || self->generation != job->generation)
    status = UMI_STATUS_CANCELLED;
  else {
    umi_structured_form_bundle_review_destroy(self->review);
    self->review = job->review;
    job->review = NULL;
    self->preview = candidate;
    status = umi_structured_form_bundle_summary(self->review, &self->state.reviewed);
    self->state.has_review = status == UMI_STATUS_OK;
    gtk_box_append(GTK_BOX(box), candidate);
    /* Publication can notify a host that edits an input. A changed selection
     * cannot be reported as the accepted review merely because append returned. */
#if 0
    if (self->state.retired) status = UMI_STATUS_CANCELLED;
#endif
    if (self->state.retired || self->generation != job->generation || !self->state.has_review)
      status = UMI_STATUS_CANCELLED;
  }
  g_object_unref(box);
  g_object_unref(candidate);
  return status;
}
/* Presentation can fail after a real commit. Report it independently and keep
 * the successful storage receipt available through StateRead for later review. */
static UmiStatus show_receipt(UmiFormImportRoot *self, const UmiRecordTransferReceipt *receipt) {
  if (self->state.retired)
    return UMI_STATUS_UNAVAILABLE;
  GtkWidget *view = NULL;
  UmiStatus status = UmiRecordTransferReceiptGtk4New(receipt, &view);
  if (status != UMI_STATUS_OK)
    return status;
  g_object_ref_sink(view);
  GtkWidget *box = g_object_ref(self->receipt_box), *old;
  while (!self->state.retired && (old = gtk_widget_get_first_child(box)) != NULL)
    gtk_box_remove(GTK_BOX(box), old);
  if (!self->state.retired)
    gtk_box_append(GTK_BOX(box), gtk_label_new("Last completed import receipt"));
  if (!self->state.retired)
    gtk_box_append(GTK_BOX(box), view);
  status = self->state.retired ? UMI_STATUS_UNAVAILABLE : UMI_STATUS_OK;
  g_object_unref(box);
  g_object_unref(view);
  return status;
}
static void completed(GObject *source, GAsyncResult *result, gpointer context) {
  (void)context;
  UmiFormImportRoot *self = (UmiFormImportRoot *)source;
  ImportJob *job = g_task_get_task_data(G_TASK(result));
  (void)g_task_propagate_boolean(G_TASK(result), NULL);
  self->working = true;
  UmiStatus status = job->status;
  if (job->operation == UMI_FORM_IMPORT_REVIEW && status == UMI_STATUS_OK)
    status = accept_review(self, job);
  if (job->operation == UMI_FORM_IMPORT_RESTORE && status == UMI_STATUS_OK)
    self->state.restored = job->restored;
  if (job->operation == UMI_FORM_IMPORT_RESTORE && status == UMI_STATUS_OK && job->receipted) {
    self->state.has_receipt = true;
    self->state.receipt = job->receipt;
    self->state.receipt_display_status = show_receipt(self, &job->receipt);
  }
  self->state.busy = false;
  self->state.status = status;
  feedback(self, status == UMI_STATUS_OK ? (job->operation == UMI_FORM_IMPORT_REVIEW
                                                ? "Review every original revision, then select the "
                                                  "confirmation box before importing."
                                                : "Imported the captured history into the captured "
                                                  "destination. Existing records were retained.")
                                         : umi_status_text(status));
  if (status == UMI_STATUS_OK && self->state.has_receipt &&
      self->state.receipt_display_status != UMI_STATUS_OK)
    feedback(self, "Import and receipt committed. Receipt preview is unavailable; the captured "
                   "receipt remains available to the host.");
  self->working = false;
  /* Publish state before callbacks; a handler may close this view or start a
   * new operation. Do not touch its mutable state after emitting completion. */
  g_signal_emit(self, finished_signal, 0, (int)job->operation, (int)status);
}
static void dispatch(UmiFormImportRoot *self, ImportJob *job) {
  self->state.busy = true;
  self->state.operation = job->operation;
  memcpy(self->state.operation_path, job->path, sizeof(job->path));
  memset(&self->state.restored, 0, sizeof(self->state.restored));
  self->state.has_receipt = false;
  self->state.receipt_display_status = UMI_STATUS_UNAVAILABLE;
  memset(&self->state.receipt, 0, sizeof(self->state.receipt));
  GTask *task = g_task_new(self, NULL, completed, NULL);
  g_task_set_task_data(task, job, job_free);
  g_task_set_check_cancellable(task, FALSE);
  g_task_run_in_thread(task, worker);
  g_object_unref(task);
}
UmiStatus UmiFormRecordImportGtk4Review(GtkWidget *widget, const char *path) {
  UmiFormImportRoot *self = instance(widget);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  ImportJob *job = g_try_new0(ImportJob, 1);
  if (!job)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  status = copy_path(path, job->path);
  if (status == UMI_STATUS_OK) {
    job->operation = UMI_FORM_IMPORT_REVIEW;
    job->generation = self->generation;
    job->catalogue = self->options.catalogue;
    job->key_check = self->options.key_check;
    job->key_context = self->options.key_context;
    memcpy(job->form_id, self->form_id, sizeof(job->form_id));
    memcpy(job->prefix, self->prefix, sizeof(job->prefix));
    self->state.has_review = false;
    umi_structured_form_bundle_review_destroy(self->review);
    self->review = NULL;
    if (self->preview)
      UmiFormBundleReviewGtk4Retire(self->preview);
    if (self->state.retired)
      status = UMI_STATUS_INVALID_STATE;
  }
  if (status == UMI_STATUS_OK)
    dispatch(self, job);
  else
    job_free(job);
  return leave(self, status, "Reading and checking the selected history file...");
}
/* Receipt-aware admission below keeps the original review/authorization checks
 * and copies attribution before callbacks. Retain the original route for review. */
#if 0
UmiStatus UmiFormRecordImportGtk4Restore(GtkWidget *widget, const char *path) {
  UmiFormImportRoot *self = instance(widget);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  ImportJob *job = g_try_new0(ImportJob, 1);
  if (!job)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  UmiStructuredFormBundleSummary summary;
  status = copy_path(path, job->path);
  if (status == UMI_STATUS_OK && (!self->state.has_review || !self->review || !self->preview))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = UmiFormBundleReviewGtk4Confirmed(self->preview, &summary);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_bundle_review_clone(self->review, &job->review);
  if (status == UMI_STATUS_OK) {
    job->operation = UMI_FORM_IMPORT_RESTORE;
    job->generation = self->generation;
    memcpy(job->digest, summary.bundle_sha256, sizeof(job->digest));
    status = self->options.authorize_restore(job->path, &summary, self->options.authorize_context);
    /* Retain the original input-only check for review. Authorization callbacks
     * can also withdraw the visible confirmation without changing a path. */
#if 0
    if (status == UMI_STATUS_OK && (self->state.retired || self->generation != job->generation)) status = UMI_STATUS_CANCELLED;
#endif
    if (status == UMI_STATUS_OK && (self->state.retired || self->generation != job->generation))
      status = UMI_STATUS_CANCELLED;
    UmiStructuredFormBundleSummary confirmed;
    if (status == UMI_STATUS_OK &&
        (!self->preview ||
         UmiFormBundleReviewGtk4Confirmed(self->preview, &confirmed) != UMI_STATUS_OK ||
         strcmp(confirmed.bundle_sha256, job->digest) != 0))
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK)
    dispatch(self, job);
  else
    job_free(job);
  return leave(self, status, "Importing the approved history into the captured database...");
}
#endif
static UmiStatus restore_with_request(GtkWidget *widget, const char *path,
                                      const UmiRecordTransferRequest *request) {
  UmiFormImportRoot *self = instance(widget);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  ImportJob *job = g_try_new0(ImportJob, 1);
  if (!job)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY, "");
  if (request != NULL) {
    job->receipt_request = *request;
    job->receipted = true;
  } else if (self->receipt_source[0] != '\0') {
    job->receipted = true;
    status =
        umi_text_replace(job->receipt_request.receipt_id, sizeof(job->receipt_request.receipt_id),
                         gtk_editable_get_text(GTK_EDITABLE(self->receipt_id)));
    if (status == UMI_STATUS_OK)
      status = umi_text_replace(job->receipt_request.attribution.actor,
                                sizeof(job->receipt_request.attribution.actor),
                                gtk_editable_get_text(GTK_EDITABLE(self->receipt_actor)));
    if (status == UMI_STATUS_OK)
      status = umi_text_replace(job->receipt_request.attribution.reason,
                                sizeof(job->receipt_request.attribution.reason),
                                gtk_editable_get_text(GTK_EDITABLE(self->receipt_reason)));
    memcpy(job->receipt_request.attribution.source, self->receipt_source,
           sizeof(self->receipt_source));
    gint64 now = g_get_real_time();
    if (status == UMI_STATUS_OK && (now <= 0 || (uint64_t)now > UINT64_MAX / 1000U))
      status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
      job->receipt_request.attribution.recorded_at_nanoseconds = (uint64_t)now * 1000U;
  }
  if (status == UMI_STATUS_OK && job->receipted)
    status = umi_record_transfer_request_validate(&job->receipt_request);
  if (status == UMI_STATUS_OK && job->receipted &&
      (!g_utf8_validate(job->receipt_request.attribution.source, -1, NULL) ||
       !g_utf8_validate(job->receipt_request.attribution.actor, -1, NULL) ||
       !g_utf8_validate(job->receipt_request.attribution.reason, -1, NULL)))
    status = UMI_STATUS_INVALID_ARGUMENT;
  if (status == UMI_STATUS_OK && self->receipt_source[0] &&
      memcmp(job->receipt_request.attribution.source, self->receipt_source,
             strlen(self->receipt_source) + 1) != 0)
    status = UMI_STATUS_INVALID_ARGUMENT;
  UmiStructuredFormBundleSummary summary;
  if (status == UMI_STATUS_OK)
    status = copy_path(path, job->path);
  if (status == UMI_STATUS_OK && (!self->state.has_review || !self->review || !self->preview))
    status = UMI_STATUS_INVALID_STATE;
  if (status == UMI_STATUS_OK)
    status = UmiFormBundleReviewGtk4Confirmed(self->preview, &summary);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_bundle_review_clone(self->review, &job->review);
  if (status == UMI_STATUS_OK) {
    job->operation = UMI_FORM_IMPORT_RESTORE;
    job->generation = self->generation;
    memcpy(job->digest, summary.bundle_sha256, sizeof(job->digest));
    status = self->options.authorize_restore(job->path, &summary, self->options.authorize_context);
    /* Retain the original input-only check for review. Authorization callbacks
     * can also withdraw the visible confirmation without changing a path. */
#if 0
    if (status == UMI_STATUS_OK && (self->state.retired || self->generation != job->generation)) status = UMI_STATUS_CANCELLED;
#endif
    if (status == UMI_STATUS_OK && (self->state.retired || self->generation != job->generation))
      status = UMI_STATUS_CANCELLED;
    UmiStructuredFormBundleSummary confirmed;
    if (status == UMI_STATUS_OK &&
        (!self->preview ||
         UmiFormBundleReviewGtk4Confirmed(self->preview, &confirmed) != UMI_STATUS_OK ||
         strcmp(confirmed.bundle_sha256, job->digest) != 0))
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK)
    dispatch(self, job);
  else
    job_free(job);
  return leave(self, status, "Importing the approved history into the captured database...");
}
UmiStatus UmiFormRecordImportGtk4Restore(GtkWidget *widget, const char *path) {
  return restore_with_request(widget, path, NULL);
}
UmiStatus UmiFormRecordImportGtk4RestoreWithReceipt(GtkWidget *widget, const char *path,
                                                    const UmiRecordTransferRequest *request) {
  if (!request)
    return UMI_STATUS_INVALID_ARGUMENT;
  return restore_with_request(widget, path, request);
}
UmiStatus UmiFormRecordImportGtk4StateRead(GtkWidget *widget, UmiFormRecordImportGtk4State *out) {
  UmiFormImportRoot *self = instance(widget);
  if (!self || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = self->state;
  return UMI_STATUS_OK;
}
bool UmiFormRecordImportGtk4CanClose(GtkWidget *widget) {
  UmiFormImportRoot *self = instance(widget);
  return self && !self->state.busy && !self->working;
}
void UmiFormRecordImportGtk4Retire(GtkWidget *widget) {
  UmiFormImportRoot *self = instance(widget);
  if (self)
    retire(self);
}
static void review_clicked(GtkButton *button, gpointer context) {
  (void)button;
  UmiFormImportRoot *self = context;
  if (!self->state.retired)
    (void)UmiFormRecordImportGtk4Review(GTK_WIDGET(self),
                                        gtk_editable_get_text(GTK_EDITABLE(self->file)));
}
static void restore_clicked(GtkButton *button, gpointer context) {
  (void)button;
  UmiFormImportRoot *self = context;
  if (!self->state.retired)
    (void)UmiFormRecordImportGtk4Restore(GTK_WIDGET(self),
                                         gtk_editable_get_text(GTK_EDITABLE(self->database)));
}
static GtkWidget *path_row(UmiFormImportRoot *self, const char *title, const char *name) {
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_append(GTK_BOX(self), gtk_label_new(title));
  GtkWidget *entry = gtk_entry_new();
  gtk_widget_set_name(entry, name);
  gtk_widget_set_hexpand(entry, TRUE);
  gtk_box_append(GTK_BOX(row), entry);
  GtkWidget *browse =
      UmiPathPickerGtk4Create(GTK_ENTRY(entry), UMI_PATH_PICKER_EXISTING_FILE, title, NULL);
  if (browse)
    gtk_box_append(GTK_BOX(row), browse);
  gtk_box_append(GTK_BOX(self), row);
  g_signal_connect(entry, "changed", G_CALLBACK(changed), self);
  return entry;
}
UmiStatus UmiFormRecordImportGtk4New(const UmiFormRecordImportGtk4Options *options,
                                     GtkWidget **out) {
  if (!options || !out || !options->catalogue || !options->authorize_restore || !options->form_id ||
      !options->entity_namespace)
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t form_length = strlen(options->form_id), prefix_length = strlen(options->entity_namespace);
  if (!form_length || form_length >= UMI_APPLICATION_EXPERIENCE_ID_CAPACITY || prefix_length < 2 ||
      prefix_length >= 64 || options->entity_namespace[prefix_length - 1] != '.')
    return UMI_STATUS_INVALID_ARGUMENT;
  const UmiStructuredFormSchema *schema = NULL;
  UmiStatus status =
      umi_structured_form_catalogue_latest(options->catalogue, options->form_id, &schema);
  if (status != UMI_STATUS_OK)
    return status;
  char receipt_source[160] = {0};
  if (options->receipt_source != NULL) {
    if (!options->receipt_source[0] || !g_utf8_validate(options->receipt_source, -1, NULL))
      return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_text_replace(receipt_source, sizeof(receipt_source), options->receipt_source);
    if (status != UMI_STATUS_OK)
      return status;
    UmiRecordTransferRequest check = {.receipt_id = "source-check",
                                      .attribution = {.actor = "Source validation",
                                                      .reason = "Source validation",
                                                      .recorded_at_nanoseconds = 1}};
    memcpy(check.attribution.source, receipt_source, sizeof(receipt_source));
    status = umi_record_transfer_request_validate(&check);
    if (status != UMI_STATUS_OK)
      return status;
  }
  GtkWidget *root = g_object_new(umi_form_import_root_get_type(), NULL);
  UmiFormImportRoot *self = (UmiFormImportRoot *)root;
  self->options = *options;
  memcpy(self->receipt_source, receipt_source, sizeof(receipt_source));
  self->options.receipt_source = self->receipt_source[0] ? self->receipt_source : NULL;
  memcpy(self->form_id, options->form_id, form_length + 1);
  memcpy(self->prefix, options->entity_namespace, prefix_length + 1);
  self->options.form_id = self->form_id;
  self->options.entity_namespace = self->prefix;
  self->feedback = gtk_label_new("Choose a history bundle and a destination database, then review "
                                 "the file. A new database path can also be entered explicitly.");
  gtk_label_set_wrap(GTK_LABEL(self->feedback), TRUE);
  gtk_label_set_xalign(GTK_LABEL(self->feedback), 0);
  gtk_widget_set_name(self->feedback, "form-import-feedback");
  self->file = path_row(self, "History bundle", "form-import-file");
  self->database = path_row(self, "Destination SQLite database", "form-import-database");
  if (self->receipt_source[0]) {
    GtkWidget **entries[] = {&self->receipt_id, &self->receipt_actor, &self->receipt_reason};
    const char *names[] = {"form-import-receipt-id", "form-import-receipt-actor",
                           "form-import-receipt-reason"};
    const char *labels[] = {"Unique import receipt ID",
                            "Import operator label (not authentication)", "Reason for this import"};
    for (size_t i = 0; i < 3; ++i) {
      *entries[i] = gtk_entry_new();
      gtk_widget_set_name(*entries[i], names[i]);
      gtk_entry_set_placeholder_text(GTK_ENTRY(*entries[i]), labels[i]);
      gtk_box_append(GTK_BOX(self), *entries[i]);
      g_signal_connect(*entries[i], "changed", G_CALLBACK(changed), self);
    }
    gtk_box_append(GTK_BOX(self), gtk_label_new("Fill import attribution before reviewing. "
                                                "Original authorship is retained separately."));
  }
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  GtkWidget *review = gtk_button_new_with_label("Review history file"),
            *restore = gtk_button_new_with_label("Import confirmed history");
  gtk_widget_set_name(review, "form-import-review");
  gtk_widget_set_name(restore, "form-import-restore");
  gtk_box_append(GTK_BOX(row), review);
  gtk_box_append(GTK_BOX(row), restore);
  gtk_box_append(GTK_BOX(root), row);
  gtk_box_append(GTK_BOX(root), self->feedback);
  self->preview_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_box_append(GTK_BOX(root), self->preview_box);
  self->receipt_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  gtk_widget_set_name(self->receipt_box, "form-import-receipt-view");
  gtk_box_append(GTK_BOX(root), self->receipt_box);
  g_signal_connect(review, "clicked", G_CALLBACK(review_clicked), self);
  g_signal_connect(restore, "clicked", G_CALLBACK(restore_clicked), self);
  *out = root;
  return UMI_STATUS_OK;
}
