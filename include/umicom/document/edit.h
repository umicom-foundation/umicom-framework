/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/edit.h
 *
 * PURPOSE:
 *   Edit the intended document through its existing coordinator, even when a
 *   clipboard request completes after the user has selected another tab.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_EDIT_H
#define UMICOM_DOCUMENT_EDIT_H
#include "umicom/document/coordinator.h"
#ifdef __cplusplus
extern "C" {
#endif

/** Shared command identities used by menus and native editor adapters. */
typedef enum UmiDocumentEditCommand {
    UMI_DOCUMENT_EDIT_UNDO = 1,
    UMI_DOCUMENT_EDIT_REDO,
    UMI_DOCUMENT_EDIT_CUT,
    UMI_DOCUMENT_EDIT_COPY,
    UMI_DOCUMENT_EDIT_PASTE,
    UMI_DOCUMENT_EDIT_SELECT_ALL,
    UMI_DOCUMENT_EDIT_DELETE
} UmiDocumentEditCommand;

/** A copied description; offsets and sizes count UTF-8 bytes, not characters.
 * can_undo includes an edited draft not yet synchronised into DocumentStore.
 * A later edit may change availability, so execution always checks again. */
typedef struct UmiDocumentEditState {
    UmiDocumentId document_id;
    size_t text_bytes;
    size_t selection_offset;
    size_t selection_bytes;
    int read_only;
    int can_undo;
    int can_redo;
} UmiDocumentEditState;

/** Map edit.undo/redo/cut/copy/paste/select-all/delete without changing output
 * on an unknown name (NOT_FOUND). No application creates another mapping. */
UmiStatus UmiDocumentEditCommandFromId(const char *commandId,
    UmiDocumentEditCommand *outCommand);
/** Inspect a named working copy without selecting, saving or synchronising it.
 * The coordinator must be used from its owning thread. Zero IDs are invalid. */
UmiStatus UmiDocumentCoordinatorGetEditState(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, UmiDocumentEditState *outState);
/** Undo/redo and Select All operate on documentId without activating another
 * tab. Undo/redo use the coordinator's existing bounded history. Read-only
 * documents permit selection/copy but reject changes and history traversal. */
UmiStatus UmiDocumentCoordinatorUndo(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId);
UmiStatus UmiDocumentCoordinatorRedo(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId);
UmiStatus UmiDocumentCoordinatorSelectAll(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId);

/** Record an accepted group of native typing in the existing history without
 * saving to disk or selecting its tab. Call once at end-user-action, not once
 * for each intermediate deletion and insertion inside a paste. */
UmiStatus UmiDocumentCoordinatorSyncDocument(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId);

/** A prepared selection owns its complete UTF-8 draft and borrows its original
 * coordinator. The coordinator must remain alive through ApplyEdit. Destroying
 * the plan alone does not access its owner. Keep pending native reads bounded.
 * See examples/editor_workflow/edit_commands.c for a complete caller. */
typedef struct UmiDocumentEditPlan UmiDocumentEditPlan;
/** Capture identity, saved/draft revisions, caret and selection. Preparation
 * changes nothing and is also allowed for Copy from a read-only document. */
UmiStatus UmiDocumentCoordinatorPrepareEdit(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, UmiDocumentEditPlan **outPlan);
/** Borrow selected bytes until plan destruction. The slice is NOT necessarily
 * zero-terminated; its explicit length is mandatory. Empty selection is valid.
 * Both outputs are cleared on failure. Never free or modify borrowed bytes. */
UmiStatus UmiDocumentEditPlanSelection(const UmiDocumentEditPlan *plan,
    const char **outText, size_t *outBytes);
void UmiDocumentEditPlanDestroy(UmiDocumentEditPlan *plan);
/** Replace the captured selection, or insert at its caret, as one edit through
 * the existing history/store/view path. replacement contains exactly bytes readable bytes;
 * embedded zeros and malformed UTF-8 are rejected. An empty string deletes.
 * Closing the target, changing its saved/draft revision or path, or moving its
 * caret/selection rejects an old plan; changing another tab does not redirect it. Read-only, stale,
 * allocation and size failures preserve text/history. Success consumes the plan,
 * including an identical-text edit. Destroy it afterwards. No file is written.
 * This is an owner-thread operation, not a cross-thread or filesystem lock. */
UmiStatus UmiDocumentCoordinatorApplyEdit(UmiDocumentCoordinator *coordinator,
    UmiDocumentEditPlan *plan, const char *replacement, size_t bytes);
#ifdef __cplusplus
}
#endif
#endif
