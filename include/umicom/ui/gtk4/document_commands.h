/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/document_commands.h
 * PURPOSE: Bind native editing and the system clipboard to DocumentCoordinator.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_DOCUMENT_COMMANDS_H
#define UMICOM_UI_GTK4_DOCUMENT_COMMANDS_H
#include "umicom/document/replacement_session.h"
#include "umicom/ui/gtk4.h"
#include "umicom/document/edit.h"
#include "umicom/document/close.h"
#include "umicom/document/close_session.h"
#include "umicom/document/save_session.h"
#include "umicom/document/navigation.h"
#include "umicom/document/delimiter_navigation.h"
#include "umicom/document/reopen.h"
#include "umicom/document/navigation_history.h"
#include "umicom/language_runtime/navigation_query.h"
#include "umicom/language_runtime/call_query.h"
#include "umicom/editor/search_engine.h"
#include "umicom/document/line_edit.h"
#include "umicom/document/format.h"
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


/** Query whether the shared closed-document history can be changed now.
 * Reopen and Forget are unavailable during Paste, Save All or reviewed Close.
 * Querying does not read the file; a missing file is reported upon Reopen. */
int UmiGtk4AdapterReopenDocumentEnabled(UmiGtk4Adapter *adapter);
/** Reload the newest saved path through the bound coordinator. Reuses its
 * current working copy if already open and never restores discarded text.
 * An immediate result is reported once through the live editing completion;
 * completion is last and may detach the host. A detached binding is silent. */
UmiStatus UmiGtk4AdapterReopenDocument(UmiGtk4Adapter *adapter);
/** Forget only the newest closed path, without deleting or writing a file.
 * Uses the same busy checks and completion contract as Reopen. */
UmiStatus UmiGtk4AdapterForgetClosedDocument(UmiGtk4Adapter *adapter);

/** Select the adjacent managed source (+1 next, -1 previous), then refresh and
 * deliver the existing editing completion. Does not read the clipboard or save.
 * A pending paste retains its original captured document independently. */
UmiStatus UmiGtk4AdapterCycleDocument(UmiGtk4Adapter *adapter, int direction);
/** Request a reviewed close for viewId (NULL selects the active source).
 * Clean managed documents close through their coordinator. Pending documents
 * show Save and Close, Discard and Close, and Cancel, with Cancel the default.
 * Untitled Save requests use the native filename chooser. A stale decision
 * never discards newer text; a failed Save never becomes a forced close.
 * One close question is allowed per editing binding. Save All/pending Paste
 * block a new question; they are rechecked before applying its decision.
 * Existing tab-pin policy is enforced by the tab caller, not this File action.
 * Completion uses the editing callback; OK from this request may mean only
 * that the question opened. Cancel reports CANCELLED. Rebind or destruction
 * dismisses questions and invalidates late filename replies without calling
 * the detached host. Use only on the GTK/document owner thread. */
UmiStatus UmiGtk4AdapterRequestDocumentClose(UmiGtk4Adapter *adapter,
    const char *viewId);

/** Result of one Close All/Close Others run. Snapshot is borrowed for the
 * callback only; copy it for a report. Called after detaching the finished run;
 * the callback may unbind or destroy the host. Teardown suppresses callbacks. */
typedef void (*UmiGtk4DocumentCloseResultFn)(void *context,
    const UmiDocumentCloseProgress *progress);

/** Capture the bound sources and schedule one operation per GTK idle dispatch.
 * OTHERS keeps the originally active source, even if tabs change afterwards.
 * Each pending draft uses the same Save/Discard/Cancel form as File Close.
 * Cancellation/failure stops later work, without reopening already closed
 * sources. New sources opened during the run are not captured. Explicit File
 * closing includes pinned sources. The callback context and coordinator must
 * outlive the binding. All calls require the GTK owner thread/main context. */
UmiStatus UmiGtk4AdapterCloseDocuments(UmiGtk4Adapter *adapter,
    UmiDocumentCloseScope scope, UmiGtk4DocumentCloseResultFn completed,
    void *context);

/** Stop a group close, dismiss its question and cancel a pending filename
 * chooser. Already completed saves/closes are not rolled back. No-op when no
 * group run is active. A synchronous write cannot be interrupted. */
UmiStatus UmiGtk4AdapterCancelCloseDocuments(UmiGtk4Adapter *adapter);

/** Group-close query only; single-document questions are not group runs. */
int UmiGtk4AdapterCloseDocumentsBusy(const UmiGtk4Adapter *adapter);

/** True for a single close question or a queued/active group close. Save All
 * and asynchronous Paste use this boundary to avoid overlapping decisions. */
int UmiGtk4AdapterDocumentCloseBusy(const UmiGtk4Adapter *adapter);

/** Copy current group progress without advancing work. NOT_FOUND after the run
 * detaches; retain the final callback copy instead. Output is unchanged on
 * error. No source text, screenshots or automatic uploads are involved. */
UmiStatus UmiGtk4AdapterCloseDocumentsProgress(const UmiGtk4Adapter *adapter,
    UmiDocumentCloseProgress *outProgress);

/** direction is -1 for Back, +1 for Forward, or zero for Clear History.
 * Availability is false during Paste, Save All and reviewed Close. A destination
 * may later be unavailable if its tab closes or its source line is removed. */
int UmiGtk4AdapterNavigationHistoryEnabled(UmiGtk4Adapter *adapter, int direction);
/** Apply Back/Forward through the bound document owner. Completion uses the
 * existing editing callback exactly once while the binding is alive. */
UmiStatus UmiGtk4AdapterTravelDocument(UmiGtk4Adapter *adapter, int direction);
/** Clear location metadata without changing any open source or its undo stack. */
UmiStatus UmiGtk4AdapterClearNavigationHistory(UmiGtk4Adapter *adapter);

/* Open a modal, per-document replacement review over the currently open
 * sources. One preparation runs per idle dispatch; each changed draft requires
 * Apply or Skip. Stop preserves earlier accepted edits. No file is saved.
 * The existing edit binding owns lifetime. Rebinding/teardown cancels queued
 * work and makes retained controls inert. The editing completion callback runs
 * once when the form closes while its binding remains alive. An immediate
 * failure is returned directly and has not opened a form. Use the GTK thread. */
UmiStatus UmiGtk4AdapterReviewOpenReplacements(UmiGtk4Adapter *adapter,
    const char *needle, const char *replacement);
/* Review the complete captured set, mark each changed comparison reviewed,
 * then explicitly approve one atomic draft update. Uses the same editing
 * binding and busy guards as sequential replacement; neither workflow saves. */
UmiStatus UmiGtk4AdapterReviewReplacementSet(UmiGtk4Adapter *adapter,
    const char *needle, const char *replacement);
/** Explicit case/whole-word variants. Each review captures these values before
 * presenting a question; later toolbar changes do not alter that review.
 * NULL retains the legacy smart-case policy. No match-count limit is applied. */
UmiStatus UmiGtk4AdapterReviewOpenReplacementsWithOptions(UmiGtk4Adapter *adapter,
    const char *needle, const char *replacement, const UmiEditorSearchOptions *options);
UmiStatus UmiGtk4AdapterReviewReplacementSetWithOptions(UmiGtk4Adapter *adapter,
    const char *needle, const char *replacement, const UmiEditorSearchOptions *options);
int UmiGtk4AdapterReplacementReviewBusy(const UmiGtk4Adapter *adapter);
UmiStatus UmiGtk4AdapterReplacementReviewProgress(const UmiGtk4Adapter *adapter,
    UmiDocumentReplacementProgress *outProgress);
/* Explicitly stop and close a live review; no-op when none exists. */
UmiStatus UmiGtk4AdapterCancelReplacementReview(UmiGtk4Adapter *adapter);

/* Session bookmark actions share the editing completion and lifetime. -1/+1
 * visits previous/next; zero toggles the active line; two clears all metadata.
 * Availability and dispatch both reject overlapping native document decisions. */
int UmiGtk4AdapterBookmarkEnabled(UmiGtk4Adapter *adapter, int action);
UmiStatus UmiGtk4AdapterBookmarkCommand(UmiGtk4Adapter *adapter, int action);

/* Open a modal replacement form for the active document's nonempty selection.
 * The user types or pastes replacement text, previews both complete drafts and
 * explicitly approves Apply. This never fetches AI text or reads the clipboard
 * automatically. Original target identity survives caret/tab movement; changed
 * source, saved state or permissions rejects Apply. Successful Apply uses Undo
 * and does not save a file. Editing proposed text invalidates preview/approval.
 * One form belongs to the existing editing binding. Unbind before destroying
 * its coordinator; retained controls become inert. Completion reports Apply or
 * cancellation once, after the form closes, and may detach the host. Immediate
 * opening failures return directly. Use the GTK/document owner thread. */
UmiStatus UmiGtk4AdapterReviewSelectedCode(UmiGtk4Adapter *adapter);
int UmiGtk4AdapterDocumentProposalBusy(const UmiGtk4Adapter *adapter);
UmiStatus UmiGtk4AdapterCancelDocumentProposal(UmiGtk4Adapter *adapter);


/* Capture the active draft and open an explicit native completion review.
 * Enter a trusted language-server executable and project folder, request
 * suggestions, select one, preview the full result and approve Apply.
 * An existing selection is the fallback replacement range; with no selection
 * the fallback is insertion at the caret. Server edit ranges take precedence.
 * Request position is the end of the captured selection. Same-line selection
 * only. The workflow uses a temporary worker connection, not automatic typing
 * completion. Source is sent only after Request suggestions.
 * The document editing binding owns lifetime; unbind before destroying its
 * coordinator. Retained controls and late workers cannot edit a retired owner.
 * Completion reports accepted application or cancellation after closing.
 * Folder is an optional copied draft. Nothing launches when the form opens. */
UmiStatus UmiGtk4AdapterReviewCompletion(UmiGtk4Adapter *adapter, const char *workspace_folder);
/* Add explicit Save/Load controls for the captured document's language.
 * application_directory is a local product namespace such as "Studio".
 * base_override is NULL for user settings, or an absolute portable/test root.
 * Values are copied before the form is shown. Loading fills a draft only;
 * Request remains the separate approval to launch a server. Explicit saves
 * may finish after closing the form. Program paths and ordinary arguments are
 * stored as local JSON, not encrypted credentials. A concurrent UI observer
 * interrupting a load stops further field publication; review any loaded field. */
UmiStatus UmiGtk4AdapterReviewCompletionWithSettings(UmiGtk4Adapter *adapter,
    const char *workspace_folder,const char *application_directory,const char *base_override);

/* Formatting captures the complete active draft and offers tab width/spaces
 * controls, a full comparison and explicit approval before Apply. It shares
 * the completion review's busy/cancel ownership gate, local server settings,
 * stale-document checks and Undo path. Save remains separate. */
UmiStatus UmiGtk4AdapterReviewFormatting(UmiGtk4Adapter *adapter,const char *workspace_folder);
UmiStatus UmiGtk4AdapterReviewFormattingWithSettings(UmiGtk4Adapter *adapter,const char *workspace_folder,
    const char *application_directory,const char *base_override);

/* Capture source information at the active draft's caret using an explicit
 * temporary language-server request. Results are shown in a read-only text
 * view; markdown, HTML and code examples are literal text. No source edit is
 * staged. This shares the source review busy/cancel gate and captured-source
 * checks. Writable and read-only drafts are supported through an inspection-only capture.
 * The WithSettings form uses the same explicit local Save/Load workflow. */
UmiStatus UmiGtk4AdapterReviewSourceInformation(UmiGtk4Adapter *adapter,const char *workspace_folder);
UmiStatus UmiGtk4AdapterReviewSourceInformationWithSettings(UmiGtk4Adapter *adapter,const char *workspace_folder,
    const char *application_directory,const char *base_override);

/* Inspect definition/reference locations at the captured caret. Results are
 * listed without opening files. Open selected location explicitly resolves a
 * local target through the existing document provider and preserves unsaved
 * drafts. Read-only source and target documents permit navigation. This uses
 * the same source-tool busy/cancel gate; it never stages or applies source edits.
 * Server settings and owner lifetime follow the other source review forms. */
UmiStatus UmiGtk4AdapterReviewSourceNavigation(UmiGtk4Adapter *adapter,const char *workspace_folder,
    UmiLanguageNavigationKind kind);
UmiStatus UmiGtk4AdapterReviewSourceNavigationWithSettings(UmiGtk4Adapter *adapter,const char *workspace_folder,
    UmiLanguageNavigationKind kind,const char *application_directory,const char *base_override);

/* Capture the current complete draft, request its document symbols explicitly,
 * and present searchable names with hierarchy indentation. Opening a selected
 * row uses the document navigation owner. A flat symbol opens its starting
 * caret; a hierarchical symbol selects its identifier range. No edit approval
 * is exposed. This shares the source-tool busy gate and server preferences. */
UmiStatus UmiGtk4AdapterReviewDocumentSymbols(UmiGtk4Adapter *adapter,const char *workspace_folder);
UmiStatus UmiGtk4AdapterReviewDocumentSymbolsWithSettings(UmiGtk4Adapter *adapter,const char *workspace_folder,
    const char *application_directory,const char *base_override);

/* Inspect parameter help at the captured caret. The form lists all returned
 * overloads, marks the active parameter and presents descriptions as literal
 * text. It uses inspection-only capture, so read-only source is supported and
 * no edit approval is exposed. Opening the form never launches a server. */
UmiStatus UmiGtk4AdapterReviewParameterHelp(UmiGtk4Adapter *adapter,const char *workspace_folder);
UmiStatus UmiGtk4AdapterReviewParameterHelpWithSettings(UmiGtk4Adapter *adapter,const char *workspace_folder,
    const char *application_directory,const char *base_override);

int UmiGtk4AdapterCompletionReviewBusy(const UmiGtk4Adapter *adapter);
UmiStatus UmiGtk4AdapterCancelCompletionReview(UmiGtk4Adapter *adapter);

/* Review a server-proposed symbol rename for the captured writable draft.
 * The form owns copied request inputs, uses the selected local server, and
 * shows every returned document and change annotation as literal text. Apply
 * is available only when the whole proposal targets this exact document.
 * Required confirmation notes are included in explicit approval. Source,
 * settings or name changes invalidate approval. One Undo restores the draft;
 * no Save, resource operation or partial multi-document application occurs. */
UmiStatus UmiGtk4AdapterReviewSymbolRename(UmiGtk4Adapter *adapter,const char *workspace_folder);
UmiStatus UmiGtk4AdapterReviewSymbolRenameWithSettings(UmiGtk4Adapter *adapter,const char *workspace_folder,
    const char *application_directory,const char *base_override);
/* Review code actions for the captured caret or selected range. Complete
 * edit-only actions can be previewed and explicitly approved when every edit
 * targets this one writable document. Show disabled reasons and required
 * confirmation notes literally. Never execute commands, resolve lazy actions,
 * apply other-document changes or Save. Source/settings/choice changes retire
 * approval; one Undo restores a successfully updated draft. */
UmiStatus UmiGtk4AdapterReviewCodeActions(UmiGtk4Adapter *adapter,const char *workspace_folder);
UmiStatus UmiGtk4AdapterReviewCodeActionsWithSettings(UmiGtk4Adapter *adapter,const char *workspace_folder,
    const char *application_directory,const char *base_override);
/* Inspect the first matching diagnostic publication for the captured source.
 * Supports read-only drafts. Display complete literal messages and related
 * locations, then explicitly navigate the selected primary range. Links and
 * fixes are not executed; source/settings changes retire the result. */
UmiStatus UmiGtk4AdapterReviewSourceDiagnostics(UmiGtk4Adapter *adapter,const char *workspace_folder);
UmiStatus UmiGtk4AdapterReviewSourceDiagnosticsWithSettings(UmiGtk4Adapter *adapter,const char *workspace_folder,
    const char *application_directory,const char *base_override);
/* Inspect direct callers or callees at the captured caret, including every
 * returned call-site range. The form starts no process until Find is pressed.
 * Choose a relationship and an explicit destination before navigating. Supports
 * read-only drafts; never applies source edits or changes text Undo. Other open
 * drafts are not synchronized. Source/settings changes retire prior results. */
UmiStatus UmiGtk4AdapterReviewCallHierarchy(UmiGtk4Adapter *adapter,const char *workspace_folder,UmiLanguageCallDirection direction);
UmiStatus UmiGtk4AdapterReviewCallHierarchyWithSettings(UmiGtk4Adapter *adapter,const char *workspace_folder,
    UmiLanguageCallDirection direction,const char *application_directory,const char *base_override);
/* Review a local snippet template against the captured writable draft. The
 * shared completion Busy/Cancel commands also cover this form. Values are edited
 * before insertion; a complete preview and explicit approval produce one Undo
 * step. No server, filesystem write or automatic snippet execution is involved.
 * Numbered values/defaults/choices and $0 follow snippet_session's bounded syntax. */
UmiStatus UmiGtk4AdapterReviewSnippet(UmiGtk4Adapter *adapter);
/* Add explicitly named template Save/Load controls in the application's local
 * settings namespace. No I/O happens until a button is chosen. Loading fills
 * the template field only; insertion still requires expansion and approval.
 * Save replaces the name for the captured language and may finish after the
 * form closes. base_override must be absolute when supplied. */
UmiStatus UmiGtk4AdapterReviewSnippetWithSettings(UmiGtk4Adapter *adapter,
    const char *application_directory,const char *base_override);

/* Collapse the active native editor's selected complete lines after its first
 * line, or reveal every manually folded range in that editor. Source text,
 * dirty state and text Undo are unchanged; the fold action moves the caret to
 * its visible header. Draft changes or navigation into a hidden range reveal
 * it. Read-only documents are supported; no parser or server is started. */
UmiStatus UmiGtk4AdapterFoldSelectedCode(UmiGtk4Adapter *adapter);
UmiStatus UmiGtk4AdapterRevealFoldedCode(UmiGtk4Adapter *adapter);
/* Navigate the active draft to a matching bracket or select its enclosing
 * pair through the document coordinator. C and JSON language identities skip
 * their quoted/comment content; other languages use literal matching. Source
 * bytes and text Undo are unchanged. Completion follows the editing binding. */
UmiStatus UmiGtk4AdapterNavigateDelimiter(UmiGtk4Adapter *adapter,UmiDocumentDelimiterAction action);
/* Route one explicit line-edit command to the active document and existing
 * completion callback. Native refresh and retained controls share the normal
 * editing lifetime; this does not create a parallel text or Undo model. */
UmiStatus UmiGtk4AdapterEditLines(UmiGtk4Adapter *adapter,UmiEditorEditCommandKind kind,
    const UmiDocumentLineEditOptions *options);
/* Change active-document source/save format through its existing history and
 * completion binding. Native refresh may retire the host; no file I/O occurs. */
UmiStatus UmiGtk4AdapterSetDocumentFormat(UmiGtk4Adapter *adapter,const UmiDocumentFormatOptions *options);
#ifdef __cplusplus
}
#endif
#endif
