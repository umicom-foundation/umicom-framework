/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_database_maintenance_gtk4.c
 *
 * PURPOSE:
 *   Exercise native admission, captured file identity and asynchronous outcomes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Exercise native admission, captured file identity and asynchronous outcomes. */
#include "readonly_fixture.h"
#include "umicom/ui/gtk4/database_maintenance.h"
static GtkWidget *find(GtkWidget *root, const char *name) {
  if (strcmp(gtk_widget_get_name(root), name) == 0)
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = find(child, name);
    if (found)
      return found;
  }
  return NULL;
}
typedef struct Authorization {
  GtkWidget *widget;
  const char *mode;
  char source[1024], destination[1024];
  char *caller_source, *caller_destination;
  int calls, completions;
  UmiStatus nested, followup;
} Authorization;
static UmiStatus authorize(UmiDatabaseMaintenanceOperation operation, const char *source,
                           const char *destination, void *context) {
  Authorization *a = context;
  ++a->calls;
  if (strcmp(source, a->source) != 0)
    return UMI_STATUS_INVALID_STATE;
  if (strcmp(destination, operation == UMI_DATABASE_MAINTENANCE_CHECK ? "" : a->destination) != 0)
    return UMI_STATUS_INVALID_STATE;
  if (strcmp(a->mode, "denied") == 0)
    return UMI_STATUS_PERMISSION_DENIED;
  if (strcmp(a->mode, "edit-authorize") == 0)
    gtk_editable_set_text(GTK_EDITABLE(find(a->widget, "database-maintenance-destination")),
                          "changed");
  if (strcmp(a->mode, "retire-authorize") == 0)
    UmiDatabaseMaintenanceGtk4Retire(a->widget);
  if (strcmp(a->mode, "reenter-authorize") == 0)
    a->nested = UmiDatabaseMaintenanceGtk4Run(a->widget, operation, source, destination);
  if (strcmp(a->mode, "captured-source") == 0)
    memcpy(a->caller_source, "changed.sqlite", sizeof("changed.sqlite"));
  if (strcmp(a->mode, "captured-destination") == 0)
    memcpy(a->caller_destination, "changed.sqlite", sizeof("changed.sqlite"));
  return UMI_STATUS_OK;
}
static void finished(GtkWidget *widget, int status, gpointer context) {
  Authorization *a = context;
  ++a->completions;
  if (strcmp(a->mode, "completion-reentry") == 0 && a->completions == 1 && status == UMI_STATUS_OK)
    a->followup = UmiDatabaseMaintenanceGtk4Run(widget, UMI_DATABASE_MAINTENANCE_BACKUP, a->source,
                                                a->destination);
}
static bool idle(GtkWidget *widget) {
  gint64 until = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  UmiDatabaseMaintenanceGtk4State state;
  while (UmiDatabaseMaintenanceGtk4StateRead(widget, &state) == UMI_STATUS_OK && state.busy &&
         g_get_monotonic_time() < until) {
    (void)g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return UmiDatabaseMaintenanceGtk4StateRead(widget, &state) == UMI_STATUS_OK && !state.busy;
}
int main(int argc, char **argv) {
  const char *cases[] = {"check",
                         "check-unhealthy",
                         "backup",
                         "recover",
                         "button-check",
                         "button-backup",
                         "button-recover",
                         "denied",
                         "missing",
                         "relative-source",
                         "relative-destination",
                         "invalid-operation",
                         "null-authorizer",
                         "busy",
                         "captured-source",
                         "captured-destination",
                         "edit-authorize",
                         "retire-authorize",
                         "reenter-authorize",
                         "changed-check",
                         "retired-check",
                         "changed-copy",
                         "retired-copy",
                         "disposed-copy",
                         "occupied",
                         "no-destination-check",
                         "prior-result",
                         "completion-reentry",
                         "changed-after-check",
                         "changed-after-copy"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof cases / sizeof cases[0]))
    return 2;
  if (!gtk_init_check())
    return 77;
  int result = 0;
  const char *name = argv[1];
  GtkWidget *root = NULL;
  UmiDataServer *writer = NULL, *copy = NULL;
  Authorization *a = g_new0(Authorization, 1);
  a->mode = name;
  char caller_source[1024], caller_destination[1024];
  UmiDatabaseMaintenanceGtk4State state;
  UmiDatabaseMaintenanceGtk4Options options = {.authorize = authorize, .authorize_context = a};
  if (strcmp(name, "null-authorizer") == 0) {
    options.authorize = NULL;
    CHECK(UmiDatabaseMaintenanceGtk4New(&options, &root) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!root);
    goto cleanup;
  }
  CHECK(ro_path(argv[2], name, a->source, true));
  CHECK(snprintf(a->destination, sizeof a->destination, "%s.copy", a->source) > 0);
  CHECK(strlen(a->source) + 5 < sizeof a->destination);
  OK(umi_data_server_create_sqlite(a->source, &writer));
  OK(umi_data_server_set(writer, "example", "fictional"));
  if (strcmp(name, "check-unhealthy") == 0)
    OK(umi_data_server_execute(
        writer,
        "CREATE TABLE invalid(value INTEGER CHECK(value > 0)); PRAGMA ignore_check_constraints=ON; "
        "INSERT INTO invalid VALUES(-1); PRAGMA ignore_check_constraints=OFF;"));
  umi_data_server_destroy(writer);
  writer = NULL;
  if (strcmp(name, "missing") == 0) {
    size_t n = strlen(a->source);
    CHECK(n + sizeof(".missing") <= sizeof a->source);
    memcpy(a->source + n, ".missing", sizeof(".missing"));
  }
  if (strcmp(name, "occupied") == 0) {
    FILE *file = fopen(a->destination, "wbx");
    CHECK(file);
    bool written = fputs("keep", file) >= 0;
    bool closed = fclose(file) == 0;
    CHECK(written && closed);
  }
  OK(UmiDatabaseMaintenanceGtk4New(&options, &root));
  g_object_ref_sink(root);
  a->widget = root;
  g_signal_connect(root, "operation-finished", G_CALLBACK(finished), a);
  gtk_editable_set_text(GTK_EDITABLE(find(root, "database-maintenance-source")), a->source);
  gtk_editable_set_text(GTK_EDITABLE(find(root, "database-maintenance-destination")),
                        a->destination);
  bool checking = strcmp(name, "check") == 0 || strcmp(name, "check-unhealthy") == 0 ||
                  strcmp(name, "button-check") == 0 || strcmp(name, "changed-check") == 0 ||
                  strcmp(name, "retired-check") == 0 || strcmp(name, "no-destination-check") == 0 ||
                  strcmp(name, "completion-reentry") == 0 ||
                  strcmp(name, "changed-after-check") == 0;
  UmiDatabaseMaintenanceOperation operation =
      checking ? UMI_DATABASE_MAINTENANCE_CHECK
               : (strcmp(name, "recover") == 0 || strcmp(name, "button-recover") == 0
                      ? UMI_DATABASE_MAINTENANCE_RECOVER
                      : UMI_DATABASE_MAINTENANCE_BACKUP);
  if (strcmp(name, "relative-source") == 0 || strcmp(name, "relative-destination") == 0 ||
      strcmp(name, "invalid-operation") == 0) {
    CHECK(UmiDatabaseMaintenanceGtk4Run(
              root,
              strcmp(name, "invalid-operation") == 0 ? (UmiDatabaseMaintenanceOperation)99
                                                     : operation,
              strcmp(name, "relative-source") == 0 ? "relative.sqlite" : a->source,
              strcmp(name, "relative-destination") == 0 ? "relative.sqlite" : a->destination) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(a->calls == 0);
    goto cleanup;
  }
  memcpy(caller_source, a->source, sizeof caller_source);
  memcpy(caller_destination, a->destination, sizeof caller_destination);
  a->caller_source = caller_source;
  a->caller_destination = caller_destination;
  UmiStatus admitted = UMI_STATUS_OK;
  if (strncmp(name, "button-", 7) == 0) {
    const char *control = checking ? "database-maintenance-check"
                          : operation == UMI_DATABASE_MAINTENANCE_BACKUP
                              ? "database-maintenance-backup"
                              : "database-maintenance-recover";
    g_signal_emit_by_name(find(root, control), "clicked");
    OK(UmiDatabaseMaintenanceGtk4StateRead(root, &state));
    CHECK(state.busy);
  } else
    admitted = UmiDatabaseMaintenanceGtk4Run(
        root, operation, caller_source,
        strcmp(name, "no-destination-check") == 0 ? NULL : caller_destination);
  if (strcmp(name, "denied") == 0 || strcmp(name, "edit-authorize") == 0 ||
      strcmp(name, "retire-authorize") == 0) {
    CHECK(admitted ==
          (strcmp(name, "denied") == 0 ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_CANCELLED));
    OK(UmiDatabaseMaintenanceGtk4StateRead(root, &state));
    CHECK(!state.busy && !state.has_result && a->completions == 0);
    FILE *probe = fopen(a->destination, "rb");
    bool absent = probe == NULL;
    if (probe)
      fclose(probe);
    CHECK(absent);
    goto cleanup;
  }
  OK(admitted);
  CHECK(!UmiDatabaseMaintenanceGtk4CanClose(root));
  if (strcmp(name, "busy") == 0)
    CHECK(UmiDatabaseMaintenanceGtk4Run(root, operation, a->source, a->destination) ==
          UMI_STATUS_BUSY);
  if (strcmp(name, "changed-check") == 0 || strcmp(name, "changed-copy") == 0)
    gtk_editable_set_text(GTK_EDITABLE(find(root, "database-maintenance-destination")),
                          "different.sqlite");
  if (strcmp(name, "retired-check") == 0 || strcmp(name, "retired-copy") == 0)
    UmiDatabaseMaintenanceGtk4Retire(root);
  if (strcmp(name, "disposed-copy") == 0)
    g_object_run_dispose(G_OBJECT(root));
  CHECK(idle(root));
  OK(UmiDatabaseMaintenanceGtk4StateRead(root, &state));
  CHECK(!state.busy && UmiDatabaseMaintenanceGtk4CanClose(root));
  /* Disposal disconnects GObject signal handlers. The task still owns the
   * widget until its captured copy completes; verify that result below instead
   * of requiring delivery to a handler that no longer exists. Retire keeps
   * the live signal contract. Retain the former assertion for review. */
#if 0
  CHECK(a->completions == (strcmp(name, "completion-reentry") == 0 ? 2 : 1));
#endif
  CHECK(a->completions == (strcmp(name, "disposed-copy") == 0 ? 0 :
                            strcmp(name, "completion-reentry") == 0 ? 2 : 1));
  if (strcmp(name, "disposed-copy") == 0 || strcmp(name, "retired-copy") == 0) {
    CHECK(state.retired);
    CHECK(UmiDatabaseMaintenanceGtk4Run(root, operation, a->source, a->destination) ==
          UMI_STATUS_INVALID_STATE);
  }
  if (strcmp(name, "reenter-authorize") == 0)
    CHECK(a->nested == UMI_STATUS_BUSY);
  if (strcmp(name, "changed-check") == 0 || strcmp(name, "retired-check") == 0) {
    CHECK(state.status == UMI_STATUS_CANCELLED && !state.has_result);
    goto cleanup;
  }
  if (strcmp(name, "missing") == 0 || strcmp(name, "occupied") == 0) {
    CHECK(state.status != UMI_STATUS_OK && !state.has_result);
    if (strcmp(name, "occupied") == 0) {
      CHECK(state.status == UMI_STATUS_ALREADY_EXISTS);
      FILE *file = fopen(a->destination, "rb");
      CHECK(file);
      char text[5] = {0};
      size_t n = fread(text, 1, 4, file);
      bool closed = fclose(file) == 0;
      CHECK(closed && n == 4 && strcmp(text, "keep") == 0);
    }
    goto cleanup;
  }
  CHECK(state.status == UMI_STATUS_OK && state.has_result);
  CHECK(strcmp(state.source, a->source) == 0);
  if (strcmp(name, "completion-reentry") == 0) {
    OK(a->followup);
    checking = false;
  }
  if (checking)
    CHECK(state.health.sound == (strcmp(name, "check-unhealthy") != 0));
  else {
    CHECK(strcmp(state.destination, a->destination) == 0);
    CHECK(state.copy.sqlite_quick_check_passed && state.copy.record_count == 1);
    OK(umi_data_server_open_sqlite_readonly(a->destination, &copy));
    char text[32];
    OK(umi_data_server_get(copy, "example", text, sizeof text));
    CHECK(strcmp(text, "fictional") == 0);
  }
  if (strcmp(name, "prior-result") == 0) {
    CHECK(UmiDatabaseMaintenanceGtk4Run(root, operation, "relative.sqlite", a->destination) ==
          UMI_STATUS_INVALID_ARGUMENT);
    OK(UmiDatabaseMaintenanceGtk4StateRead(root, &state));
    CHECK(state.has_result && state.status == UMI_STATUS_OK &&
          strcmp(state.destination, a->destination) == 0);
  }
  if (strcmp(name, "changed-after-check") == 0 || strcmp(name, "changed-after-copy") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(find(root, "database-maintenance-source")),
                          "another.sqlite");
    OK(UmiDatabaseMaintenanceGtk4StateRead(root, &state));
    CHECK(state.has_result == !checking);
  }
  if (strcmp(name, "retired-copy") == 0 || strcmp(name, "disposed-copy") == 0)
    CHECK(state.retired);
cleanup:
  umi_data_server_destroy(copy);
  umi_data_server_destroy(writer);
  if (root) {
    UmiDatabaseMaintenanceGtk4Retire(root);
    bool drained = idle(root);
    g_object_unref(root);
    /* On timeout the worker still owns its widget and borrowed callback
     * context. Keep that context until process exit instead of freeing it. */
    if (drained)
      g_free(a);
    else
      result = 1;
  } else
    g_free(a);
  return result;
}
