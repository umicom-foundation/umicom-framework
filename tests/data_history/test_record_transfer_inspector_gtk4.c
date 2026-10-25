/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_record_transfer_inspector_gtk4.c
 *
 * PURPOSE:
 *   Drive actual GTK controls and retain fixtures for failed owner-run checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Drive actual GTK controls and retain fixtures for failed owner-run checks. */
#include "../../src/data/include/record_history_internal.h"
#include "readonly_fixture.h"
#include "umicom/ui/gtk4/record_transfer_inspector.h"
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
typedef struct InspectAuthorization {
  GtkWidget *widget;
  const char *mode;
  char path[1024];
  char *caller_path;
  int calls, completions;
  UmiStatus nested, followup;
} InspectAuthorization;
static UmiStatus authorize(const char *path, const char *id, bool verify, void *context) {
  (void)verify;
  InspectAuthorization *a = context;
  ++a->calls;
  if (strcmp(path, a->path) != 0 || strcmp(id, "inspect-example") != 0)
    return UMI_STATUS_INVALID_STATE;
  if (strcmp(a->mode, "denied") == 0)
    return UMI_STATUS_PERMISSION_DENIED;
  if (strcmp(a->mode, "edit-authorize") == 0)
    gtk_editable_set_text(GTK_EDITABLE(find(a->widget, "receipt-inspector-id")), "changed");
  if (strcmp(a->mode, "retire-authorize") == 0)
    UmiRecordTransferInspectorGtk4Retire(a->widget);
  if (strcmp(a->mode, "reenter-authorize") == 0)
    a->nested = UmiRecordTransferInspectorGtk4Read(a->widget, path, id, true);
  if (strcmp(a->mode, "captured-path") == 0 && a->caller_path)
    memcpy(a->caller_path, "changed.sqlite", sizeof("changed.sqlite"));
  return UMI_STATUS_OK;
}
static void finished(GtkWidget *widget, int status, gpointer context) {
  InspectAuthorization *a = context;
  ++a->completions;
  if (strcmp(a->mode, "completion-reentry") == 0 && a->completions == 1 && status == UMI_STATUS_OK)
    a->followup = UmiRecordTransferInspectorGtk4Read(widget, a->path, "inspect-example", true);
}
static bool idle(GtkWidget *widget) {
  gint64 until = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  UmiRecordTransferInspectorGtk4State state;
  while (UmiRecordTransferInspectorGtk4StateRead(widget, &state) == UMI_STATUS_OK && state.busy &&
         g_get_monotonic_time() < until) {
    (void)g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return UmiRecordTransferInspectorGtk4StateRead(widget, &state) == UMI_STATUS_OK && !state.busy;
}
int main(int argc, char **argv) {
  const char *cases[] = {"view",
                         "verify",
                         "buttons",
                         "denied",
                         "missing-store",
                         "missing-receipt",
                         "corrupt-history",
                         "view-corrupt-history",
                         "relative",
                         "invalid-id",
                         "null-authorizer",
                         "busy",
                         "changed-read",
                         "retired-read",
                         "disposed-read",
                         "edit-authorize",
                         "retire-authorize",
                         "reenter-authorize",
                         "completion-reentry",
                         "captured-path",
                         "changed-after-read"};
  if (argc != 3 || !ro_case(argv[1], cases, sizeof(cases) / sizeof(cases[0])))
    return 2;
  if (!gtk_init_check())
    return 77;
  int result = 0;
  const char *name = argv[1];
  GtkWidget *root = NULL;
  UmiDataServer *source = NULL, *writer = NULL;
  UmiRecordBundle *bundle = NULL;
  unsigned char *bytes = NULL;
  size_t length = 0;
  InspectAuthorization *a = g_new0(InspectAuthorization, 1);
  a->mode = name;
  char caller_path[1024];
  UmiRecordTransferInspectorGtk4State state;
  UmiRecordTransferInspectorGtk4Options options = {.authorize_read = authorize,
                                                   .authorize_context = a};
  if (strcmp(name, "null-authorizer") == 0) {
    options.authorize_read = NULL;
    CHECK(UmiRecordTransferInspectorGtk4New(&options, &root) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(root == NULL);
    goto cleanup;
  }
  CHECK(ro_path(argv[2], name, a->path, true));
  OK(umi_data_server_create_memory(&source));
  OK(umi_data_server_create_sqlite(a->path, &writer));
  UmiRecordAmendment author = {.entity_id = "inspection.example",
                               .source = "Fictional source",
                               .actor = "Original author",
                               .reason = "Fictional record",
                               .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  OK(umi_record_document_append(source, &author, "first", 5, 0, &saved));
  OK(umi_record_bundle_capture(source, author.entity_id, &bundle));
  OK(umi_record_bundle_encode(bundle, &bytes, &length));
  char digest[65] = {0};
  memcpy(digest, bytes + length - 64, 64);
  UmiRecordTransferRequest request = {.receipt_id = "inspect-example",
                                      .attribution = {.source = "Inspection fixture",
                                                      .actor = "Import operator",
                                                      .reason = "Reviewed example",
                                                      .recorded_at_nanoseconds = 2}};
  UmiRecordTransferReceipt receipt;
  OK(umi_record_transfer_restore(writer, bundle, digest, &request, &receipt));
  if (strcmp(name, "corrupt-history") == 0 || strcmp(name, "view-corrupt-history") == 0) {
    char key[192];
    OK(umi_record_history_key(author.entity_id, 1, key, sizeof(key)));
    OK(umi_data_server_set(writer, key, "broken"));
  }
  umi_data_server_destroy(writer);
  writer = NULL;
  if (strcmp(name, "missing-store") == 0) {
    size_t n = strlen(a->path);
    CHECK(n + 8 < sizeof(a->path));
    memcpy(a->path + n, ".missing", 9);
  }
  OK(UmiRecordTransferInspectorGtk4New(&options, &root));
  g_object_ref_sink(root);
  a->widget = root;
  g_signal_connect(root, "operation-finished", G_CALLBACK(finished), a);
  gtk_editable_set_text(GTK_EDITABLE(find(root, "receipt-inspector-database")), a->path);
  gtk_editable_set_text(GTK_EDITABLE(find(root, "receipt-inspector-id")), "inspect-example");
  if (strcmp(name, "relative") == 0) {
    CHECK(UmiRecordTransferInspectorGtk4Read(root, "relative.sqlite", "inspect-example", false) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(a->calls == 0);
    goto cleanup;
  }
  if (strcmp(name, "invalid-id") == 0) {
    CHECK(UmiRecordTransferInspectorGtk4Read(root, a->path, "bad.id", false) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(a->calls == 0);
    goto cleanup;
  }
  memcpy(caller_path, a->path, sizeof(caller_path));
  a->caller_path = caller_path;
  bool verify = strcmp(name, "verify") == 0 || strcmp(name, "corrupt-history") == 0;
  UmiStatus admitted;
  if (strcmp(name, "buttons") == 0) {
    g_signal_emit_by_name(find(root, "receipt-inspector-view"), "clicked");
    OK(UmiRecordTransferInspectorGtk4StateRead(root, &state));
    admitted = state.status;
  } else if (strcmp(name, "missing-receipt") ==
             0) { /* Authorization still selects the requested exact identity. Use a missing store
                     row with the expected identifier. */
    OK(umi_data_server_create_sqlite(a->path, &writer));
    char key[192];
    OK(umi_record_history_key("transfer.receipt.inspect-example", 0, key, sizeof(key)));
    OK(umi_data_server_delete(writer, key));
    umi_data_server_destroy(writer);
    writer = NULL;
    admitted = UmiRecordTransferInspectorGtk4Read(root, caller_path, "inspect-example", false);
  } else
    admitted = UmiRecordTransferInspectorGtk4Read(root, caller_path, "inspect-example", verify);
  if (strcmp(name, "denied") == 0) {
    CHECK(admitted == UMI_STATUS_PERMISSION_DENIED);
    CHECK(a->calls == 1 && a->completions == 0);
    goto cleanup;
  }
  if (strcmp(name, "edit-authorize") == 0 || strcmp(name, "retire-authorize") == 0) {
    CHECK(admitted == UMI_STATUS_CANCELLED);
    CHECK(a->completions == 0);
    goto cleanup;
  }
  CHECK(admitted == UMI_STATUS_OK);
  CHECK(!UmiRecordTransferInspectorGtk4CanClose(root));
  if (strcmp(name, "busy") == 0)
    CHECK(UmiRecordTransferInspectorGtk4Read(root, a->path, "inspect-example", false) ==
          UMI_STATUS_BUSY);
  if (strcmp(name, "reenter-authorize") == 0)
    CHECK(a->nested == UMI_STATUS_BUSY);
  if (strcmp(name, "changed-read") == 0)
    gtk_editable_set_text(GTK_EDITABLE(find(root, "receipt-inspector-id")), "later");
  if (strcmp(name, "retired-read") == 0)
    UmiRecordTransferInspectorGtk4Retire(root);
  if (strcmp(name, "disposed-read") == 0)
    g_object_run_dispose(G_OBJECT(root));
  CHECK(idle(root));
  OK(UmiRecordTransferInspectorGtk4StateRead(root, &state));
  CHECK(UmiRecordTransferInspectorGtk4CanClose(root));
  if (strcmp(name, "changed-read") == 0 || strcmp(name, "retired-read") == 0 ||
      strcmp(name, "disposed-read") == 0) {
    CHECK(state.status == UMI_STATUS_CANCELLED && !state.has_receipt);
    goto cleanup;
  }
  if (strcmp(name, "missing-store") == 0 || strcmp(name, "missing-receipt") == 0 ||
      strcmp(name, "corrupt-history") == 0) {
    CHECK(state.status != UMI_STATUS_OK && !state.has_receipt);
    if (strcmp(name, "missing-store") == 0) {
      FILE *probe = fopen(a->path, "rb");
      bool absent = probe == NULL;
      if (probe)
        fclose(probe);
      CHECK(absent);
    }
    goto cleanup;
  }
  CHECK(state.status == UMI_STATUS_OK && state.has_receipt &&
        state.display_status == UMI_STATUS_OK);
  CHECK(strcmp(state.operation_path, a->path) == 0);
  CHECK(strcmp(state.receipt.receipt.amendment.actor, "Import operator") == 0);
  CHECK(find(root, "receipt-identity") != NULL);
  if (strcmp(name, "completion-reentry") == 0)
    CHECK(a->completions == 2 && a->followup == UMI_STATUS_OK && state.verified);
  else
    CHECK(a->completions == 1 && state.verified == verify);
  if (strcmp(name, "buttons") == 0 || strcmp(name, "view-corrupt-history") == 0) {
    g_signal_emit_by_name(find(root, "receipt-inspector-verify"), "clicked");
    CHECK(idle(root));
    OK(UmiRecordTransferInspectorGtk4StateRead(root, &state));
    if (strcmp(name, "buttons") == 0)
      CHECK(state.status == UMI_STATUS_OK && state.verified);
    else
      CHECK(state.status != UMI_STATUS_OK && !state.has_receipt);
  }
  if (strcmp(name, "changed-after-read") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(find(root, "receipt-inspector-id")), "later");
    OK(UmiRecordTransferInspectorGtk4StateRead(root, &state));
    CHECK(!state.has_receipt && !state.verified);
    CHECK(gtk_widget_get_first_child(find(root, "receipt-inspector-preview")) == NULL);
  }
cleanup:
  /* On timeout the process exits with the callback context still alive; never
   * free a borrowed context while an accepted worker retains its widget. */
  if (root && !idle(root))
    return 1;
  if (root)
    g_object_unref(root);
  g_free(a);
  umi_record_bundle_bytes_free(bytes);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(writer);
  umi_data_server_destroy(source);
  return result;
}
