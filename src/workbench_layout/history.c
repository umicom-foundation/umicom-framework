/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/history.c
 *
 * PURPOSE:
 *   Implement bounded layout undo, redo and named checkpoints while preserving complete semantic documents and stable revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/history.h"

#include <string.h>

#include "internal.h"

/* Provide the initialise entry operation used by this module and its client applications. */
static UmiStatus initialise_entry(
    UmiWorkbenchLayoutHistoryEntry *entry,
    const UmiWorkbenchLayoutDocument *document,
    const char *entry_id,
    const char *label,
    const char *actor_id,
    const char *correlation_id,
    uint64_t captured_at_ms,
    bool checkpoint)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || document == NULL ||
        !umi_workbench_layout_text_present(entry_id) ||
        !umi_workbench_layout_text_present(label)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(entry, 0, sizeof(*entry));
    entry->structure_size = sizeof(*entry);
    status = umi_workbench_layout_copy_text(
        entry->entry_id,
        sizeof(entry->entry_id),
        entry_id,
        false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            entry->label,
            sizeof(entry->label),
            label,
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            entry->actor_id,
            sizeof(entry->actor_id),
            actor_id != NULL ? actor_id : "",
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            entry->correlation_id,
            sizeof(entry->correlation_id),
            correlation_id != NULL ? correlation_id : "",
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_copy(
            &entry->document, document);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(entry, 0, sizeof(*entry));
        return status;
    }

    entry->captured_at_ms = captured_at_ms;
    entry->checkpoint = checkpoint;
    return UMI_STATUS_OK;
}

/*
 * Provide the discard redo entries operation used by this module and its client
 * applications.
 */
static void discard_redo_entries(
    UmiWorkbenchLayoutHistory *history)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || history->count == 0U ||
        history->cursor + 1U >= history->count) {
        return;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = history->cursor + 1U;
         index < history->count;
         ++index) {
        (void)memset(
            &history->entries[index],
            0,
            sizeof(history->entries[index]));
    }
    history->count = history->cursor + 1U;
}

/*
 * Provide the discard oldest entry operation used by this module and its client
 * applications.
 */
static void discard_oldest_entry(
    UmiWorkbenchLayoutHistory *history)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || history->count == 0U) {
        return;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < history->count; ++index) {
        history->entries[index - 1U] = history->entries[index];
    }
    (void)memset(
        &history->entries[history->count - 1U],
        0,
        sizeof(history->entries[history->count - 1U]));
    history->count -= 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (history->cursor > 0U) {
        history->cursor -= 1U;
    }
}

/*
 * Initialise workbench layout history from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_history_init(
    UmiWorkbenchLayoutHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) {
        return;
    }
    (void)memset(history, 0, sizeof(*history));
    history->structure_size = sizeof(*history);
    history->cursor = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    history->revision = 1U;
}

/*
 * Provide the workbench layout history seed operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_history_seed(
    UmiWorkbenchLayoutHistory *history,
    const UmiWorkbenchLayoutDocument *document,
    const char *entry_id,
    const char *label)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_workbench_layout_history_init(history);
    status = initialise_entry(
        &history->entries[0],
        document,
        entry_id,
        label,
        "",
        "",
        0U,
        true);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    history->count = 1U;
    history->cursor = 0U;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
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
    bool checkpoint)
{
    UmiWorkbenchLayoutHistoryEntry entry;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = initialise_entry(
        &entry,
        document,
        entry_id,
        label,
        actor_id,
        correlation_id,
        captured_at_ms,
        checkpoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count > 0U &&
        history->cursor != UMI_WORKBENCH_LAYOUT_INDEX_NONE) {
        discard_redo_entries(history);
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count >= UMI_WORKBENCH_LAYOUT_MAX_HISTORY) {
        discard_oldest_entry(history);
    }

    history->entries[history->count] = entry;
    history->count += 1U;
    history->cursor = history->count - 1U;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout history can undo operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_history_can_undo(
    const UmiWorkbenchLayoutHistory *history)
{
    return history != NULL &&
           history->count > 1U &&
           history->cursor != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
           history->cursor > 0U;
}

/*
 * Provide the workbench layout history can redo operation used by this module and its
 * client applications.
 */
bool umi_workbench_layout_history_can_redo(
    const UmiWorkbenchLayoutHistory *history)
{
    return history != NULL &&
           history->cursor != UMI_WORKBENCH_LAYOUT_INDEX_NONE &&
           history->cursor + 1U < history->count;
}

/*
 * Provide the workbench layout history undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_history_undo(
    UmiWorkbenchLayoutHistory *history,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_layout_history_can_undo(history)) {
        return UMI_STATUS_NOT_FOUND;
    }

    history->cursor -= 1U;
    status = umi_workbench_layout_document_copy(
        out_document,
        &history->entries[history->cursor].document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        history->cursor += 1U;
        return status;
    }
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout history redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_layout_history_redo(
    UmiWorkbenchLayoutHistory *history,
    UmiWorkbenchLayoutDocument *out_document)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_document == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_workbench_layout_history_can_redo(history)) {
        return UMI_STATUS_NOT_FOUND;
    }

    history->cursor += 1U;
    status = umi_workbench_layout_document_copy(
        out_document,
        &history->entries[history->cursor].document);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        history->cursor -= 1U;
        return status;
    }
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout history restore entry operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_history_restore_entry(
    UmiWorkbenchLayoutHistory *history,
    const char *entry_id,
    UmiWorkbenchLayoutDocument *out_document)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_document == NULL ||
        !umi_workbench_layout_text_present(entry_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < history->count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_workbench_layout_text_equal(
                history->entries[index].entry_id, entry_id)) {
            status = umi_workbench_layout_document_copy(
                out_document, &history->entries[index].document);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) {
                return status;
            }
            history->cursor = index;
            history->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the workbench layout history current operation used by this module and its
 * client applications.
 */
const UmiWorkbenchLayoutHistoryEntry *
umi_workbench_layout_history_current(
    const UmiWorkbenchLayoutHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL ||
        history->cursor == UMI_WORKBENCH_LAYOUT_INDEX_NONE ||
        history->cursor >= history->count) {
        return NULL;
    }
    return &history->entries[history->cursor];
}

/*
 * Find workbench layout history while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiWorkbenchLayoutHistoryEntry *
umi_workbench_layout_history_at(
    const UmiWorkbenchLayoutHistory *history,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || index >= history->count) {
        return NULL;
    }
    return &history->entries[index];
}

/*
 * Return the number of records represented by workbench layout history checkpoint without
 * changing their state.
 */
size_t umi_workbench_layout_history_checkpoint_count(
    const UmiWorkbenchLayoutHistory *history)
{
    size_t index;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < history->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (history->entries[index].checkpoint) {
            count += 1U;
        }
    }
    return count;
}
