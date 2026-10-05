/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/library_exchange.h
 * PURPOSE: Native file exchange controls over the shared immutable layout review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_WORKSTATION_LIBRARY_EXCHANGE_H
#define UMICOM_UI_GTK4_WORKSTATION_LIBRARY_EXCHANGE_H
#include <gtk/gtk.h>
#include "umicom/ui/workspace_library_exchange.h"
G_BEGIN_DECLS
typedef struct UmiGtk4WorkspaceLibraryExchange UmiGtk4WorkspaceLibraryExchange;
/* Callbacks run on the GTK owner thread. Export follows the core measure/fill
 * contract; review returns an owned immutable review; apply publishes only a
 * candidate accepted by the native host. Context stays alive until a callback
 * returns, including when that callback destroys the controls. */
typedef UmiStatus (*UmiGtk4WorkspaceLibraryExportHandler)(char *, size_t, size_t *, void *);
typedef UmiStatus (*UmiGtk4WorkspaceLibraryImportHandler)(const void *, size_t,
    UmiUiWorkspaceLibraryImport **, void *);
typedef UmiStatus (*UmiGtk4WorkspaceLibraryImportApplyHandler)(const UmiUiWorkspaceLibraryImport *, void *);
UmiStatus umi_gtk4_ws_library_exchange_create(
    UmiGtk4WorkspaceLibraryExportHandler export_handler,
    UmiGtk4WorkspaceLibraryImportHandler import_handler,
    UmiGtk4WorkspaceLibraryImportApplyHandler apply_handler,
    void *context, UmiGtk4WorkspaceLibraryExchange **out_exchange);
/* Review an already-read archive, for example from a host drag-and-drop
 * action. The callback copies its bytes before returning; successful review
 * still requires the user's confirmation before Apply becomes available.
 * Refusal clears any previous review, preventing an old Apply from surviving
 * a failed attempt to review new content. No file dialog or I/O occurs here. */
UmiStatus umi_gtk4_ws_library_exchange_review_bytes(UmiGtk4WorkspaceLibraryExchange *exchange,
    const void *bytes, size_t size);
/* Optional explicit saved-store review. The callback returns owned evidence
 * through checkpoint_review; binding never performs I/O. Both saved and file
 * reviews use the existing apply callback and confirmation controls. NULL
 * disables the action. Rebinding discards earlier reviews; busy returns BUSY.
 * The callback runs on the GTK thread and may destroy the view. Its borrowed
 * context must survive until return. */
typedef UmiStatus (*UmiGtk4WorkspaceLibrarySavedReviewHandler)(
    UmiUiWorkspaceLibraryImport **out_review, void *context);
UmiStatus umi_gtk4_ws_library_exchange_set_saved_review_handler(
    UmiGtk4WorkspaceLibraryExchange *exchange,
    UmiGtk4WorkspaceLibrarySavedReviewHandler handler, void *context);
GtkWidget *umi_gtk4_ws_library_exchange_widget(UmiGtk4WorkspaceLibraryExchange *exchange);
/* Cancel native dialogs and outstanding I/O. Retained widgets become inert;
 * asynchronous completions release their own state without calling the owner. */
void umi_gtk4_ws_library_exchange_destroy(UmiGtk4WorkspaceLibraryExchange *exchange);
G_END_DECLS
#endif
