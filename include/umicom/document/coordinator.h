/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/coordinator.h
 *
 * PURPOSE:
 *   Define the authoritative bridge between DocumentStore working copies and
 *   toolkit-neutral UI document views, including history and conflict checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_COORDINATOR_H
#define UMICOM_DOCUMENT_COORDINATOR_H

#include "umicom/document/provider.h"
#include "umicom/platform/search.h"
#include "umicom/platform/document_store.h"
#include "umicom/ui/workbench.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY 32U
/* Each undo or redo stack retains at most this many owned text bytes,
 * including terminators. Oldest entries are evicted only after a successful
 * edit. The snapshot reports the number of steps still available. */
#define UMI_DOCUMENT_COORDINATOR_HISTORY_BYTE_BUDGET (16U * 1024U * 1024U)

/**
 * Represent the document working copy snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDocumentWorkingCopySnapshot {
    UmiDocumentId document_id;
    char view_id[UMI_UI_ID_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char uri[UMI_DOCUMENT_URI_CAPACITY];
    char display_name[UMI_DOCUMENT_NAME_CAPACITY];
    char language_id[UMI_DOCUMENT_LANGUAGE_CAPACITY];
    UmiDocumentTextEncoding encoding;
    UmiDocumentLineEnding line_ending;
    UmiDocumentConflictState conflict;
    size_t text_length;
    size_t undo_count;
    size_t redo_count;
    uint64_t revision;
    int dirty;
    int has_path;
    int active;
} UmiDocumentWorkingCopySnapshot;

/**
 * Represent the document coordinator data shared with callers of this public contract.
 */
typedef struct UmiDocumentCoordinator UmiDocumentCoordinator;

/**
 * Initialise document coordinator from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_document_coordinator_create(
    UmiDocumentStore *store,
    UmiUiWorkbench *workbench,
    const UmiDocumentProvider *provider,
    UmiDocumentCoordinator **out_coordinator);
/**
 * Release or reset state held by document coordinator so the same storage can be reused
 * safely.
 */
void umi_document_coordinator_destroy(UmiDocumentCoordinator *coordinator);
/**
 * Provide the document coordinator new operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_new(UmiDocumentCoordinator *coordinator,
                                       const char *suggested_name,
                                       char *out_view_id,
                                       size_t capacity);
/**
 * Provide the document coordinator open operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_open(UmiDocumentCoordinator *coordinator,
                                        const char *path,
                                        char *out_view_id,
                                        size_t capacity);
/**
 * Provide the document coordinator save active operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_save_active(
    UmiDocumentCoordinator *coordinator);
/**
 * Provide the document coordinator save active as operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_save_active_as(
    UmiDocumentCoordinator *coordinator,
    const char *path);
/**
 * Provide the document coordinator close active operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_close_active(
    UmiDocumentCoordinator *coordinator,
    int force);
/**
 * Provide the document coordinator sync active operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_sync_active(
    UmiDocumentCoordinator *coordinator);
/**
 * Provide the document coordinator undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_undo(UmiDocumentCoordinator *coordinator);
/**
 * Provide the document coordinator redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_redo(UmiDocumentCoordinator *coordinator);
/**
 * Find document coordinator while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_document_coordinator_find(UmiDocumentCoordinator *coordinator,
                                        const char *needle,
                                        size_t *out_offset);
/**
 * Provide the document coordinator replace operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_replace(UmiDocumentCoordinator *coordinator,
                                           const char *needle,
                                           const char *replacement,
                                           size_t *out_offset);
/**
 * Provide the document coordinator go to line operation used by this module and its client
 * applications.
 */
UmiStatus umi_document_coordinator_go_to_line(
    UmiDocumentCoordinator *coordinator,
    size_t one_based_line,
    size_t *out_offset);
/** Move the caret within the visible complete draft without saving it.
 * Line is one-based; column is a one-based UTF-8 byte column (zero means
 * unspecified). Columns past the line end clamp there; a column inside a
 * multibyte character moves to its first byte. Missing lines return NOT_FOUND
 * without changing the caret or outOffset. Use from the document owner's thread. */
UmiStatus UmiDocumentCoordinatorGoToPosition(UmiDocumentCoordinator *coordinator,
    size_t oneBasedLine, size_t oneBasedByteColumn, size_t *outOffset);

/**
 * Provide the document coordinator check external change operation used by this module and
 * its client applications.
 */
UmiStatus umi_document_coordinator_check_external_change(
    UmiDocumentCoordinator *coordinator,
    int *out_changed);
/**
 * Provide the document coordinator active snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_document_coordinator_active_snapshot(
    const UmiDocumentCoordinator *coordinator,
    UmiDocumentWorkingCopySnapshot *out_snapshot);
/**
 * Find document coordinator while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_document_coordinator_at(
    const UmiDocumentCoordinator *coordinator,
    size_t index,
    UmiDocumentWorkingCopySnapshot *out_snapshot);
/**
 * Return the number of records represented by document coordinator without changing their
 * state.
 */
size_t umi_document_coordinator_count(
    const UmiDocumentCoordinator *coordinator);

/** Save dirty working copies through their existing conflict-aware saver.
 * Reject a dirty untitled copy before writing any file. outSaved is optional
 * and reports completed saves, including when a subsequent save fails.
 * Call on the document owner's thread; no UI selection is changed. */
UmiStatus UmiDocumentCoordinatorSaveAll(UmiDocumentCoordinator *coordinator,
    size_t *outSaved);


/** Save the document identified when an asynchronous action began, regardless
 * of which tab is now active. A closed ID returns NOT_FOUND. The active tab
 * is not switched, and another open document's destination is rejected.
 * Use from the coordinator's owning thread. No input pointer is retained. */
UmiStatus UmiDocumentCoordinatorSaveAs(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, const char *path);

/** Close a captured working copy without changing another tab's selection.
 * force must reflect an explicit discard decision. A closed ID is NOT_FOUND.
 * Like the coordinator's other operations, call on the document owner's thread. */
UmiStatus UmiDocumentCoordinatorClose(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, int force);

/** Navigate literal matches in visible editor text. A nonzero backwards flag
 * selects the previous match; both directions wrap once. Outputs are optional
 * and change only on success. Search does not synchronise or save a draft. */
UmiStatus UmiDocumentCoordinatorFindNext(UmiDocumentCoordinator *coordinator,
    const char *needle, int backwards, size_t *outOffset, int *outWrapped);
/** Replace the selected literal match, or the next match when no match is
 * selected. Reject null replacement; an empty string deletes the match.
 * Uses the existing smart ASCII-case rule; no regular expressions are evaluated. */
UmiStatus UmiDocumentCoordinatorReplaceNext(UmiDocumentCoordinator *coordinator,
    const char *needle, const char *replacement, size_t *outOffset);
/** Replace all non-overlapping matches in one undoable operation. Insufficient
 * presentation capacity leaves the draft, store and history unchanged. Read-only
 * documents reject editing. outCount is optional and changes only on success. */
UmiStatus UmiDocumentCoordinatorReplaceAll(UmiDocumentCoordinator *coordinator,
    const char *needle, const char *replacement, size_t *outCount);

/** Open a saved-file search match without replacing any visible draft. The
 * opened/selected file stays open when its text no longer matches the saved
 * location; NOT_FOUND then leaves its caret unchanged. No file is saved. */
UmiStatus UmiDocumentCoordinatorOpenSearchMatch(UmiDocumentCoordinator *coordinator,
    const UmiSearchMatch *match, const char *query, int caseSensitive,
    size_t *outOffset);

/** A prepared reload owns both the incoming text and the previous visible
 * draft. It never owns the coordinator. Destroy it on acceptance, cancellation
 * or failure. Calls that prepare/apply use the document owner's thread; the
 * coordinator must still be alive when applying. Destroying a plan itself
 * does not access the coordinator. Keep outstanding plans bounded in the UI. */
typedef struct UmiDocumentReloadPlan UmiDocumentReloadPlan;

typedef struct UmiDocumentReloadSummary {
    UmiDocumentId document_id;
    char display_name[UMI_DOCUMENT_NAME_CAPACITY];
    size_t previous_bytes;
    size_t incoming_bytes;
    int has_unsaved_changes;
    int text_changes;
    UmiDocumentTextEncoding incoming_encoding;
    UmiDocumentLineEnding incoming_line_ending;
} UmiDocumentReloadSummary;

/** Read and decode a saved file without altering its draft, caret, history,
 * store, active tab or disk contents. Untitled documents are not reloadable.
 * The plan captures document identity, path and text/store revisions.
 * The incoming decoded text must fit the complete-document limit. */
UmiStatus UmiDocumentCoordinatorPrepareReload(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, UmiDocumentReloadPlan **outPlan);
UmiStatus UmiDocumentReloadPlanSummary(const UmiDocumentReloadPlan *plan,
    UmiDocumentReloadSummary *outSummary);
void UmiDocumentReloadPlanDestroy(UmiDocumentReloadPlan *plan);

/** Adopt the prepared text only when the captured document and provider bytes
 * are still current. Changing another tab is permitted. Editing, saving or
 * Save As of the target invalidates an older plan. discardUnsaved must be 0
 * or 1; only explicit approval should pass 1. Success is one Undo step when
 * text differs, marks the incoming text saved, and does not write the file.
 * The caret is clamped to a UTF-8 boundary; another active tab stays active.
 * Allocation, decoding, limit and stale-plan failures preserve visible text
 * and history. Storage reservations may grow without a semantic change.
 * A file can still change after the final read; the new baseline ensures a
 * later save checks again. This is not a cross-process file lock. */
UmiStatus UmiDocumentCoordinatorApplyReload(UmiDocumentCoordinator *coordinator,
    UmiDocumentReloadPlan *plan, int discardUnsaved);

/** Targeted external-change inspection. outChanged is initialised to zero.
 * Missing saved files set the deleted conflict and return Not Found with 1;
 * access/read errors are propagated, not interpreted as deletion. A file
 * restored to its baseline clears a previously recorded external conflict.
 * The existing active-document function delegates to the same implementation. */
UmiStatus UmiDocumentCoordinatorCheckExternalChange(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, int *outChanged);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_COORDINATOR_H */
