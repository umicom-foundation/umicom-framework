/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * GTK captures intent; a worker owns its read-only connection and copied result. */
#include "umicom/platform/output_file.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/ui/gtk4/record_transfer_inspector.h"
#include <string.h>
typedef struct UmiReceiptInspector {
  GtkBox parent;
  UmiRecordTransferInspectorGtk4Options options;
  UmiRecordTransferInspectorGtk4State state;
  GtkWidget *database, *identity, *preview, *feedback;
  uint64_t generation;
  bool working;
} UmiReceiptInspector;
typedef struct UmiReceiptInspectorClass {
  GtkBoxClass parent;
} UmiReceiptInspectorClass;
GType umi_receipt_inspector_get_type(void);
G_DEFINE_TYPE(UmiReceiptInspector, umi_receipt_inspector, GTK_TYPE_BOX)
static guint finished_signal;
typedef struct ReceiptReadJob {
  char path[UMI_PATH_CAPACITY], identity[UMI_RECORD_TRANSFER_ID_CAPACITY];
  uint64_t generation;
  bool verify;
  UmiStatus status;
  UmiRecordTransferReceipt receipt;
} ReceiptReadJob;
static UmiReceiptInspector *instance(GtkWidget *widget) {
  return widget && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_receipt_inspector_get_type())
             ? (UmiReceiptInspector *)widget
             : NULL;
}
static void retire(UmiReceiptInspector *self) {
  self->state.retired = true;
  self->state.has_receipt = false;
  self->state.verified = false;
  memset(&self->state.receipt, 0, sizeof(self->state.receipt));
}
static void inspector_unroot(GtkWidget *widget) {
  retire((UmiReceiptInspector *)widget);
  GTK_WIDGET_CLASS(umi_receipt_inspector_parent_class)->unroot(widget);
}
static void inspector_dispose(GObject *object) {
  retire((UmiReceiptInspector *)object);
  G_OBJECT_CLASS(umi_receipt_inspector_parent_class)->dispose(object);
}
static void umi_receipt_inspector_class_init(UmiReceiptInspectorClass *klass) {
  GTK_WIDGET_CLASS(klass)->unroot = inspector_unroot;
  G_OBJECT_CLASS(klass)->dispose = inspector_dispose;
  finished_signal = g_signal_new("operation-finished", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
                                 0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_INT);
}
static void umi_receipt_inspector_init(UmiReceiptInspector *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 10);
  self->generation = 1;
  self->state.status = UMI_STATUS_UNAVAILABLE;
  self->state.display_status = UMI_STATUS_UNAVAILABLE;
}
static void feedback(UmiReceiptInspector *self, const char *text) {
  if (self->state.retired)
    return;
  GtkWidget *label = g_object_ref(self->feedback);
  gtk_label_set_text(GTK_LABEL(label), text);
  g_object_unref(label);
}
static void clear_view(UmiReceiptInspector *self) {
  if (self->state.retired)
    return;
  GtkWidget *box = g_object_ref(self->preview), *child;
  while (!self->state.retired && (child = gtk_widget_get_first_child(box)) != NULL)
    gtk_box_remove(GTK_BOX(box), child);
  g_object_unref(box);
}
static void invalidate(UmiReceiptInspector *self) {
  self->state.has_receipt = false;
  self->state.verified = false;
  self->state.display_status = UMI_STATUS_UNAVAILABLE;
  memset(&self->state.receipt, 0, sizeof(self->state.receipt));
  clear_view(self);
}
static void changed(GtkEditable *entry, gpointer context) {
  (void)entry;
  UmiReceiptInspector *self = context;
  if (self->state.retired)
    return;
  g_object_ref(self);
  if (self->generation == UINT64_MAX)
    retire(self);
  else
    ++self->generation;
  invalidate(self);
  feedback(self, "Inputs changed. Read the selected receipt again.");
  g_object_unref(self);
}
static UmiStatus enter(UmiReceiptInspector *self) {
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
static UmiStatus leave(UmiReceiptInspector *self, UmiStatus status) {
  self->state.status = status;
  feedback(self,
           status == UMI_STATUS_OK ? "Reading the selected receipt..." : umi_status_text(status));
  self->working = false;
  g_object_unref(self);
  return status;
}
static void worker(GTask *task, gpointer source, gpointer task_data, GCancellable *cancel) {
  (void)source;
  (void)cancel;
  ReceiptReadJob *job = task_data;
  UmiDataServer *server = NULL;
  job->status = umi_data_server_open_sqlite_readonly(job->path, &server);
  if (job->status == UMI_STATUS_OK)
    job->status = job->verify
                      ? umi_record_transfer_receipt_verify(server, job->identity, &job->receipt)
                      : umi_record_transfer_receipt_load(server, job->identity, &job->receipt);
  umi_data_server_destroy(server);
  g_task_return_boolean(task, TRUE);
}
static UmiStatus present(UmiReceiptInspector *self, ReceiptReadJob *job) {
  GtkWidget *view = NULL;
  UmiStatus status = UmiRecordTransferReceiptGtk4New(&job->receipt, &view);
  if (status != UMI_STATUS_OK)
    return status;
  g_object_ref_sink(view);
  GtkWidget *box = g_object_ref(self->preview);
  clear_view(self);
  if (!self->state.retired && self->generation == job->generation)
    gtk_box_append(GTK_BOX(box), view);
  else
    status = UMI_STATUS_CANCELLED;
  g_object_unref(box);
  g_object_unref(view);
  return status;
}
static void completed(GObject *source, GAsyncResult *result, gpointer context) {
  (void)context;
  UmiReceiptInspector *self = (UmiReceiptInspector *)source;
  ReceiptReadJob *job = g_task_get_task_data(G_TASK(result));
  (void)g_task_propagate_boolean(G_TASK(result), NULL);
  self->working = true;
  UmiStatus status = job->status;
  if (self->state.retired || self->generation != job->generation)
    status = UMI_STATUS_CANCELLED;
  if (status == UMI_STATUS_OK) {
    self->state.display_status = present(self, job);
    feedback(
        self,
        self->state.display_status != UMI_STATUS_OK
            ? "Receipt read; its preview is unavailable."
            : (job->verify
                   ? "Imported history checksum verified. This does not authenticate the author."
                   : "Receipt integrity checked. Original imported history has not been "
                     "verified."));
    /* Rendering may notify application observers. Recheck intent before
     * exposing copied data, not merely before creating the first label. */
    if (self->state.retired || self->generation != job->generation)
      status = UMI_STATUS_CANCELLED;
    else {
      self->state.has_receipt = true;
      self->state.verified = job->verify;
      self->state.receipt = job->receipt;
    }
  } else
    feedback(self, umi_status_text(status));
  if (status != UMI_STATUS_OK)
    invalidate(self);
  self->state.status = status;
  self->state.busy = false;
  self->working = false;
  /* No mutable state access after emission: a host may immediately read again. */
  g_signal_emit(self, finished_signal, 0, (int)status);
}
UmiStatus UmiRecordTransferInspectorGtk4Read(GtkWidget *widget, const char *path, const char *id,
                                             bool verify) {
  UmiReceiptInspector *self = instance(widget);
  UmiStatus status = enter(self);
  if (status != UMI_STATUS_OK)
    return status;
  ReceiptReadJob *job = g_try_new0(ReceiptReadJob, 1);
  if (!job)
    return leave(self, UMI_STATUS_OUT_OF_MEMORY);
  if (!path || !g_utf8_validate(path, -1, NULL))
    status = UMI_STATUS_INVALID_ARGUMENT;
  else if (strlen(path) >= sizeof(job->path))
    status = UMI_STATUS_CAPACITY_EXCEEDED;
  else
    status = UmiOutputFileValidatePath(path);
  if (status == UMI_STATUS_OK)
    status = umi_record_transfer_id_validate(id);
  if (status == UMI_STATUS_OK) {
    memcpy(job->path, path, strlen(path) + 1);
    memcpy(job->identity, id, strlen(id) + 1);
    job->verify = verify;
    job->generation = self->generation;
    status = self->options.authorize_read(job->path, job->identity, verify,
                                          self->options.authorize_context);
    if (status == UMI_STATUS_OK && (self->state.retired || self->generation != job->generation))
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK) {
    invalidate(self);
    if (self->state.retired || self->generation != job->generation)
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK) {
    self->state.busy = true;
    memcpy(self->state.operation_path, job->path, sizeof(job->path));
    memcpy(self->state.receipt_id, job->identity, sizeof(job->identity));
    GTask *task = g_task_new(self, NULL, completed, NULL);
    g_task_set_task_data(task, job, g_free);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, worker);
    g_object_unref(task);
  } else
    g_free(job);
  return leave(self, status);
}
UmiStatus UmiRecordTransferInspectorGtk4StateRead(GtkWidget *widget,
                                                  UmiRecordTransferInspectorGtk4State *out) {
  UmiReceiptInspector *self = instance(widget);
  if (!self || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = self->state;
  return UMI_STATUS_OK;
}
bool UmiRecordTransferInspectorGtk4CanClose(GtkWidget *widget) {
  UmiReceiptInspector *self = instance(widget);
  return self && !self->working && !self->state.busy;
}
void UmiRecordTransferInspectorGtk4Retire(GtkWidget *widget) {
  UmiReceiptInspector *self = instance(widget);
  if (self)
    retire(self);
}
static void clicked(GtkButton *button, gpointer context) {
  UmiReceiptInspector *self = context;
  if (self->state.retired)
    return;
  bool verify = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "verify-history")) != 0;
  (void)UmiRecordTransferInspectorGtk4Read(
      GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->database)),
      gtk_editable_get_text(GTK_EDITABLE(self->identity)), verify);
}
UmiStatus UmiRecordTransferInspectorGtk4New(const UmiRecordTransferInspectorGtk4Options *options,
                                            GtkWidget **out) {
  if (!options || !options->authorize_read || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiReceiptInspector *self = g_object_new(umi_receipt_inspector_get_type(), NULL);
  self->options = *options;
  gtk_box_append(GTK_BOX(self), gtk_label_new("Inspect saved import receipts"));
  gtk_box_append(GTK_BOX(self),
                 gtk_label_new("Choose an existing database and enter the import receipt ID."));
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  self->database = gtk_entry_new();
  gtk_widget_set_hexpand(self->database, TRUE);
  gtk_widget_set_name(self->database, "receipt-inspector-database");
  gtk_entry_set_placeholder_text(GTK_ENTRY(self->database), "Existing SQLite database");
  gtk_box_append(GTK_BOX(row), self->database);
  GtkWidget *browse =
      UmiPathPickerGtk4Create(GTK_ENTRY(self->database), UMI_PATH_PICKER_EXISTING_FILE,
                              "Select an existing receipt database", NULL);
  if (!browse) {
    g_object_ref_sink(self);
    g_object_unref(self);
    g_object_ref_sink(row);
    g_object_unref(row);
    return UMI_STATUS_OUT_OF_MEMORY;
  }
  gtk_box_append(GTK_BOX(row), browse);
  gtk_box_append(GTK_BOX(self), row);
  self->identity = gtk_entry_new();
  gtk_widget_set_name(self->identity, "receipt-inspector-id");
  gtk_entry_set_placeholder_text(GTK_ENTRY(self->identity), "Receipt ID");
  gtk_box_append(GTK_BOX(self), self->identity);
  GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8),
            *read = gtk_button_new_with_label("View receipt"),
            *verify = gtk_button_new_with_label("Verify imported history");
  gtk_widget_set_name(read, "receipt-inspector-view");
  gtk_widget_set_name(verify, "receipt-inspector-verify");
  g_object_set_data(G_OBJECT(verify), "verify-history", GINT_TO_POINTER(1));
  gtk_box_append(GTK_BOX(actions), read);
  gtk_box_append(GTK_BOX(actions), verify);
  gtk_box_append(GTK_BOX(self), actions);
  self->feedback = gtk_label_new("No receipt loaded.");
  gtk_label_set_wrap(GTK_LABEL(self->feedback), TRUE);
  gtk_widget_set_name(self->feedback, "receipt-inspector-feedback");
  gtk_box_append(GTK_BOX(self), self->feedback);
  self->preview = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_widget_set_name(self->preview, "receipt-inspector-preview");
  gtk_box_append(GTK_BOX(self), self->preview);
  g_signal_connect(self->database, "changed", G_CALLBACK(changed), self);
  g_signal_connect(self->identity, "changed", G_CALLBACK(changed), self);
  g_signal_connect(read, "clicked", G_CALLBACK(clicked), self);
  g_signal_connect(verify, "clicked", G_CALLBACK(clicked), self);
  *out = GTK_WIDGET(self);
  return UMI_STATUS_OK;
}
