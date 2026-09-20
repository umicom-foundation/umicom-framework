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
#include "umicom/document/save_session.h"
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
/** Receives a borrowed progress snapshot for this callback only. Copy it to
 * retain it. Completion runs on the GTK/document owner thread, after the run
 * stops; saved files are not rolled back on failure or cancellation. */
typedef void (*UmiGtk4DocumentSaveResultFn)(void *context,
    const UmiDocumentSaveProgress *progress);

/** Begin Save All over the currently bound coordinator. At most one save run
 * and no outstanding clipboard read may exist at start. Returns OK when
 * scheduling succeeds, not when all files are saved. Each named save runs in
 * a separate idle dispatch; untitled documents use GTK's save-file dialog.
 * The callback/context must live until completion or binding teardown.
 * Teardown cancels work and suppresses callbacks to the detached context. */
UmiStatus UmiGtk4AdapterDocumentSaveAll(UmiGtk4Adapter *adapter,
    UmiGtk4DocumentSaveResultFn completed, void *context);
/** Request cancellation without discarding unsaved documents. A synchronous
 * write already in progress cannot be interrupted. Safe when no run exists. */
UmiStatus UmiGtk4AdapterCancelDocumentSaveAll(UmiGtk4Adapter *adapter);
/** Copy the current Save All counters on the GTK owner thread without advancing
 * the run, reading a clipboard or writing a file. The output owns its copied
 * values. NOT_FOUND means there is no active run; output is then unchanged.
 * A completed run is removed before its completion callback: retain that
 * callback's final snapshot for a permanent summary. Example: the native
 * document-saving tests query progress before their first event-loop step. */
UmiStatus UmiGtk4AdapterDocumentSaveAllProgress(const UmiGtk4Adapter *adapter,
    UmiDocumentSaveProgress *outProgress);

/** Query whether this binding has a queued save, active write or filename prompt. */
int UmiGtk4AdapterDocumentSaveAllBusy(const UmiGtk4Adapter *adapter);


/** Select the adjacent managed source (+1 next, -1 previous), then refresh and
 * deliver the existing editing completion. Does not read the clipboard or save.
 * A pending paste retains its original captured document independently. */
UmiStatus UmiGtk4AdapterCycleDocument(UmiGtk4Adapter *adapter, int direction);
#ifdef __cplusplus
}
#endif
#endif
