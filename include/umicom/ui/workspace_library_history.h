/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workspace_library_history.h
 * PURPOSE: Recover committed layout-library changes with bounded, revision-checked history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSPACE_LIBRARY_HISTORY_H
#define UMICOM_UI_WORKSPACE_LIBRARY_HISTORY_H
#include "umicom/ui/workspace_library_exchange.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_UI_WORKSPACE_LIBRARY_HISTORY_LIMIT 8U
typedef struct UmiUiWorkspaceLibraryHistory UmiUiWorkspaceLibraryHistory;
typedef enum UmiUiWorkspaceLibraryHistoryDirection {
    UMI_UI_WORKSPACE_LIBRARY_HISTORY_UNDO = 1,
    UMI_UI_WORKSPACE_LIBRARY_HISTORY_REDO = 2
} UmiUiWorkspaceLibraryHistoryDirection;

/* Counts describe this session only. A stale chain cannot be navigated: a
 * workspace edit outside the history has changed its revision. The next
 * successful recorded change starts a new chain from that current workspace. */
typedef struct UmiUiWorkspaceLibraryHistoryState {
    size_t undo_count;
    size_t redo_count;
    uint64_t expected_revision;
    bool stale;
    bool busy;
} UmiUiWorkspaceLibraryHistoryState;

/* The host accepts widgets before publishing its model. On failure it must
 * leave the live model unchanged and restore any tentative widget changes.
 * candidate is borrowed only during this synchronous callback. Do not retain
 * it, destroy history or change the owner from another callback/thread.
 * Nested record/navigation calls are refused with BUSY. */
typedef UmiStatus (*UmiUiWorkspaceLibraryHistoryPublisher)(
    const UmiUiWorkspaceCustomisation *candidate, void *context);

/* All calls belong to the workspace owner's thread. History owns bounded
 * before/after layout archives, never document buffers, orders or accounts.
 * There is no disk write. Destroy after outstanding callbacks return. */
UmiStatus umi_ui_workspace_library_history_create(UmiUiWorkspaceLibraryHistory **out_history);
void umi_ui_workspace_library_history_destroy(UmiUiWorkspaceLibraryHistory *history);
UmiStatus umi_ui_workspace_library_history_read(const UmiUiWorkspaceLibraryHistory *history,
    uint64_t current_revision, UmiUiWorkspaceLibraryHistoryState *out_state);

/* Stage both archives before asking the host to publish. Allocation, scope,
 * validation or publication failure preserves the history. The new model
 * revision must exceed the old revision. Success discards redo entries and
 * evicts the oldest change when the fixed entry limit is reached. A history
 * binds to the first successful scope; reuse in another workspace is refused. */
UmiStatus umi_ui_workspace_library_history_record(UmiUiWorkspaceLibraryHistory *history,
    const UmiUiWorkspaceCheckpointScope *scope,
    const UmiUiWorkspaceCustomisation *before, const UmiUiWorkspaceCustomisation *candidate,
    UmiUiWorkspaceLibraryHistoryPublisher publish, void *context);

/* Undo/Redo revalidates archived layouts against the current tool catalogue
 * and creates a fresh model revision. The expected revision is copied when
 * the user chooses the action, so delayed requests never replace newer work.
 * A failed publication leaves the cursor available for a deliberate retry. */
UmiStatus umi_ui_workspace_library_history_navigate(UmiUiWorkspaceLibraryHistory *history,
    const UmiUiWorkspaceCheckpointScope *scope, const UmiUiWorkspaceCustomisation *model,
    UmiUiWorkspaceLibraryHistoryDirection direction, uint64_t expected_revision,
    UmiUiWorkspaceLibraryHistoryPublisher publish, void *context);
#ifdef __cplusplus
}
#endif
#endif
