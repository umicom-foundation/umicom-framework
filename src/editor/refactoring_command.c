/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/refactoring_command.c
 *
 * PURPOSE:
 *   Implement the canonical quick-fix and refactoring command catalogue used
 *   by Studio and future Umicom application shells.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/refactoring_command.h"

#include <string.h>

#define COMMAND(kind_value, id_value, label_value, key_value, document, selection, symbol, plan, view, modifies) \
    {                                                                                                             \
        (uint32_t)sizeof(UmiEditorRefactoringCommandDescriptor),                                                  \
        UMI_EDITOR_REFACTORING_COMMAND_API_VERSION, kind_value, id_value,                                         \
        label_value, key_value, document, selection, symbol, plan, view, modifies                                 \
    }

static const UmiEditorRefactoringCommandDescriptor COMMANDS[] = {
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_SHOW_CODE_ACTIONS,
            "editor.code-actions.show", "Show Code Actions", "Ctrl+.",
            1, 0, 0, 0, 1, 0),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_APPLY_PREFERRED,
            "editor.code-actions.apply-preferred", "Apply Preferred Quick Fix", "",
            1, 0, 0, 0, 0, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_FIX_ALL_DOCUMENT,
            "editor.code-actions.fix-all-document", "Fix All in Document", "",
            1, 0, 0, 0, 1, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_ORGANISE_IMPORTS,
            "editor.code-actions.organise-imports", "Organise Imports", "Shift+Alt+O",
            1, 0, 0, 0, 0, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_RENAME_SYMBOL,
            "editor.refactor.rename", "Rename Symbol", "F2",
            1, 0, 1, 0, 1, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_EXTRACT_FUNCTION,
            "editor.refactor.extract-function", "Extract Function", "",
            1, 1, 0, 0, 1, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_EXTRACT_VARIABLE,
            "editor.refactor.extract-variable", "Extract Variable", "",
            1, 1, 0, 0, 1, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_EXTRACT_CONSTANT,
            "editor.refactor.extract-constant", "Extract Constant", "",
            1, 1, 0, 0, 1, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_INLINE,
            "editor.refactor.inline", "Inline Symbol", "",
            1, 0, 1, 0, 1, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_MOVE,
            "editor.refactor.move", "Move Symbol", "",
            1, 0, 1, 0, 1, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_CHANGE_SIGNATURE,
            "editor.refactor.change-signature", "Change Signature", "",
            1, 0, 1, 0, 1, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_SHOW_PREVIEW,
            "editor.refactor.preview.show", "Show Refactoring Preview", "",
            0, 0, 0, 1, 1, 0),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_INCLUDE_EDIT,
            "editor.refactor.preview.include-edit", "Include Preview Edit", "",
            0, 0, 0, 1, 1, 0),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_EXCLUDE_EDIT,
            "editor.refactor.preview.exclude-edit", "Exclude Preview Edit", "",
            0, 0, 0, 1, 1, 0),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_INCLUDE_ALL,
            "editor.refactor.preview.include-all", "Include All Preview Edits", "",
            0, 0, 0, 1, 1, 0),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_EXCLUDE_OPTIONAL,
            "editor.refactor.preview.exclude-optional", "Exclude Optional Preview Edits", "",
            0, 0, 0, 1, 1, 0),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_APPLY,
            "editor.refactor.apply", "Apply Refactoring", "Ctrl+Enter",
            0, 0, 0, 1, 0, 1),
    COMMAND(UMI_EDITOR_REFACTORING_COMMAND_CANCEL,
            "editor.refactor.cancel", "Cancel Refactoring", "Escape",
            0, 0, 0, 1, 0, 0)
};

#undef COMMAND

size_t umi_editor_refactoring_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

const UmiEditorRefactoringCommandDescriptor *
umi_editor_refactoring_command_at(size_t index)
{
    return index < umi_editor_refactoring_command_count() ? &COMMANDS[index]
                                                           : NULL;
}

const UmiEditorRefactoringCommandDescriptor *
umi_editor_refactoring_command_find(const char *id)
{
    size_t index;

    if (id == NULL) return NULL;
    for (index = 0U; index < umi_editor_refactoring_command_count(); ++index) {
        if (strcmp(COMMANDS[index].id, id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

const UmiEditorRefactoringCommandDescriptor *
umi_editor_refactoring_command_for_kind(UmiEditorRefactoringCommandKind kind)
{
    size_t index;

    for (index = 0U; index < umi_editor_refactoring_command_count(); ++index) {
        if (COMMANDS[index].kind == kind) return &COMMANDS[index];
    }
    return NULL;
}
