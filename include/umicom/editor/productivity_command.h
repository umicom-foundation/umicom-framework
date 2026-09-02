/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/productivity_command.h
 *
 * PURPOSE:
 *   Publish stable Framework command identities and enablement requirements
 *   for formatting, import organisation, snippets and linked editing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_PRODUCTIVITY_COMMAND_H
#define UMICOM_EDITOR_PRODUCTIVITY_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_PRODUCTIVITY_COMMAND_API_VERSION 1U
#define UMI_EDITOR_PRODUCTIVITY_COMMAND_ID_CAPACITY 96U
#define UMI_EDITOR_PRODUCTIVITY_COMMAND_LABEL_CAPACITY 128U
#define UMI_EDITOR_PRODUCTIVITY_COMMAND_KEY_CAPACITY 48U

/**
 * List the named editor productivity command kind values accepted by this public contract.
 */
typedef enum UmiEditorProductivityCommandKind {
    UMI_EDITOR_PRODUCTIVITY_COMMAND_FORMAT_DOCUMENT = 1,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_FORMAT_SELECTION = 2,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_TOGGLE_FORMAT_ON_SAVE = 3,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_TOGGLE_FORMAT_ON_TYPE = 4,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_SELECT_FORMATTER = 5,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_ORGANISE_IMPORTS = 6,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_REMOVE_UNUSED_IMPORTS = 7,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_SORT_IMPORTS = 8,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_INSERT_SNIPPET = 9,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_NEXT_SNIPPET_PLACEHOLDER = 10,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_PREVIOUS_SNIPPET_PLACEHOLDER = 11,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_CANCEL_SNIPPET = 12,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_START_LINKED_EDITING = 13,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_NEXT_LINKED_RANGE = 14,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_APPLY_LINKED_EDIT = 15,
    UMI_EDITOR_PRODUCTIVITY_COMMAND_CANCEL_LINKED_EDITING = 16
} UmiEditorProductivityCommandKind;

/**
 * Represent the editor productivity command descriptor data shared with callers of this
 * public contract.
 */
typedef struct UmiEditorProductivityCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorProductivityCommandKind kind;
    char id[UMI_EDITOR_PRODUCTIVITY_COMMAND_ID_CAPACITY];
    char label[UMI_EDITOR_PRODUCTIVITY_COMMAND_LABEL_CAPACITY];
    char default_key[UMI_EDITOR_PRODUCTIVITY_COMMAND_KEY_CAPACITY];
    int requires_document;
    int requires_selection;
    int requires_active_session;
    int opens_view;
} UmiEditorProductivityCommandDescriptor;

/**
 * Return the number of records represented by editor productivity command without changing
 * their state.
 */
size_t umi_editor_productivity_command_count(void);
/**
 * Find editor productivity command while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiEditorProductivityCommandDescriptor *
umi_editor_productivity_command_at(size_t index);
/**
 * Find editor productivity command while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiEditorProductivityCommandDescriptor *
umi_editor_productivity_command_find(const char *id);
/**
 * Provide the editor productivity command for kind operation used by this module and its
 * client applications.
 */
const UmiEditorProductivityCommandDescriptor *
umi_editor_productivity_command_for_kind(UmiEditorProductivityCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_PRODUCTIVITY_COMMAND_H */
