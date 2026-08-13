/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/edit_history.h
 *
 * PURPOSE:
 *   Store bounded undo/redo edit payloads without binding history to a widget.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_EDIT_HISTORY_H
#define UMICOM_EDITOR_EDIT_HISTORY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_EDIT_HISTORY_CAPACITY 256U
#define UMI_EDITOR_EDIT_PAYLOAD_CAPACITY 1024U

typedef struct UmiEditorEditRecord {
    uint64_t operation_id;
    size_t offset;
    size_t removed_byte_count;
    size_t inserted_byte_count;
    char removed_bytes[UMI_EDITOR_EDIT_PAYLOAD_CAPACITY];
    char inserted_bytes[UMI_EDITOR_EDIT_PAYLOAD_CAPACITY];
    uint64_t before_revision;
    uint64_t after_revision;
} UmiEditorEditRecord;

typedef struct UmiEditorEditHistory UmiEditorEditHistory;

UmiStatus umi_editor_edit_history_create(UmiEditorEditHistory **out_history);
void umi_editor_edit_history_destroy(UmiEditorEditHistory *history);
UmiStatus umi_editor_edit_history_push(UmiEditorEditHistory *history,
                                        const UmiEditorEditRecord *record);
UmiStatus umi_editor_edit_history_undo(UmiEditorEditHistory *history,
                                        UmiEditorEditRecord *out_inverse);
UmiStatus umi_editor_edit_history_redo(UmiEditorEditHistory *history,
                                        UmiEditorEditRecord *out_record);
void umi_editor_edit_history_clear(UmiEditorEditHistory *history);
size_t umi_editor_edit_history_undo_count(const UmiEditorEditHistory *history);
size_t umi_editor_edit_history_redo_count(const UmiEditorEditHistory *history);
uint64_t umi_editor_edit_history_revision(const UmiEditorEditHistory *history);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDIT_HISTORY_H */
