/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/editor_config_adoption.h
 *
 * PURPOSE:
 *   Expose an explicit asynchronous configuration-to-document indentation review.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_EDITOR_CONFIG_ADOPTION_H
#define UMICOM_UI_GTK4_EDITOR_CONFIG_ADOPTION_H
/* This native API names GtkWindow directly. Declare it here so a consumer
 * can include this header first without relying on unrelated GTK includes. */
#include <gtk/gtk.h>
#include "umicom/document/coordinator.h"
#include "umicom/ui/gtk4.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Open a modeless, document-bound review. Zero selects the active saved file.
 * The explicit project root must contain that file. Inspect reads on a worker;
 * Apply re-reads before applying only the reviewed document indentation.
 * Root and file are copied and not editable in this window. One review per
 * editing binding. No project command, source write or automatic save occurs.
 * Retirement/cancellation prevents later mutation. GTK owner thread only.
 * OK means the form opened; it does not mean inspection or adoption succeeded. */
UmiStatus UmiGtk4AdapterReviewEditorConfig(UmiGtk4Adapter *adapter, const char *project_root,
                                           UmiDocumentId document_id);
/* Keep a window reference while querying; Busy includes worker retirement.
 * Status is UNAVAILABLE initially, BUSY during work and CANCELLED after close. */
int UmiGtk4EditorConfigAdoptionBusy(GtkWindow *window);
UmiStatus UmiGtk4EditorConfigAdoptionStatus(GtkWindow *window);
#ifdef __cplusplus
}
#endif
#endif
