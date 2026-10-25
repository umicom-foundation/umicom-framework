/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_form_record_import_gtk4.c
 *
 * PURPOSE:
 *   Exercise the actual native review/confirmation flow and captured write lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Exercise the actual native review/confirmation flow and captured write lifetime. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/record_bundle_files.h"
#include "umicom/data/structured_form_record.h"
#include "umicom/ui/gtk4/form_record_import.h"
#include <glib/gstdio.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
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
  const char *path, *mode;
  int calls, completions;
  UmiStatus nested, followup;
  UmiRecordTransferRequest *request;
} Authorization;
static UmiStatus authorize(const char *path, const UmiStructuredFormBundleSummary *summary,
                           void *context) {
  Authorization *a = context;
  ++a->calls;
  if (strcmp(path, a->path) != 0 ||
      strcmp(summary->latest.amendment.entity_id, "import.parcel") != 0 ||
      summary->revision_count != 2)
    return UMI_STATUS_INVALID_STATE;
  if (strcmp(a->mode, "receipt-mutate-authorize") == 0 && a->request)
    memcpy(a->request->attribution.actor, "Later caller", sizeof("Later caller"));
  if (strcmp(a->mode, "receipt-visible-authorize") == 0)
    gtk_editable_set_text(GTK_EDITABLE(find(a->widget, "form-import-receipt-actor")),
                          "Later operator");
  if (strcmp(a->mode, "denied") == 0 || strcmp(a->mode, "receipt-denied") == 0)
    return UMI_STATUS_PERMISSION_DENIED;
  if (strcmp(a->mode, "retire-authorize") == 0)
    UmiFormRecordImportGtk4Retire(a->widget);
  if (strcmp(a->mode, "edit-authorize") == 0)
    gtk_editable_set_text(GTK_EDITABLE(find(a->widget, "form-import-database")), "another.sqlite3");
  if (strcmp(a->mode, "withdraw-authorize") == 0)
    gtk_check_button_set_active(GTK_CHECK_BUTTON(find(a->widget, "form-bundle-confirm")), FALSE);
  if (strcmp(a->mode, "reenter-authorize") == 0)
    a->nested = UmiFormRecordImportGtk4Restore(a->widget, path);
  return UMI_STATUS_OK;
}
static void finished(GtkWidget *widget, int operation, int status, gpointer context) {
  Authorization *a = context;
  ++a->completions;
  if (strcmp(a->mode, "completion-reentry") == 0 && operation == UMI_FORM_IMPORT_REVIEW &&
      status == UMI_STATUS_OK) {
    gtk_check_button_set_active(GTK_CHECK_BUTTON(find(widget, "form-bundle-confirm")), TRUE);
    a->followup = UmiFormRecordImportGtk4Restore(widget, a->path);
  }
}
static int idle(GtkWidget *widget) {
  gint64 until = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  UmiFormRecordImportGtk4State state;
  while (UmiFormRecordImportGtk4StateRead(widget, &state) == UMI_STATUS_OK && state.busy &&
         g_get_monotonic_time() < until) {
    (void)g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return UmiFormRecordImportGtk4StateRead(widget, &state) == UMI_STATUS_OK && !state.busy;
}
int main(int argc, char **argv) {
  const char *cases[] = {"receipt-buttons",
                         "receipt-explicit",
                         "receipt-source",
                         "receipt-empty",
                         "receipt-invalid",
                         "receipt-collision",
                         "receipt-retire",
                         "receipt-later-edit",
                         "receipt-mutate-authorize",
                         "receipt-visible-authorize",
                         "receipt-denied",
                         "receipt-time",
                         "receipt-null",
                         "receipt-read-invalidated",
                         "receipt-utf8",
                         "review",
                         "restore",
                         "buttons",
                         "unconfirmed",
                         "denied",
                         "retire-authorize",
                         "edit-authorize",
                         "withdraw-authorize",
                         "reenter-authorize",
                         "completion-reentry",
                         "changed-read",
                         "retire-read",
                         "dispose-read",
                         "retire-write",
                         "edit-write",
                         "collision",
                         "bad-file",
                         "wrong-namespace",
                         "relative",
                         "null-authorizer",
                         "double-admission",
                         "read-no-write"};
  if (argc != 3)
    return 2;
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected)
    return 2;
  if (!gtk_init_check())
    return 77;
  const char *name = argv[1];
  int result = 0;
  FormFixture *f = form_fixture_new();
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiDataServer *source = NULL, *target = NULL;
  UmiRecordBundle *bundle = NULL;
  GtkWidget *root = NULL;
  char *directory = NULL, *file = NULL, *database = NULL, *other = NULL;
  UmiRecordDocument *document = malloc(sizeof(*document));
  UmiFormRecordImportGtk4State state;
  CHECK(f && document);
  CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
  directory = g_build_filename(argv[2], "form-import-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory));
  file = g_build_filename(directory, "history.umibundle", NULL);
  database = g_build_filename(directory, "destination.sqlite3", NULL);
  other = g_build_filename(directory, "other.sqlite3", NULL);
  if (strcmp(name, "read-no-write") == 0)
    CHECK(g_file_set_contents(database, "untouched destination", -1, NULL));
  const UmiStructuredFormSchema *schemas[] = {&f->schema};
  OK(umi_structured_form_catalogue_create(schemas, 1, &catalogue));
  OK(umi_data_server_create_memory(&source));
  UmiRecordAmendment metadata = {.entity_id = "import.parcel",
                                 .source = "Synthetic source",
                                 .actor = "Example author",
                                 .reason = "Original",
                                 .recorded_at_nanoseconds = 100};
  if (strcmp(name, "wrong-namespace") == 0)
    memcpy(metadata.entity_id, "other.parcel", 13);
  UmiFormAnswer answer = form_mass(7);
  UmiRecordRevision saved;
  OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
  OK(umi_structured_form_record_save(source, &metadata, &f->schema, &f->before, 0, &saved));
  answer = form_mass(9);
  metadata.recorded_at_nanoseconds = 200;
  OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
  OK(umi_structured_form_record_save(source, &metadata, &f->schema, &f->before, 1, &saved));
  OK(umi_record_bundle_capture(source, metadata.entity_id, &bundle));
  if (strcmp(name, "bad-file") == 0)
    CHECK(g_file_set_contents(file, "not a record bundle", -1, NULL));
  else
    OK(umi_record_bundle_write_new(file, bundle));
  Authorization auth = {.path = database,
                        .mode = name,
                        .nested = UMI_STATUS_INTERNAL_ERROR,
                        .followup = UMI_STATUS_INTERNAL_ERROR};
  UmiFormRecordImportGtk4Options options = {.catalogue = catalogue,
                                            .form_id = "example.parcel",
                                            .entity_namespace = "import.",
                                            .authorize_restore = authorize,
                                            .authorize_context = &auth};
  if (strcmp(name, "null-authorizer") == 0) {
    options.authorize_restore = NULL;
    CHECK(UmiFormRecordImportGtk4New(&options, &root) == UMI_STATUS_INVALID_ARGUMENT &&
          root == NULL);
    goto cleanup;
  }
  if (strncmp(name, "receipt-", 8) == 0 && strcmp(name, "receipt-explicit") != 0)
    options.receipt_source = "Synthetic import host";
  OK(UmiFormRecordImportGtk4New(&options, &root));
  g_object_ref_sink(root);
  auth.widget = root;
  g_signal_connect(root, "operation-finished", G_CALLBACK(finished), &auth);
  GtkWidget *file_entry = find(root, "form-import-file"),
            *database_entry = find(root, "form-import-database");
  CHECK(file_entry && database_entry);
  if (options.receipt_source != NULL && strcmp(name, "receipt-empty") != 0) {
    GtkWidget *id = find(root, "form-import-receipt-id"),
              *actor = find(root, "form-import-receipt-actor"),
              *reason = find(root, "form-import-receipt-reason");
    CHECK(id && actor && reason);
    gtk_editable_set_text(GTK_EDITABLE(id),
                          strcmp(name, "receipt-invalid") == 0 ? "bad.id" : "native-1");
    gtk_editable_set_text(GTK_EDITABLE(actor), "Import operator");
    gtk_editable_set_text(GTK_EDITABLE(reason), "Explicit review");
  }
  gtk_editable_set_text(GTK_EDITABLE(file_entry), file);
  gtk_editable_set_text(GTK_EDITABLE(database_entry), database);
  if (strcmp(name, "relative") == 0) {
    CHECK(UmiFormRecordImportGtk4Review(root, "relative.umibundle") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFormRecordImportGtk4CanClose(root));
    goto cleanup;
  }
  if (strcmp(name, "buttons") == 0)
    g_signal_emit_by_name(find(root, "form-import-review"), "clicked");
  else
    OK(UmiFormRecordImportGtk4Review(root, file));
  OK(UmiFormRecordImportGtk4StateRead(root, &state));
  CHECK(state.busy && !UmiFormRecordImportGtk4CanClose(root));
  if (strcmp(name, "double-admission") == 0) {
    CHECK(UmiFormRecordImportGtk4Review(root, file) == UMI_STATUS_BUSY);
    CHECK(UmiFormRecordImportGtk4Restore(root, database) == UMI_STATUS_BUSY);
  }
  if (strcmp(name, "changed-read") == 0)
    gtk_editable_set_text(GTK_EDITABLE(file_entry), other);
  if (strcmp(name, "retire-read") == 0)
    UmiFormRecordImportGtk4Retire(root);
  if (strcmp(name, "receipt-read-invalidated") == 0)
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-import-receipt-actor")),
                          "Changed during read");
  if (strcmp(name, "dispose-read") == 0)
    g_object_run_dispose(G_OBJECT(root));
  CHECK(idle(root));
  OK(UmiFormRecordImportGtk4StateRead(root, &state));
  if (strcmp(name, "receipt-read-invalidated") == 0 || strcmp(name, "changed-read") == 0 ||
      strcmp(name, "retire-read") == 0 || strcmp(name, "dispose-read") == 0 ||
      strcmp(name, "bad-file") == 0 || strcmp(name, "wrong-namespace") == 0) {
    CHECK(state.status != UMI_STATUS_OK && !state.has_review &&
          !g_file_test(database, G_FILE_TEST_EXISTS));
    CHECK(auth.calls == 0 && auth.completions == 1);
    goto cleanup;
  }
  OK(state.status);
  if (strcmp(name, "completion-reentry") == 0) {
    OK(auth.followup);
    CHECK(auth.completions == 2 && auth.calls == 1);
    goto verify;
  }
  CHECK(state.has_review && state.reviewed.revision_count == 2 && auth.calls == 0 &&
        auth.completions == 1);
  if (strcmp(name, "read-no-write") == 0) {
    char *content = NULL;
    gsize length = 0;
    CHECK(g_file_get_contents(database, &content, &length, NULL));
    int unchanged = length == 21 && memcmp(content, "untouched destination", 21) == 0;
    g_free(content);
    CHECK(unchanged && auth.calls == 0);
    goto cleanup;
  }
  if (strcmp(name, "review") == 0 || strcmp(name, "double-admission") == 0) {
    CHECK(!g_file_test(database, G_FILE_TEST_EXISTS) && UmiFormRecordImportGtk4CanClose(root));
    goto cleanup;
  }
  if (strncmp(name, "receipt-", 8) == 0) {
    GtkWidget *confirmation = find(root, "form-bundle-confirm");
    CHECK(confirmation);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(confirmation), TRUE);
    UmiRecordTransferRequest request = {.receipt_id = "native-1",
                                        .attribution = {.source = "Synthetic import host",
                                                        .actor = "Import operator",
                                                        .reason = "Explicit review",
                                                        .recorded_at_nanoseconds = 300}};
    auth.request = &request;
    if (strcmp(name, "receipt-source") == 0)
      memcpy(request.attribution.source, "Wrong host", sizeof("Wrong host"));
    if (strcmp(name, "receipt-time") == 0)
      request.attribution.recorded_at_nanoseconds = 0;
    if (strcmp(name, "receipt-utf8") == 0) {
      request.attribution.actor[0] = (char)0xff;
      request.attribution.actor[1] = '\0';
    }
    if (strcmp(name, "receipt-collision") == 0) {
      OK(umi_data_server_create_sqlite(database, &target));
      UmiRecordAmendment occupied = metadata;
      memcpy(occupied.entity_id, "transfer.receipt.native-1", sizeof("transfer.receipt.native-1"));
      OK(umi_record_document_append(target, &occupied, "retained", 8, 0, &saved));
      umi_data_server_destroy(target);
      target = NULL;
    }
    UmiStatus admission;
    if (strcmp(name, "receipt-buttons") == 0) {
      g_signal_emit_by_name(find(root, "form-import-restore"), "clicked");
      OK(UmiFormRecordImportGtk4StateRead(root, &state));
      CHECK(state.busy);
      admission = state.status;
    } else if (strcmp(name, "receipt-empty") == 0 || strcmp(name, "receipt-invalid") == 0)
      admission = UmiFormRecordImportGtk4Restore(root, database);
    else
      admission = UmiFormRecordImportGtk4RestoreWithReceipt(
          root, database, strcmp(name, "receipt-null") == 0 ? NULL : &request);
    if (strcmp(name, "receipt-source") == 0 || strcmp(name, "receipt-empty") == 0 ||
        strcmp(name, "receipt-invalid") == 0 || strcmp(name, "receipt-time") == 0 ||
        strcmp(name, "receipt-null") == 0 || strcmp(name, "receipt-utf8") == 0) {
      CHECK(admission != UMI_STATUS_OK && auth.calls == 0 &&
            !g_file_test(database, G_FILE_TEST_EXISTS));
      goto cleanup;
    }
    if (strcmp(name, "receipt-visible-authorize") == 0 || strcmp(name, "receipt-denied") == 0) {
      CHECK(admission != UMI_STATUS_OK && auth.calls == 1 &&
            !g_file_test(database, G_FILE_TEST_EXISTS));
      goto cleanup;
    }
    OK(admission);
    if (strcmp(name, "receipt-retire") == 0)
      UmiFormRecordImportGtk4Retire(root);
    if (strcmp(name, "receipt-later-edit") == 0)
      gtk_editable_set_text(GTK_EDITABLE(find(root, "form-import-receipt-actor")),
                            "Later operator");
    CHECK(idle(root));
    OK(UmiFormRecordImportGtk4StateRead(root, &state));
    OK(umi_data_server_create_sqlite(database, &target));
    if (strcmp(name, "receipt-collision") == 0) {
      CHECK(state.status != UMI_STATUS_OK && !state.has_receipt);
      CHECK(umi_record_document_latest(target, "import.parcel", document) == UMI_STATUS_NOT_FOUND);
      OK(umi_record_document_latest(target, "transfer.receipt.native-1", document));
      CHECK(document->length == 8 && memcmp(document->content, "retained", 8) == 0);
      goto cleanup;
    }
    OK(state.status);
    CHECK(state.has_receipt && state.restored.revision == 2);
    if (strcmp(name, "receipt-retire") != 0) {
      OK(state.receipt_display_status);
      CHECK(find(root, "receipt-identity") != NULL && find(root, "receipt-operator") != NULL);
    }
    CHECK(strcmp(state.receipt.receipt.amendment.actor, "Import operator") == 0);
    CHECK(strcmp(state.receipt.imported_head.amendment.actor, "Example author") == 0);
    UmiRecordTransferReceipt checked;
    OK(umi_record_transfer_receipt_verify(target, "native-1", &checked));
    CHECK(checked.receipt.amendment.recorded_at_nanoseconds > 0);
    if (strcmp(name, "receipt-mutate-authorize") == 0)
      CHECK(strcmp(request.attribution.actor, "Later caller") == 0);
    goto cleanup;
  }
  if (strcmp(name, "unconfirmed") == 0) {
    CHECK(UmiFormRecordImportGtk4Restore(root, database) == UMI_STATUS_INVALID_STATE);
    CHECK(auth.calls == 0 && !g_file_test(database, G_FILE_TEST_EXISTS));
    goto cleanup;
  }
  GtkWidget *confirm = find(root, "form-bundle-confirm");
  CHECK(confirm);
  gtk_check_button_set_active(GTK_CHECK_BUTTON(confirm), TRUE);
  if (strcmp(name, "collision") == 0) {
    OK(umi_data_server_create_sqlite(database, &target));
    OK(umi_record_document_append(target, &metadata, "retained", 8, 0, &saved));
    umi_data_server_destroy(target);
    target = NULL;
  }
  UmiStatus status = UMI_STATUS_OK;
  if (strcmp(name, "buttons") == 0)
    g_signal_emit_by_name(find(root, "form-import-restore"), "clicked");
  else
    status = UmiFormRecordImportGtk4Restore(root, database);
  if (strcmp(name, "denied") == 0 || strcmp(name, "retire-authorize") == 0 ||
      strcmp(name, "edit-authorize") == 0 || strcmp(name, "withdraw-authorize") == 0) {
    CHECK(status ==
          (strcmp(name, "denied") == 0 ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_CANCELLED));
    OK(UmiFormRecordImportGtk4StateRead(root, &state));
    CHECK(!state.busy && !g_file_test(database, G_FILE_TEST_EXISTS));
    CHECK(auth.calls == 1 && auth.completions == 1);
    goto cleanup;
  }
  OK(status);
  if (strcmp(name, "reenter-authorize") == 0)
    CHECK(auth.nested == UMI_STATUS_BUSY);
  if (strcmp(name, "retire-write") == 0)
    UmiFormRecordImportGtk4Retire(root);
  if (strcmp(name, "edit-write") == 0)
    gtk_editable_set_text(GTK_EDITABLE(database_entry), other);
  CHECK(idle(root));
  OK(UmiFormRecordImportGtk4StateRead(root, &state));
  if (strcmp(name, "collision") == 0) {
    CHECK(state.status != UMI_STATUS_OK && state.restored.revision == 0);
    OK(umi_data_server_create_sqlite(database, &target));
    OK(umi_record_document_latest(target, "import.parcel", document));
    CHECK(document->length == 8 && memcmp(document->content, "retained", 8) == 0);
    goto cleanup;
  }
verify:
  OK(state.status);
  CHECK(state.restored.revision == 2 && strcmp(state.operation_path, database) == 0);
  CHECK(!g_file_test(other, G_FILE_TEST_EXISTS) && auth.calls == 1 && auth.completions == 2);
  OK(umi_data_server_create_sqlite(database, &target));
  OK(umi_record_document_load(target, "import.parcel", 1, document));
  CHECK(document->record.amendment.recorded_at_nanoseconds == 100);
  OK(umi_record_document_load(target, "import.parcel", 2, document));
  CHECK(document->record.amendment.recorded_at_nanoseconds == 200);
cleanup:
  /* Keep the old cleanup for review: a timeout must not free borrowed schema or
   * callback state while a worker may still use it. End this test process with
   * failure instead of manufacturing a lifetime crash during cleanup. */
#if 0
  if (root) { (void)idle(root); g_object_unref(root); }
#endif
  if (root && !idle(root)) {
    fprintf(stderr, "Worker timeout; retaining borrowed fixture state until process exit.\n");
    return 1;
  }
  if (root)
    g_object_unref(root);
  umi_data_server_destroy(target);
  umi_data_server_destroy(source);
  umi_record_bundle_destroy(bundle);
  umi_structured_form_catalogue_destroy(catalogue);
  free(f);
  free(document);
  g_free(other);
  g_free(database);
  g_free(file);
  g_free(directory);
  return result;
}
