/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/document_inspection.h
 *
 * PURPOSE:
 *   Open an independent source observation for the active or explicitly named document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DOCUMENT_INSPECTION_H
#define UMICOM_UI_GTK4_DOCUMENT_INSPECTION_H
/* This native API names GtkWindow directly. Declare it here so a consumer
 * can include this header first without relying on unrelated GTK includes. */
#include <gtk/gtk.h>
#include "umicom/document/inspection.h"
#include "umicom/ui/gtk4/document_commands.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Capture the named draft (zero selects active) and show a read-only native
 * window. No Save, history, selection or provider operation is performed.
 * One open inspection per editing binding; close it before capturing again.
 * Initial owner retirement cancels construction. Later edits/tab changes do
 * not retarget the snapshot. Closing the parent destroys the native window.
 * GTK owner thread; the immutable source data belongs to that window. */
UmiStatus UmiGtk4AdapterInspectDocumentText(UmiGtk4Adapter *adapter, UmiDocumentId id);
/** Read the captured summary even from a retained, destroyed window reference.
 * This grants no permission to navigate or edit the current document. */
UmiStatus UmiGtk4DocumentInspectionRead(GtkWindow *window, UmiDocumentInspectionInfo *info,
                                        UmiEditorTextInspectionSummary *summary);
#ifdef __cplusplus
}
#endif
#endif
