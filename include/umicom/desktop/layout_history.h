/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/layout_history.h
 *
 * PURPOSE:
 *   Define Master Controller-owned history transactions spanning layouts,
 *   windows, monitor affinity and context links. Applications identify an
 *   operation; Framework captures, commits, rolls back, undoes and redoes it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_LAYOUT_HISTORY_H
#define UMICOM_DESKTOP_LAYOUT_HISTORY_H

#include "umicom/desktop/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_LAYOUT_HISTORY_MAX 16U

typedef enum UmiDesktopLayoutHistoryState {
    UMI_DESKTOP_LAYOUT_HISTORY_OPEN = 1,
    UMI_DESKTOP_LAYOUT_HISTORY_COMMITTED = 2,
    UMI_DESKTOP_LAYOUT_HISTORY_ROLLED_BACK = 3,
    UMI_DESKTOP_LAYOUT_HISTORY_UNDONE = 4
} UmiDesktopLayoutHistoryState;

typedef struct UmiDesktopLayoutHistoryEntrySnapshot {
    uint32_t structure_size;
    uint64_t sequence;
    char transaction_id[UMI_DESKTOP_ID_CAPACITY];
    char operation_id[UMI_DESKTOP_ID_CAPACITY];
    char source_application_id[UMI_DESKTOP_ID_CAPACITY];
    char label[UMI_DESKTOP_TITLE_CAPACITY];
    UmiDesktopLayoutHistoryState state;
    bool applied;
    uint64_t revision;
} UmiDesktopLayoutHistoryEntrySnapshot;

typedef struct UmiDesktopLayoutHistorySnapshot {
    uint32_t structure_size;
    size_t entry_count;
    size_t applied_count;
    char pending_transaction_id[UMI_DESKTOP_ID_CAPACITY];
    bool transaction_open;
    bool can_undo;
    bool can_redo;
    uint64_t last_sequence;
    uint64_t revision;
} UmiDesktopLayoutHistorySnapshot;

typedef struct UmiDesktopLayoutHistory UmiDesktopLayoutHistory;

UmiStatus umi_desktop_layout_history_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopLayoutHistory **out_history);
void umi_desktop_layout_history_destroy(UmiDesktopLayoutHistory *history);
UmiStatus umi_desktop_layout_history_begin(
    UmiDesktopLayoutHistory *history,
    const char *transaction_id,
    const char *operation_id,
    const char *source_application_id,
    const char *label);
UmiStatus umi_desktop_layout_history_commit(
    UmiDesktopLayoutHistory *history,
    const char *transaction_id);
UmiStatus umi_desktop_layout_history_rollback(
    UmiDesktopLayoutHistory *history,
    const char *transaction_id);
UmiStatus umi_desktop_layout_history_undo(UmiDesktopLayoutHistory *history);
UmiStatus umi_desktop_layout_history_redo(UmiDesktopLayoutHistory *history);
UmiStatus umi_desktop_layout_history_entry_at(
    const UmiDesktopLayoutHistory *history,
    size_t index,
    UmiDesktopLayoutHistoryEntrySnapshot *out_entry);
UmiStatus umi_desktop_layout_history_snapshot(
    const UmiDesktopLayoutHistory *history,
    UmiDesktopLayoutHistorySnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
