/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/form_record_import.h
 *
 * PURPOSE:
 *   Explicit native record import composes shared file, review and store services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Explicit native record import composes shared file, review and store services. */
#ifndef UMICOM_UI_GTK4_FORM_RECORD_IMPORT_H
#define UMICOM_UI_GTK4_FORM_RECORD_IMPORT_H
#include "umicom/platform/path.h"
#include "umicom/ui/gtk4/form_bundle_review.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef UmiStatus (*UmiFormRecordImportAuthorize)(const char *database_path,
                                                  const UmiStructuredFormBundleSummary *summary,
                                                  void *context);
typedef struct UmiFormRecordImportGtk4Options {
  const UmiStructuredFormCatalogue *catalogue;
  const char *form_id;
  const char *entity_namespace;
  UmiStructuredFormBundleKeyCheck key_check;
  void *key_context;
  UmiFormRecordImportAuthorize authorize_restore;
  void *authorize_context;
  /* Optional copied nonempty source label: require an atomic receipt, expose
   * receipt ID/operator/reason entries and capture UTC time at Restore. NULL
   * preserves the original unreceipted workflow. */
  const char *receipt_source;
} UmiFormRecordImportGtk4Options;
typedef enum UmiFormRecordImportOperation {
  UMI_FORM_IMPORT_NONE,
  UMI_FORM_IMPORT_REVIEW,
  UMI_FORM_IMPORT_RESTORE
} UmiFormRecordImportOperation;
typedef struct UmiFormRecordImportGtk4State {
  UmiStatus status;
  UmiFormRecordImportOperation operation;
  bool busy, retired, has_review;
  UmiStructuredFormBundleSummary reviewed;
  UmiRecordRevision restored;
  bool has_receipt;
  UmiRecordTransferReceipt receipt;
  /* Display errors never replace the actual storage result in status. */
  UmiStatus receipt_display_status;
  char operation_path[UMI_PATH_CAPACITY];
} UmiFormRecordImportGtk4State;
/* The catalogue/schemas and callback contexts are borrowed and must outlive the
 * widget AND its accepted work. key_check is a pure worker-thread callback.
 * authorize_restore is REQUIRED and runs on the GTK thread after confirmation;
 * it must authorize the explicit database and record, not show another chooser.
 * Reentry, retirement or input changes during authorization revoke admission.
 * All other options are copied. Paths are explicit, local absolute paths.
 * The floating widget includes Browse controls, review and restore buttons.
 * All public methods run on the GTK owner thread. No implicit record overwrite. */
UmiStatus UmiFormRecordImportGtk4New(const UmiFormRecordImportGtk4Options *options,
                                     GtkWidget **out);
/* Review loads bounded bytes and validates all revisions on a worker. These
 * functions return admission status; read State or listen for operation-finished
 * (operation integer, UmiStatus integer) to observe completion. Changing either
 * visible path invalidates the review and any pending read result.
 * Explicit retirement preserves completion handlers. GObject disposal removes
 * those handlers; a host retaining a disposed instance observes completion
 * through StateRead instead of expecting a signal from the disposed widget. */
UmiStatus UmiFormRecordImportGtk4Review(GtkWidget *widget, const char *bundle_path);
UmiStatus UmiFormRecordImportGtk4Restore(GtkWidget *widget, const char *database_path);
/* Explicit caller-supplied receipt, captured before authorization. The same
 * confirmation and destination checks apply. A configured receipt_source must
 * match the supplied source exactly. Other labels remain host-owned input. */
UmiStatus UmiFormRecordImportGtk4RestoreWithReceipt(GtkWidget *widget, const char *database_path,
                                                    const UmiRecordTransferRequest *request);
UmiStatus UmiFormRecordImportGtk4StateRead(GtkWidget *widget, UmiFormRecordImportGtk4State *out);
bool UmiFormRecordImportGtk4CanClose(GtkWidget *widget);
/* Retirement discards read results but does not cancel an admitted write.
 * Keep the main context and borrowed callback/schema lifetimes until busy=false.
 * The completed state reports the CAPTURED destination even after later typing.
 * A new database may be created; failures can leave an empty SQLite file, but
 * existing rows are never replaced and the complete restore is one transaction. */
void UmiFormRecordImportGtk4Retire(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
