/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Receipt views own literal provenance text and never mutate stored history. */
#include "umicom/ui/gtk4/record_transfer_receipt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  for (GtkWidget *c = gtk_widget_get_first_child(root); c; c = gtk_widget_get_next_sibling(c)) {
    GtkWidget *m = find(c, name);
    if (m)
      return m;
  }
  return NULL;
}
static bool readonly(GtkWidget *root) {
  if (GTK_IS_EDITABLE(root) || GTK_IS_TEXT_VIEW(root) || GTK_IS_BUTTON(root))
    return false;
  for (GtkWidget *c = gtk_widget_get_first_child(root); c; c = gtk_widget_get_next_sibling(c))
    if (!readonly(c))
      return false;
  return true;
}
int main(int argc, char **argv) {
  const char *cases[] = {"fields",  "owned", "readonly", "markup",       "utf8",         "clock",
                         "unknown", "null",  "output",   "wrong-widget", "changed-copy", "unicode"};
  if (argc != 2)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  const char *name = argv[1];
  int result = 0;
  UmiDataServer *source = NULL, *target = NULL;
  UmiRecordBundle *bundle = NULL;
  unsigned char *bytes = NULL;
  size_t length = 0;
  GtkWidget *root = NULL, *other = NULL;
  OK(umi_data_server_create_memory(&source));
  OK(umi_data_server_create_memory(&target));
  UmiRecordAmendment original = {.entity_id = "view.record",
                                 .source = "Example source",
                                 .actor = "<b>Original author</b>",
                                 .reason = "Original edit",
                                 .recorded_at_nanoseconds = 1};
  if (strcmp(name, "utf8") == 0) {
    original.actor[0] = (char)0xff;
    original.actor[1] = '\0';
  }
  if (strcmp(name, "unicode") == 0)
    memcpy(original.actor, "Caf\xc3\xa9 author", sizeof("Caf\xc3\xa9 author"));
  UmiRecordRevision saved;
  OK(umi_record_document_append(source, &original, "fictional", 9, 0, &saved));
  OK(umi_record_bundle_capture(source, original.entity_id, &bundle));
  OK(umi_record_bundle_encode(bundle, &bytes, &length));
  char digest[65] = {0};
  memcpy(digest, bytes + length - 64, 64);
  UmiRecordTransferRequest request = {.receipt_id = "view-example",
                                      .attribution = {.source = "Example import",
                                                      .actor = "Import operator",
                                                      .reason = "Reviewed file",
                                                      .recorded_at_nanoseconds = 2}};
  if (strcmp(name, "clock") == 0)
    request.attribution.recorded_at_nanoseconds = UINT64_MAX;
  UmiRecordTransferReceipt receipt, copy;
  OK(umi_record_transfer_restore(target, bundle, digest, &request, &receipt));
  if (strcmp(name, "null") == 0) {
    CHECK(UmiRecordTransferReceiptGtk4New(NULL, &root) == UMI_STATUS_INVALID_ARGUMENT &&
          root == NULL);
    CHECK(UmiRecordTransferReceiptGtk4New(&receipt, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  /* A refused construction must preserve an already owned host widget too. */
  if (strcmp(name, "output") == 0) {
    receipt.bundle_sha256[0] = receipt.bundle_sha256[0] == 'a' ? 'b' : 'a';
    root = gtk_label_new("Retain existing view");
    g_object_ref_sink(root);
    GtkWidget *previous = root;
    CHECK(UmiRecordTransferReceiptGtk4New(&receipt, &root) != UMI_STATUS_OK && root == previous);
    CHECK(strcmp(gtk_label_get_text(GTK_LABEL(root)), "Retain existing view") == 0);
    goto cleanup;
  }
  if (strcmp(name, "changed-copy") == 0) {
    memcpy(receipt.imported_head.amendment.actor, "Changed author", sizeof("Changed author"));
    CHECK(UmiRecordTransferReceiptGtk4New(&receipt, &root) != UMI_STATUS_OK && root == NULL);
    goto cleanup;
  }
  if (strcmp(name, "output") == 0 || strcmp(name, "changed-copy") == 0)
    receipt.bundle_sha256[0] = receipt.bundle_sha256[0] == 'a' ? 'b' : 'a';
  UmiStatus status = UmiRecordTransferReceiptGtk4New(&receipt, &root);
  if (strcmp(name, "utf8") == 0 || strcmp(name, "output") == 0 ||
      strcmp(name, "changed-copy") == 0) {
    CHECK(status != UMI_STATUS_OK && root == NULL);
    goto cleanup;
  }
  OK(status);
  g_object_ref_sink(root);
  if (strcmp(name, "wrong-widget") == 0) {
    other = gtk_label_new("Other");
    g_object_ref_sink(other);
    memset(&copy, 0x6a, sizeof(copy));
    UmiRecordTransferReceipt before = copy;
    CHECK(UmiRecordTransferReceiptGtk4Read(other, &copy) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&copy, &before, sizeof(copy)) == 0);
    goto cleanup;
  }
  if (strcmp(name, "owned") == 0) {
    memset(&receipt, 0, sizeof(receipt));
    umi_data_server_destroy(source);
    source = NULL;
    umi_data_server_destroy(target);
    target = NULL;
  }
  OK(UmiRecordTransferReceiptGtk4Read(root, &copy));
  CHECK(strcmp(copy.receipt.amendment.actor, "Import operator") == 0);
  const char *fields[] = {"receipt-identity",
                          "receipt-operator",
                          "receipt-source",
                          "receipt-reason",
                          "receipt-time",
                          "receipt-original-identity",
                          "receipt-original-revision",
                          "receipt-original-actor",
                          "receipt-original-source",
                          "receipt-original-reason",
                          "receipt-original-time",
                          "receipt-original-observed",
                          "receipt-digest",
                          "receipt-scope"};
  for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i) {
    GtkWidget *label = find(root, fields[i]);
    CHECK(label && GTK_IS_LABEL(label) && !gtk_label_get_use_markup(GTK_LABEL(label)));
  }
  CHECK(readonly(root));
  GtkWidget *actor = find(root, "receipt-original-actor"), *time = find(root, "receipt-time"),
            *observed = find(root, "receipt-original-observed");
  if (strcmp(name, "markup") == 0)
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(actor)), "<b>Original author</b>") != NULL);
  if (strcmp(name, "unicode") == 0)
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(actor)), "Caf\xc3\xa9 author") != NULL);
  if (strcmp(name, "clock") == 0)
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(time)), ".709551615 UTC") != NULL);
  if (strcmp(name, "unknown") == 0)
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(observed)), "Unknown") != NULL);
cleanup:
  if (other)
    g_object_unref(other);
  if (root)
    g_object_unref(root);
  umi_record_bundle_bytes_free(bytes);
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(target);
  umi_data_server_destroy(source);
  return result;
}
