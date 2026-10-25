/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/database_maintenance_gtk4.c
 *
 * PURPOSE:
 *   Workers own storage handles; GTK owns intent, admission and presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Workers own storage handles; GTK owns intent, admission and presentation. */
#include "umicom/platform/output_file.h"
#include "umicom/ui/gtk4/database_maintenance.h"
#include "umicom/ui/gtk4/path_picker.h"
#include <string.h>

typedef struct UmiDatabaseMaintenance {
  GtkBox parent;
  UmiDatabaseMaintenanceGtk4Options options;
  UmiDatabaseMaintenanceGtk4State state;
  GtkWidget *source, *destination, *feedback;
  uint64_t generation;
  bool working;
} UmiDatabaseMaintenance;
typedef struct UmiDatabaseMaintenanceClass {
  GtkBoxClass parent;
} UmiDatabaseMaintenanceClass;
GType umi_database_maintenance_get_type(void);
G_DEFINE_TYPE(UmiDatabaseMaintenance, umi_database_maintenance, GTK_TYPE_BOX)
static guint finished_signal;
typedef struct MaintenanceJob {
  UmiDatabaseMaintenanceGtk4State result;
  uint64_t generation;
} MaintenanceJob;
static UmiDatabaseMaintenance *instance(GtkWidget *widget) {
  return widget && G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_database_maintenance_get_type())
             ? (UmiDatabaseMaintenance *)widget
             : NULL;
}
static void retire(UmiDatabaseMaintenance *self) {
  self->state.retired = true;
  if (self->state.operation == UMI_DATABASE_MAINTENANCE_CHECK) {
    self->state.has_result = false;
    memset(&self->state.health, 0, sizeof self->state.health);
  }
}
static void maintenance_unroot(GtkWidget *widget) {
  retire((UmiDatabaseMaintenance *)widget);
  GTK_WIDGET_CLASS(umi_database_maintenance_parent_class)->unroot(widget);
}
static void maintenance_dispose(GObject *object) {
  retire((UmiDatabaseMaintenance *)object);
  G_OBJECT_CLASS(umi_database_maintenance_parent_class)->dispose(object);
}
static void umi_database_maintenance_class_init(UmiDatabaseMaintenanceClass *klass) {
  GTK_WIDGET_CLASS(klass)->unroot = maintenance_unroot;
  G_OBJECT_CLASS(klass)->dispose = maintenance_dispose;
  finished_signal = g_signal_new("operation-finished", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
                                 0, NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_INT);
}
static void umi_database_maintenance_init(UmiDatabaseMaintenance *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 10);
  self->generation = 1;
  self->state.status = UMI_STATUS_UNAVAILABLE;
}
static void feedback(UmiDatabaseMaintenance *self, const char *text) {
  if (self->state.retired)
    return;
  GtkWidget *label = g_object_ref(self->feedback);
  gtk_label_set_text(GTK_LABEL(label), text);
  g_object_unref(label);
}
static void changed(GtkEditable *entry, gpointer context) {
  (void)entry;
  UmiDatabaseMaintenance *self = context;
  if (self->state.retired)
    return;
  g_object_ref(self);
  if (self->generation == UINT64_MAX)
    retire(self);
  else
    ++self->generation;
  if (self->state.operation == UMI_DATABASE_MAINTENANCE_CHECK) {
    self->state.has_result = false;
    memset(&self->state.health, 0, sizeof self->state.health);
  }
  /* A new visible destination must not redirect an already admitted copy. */
  if (!self->working && !self->state.busy)
    feedback(self, "Selection changed. Previous copies remain at their captured destinations.");
  g_object_unref(self);
}
static UmiStatus capture_path(const char *path, char out[1024]) {
  if (!path || !g_utf8_validate(path, -1, NULL))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (strlen(path) >= 1024)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  UmiStatus status = UmiOutputFileValidatePath(path);
  if (status == UMI_STATUS_OK)
    memcpy(out, path, strlen(path) + 1);
  return status;
}
static void worker(GTask *task, gpointer source, gpointer task_data, GCancellable *cancel) {
  (void)source;
  (void)cancel;
  MaintenanceJob *job = task_data;
  UmiDatabaseMaintenanceGtk4State *result = &job->result;
  UmiDataServer *server = NULL;
  if (result->operation == UMI_DATABASE_MAINTENANCE_RECOVER)
    result->status =
        umi_data_backup_restore_checked(result->source, result->destination, &result->copy);
  else {
    result->status = umi_data_server_open_sqlite_readonly(result->source, &server);
    if (result->status == UMI_STATUS_OK)
      result->status =
          result->operation == UMI_DATABASE_MAINTENANCE_CHECK
              ? umi_sqlite_health_read(server, &result->health)
              : umi_data_backup_create_checked(server, result->destination, &result->copy);
    umi_data_server_destroy(server);
  }
  result->has_result = result->status == UMI_STATUS_OK;
  g_task_return_boolean(task, TRUE);
}
static void completed(GObject *source, GAsyncResult *result, gpointer context) {
  (void)context;
  UmiDatabaseMaintenance *self = (UmiDatabaseMaintenance *)source;
  MaintenanceJob *job = g_task_get_task_data(G_TASK(result));
  (void)g_task_propagate_boolean(G_TASK(result), NULL);
  self->working = true;
  bool retired = self->state.retired;
  self->state = job->result;
  self->state.retired = retired;
  self->state.busy = true;
  bool reading = self->state.operation == UMI_DATABASE_MAINTENANCE_CHECK;
  if (reading && (retired || self->generation != job->generation)) {
    self->state.status = UMI_STATUS_CANCELLED;
    self->state.has_result = false;
    memset(&self->state.health, 0, sizeof self->state.health);
  }
  char *message = NULL;
  if (self->state.status != UMI_STATUS_OK)
    message =
        g_strdup_printf("%s\nSource: %s\nDestination: %s", umi_status_text(self->state.status),
                        self->state.source, self->state.destination);
  else if (reading) {
    const char *diagnostic = self->state.health.first_diagnostic;
    if (!g_utf8_validate(diagnostic, -1, NULL))
      diagnostic = "Diagnostic text is not valid UTF-8.";
    message = g_strdup_printf("%s\nSource: %s\n%s",
                              self->state.health.sound ? "SQLite quick_check reported no issues."
                                                       : "SQLite reported database issues.",
                              self->state.source, diagnostic);
  } else
    message = g_strdup_printf("New database created: %s\n%zu pages; %zu key/value records.\nThe "
                              "active application database has not changed.",
                              self->state.destination, self->state.copy.page_count,
                              self->state.copy.record_count);
  feedback(self, message);
  g_free(message);
  /* Native property notifications can change selection during presentation.
   * Recheck a read, but retain the true outcome of an admitted file creation. */
  if (reading && (self->state.retired || self->generation != job->generation)) {
    self->state.status = UMI_STATUS_CANCELLED;
    self->state.has_result = false;
    memset(&self->state.health, 0, sizeof self->state.health);
    feedback(self, "Selection changed. Run the database check again.");
  }
  self->state.busy = false;
  self->working = false;
  g_signal_emit(self, finished_signal, 0, (int)self->state.status);
}
UmiStatus UmiDatabaseMaintenanceGtk4Run(GtkWidget *widget,
                                        UmiDatabaseMaintenanceOperation operation,
                                        const char *source, const char *destination) {
  UmiDatabaseMaintenance *self = instance(widget);
  if (!self)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (self->state.retired)
    return UMI_STATUS_INVALID_STATE;
  if (self->working || self->state.busy)
    return UMI_STATUS_BUSY;
  g_object_ref(self);
  self->working = true;
  MaintenanceJob *job = g_try_new0(MaintenanceJob, 1);
  UmiStatus status = job ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
  if (status == UMI_STATUS_OK && operation != UMI_DATABASE_MAINTENANCE_CHECK &&
      operation != UMI_DATABASE_MAINTENANCE_BACKUP && operation != UMI_DATABASE_MAINTENANCE_RECOVER)
    status = UMI_STATUS_INVALID_ARGUMENT;
  if (status == UMI_STATUS_OK)
    status = capture_path(source, job->result.source);
  if (status == UMI_STATUS_OK && operation != UMI_DATABASE_MAINTENANCE_CHECK)
    status = capture_path(destination, job->result.destination);
  if (status == UMI_STATUS_OK) {
    job->result.operation = operation;
    job->generation = self->generation;
    status = self->options.authorize(operation, job->result.source, job->result.destination,
                                     self->options.authorize_context);
    if (status == UMI_STATUS_OK && (self->state.retired || self->generation != job->generation))
      status = UMI_STATUS_CANCELLED;
  }
  if (status == UMI_STATUS_OK) {
    self->state = job->result;
    self->state.status = UMI_STATUS_OK;
    self->state.busy = true;
    GTask *task = g_task_new(self, NULL, completed, NULL);
    g_task_set_task_data(task, job, g_free);
    g_task_set_check_cancellable(task, FALSE);
    g_task_run_in_thread(task, worker);
    g_object_unref(task);
  } else
    g_free(job);
  /* Failed admission leaves the prior captured result intact. Its status
   * describes that result; the returned status describes the rejected call. */
  feedback(self, status == UMI_STATUS_OK ? "Working on the captured selection..."
                                         : umi_status_text(status));
  self->working = false;
  g_object_unref(self);
  return status;
}
UmiStatus UmiDatabaseMaintenanceGtk4StateRead(GtkWidget *widget,
                                              UmiDatabaseMaintenanceGtk4State *out) {
  UmiDatabaseMaintenance *self = instance(widget);
  if (!self || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = self->state;
  return UMI_STATUS_OK;
}
bool UmiDatabaseMaintenanceGtk4CanClose(GtkWidget *widget) {
  UmiDatabaseMaintenance *self = instance(widget);
  return self && !self->working && !self->state.busy;
}
void UmiDatabaseMaintenanceGtk4Retire(GtkWidget *widget) {
  UmiDatabaseMaintenance *self = instance(widget);
  if (self)
    retire(self);
}
static void clicked(GtkButton *button, gpointer context) {
  UmiDatabaseMaintenance *self = context;
  if (self->state.retired)
    return;
  UmiDatabaseMaintenanceOperation operation = (UmiDatabaseMaintenanceOperation)GPOINTER_TO_INT(
      g_object_get_data(G_OBJECT(button), "database-operation"));
  (void)UmiDatabaseMaintenanceGtk4Run(GTK_WIDGET(self), operation,
                                      gtk_editable_get_text(GTK_EDITABLE(self->source)),
                                      gtk_editable_get_text(GTK_EDITABLE(self->destination)));
}
static GtkWidget *path_row(GtkWidget **entry, const char *name, UmiPathPickerMode mode,
                           const char *title) {
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  *entry = gtk_entry_new();
  gtk_widget_set_hexpand(*entry, TRUE);
  gtk_widget_set_name(*entry, name);
  gtk_entry_set_placeholder_text(GTK_ENTRY(*entry), title);
  gtk_box_append(GTK_BOX(row), *entry);
  GtkWidget *browse = UmiPathPickerGtk4Create(GTK_ENTRY(*entry), mode, title, NULL);
  if (!browse) {
    g_object_ref_sink(row);
    g_object_unref(row);
    *entry = NULL;
    return NULL;
  }
  gtk_box_append(GTK_BOX(row), browse);
  return row;
}
UmiStatus UmiDatabaseMaintenanceGtk4New(const UmiDatabaseMaintenanceGtk4Options *options,
                                        GtkWidget **out) {
  if (out)
    *out = NULL;
  if (!options || !options->authorize || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiDatabaseMaintenance *self = g_object_new(umi_database_maintenance_get_type(), NULL);
  self->options = *options;
  gtk_box_append(GTK_BOX(self), gtk_label_new("Database health, backup and recovery"));
  GtkWidget *explanation = gtk_label_new(
      "Copies use a new file. Recovery does not replace the current application database.");
  gtk_label_set_wrap(GTK_LABEL(explanation), TRUE);
  gtk_box_append(GTK_BOX(self), explanation);
  GtkWidget *row = path_row(&self->source, "database-maintenance-source",
                            UMI_PATH_PICKER_EXISTING_FILE, "Existing SQLite database");
  if (!row)
    goto failed;
  gtk_box_append(GTK_BOX(self), row);
  row = path_row(&self->destination, "database-maintenance-destination", UMI_PATH_PICKER_NEW_FILE,
                 "New database destination");
  if (!row)
    goto failed;
  gtk_box_append(GTK_BOX(self), row);
  GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  const char *labels[] = {"Check database", "Create backup", "Recover to new database"};
  const char *names[] = {"database-maintenance-check", "database-maintenance-backup",
                         "database-maintenance-recover"};
  for (int i = 0; i < 3; ++i) {
    GtkWidget *button = gtk_button_new_with_label(labels[i]);
    gtk_widget_set_name(button, names[i]);
    g_object_set_data(G_OBJECT(button), "database-operation", GINT_TO_POINTER(i));
    g_signal_connect(button, "clicked", G_CALLBACK(clicked), self);
    gtk_box_append(GTK_BOX(actions), button);
  }
  gtk_box_append(GTK_BOX(self), actions);
  self->feedback = gtk_label_new("No operation performed.");
  gtk_label_set_wrap(GTK_LABEL(self->feedback), TRUE);
  gtk_label_set_selectable(GTK_LABEL(self->feedback), TRUE);
  gtk_widget_set_name(self->feedback, "database-maintenance-feedback");
  gtk_box_append(GTK_BOX(self), self->feedback);
  g_signal_connect(self->source, "changed", G_CALLBACK(changed), self);
  g_signal_connect(self->destination, "changed", G_CALLBACK(changed), self);
  *out = GTK_WIDGET(self);
  return UMI_STATUS_OK;
failed:
  g_object_ref_sink(self);
  g_object_unref(self);
  return UMI_STATUS_OUT_OF_MEMORY;
}
