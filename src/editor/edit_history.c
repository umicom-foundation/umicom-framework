/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/edit_history.c
 *
 * PURPOSE:
 *   Implement deterministic bounded undo/redo stacks and inverse edit records.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static int record_valid(const UmiEditorEditRecord *record)
{
    return record != NULL &&
           record->removed_byte_count < UMI_EDITOR_EDIT_PAYLOAD_CAPACITY &&
           record->inserted_byte_count < UMI_EDITOR_EDIT_PAYLOAD_CAPACITY;
}

static void append_bounded(UmiEditorEditRecord *records,
                           size_t *count,
                           const UmiEditorEditRecord *record)
{
    if (*count == UMI_EDITOR_EDIT_HISTORY_CAPACITY) {
        (void)memmove(&records[0],
                      &records[1],
                      (UMI_EDITOR_EDIT_HISTORY_CAPACITY - 1U) * sizeof(records[0]));
        *count -= 1U;
    }
    records[*count] = *record;
    *count += 1U;
}

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

UmiStatus umi_editor_edit_history_create(UmiEditorEditHistory **out_history)
{
    UmiEditorEditHistory *history;
    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;
    history = (UmiEditorEditHistory *)calloc(1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->revision = 1U;
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_editor_edit_history_destroy(UmiEditorEditHistory *history)
{
    free(history);
}

UmiStatus umi_editor_edit_history_push(UmiEditorEditHistory *history,
                                        const UmiEditorEditRecord *record)
{
    if (history == NULL || !record_valid(record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    append_bounded(history->undo, &history->undo_count, record);
    history->redo_count = 0U;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_edit_history_undo(UmiEditorEditHistory *history,
                                        UmiEditorEditRecord *out_inverse)
{
    UmiEditorEditRecord record;
    if (history == NULL || out_inverse == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->undo_count == 0U) return UMI_STATUS_NOT_FOUND;
    record = history->undo[--history->undo_count];
    append_bounded(history->redo, &history->redo_count, &record);
    *out_inverse = inverse_of(&record);
    history->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_edit_history_redo(UmiEditorEditHistory *history,
                                        UmiEditorEditRecord *out_record)
{
    UmiEditorEditRecord record;
    if (history == NULL || out_record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->redo_count == 0U) return UMI_STATUS_NOT_FOUND;
    record = history->redo[--history->redo_count];
    append_bounded(history->undo, &history->undo_count, &record);
    *out_record = record;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

void umi_editor_edit_history_clear(UmiEditorEditHistory *history)
{
    if (history == NULL) return;
    history->undo_count = 0U;
    history->redo_count = 0U;
    history->revision += 1U;
}

size_t umi_editor_edit_history_undo_count(const UmiEditorEditHistory *history)
{
    return history != NULL ? history->undo_count : 0U;
}

size_t umi_editor_edit_history_redo_count(const UmiEditorEditHistory *history)
{
    return history != NULL ? history->redo_count : 0U;
}

uint64_t umi_editor_edit_history_revision(const UmiEditorEditHistory *history)
{
    return history != NULL ? history->revision : 0U;
}
