/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reopen import provenance without giving the viewer a writable connection. */
#ifndef UMICOM_UI_GTK4_RECORD_TRANSFER_INSPECTOR_H
#define UMICOM_UI_GTK4_RECORD_TRANSFER_INSPECTOR_H
#include "umicom/platform/path.h"
#include "umicom/ui/gtk4/record_transfer_receipt.h"
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef UmiStatus (*UmiRecordTransferInspectAuthorize)(const char *database_path,
                                                       const char *receipt_id, bool verify_history,
                                                       void *context);
typedef struct UmiRecordTransferInspectorGtk4Options {
  UmiRecordTransferInspectAuthorize authorize_read;
  void *authorize_context;
} UmiRecordTransferInspectorGtk4Options;
typedef struct UmiRecordTransferInspectorGtk4State {
  UmiStatus status, display_status;
  bool busy, retired, has_receipt, verified;
  char operation_path[UMI_PATH_CAPACITY];
  char receipt_id[UMI_RECORD_TRANSFER_ID_CAPACITY];
  UmiRecordTransferReceipt receipt;
} UmiRecordTransferInspectorGtk4State;
/* All methods run on the GTK owner thread. The required authorization callback
 * runs before any I/O and receives copied inputs. It must authorize the selected
 * store and receipt; changing inputs or retiring during it revokes admission.
 * Its context remains borrowed for the widget lifetime. The floating widget
 * contains database Browse, receipt ID, View and Verify history controls.
 * View checks receipt integrity only. Verify additionally checks every original
 * imported revision. Neither action authenticates authors or imports changes. */
UmiStatus UmiRecordTransferInspectorGtk4New(const UmiRecordTransferInspectorGtk4Options *options,
                                            GtkWidget **out);
/* Returns admission status. Listen for operation-finished(UmiStatus integer)
 * or read State after busy becomes false for the asynchronous result. Paths
 * must be explicit absolute local paths. Accepted work opens a read-only store
 * on one worker and closes it there. Missing stores are never created.
 * Visible input edits invalidate pending results; the captured path remains in
 * the operation state. The host must keep the GTK context alive to completion. */
UmiStatus UmiRecordTransferInspectorGtk4Read(GtkWidget *widget, const char *database_path,
                                             const char *receipt_id, bool verify_history);
UmiStatus UmiRecordTransferInspectorGtk4StateRead(GtkWidget *widget,
                                                  UmiRecordTransferInspectorGtk4State *out);
bool UmiRecordTransferInspectorGtk4CanClose(GtkWidget *widget);
/* Permanent retirement discards pending read results. It does not interrupt
 * SQLite mid-operation. Completion still releases the worker-owned resources. */
void UmiRecordTransferInspectorGtk4Retire(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
