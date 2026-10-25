/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/document_whitespace.h
 *
 * PURPOSE:
 *   Expose immutable whitespace review and explicit application to native editing hosts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_DOCUMENT_WHITESPACE_H
#define UMICOM_UI_GTK4_DOCUMENT_WHITESPACE_H
#include "umicom/document/whitespace.h"
#include "umicom/document/whitespace_commands.h"
#include "umicom/ui/gtk4/document_commands.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Open one immutable comparison per editing binding. Zero selects the active
 * document once; later tab/caret changes do not redirect the proposal. Native
 * observers may retire the binding: every Apply rechecks its original owner.
 * Closing does not edit. Successful opening does not mean the user applied. */
UmiStatus UmiGtk4AdapterReviewWhitespace(UmiGtk4Adapter *adapter, UmiDocumentId id,
                                         const UmiEditorWhitespaceOptions *options);
/* Resolve a shared command with the captured document indentation width. */
UmiStatus UmiGtk4AdapterReviewWhitespaceCommand(UmiGtk4Adapter *adapter, const char *command_id);
/* Read the last application outcome. An untouched review is UNAVAILABLE.
 * The applied output distinguishes committed text from interrupted refresh. */
UmiStatus UmiGtk4WhitespaceReviewStatus(GtkWindow *window, int *out_applied);
#ifdef __cplusplus
}
#endif
#endif
