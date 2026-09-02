/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/edit_history.c
 *
 * PURPOSE:
 *   Implement deterministic bounded undo/redo stacks and inverse edit records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/edit_history.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorEditHistory {
    UmiEditorEditRecord undo[UMI_EDITOR_EDIT_HISTORY_CAPACITY];
    UmiEditorEditRecord redo[UMI_EDITOR_EDIT_HISTORY_CAPACITY];
    size_t undo_count;
    size_t redo_count;
    uint64_t revision;
};

/* Check that record satisfies its contract before another service relies on it. */
static int record_valid(const UmiEditorEditRecord *record)
{
    return record != NULL &&
           record->removed_byte_count < UMI_EDITOR_EDIT_PAYLOAD_CAPACITY &&
           record->inserted_byte_count < UMI_EDITOR_EDIT_PAYLOAD_CAPACITY;
}

/* Provide the append bounded operation used by this module and its client applications. */
static void append_bounded(UmiEditorEditRecord *records,
                           size_t *count,
                           const UmiEditorEditRecord *record)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*count == UMI_EDITOR_EDIT_HISTORY_CAPACITY) {
        (void)memmove(&records[0],
                      &records[1],
                      (UMI_EDITOR_EDIT_HISTORY_CAPACITY - 1U) * sizeof(records[0]));
        *count -= 1U;
    }
    records[*count] = *record;
    *count += 1U;
}

/* Provide the inverse of operation used by this module and its client applications. */
static UmiEditorEditRecord inverse_of(const UmiEditorEditRecord *record)
{
    UmiEditorEditRecord inverse = {0};
    inverse.operation_id = record->operation_id;
    inverse.offset = record->offset;
    inverse.removed_byte_count = record->inserted_byte_count;
    inverse.inserted_byte_count = record->removed_byte_count;
    (void)memcpy(inverse.removed_bytes,
                 record->inserted_bytes,
                 record->inserted_byte_count);
    (void)memcpy(inverse.inserted_bytes,
                 record->removed_bytes,
                 record->removed_byte_count);
    inverse.before_revision = record->after_revision;
    inverse.after_revision = record->before_revision;
    return inverse;
}

/*
 * Initialise editor edit history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_edit_history_create(UmiEditorEditHistory **out_history)
{
    UmiEditorEditHistory *history;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;
    history = (UmiEditorEditHistory *)calloc(1U, sizeof(*history));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->revision = 1U;
    *out_history = history;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor edit history so the same storage can be reused
 * safely.
 */
void umi_editor_edit_history_destroy(UmiEditorEditHistory *history)
{
    free(history);
}

/*
 * Provide the editor edit history push operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_history_push(UmiEditorEditHistory *history,
                                        const UmiEditorEditRecord *record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || !record_valid(record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    append_bounded(history->undo, &history->undo_count, record);
    history->redo_count = 0U;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor edit history undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_history_undo(UmiEditorEditHistory *history,
                                        UmiEditorEditRecord *out_inverse)
{
    UmiEditorEditRecord record;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_inverse == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (history->undo_count == 0U) return UMI_STATUS_NOT_FOUND;
    record = history->undo[--history->undo_count];
    append_bounded(history->redo, &history->redo_count, &record);
    *out_inverse = inverse_of(&record);
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor edit history redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_history_redo(UmiEditorEditHistory *history,
                                        UmiEditorEditRecord *out_record)
{
    UmiEditorEditRecord record;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (history->redo_count == 0U) return UMI_STATUS_NOT_FOUND;
    record = history->redo[--history->redo_count];
    append_bounded(history->undo, &history->undo_count, &record);
    *out_record = record;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor edit history so the same storage can be reused
 * safely.
 */
void umi_editor_edit_history_clear(UmiEditorEditHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return;
    history->undo_count = 0U;
    history->redo_count = 0U;
    history->revision += 1U;
}

/*
 * Return the number of records represented by editor edit history undo without changing
 * their state.
 */
size_t umi_editor_edit_history_undo_count(const UmiEditorEditHistory *history)
{
    return history != NULL ? history->undo_count : 0U;
}

/*
 * Return the number of records represented by editor edit history redo without changing
 * their state.
 */
size_t umi_editor_edit_history_redo_count(const UmiEditorEditHistory *history)
{
    return history != NULL ? history->redo_count : 0U;
}

/*
 * Provide the editor edit history revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_edit_history_revision(const UmiEditorEditHistory *history)
{
    return history != NULL ? history->revision : 0U;
}
