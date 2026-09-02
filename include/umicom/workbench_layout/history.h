/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/history.h
 *
 * PURPOSE:
 *   Maintain bounded undo, redo and named checkpoint history for semantic layout documents without retaining toolkit objects.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_HISTORY_H
#define UMICOM_WORKBENCH_LAYOUT_HISTORY_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout history entry data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the workbench layout history data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchLayoutHistory {
    uint32_t structure_size;
    UmiWorkbenchLayoutHistoryEntry entries[UMI_WORKBENCH_LAYOUT_MAX_HISTORY];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiWorkbenchLayoutHistory;

/**
 * Initialise workbench layout history from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_history_init(
    UmiWorkbenchLayoutHistory *history);

/**
 * Provide the workbench layout history seed operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_history_seed(
    UmiWorkbenchLayoutHistory *history,
    const UmiWorkbenchLayoutDocument *document,
    const char *entry_id,
    const char *label);

/**
 * Provide the workbench layout history push operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_history_push(
    UmiWorkbenchLayoutHistory *history,
    const UmiWorkbenchLayoutDocument *document,
    const char *entry_id,
    const char *label,
    const char *actor_id,
    const char *correlation_id,
    uint64_t captured_at_ms,
    bool checkpoint);

/**
 * Provide the workbench layout history can undo operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_history_can_undo(
    const UmiWorkbenchLayoutHistory *history);

/**
 * Provide the workbench layout history can redo operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_history_can_redo(
    const UmiWorkbenchLayoutHistory *history);

/**
 * Provide the workbench layout history undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_history_undo(
    UmiWorkbenchLayoutHistory *history,
    UmiWorkbenchLayoutDocument *out_document);

/**
 * Provide the workbench layout history redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_history_redo(
    UmiWorkbenchLayoutHistory *history,
    UmiWorkbenchLayoutDocument *out_document);

/**
 * Provide the workbench layout history restore entry operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_history_restore_entry(
    UmiWorkbenchLayoutHistory *history,
    const char *entry_id,
    UmiWorkbenchLayoutDocument *out_document);

/**
 * Provide the workbench layout history current operation used by this module and its
 * client applications.
 */
const UmiWorkbenchLayoutHistoryEntry *
umi_workbench_layout_history_current(
    const UmiWorkbenchLayoutHistory *history);

/**
 * Find workbench layout history while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchLayoutHistoryEntry *
umi_workbench_layout_history_at(
    const UmiWorkbenchLayoutHistory *history,
    size_t index);

/**
 * Return the number of records represented by workbench layout history checkpoint without
 * changing their state.
 */
size_t umi_workbench_layout_history_checkpoint_count(
    const UmiWorkbenchLayoutHistory *history);

#ifdef __cplusplus
}
#endif

#endif
