/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/record_transfer_receipt_gtk4.c
 *
 * PURPOSE:
 *   Reusable receipt presentation keeps import labels distinct from original ones.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reusable receipt presentation keeps import labels distinct from original ones. */
#include "record_time_internal.h"
#include "umicom/ui/gtk4/record_transfer_receipt.h"
#include <string.h>
typedef struct UmiTransferReceiptRoot {
  GtkBox parent;
  UmiRecordTransferReceipt receipt;
} UmiTransferReceiptRoot;
typedef struct UmiTransferReceiptRootClass {
  GtkBoxClass parent_class;
} UmiTransferReceiptRootClass;
GType umi_transfer_receipt_root_get_type(void);
G_DEFINE_TYPE(UmiTransferReceiptRoot, umi_transfer_receipt_root, GTK_TYPE_BOX)
static void umi_transfer_receipt_root_class_init(UmiTransferReceiptRootClass *klass) {
  (void)klass;
}
static void umi_transfer_receipt_root_init(UmiTransferReceiptRoot *self) {
  gtk_orientable_set_orientation(GTK_ORIENTABLE(self), GTK_ORIENTATION_VERTICAL);
  gtk_box_set_spacing(GTK_BOX(self), 6);
}
static void text(GtkWidget *root, const char *name, const char *title, const char *value) {
  char *display = g_strdup_printf("%s: %s", title, value);
  GtkWidget *label = gtk_label_new(display);
  g_free(display);
  gtk_widget_set_name(label, name);
  gtk_label_set_xalign(GTK_LABEL(label), 0);
  gtk_label_set_wrap(GTK_LABEL(label), TRUE);
  gtk_label_set_selectable(GTK_LABEL(label), TRUE);
  gtk_box_append(GTK_BOX(root), label);
}
static bool utf8(const UmiRecordRevision *record) {
  return g_utf8_validate(record->amendment.entity_id, -1, NULL) &&
         g_utf8_validate(record->amendment.actor, -1, NULL) &&
         g_utf8_validate(record->amendment.source, -1, NULL) &&
         g_utf8_validate(record->amendment.reason, -1, NULL);
}
static void instant(GtkWidget *root, const char *name, const char *title, uint64_t time) {
  char *formatted = umi_gtk_record_time(time);
  text(root, name, title, formatted);
  g_free(formatted);
}
UmiStatus UmiRecordTransferReceiptGtk4New(const UmiRecordTransferReceipt *receipt,
                                          GtkWidget **out) {
  if (!out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_record_transfer_receipt_validate(receipt);
  if (status != UMI_STATUS_OK)
    return status;
  if (!utf8(&receipt->receipt) || !utf8(&receipt->imported_head))
    return UMI_STATUS_PARSE_ERROR;
  GtkWidget *root = g_object_new(umi_transfer_receipt_root_get_type(), NULL);
  ((UmiTransferReceiptRoot *)root)->receipt = *receipt;
  text(root, "receipt-identity", "Receipt record", receipt->receipt.amendment.entity_id);
  text(root, "receipt-operator", "Import operator label", receipt->receipt.amendment.actor);
  text(root, "receipt-source", "Import source", receipt->receipt.amendment.source);
  text(root, "receipt-reason", "Import reason", receipt->receipt.amendment.reason);
  instant(root, "receipt-time", "Import recorded time",
          receipt->receipt.amendment.recorded_at_nanoseconds);
  text(root, "receipt-original-identity", "Imported record",
       receipt->imported_head.amendment.entity_id);
  char *revision = g_strdup_printf("%" PRIu64, receipt->imported_head.revision);
  text(root, "receipt-original-revision", "Original final revision", revision);
  g_free(revision);
  text(root, "receipt-original-actor", "Original author label",
       receipt->imported_head.amendment.actor);
  text(root, "receipt-original-source", "Original source", receipt->imported_head.amendment.source);
  text(root, "receipt-original-reason", "Original reason", receipt->imported_head.amendment.reason);
  instant(root, "receipt-original-time", "Original recorded time",
          receipt->imported_head.amendment.recorded_at_nanoseconds);
  instant(root, "receipt-original-observed", "Original observed time",
          receipt->imported_head.amendment.observed_at_nanoseconds);
  text(root, "receipt-digest", "Reviewed bundle SHA-256", receipt->bundle_sha256);
  text(root, "receipt-scope", "Scope",
       "Copied local receipt. Labels are not authenticated identities; this view does not verify "
       "the store.");
  *out = root;
  return UMI_STATUS_OK;
}
UmiStatus UmiRecordTransferReceiptGtk4Read(GtkWidget *widget, UmiRecordTransferReceipt *out) {
  if (!widget || !out || !G_TYPE_CHECK_INSTANCE_TYPE(widget, umi_transfer_receipt_root_get_type()))
    return UMI_STATUS_INVALID_ARGUMENT;
  *out = ((UmiTransferReceiptRoot *)widget)->receipt;
  return UMI_STATUS_OK;
}
