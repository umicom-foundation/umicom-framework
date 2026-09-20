/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/document_commands.h
 * PURPOSE: Bind native editing and the system clipboard to DocumentCoordinator.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_DOCUMENT_COMMANDS_H
#define UMICOM_UI_GTK4_DOCUMENT_COMMANDS_H
#include "umicom/ui/gtk4.h"
#include "umicom/document/edit.h"
#include "umicom/document/navigation.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Called on the GTK owner thread after an operation or clipboard completion.
 * OK means a draft action completed; it does not mean the file was saved.
 * The callback is last: the host may detach its binding from this callback. */
typedef void (*UmiGtk4DocumentEditResultFn)(void *context, UmiStatus status);
/** Borrow the existing coordinator, never create a second document service.
 * Bind after creating the adapter window. The coordinator and context must
 * outlive the binding. Pass NULL to unbind BEFORE destroying either; adapter
 * or window destruction also invalidates pending reads. Rebinding cancels a
 * previous request. No clipboard is read until the user requests Paste. */
UmiStatus UmiGtk4AdapterBindDocumentEditing(UmiGtk4Adapter *adapter,
    UmiDocumentCoordinator *coordinator, UmiGtk4DocumentEditResultFn completed,
    void *context);
/** Check the active source document, not an arbitrary focused entry field.
 * Keyboard routing is installed on source editors only; other fields retain
 * their toolkit editing behaviour. False includes no document or a busy paste. */
int UmiGtk4AdapterDocumentCommandEnabled(UmiGtk4Adapter *adapter, const char *commandId);
/** Report whether a completion callback is installed on the editing binding.
 * This is a side-effect-free owner-thread query. Hosts can avoid showing the
 * same synchronous failure twice while retaining fallback errors when unbound. */
int UmiGtk4AdapterDocumentHasCompletion(UmiGtk4Adapter *adapter);
/** Navigate immediately in the active document using line or line:column.
 * Reports exactly once through a live editing completion binding, including
 * invalid input and missing-document errors. No clipboard request is made. */
UmiStatus UmiGtk4AdapterDocumentNavigate(UmiGtk4Adapter *adapter, const char *location);
/** Open a modal source-location form bound to the captured document identity.
 * Cancel and closing the form do not move the caret. A changed active document
 * rejects acceptance and leaves the form open with an explanation. Rebinding
 * or destruction closes the form through the existing lifetime token. Only
 * one location form is open per binding. A successful return means the form
 * was opened; the completion callback reports a later navigation attempt.
 * Use a live adapter window on the GTK owner thread. */
UmiStatus UmiGtk4AdapterPromptDocumentLocation(UmiGtk4Adapter *adapter);

/** Execute an Edit menu command on the active source document. Paste returns
 * OK when the native read is requested, then reports its result through the
 * completion callback. Only one paste is pending per binding. Late data cannot
 * replace a changed/closed target or be redirected to a newly active tab.
 * GTK owns transfer allocation; the document limit is checked before editing.
 * Every immediate failure on a live callback binding reports completion once,
 * including unknown command IDs and missing documents. Invalid/unbound adapters
 * have no callback; callers must inspect the return status in that case.
 * Plain UTF-8 text only: no rich text, files or automatic clipboard logging. */
UmiStatus UmiGtk4AdapterDocumentCommand(UmiGtk4Adapter *adapter, const char *commandId);
#ifdef __cplusplus
}
#endif
#endif
