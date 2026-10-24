/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Display copied import provenance without granting edit or execution authority. */
#ifndef UMICOM_UI_GTK4_RECORD_TRANSFER_RECEIPT_H
#define UMICOM_UI_GTK4_RECORD_TRANSFER_RECEIPT_H
#include "umicom/data/record_transfer.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Validate a coherent copied receipt and create a floating read-only GTK view.
 * Strings must be UTF-8 and are always displayed literally, without markup.
 * The widget owns its copy; no source pointers, store or credentials are retained.
 * A view is not proof of storage verification. Hosts perform receipt_load or
 * receipt_verify separately and present that operation's actual result. */
UmiStatus UmiRecordTransferReceiptGtk4New(const UmiRecordTransferReceipt *receipt, GtkWidget **out);
/* Return the captured copy without any database access. GTK owner thread only. */
UmiStatus UmiRecordTransferReceiptGtk4Read(GtkWidget *widget, UmiRecordTransferReceipt *out);
#ifdef __cplusplus
}
#endif
#endif
