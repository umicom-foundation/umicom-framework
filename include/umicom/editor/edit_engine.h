/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/edit_engine.h
 *
 * PURPOSE:
 *   Execute professional editing commands atomically over the Framework text
 *   buffer with line indexing and bounded command-transaction undo/redo.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_EDIT_ENGINE_H
#define UMICOM_EDITOR_EDIT_ENGINE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/edit_command.h"
#include "umicom/editor/line_index.h"
#include "umicom/editor/text_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_EDIT_ENGINE_API_VERSION 1U
#define UMI_EDITOR_EDIT_ENGINE_DEFAULT_HISTORY_CAPACITY 128U
#define UMI_EDITOR_EDIT_ENGINE_DEFAULT_HISTORY_BYTE_BUDGET (32U * 1024U * 1024U)

/**
 * Represent the editor edit engine data shared with callers of this public contract.
 */
typedef struct UmiEditorEditEngine UmiEditorEditEngine;

/**
 * Represent the editor edit engine config data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorEditEngineConfig {
    uint32_t struct_size;
    uint32_t api_version;
    size_t initial_buffer_capacity;
    size_t history_capacity;
    size_t history_byte_budget;
} UmiEditorEditEngineConfig;

/**
 * Represent the editor edit engine snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorEditEngineSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t byte_count;
    size_t line_count;
    size_t cursor_offset;
    size_t undo_count;
    size_t redo_count;
    size_t history_byte_count;
    size_t history_byte_budget;
    uint64_t document_revision;
    uint64_t command_revision;
    int dirty;
    int can_undo;
    int can_redo;
} UmiEditorEditEngineSnapshot;

/**
 * Initialise editor edit engine from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_edit_engine_create(
    const UmiEditorEditEngineConfig *config,
    UmiEditorEditEngine **out_engine);
/**
 * Release or reset state held by editor edit engine so the same storage can be reused
 * safely.
 */
void umi_editor_edit_engine_destroy(UmiEditorEditEngine *engine);
/**
 * Read editor edit engine into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_editor_edit_engine_load(
    UmiEditorEditEngine *engine,
    const char *bytes,
    size_t byte_count);
/**
 * Perform editor edit engine through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_editor_edit_engine_execute(
    UmiEditorEditEngine *engine,
    const UmiEditorEditCommandRequest *request,
    UmiEditorEditCommandResult *out_result);
/**
 * Provide the editor edit engine undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_engine_undo(
    UmiEditorEditEngine *engine,
    UmiEditorEditCommandResult *out_result);
/**
 * Provide the editor edit engine redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_engine_redo(
    UmiEditorEditEngine *engine,
    UmiEditorEditCommandResult *out_result);
/**
 * Provide the editor edit engine clear history operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_edit_engine_clear_history(UmiEditorEditEngine *engine);
/**
 * Provide the editor edit engine mark saved operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_engine_mark_saved(UmiEditorEditEngine *engine);
/**
 * Provide the editor edit engine snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_engine_snapshot(
    const UmiEditorEditEngine *engine,
    UmiEditorEditEngineSnapshot *out_snapshot);
/**
 * Provide the editor edit engine view operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_edit_engine_view(
    const UmiEditorEditEngine *engine,
    UmiEditorTextBufferView *out_view);
/**
 * Provide the editor edit engine line index operation used by this module and its client
 * applications.
 */
const UmiEditorLineIndex *umi_editor_edit_engine_line_index(
    const UmiEditorEditEngine *engine);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDIT_ENGINE_H */
