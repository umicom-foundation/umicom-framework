/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/edit_history.h
 *
 * PURPOSE:
 *   Store bounded undo/redo edit payloads without binding history to a widget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the editor edit record data shared with callers of this public contract.
 */
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

/**
 * Represent the editor edit history data shared with callers of this public contract.
 */
typedef struct UmiEditorEditHistory UmiEditorEditHistory;

/**
 * Initialise editor edit history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_edit_history_create(UmiEditorEditHistory **out_history);
/**
 * Release or reset state held by editor edit history so the same storage can be reused
 * safely.
 */
void umi_editor_edit_history_destroy(UmiEditorEditHistory *history);
/**
 * Provide the editor edit history push operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_history_push(UmiEditorEditHistory *history,
                                        const UmiEditorEditRecord *record);
/**
 * Provide the editor edit history undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_history_undo(UmiEditorEditHistory *history,
                                        UmiEditorEditRecord *out_inverse);
/**
 * Provide the editor edit history redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_history_redo(UmiEditorEditHistory *history,
                                        UmiEditorEditRecord *out_record);
/**
 * Release or reset state held by editor edit history so the same storage can be reused
 * safely.
 */
void umi_editor_edit_history_clear(UmiEditorEditHistory *history);
/**
 * Return the number of records represented by editor edit history undo without changing
 * their state.
 */
size_t umi_editor_edit_history_undo_count(const UmiEditorEditHistory *history);
/**
 * Return the number of records represented by editor edit history redo without changing
 * their state.
 */
size_t umi_editor_edit_history_redo_count(const UmiEditorEditHistory *history);
/**
 * Provide the editor edit history revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_edit_history_revision(const UmiEditorEditHistory *history);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDIT_HISTORY_H */
