/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/history.h
 *
 * PURPOSE:
 *   Maintain bounded undo, redo and named checkpoint history for semantic layout documents without retaining toolkit objects.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_HISTORY_H
#define UMICOM_WORKBENCH_LAYOUT_HISTORY_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchLayoutHistoryEntry {
    uint32_t structure_size;
    char entry_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char label[UMI_WORKBENCH_LAYOUT_NAME_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    UmiWorkbenchLayoutDocument document;
    uint64_t captured_at_ms;
    bool checkpoint;
} UmiWorkbenchLayoutHistoryEntry;

typedef struct UmiWorkbenchLayoutHistory {
    uint32_t structure_size;
    UmiWorkbenchLayoutHistoryEntry entries[UMI_WORKBENCH_LAYOUT_MAX_HISTORY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiWorkbenchLayoutHistory;

void umi_workbench_layout_history_init(
    UmiWorkbenchLayoutHistory *history);

UmiStatus umi_workbench_layout_history_seed(
    UmiWorkbenchLayoutHistory *history,
    const UmiWorkbenchLayoutDocument *document,
    const char *entry_id,
    const char *label);

UmiStatus umi_workbench_layout_history_push(
    UmiWorkbenchLayoutHistory *history,
    const UmiWorkbenchLayoutDocument *document,
    const char *entry_id,
    const char *label,
    const char *actor_id,
    const char *correlation_id,
    uint64_t captured_at_ms,
    bool checkpoint);

bool umi_workbench_layout_history_can_undo(
    const UmiWorkbenchLayoutHistory *history);

bool umi_workbench_layout_history_can_redo(
    const UmiWorkbenchLayoutHistory *history);

UmiStatus umi_workbench_layout_history_undo(
    UmiWorkbenchLayoutHistory *history,
    UmiWorkbenchLayoutDocument *out_document);

UmiStatus umi_workbench_layout_history_redo(
    UmiWorkbenchLayoutHistory *history,
    UmiWorkbenchLayoutDocument *out_document);

UmiStatus umi_workbench_layout_history_restore_entry(
    UmiWorkbenchLayoutHistory *history,
    const char *entry_id,
    UmiWorkbenchLayoutDocument *out_document);

const UmiWorkbenchLayoutHistoryEntry *
umi_workbench_layout_history_current(
    const UmiWorkbenchLayoutHistory *history);

const UmiWorkbenchLayoutHistoryEntry *
umi_workbench_layout_history_at(
    const UmiWorkbenchLayoutHistory *history,
    size_t index);

size_t umi_workbench_layout_history_checkpoint_count(
    const UmiWorkbenchLayoutHistory *history);

#ifdef __cplusplus
}
#endif

#endif
