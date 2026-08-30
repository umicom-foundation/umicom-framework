/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/refactoring_command.h
 *
 * PURPOSE:
 *   Publish stable Framework command identities and enablement requirements
 *   for quick fixes, source actions and professional refactoring workflows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_REFACTORING_COMMAND_H
#define UMICOM_EDITOR_REFACTORING_COMMAND_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_REFACTORING_COMMAND_API_VERSION 1U
#define UMI_EDITOR_REFACTORING_COMMAND_ID_CAPACITY 96U
#define UMI_EDITOR_REFACTORING_COMMAND_LABEL_CAPACITY 128U
#define UMI_EDITOR_REFACTORING_COMMAND_KEY_CAPACITY 48U

typedef enum UmiEditorRefactoringCommandKind {
    UMI_EDITOR_REFACTORING_COMMAND_SHOW_CODE_ACTIONS = 1,
    UMI_EDITOR_REFACTORING_COMMAND_APPLY_PREFERRED = 2,
    UMI_EDITOR_REFACTORING_COMMAND_FIX_ALL_DOCUMENT = 3,
    UMI_EDITOR_REFACTORING_COMMAND_ORGANISE_IMPORTS = 4,
    UMI_EDITOR_REFACTORING_COMMAND_RENAME_SYMBOL = 5,
    UMI_EDITOR_REFACTORING_COMMAND_EXTRACT_FUNCTION = 6,
    UMI_EDITOR_REFACTORING_COMMAND_EXTRACT_VARIABLE = 7,
    UMI_EDITOR_REFACTORING_COMMAND_EXTRACT_CONSTANT = 8,
    UMI_EDITOR_REFACTORING_COMMAND_INLINE = 9,
    UMI_EDITOR_REFACTORING_COMMAND_MOVE = 10,
    UMI_EDITOR_REFACTORING_COMMAND_CHANGE_SIGNATURE = 11,
    UMI_EDITOR_REFACTORING_COMMAND_SHOW_PREVIEW = 12,
    UMI_EDITOR_REFACTORING_COMMAND_INCLUDE_EDIT = 13,
    UMI_EDITOR_REFACTORING_COMMAND_EXCLUDE_EDIT = 14,
    UMI_EDITOR_REFACTORING_COMMAND_INCLUDE_ALL = 15,
    UMI_EDITOR_REFACTORING_COMMAND_EXCLUDE_OPTIONAL = 16,
    UMI_EDITOR_REFACTORING_COMMAND_APPLY = 17,
    UMI_EDITOR_REFACTORING_COMMAND_CANCEL = 18
} UmiEditorRefactoringCommandKind;

typedef struct UmiEditorRefactoringCommandDescriptor {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorRefactoringCommandKind kind;
    char id[UMI_EDITOR_REFACTORING_COMMAND_ID_CAPACITY];
    char label[UMI_EDITOR_REFACTORING_COMMAND_LABEL_CAPACITY];
    char default_key[UMI_EDITOR_REFACTORING_COMMAND_KEY_CAPACITY];
    int requires_document;
    int requires_selection;
    int requires_symbol;
    int requires_active_plan;
    int opens_view;
    int modifies_documents;
} UmiEditorRefactoringCommandDescriptor;

size_t umi_editor_refactoring_command_count(void);
const UmiEditorRefactoringCommandDescriptor *
umi_editor_refactoring_command_at(size_t index);
const UmiEditorRefactoringCommandDescriptor *
umi_editor_refactoring_command_find(const char *id);
const UmiEditorRefactoringCommandDescriptor *
umi_editor_refactoring_command_for_kind(UmiEditorRefactoringCommandKind kind);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_REFACTORING_COMMAND_H */
