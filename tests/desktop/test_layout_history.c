/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop/test_layout_history.c
 *
 * PURPOSE:
 *   Implement the test layout history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Desktop layout history | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>

#include "umicom/desktop/layout_history.h"

int main(void)
{
    UmiApplicationContextHub *hub = NULL;
    UmiDesktopRuntime *runtime = NULL;
    UmiDesktopLayoutHistory *history = NULL;
    UmiDesktopLayoutHistorySnapshot snapshot;
    UmiDesktopLayoutHistoryEntrySnapshot entry;
    const UmiDesktopWindow *window;
    int32_t original_x;
    assert(umi_application_context_hub_create(&hub) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_create(hub, &runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_seed(runtime) == UMI_STATUS_OK);
    assert(umi_desktop_runtime_clone_layout(
        runtime, "develop", "history-test", "History Test", true) ==
        UMI_STATUS_OK);
    assert(umi_desktop_layout_history_create(runtime, &history) ==
           UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "project-explorer");
    assert(window != NULL);
    original_x = window->bounds.x;
    assert(umi_desktop_layout_history_begin(
        history, "transaction.move", "umicom.desktop.window.move",
        "org.umicom.studio", "Move Project Explorer") == UMI_STATUS_OK);
    assert(umi_desktop_runtime_place_window(
        runtime, "project-explorer", "primary",
        (UmiDesktopRect){640, 120, 520, 720},
        UMI_DESKTOP_DOCK_FLOATING) == UMI_STATUS_OK);
    assert(umi_desktop_layout_history_commit(
        history, "transaction.move") == UMI_STATUS_OK);
    assert(umi_desktop_layout_history_snapshot(history, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.entry_count == 1U && snapshot.can_undo);
    assert(umi_desktop_layout_history_undo(history) == UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "project-explorer");
    assert(window != NULL && window->bounds.x == original_x);
    assert(umi_desktop_layout_history_redo(history) == UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "project-explorer");
    assert(window != NULL && window->bounds.x == 640);
    assert(umi_desktop_layout_history_entry_at(history, 0U, &entry) ==
           UMI_STATUS_OK);
    assert(entry.applied && entry.sequence == 1U);
    assert(umi_desktop_layout_history_begin(
        history, "transaction.rollback", "umicom.desktop.window.move",
        "org.umicom.studio", "Rollback Move") == UMI_STATUS_OK);
    assert(umi_desktop_runtime_place_window(
        runtime, "project-explorer", "primary",
        (UmiDesktopRect){900, 140, 420, 680},
        UMI_DESKTOP_DOCK_FLOATING) == UMI_STATUS_OK);
    assert(umi_desktop_layout_history_rollback(
        history, "transaction.rollback") == UMI_STATUS_OK);
    window = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(runtime), "project-explorer");
    assert(window != NULL && window->bounds.x == 640);
    umi_desktop_layout_history_destroy(history);
    umi_desktop_runtime_destroy(runtime);
    umi_application_context_hub_destroy(hub);
    return 0;
}
