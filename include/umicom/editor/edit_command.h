/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/edit_command.h
 *
 * PURPOSE:
 *   Declare canonical professional text-editing commands and their reusable,
 *   product-neutral metadata and execution requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_EDIT_COMMAND_H
#define UMICOM_EDITOR_EDIT_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_EDIT_COMMAND_API_VERSION 1U
#define UMI_EDITOR_EDIT_COMMAND_ID_CAPACITY 96U
#define UMI_EDITOR_EDIT_COMMAND_LABEL_CAPACITY 128U
#define UMI_EDITOR_EDIT_COMMAND_KEY_CAPACITY 48U

/**
 * List the named editor edit command kind values accepted by this public contract.
 */
typedef enum UmiEditorEditCommandKind {
    UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT = 1,
    UMI_EDITOR_EDIT_COMMAND_REPLACE_RANGE = 2,
    UMI_EDITOR_EDIT_COMMAND_DELETE_LINE = 3,
    UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE = 4,
    UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP = 5,
    UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN = 6,
    UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT = 7,
    UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE = 8,
    UMI_EDITOR_EDIT_COMMAND_INDENT_LINES = 9,
    UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES = 10,
    UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT = 11,
    UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE = 12,
    UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE = 13,
    UMI_EDITOR_EDIT_COMMAND_TRANSPOSE_LINES = 14
} UmiEditorEditCommandKind;

/**
 * Represent the editor edit command descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorEditCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorEditCommandKind kind;
    char id[UMI_EDITOR_EDIT_COMMAND_ID_CAPACITY];
    char label[UMI_EDITOR_EDIT_COMMAND_LABEL_CAPACITY];
    char default_key[UMI_EDITOR_EDIT_COMMAND_KEY_CAPACITY];
    int requires_editable_document;
    int uses_selection;
    int line_oriented;
} UmiEditorEditCommandDescriptor;

/**
 * Represent the editor edit command request data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorEditCommandRequest {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorEditCommandKind kind;
    size_t cursor_offset;
    size_t selection_start;
    size_t selection_end;
    const char *text;
    size_t text_byte_count;
    const char *indent_text;
    size_t indent_byte_count;
    const char *line_comment;
    size_t line_comment_byte_count;
} UmiEditorEditCommandRequest;

/**
 * Represent the editor edit command result data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorEditCommandResult {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorEditCommandKind kind;
    uint64_t before_revision;
    uint64_t after_revision;
    size_t cursor_offset;
    size_t selection_start;
    size_t selection_end;
    size_t affected_line_count;
    size_t removed_byte_count;
    size_t inserted_byte_count;
    int changed;
} UmiEditorEditCommandResult;

/**
 * Return the number of records represented by editor edit command without changing their
 * state.
 */
size_t umi_editor_edit_command_count(void);
/**
 * Find editor edit command while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiEditorEditCommandDescriptor *umi_editor_edit_command_at(size_t index);
/**
 * Find editor edit command while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiEditorEditCommandDescriptor *umi_editor_edit_command_find(
    const char *id);
/**
 * Provide the editor edit command for kind operation used by this module and its client
 * applications.
 */
const UmiEditorEditCommandDescriptor *umi_editor_edit_command_for_kind(
    UmiEditorEditCommandKind kind);
/**
 * Initialise editor edit command request from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_edit_command_request_initialize(
    UmiEditorEditCommandRequest *request,
    UmiEditorEditCommandKind kind,
    size_t cursor_offset);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_EDIT_COMMAND_H */
