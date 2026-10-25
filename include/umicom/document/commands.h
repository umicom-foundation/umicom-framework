/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/document/commands.h
 *
 * PURPOSE:
 *   Define and register canonical document commands shared by menus,
 *   keybindings, command palettes, headless automation and future frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DOCUMENT_COMMANDS_H
#define UMICOM_DOCUMENT_COMMANDS_H

#include "umicom/document/coordinator.h"
#include "umicom/editor/edit_command.h"
#include "umicom/runtime/command_registry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DOCUMENT_COMMAND_NEW "umicom.document.new"
#define UMI_DOCUMENT_COMMAND_OPEN "umicom.document.open"
#define UMI_DOCUMENT_COMMAND_SAVE "umicom.document.save"
#define UMI_DOCUMENT_COMMAND_SAVE_AS "umicom.document.save-as"
#define UMI_DOCUMENT_COMMAND_CLOSE "umicom.document.close"
#define UMI_DOCUMENT_COMMAND_UNDO "umicom.document.undo"
#define UMI_DOCUMENT_COMMAND_REDO "umicom.document.redo"
#define UMI_DOCUMENT_COMMAND_FIND "umicom.document.find"
#define UMI_DOCUMENT_COMMAND_REPLACE "umicom.document.replace"
#define UMI_DOCUMENT_COMMAND_GO_TO_LINE "umicom.document.go-to-line"

#define UMI_DOCUMENT_COMMAND_REVERT "umicom.document.revert"

/* Delimiter navigation adds three commands to the established document
 * catalogue. The previous count is retained for integration review. */
#if 0
#define UMI_DOCUMENT_COMMAND_COUNT 11U
#endif
/* Line editing extends the shared catalogue. Preserve the previous count
 * for integration review; all earlier commands remain registered. */
#if 0
#define UMI_DOCUMENT_COMMAND_COUNT 14U
#endif
/* Preserve the former catalogue size; sorting adds two selection-only commands. */
#if 0
#define UMI_DOCUMENT_COMMAND_COUNT 23U
#endif
/* Selection case conversion extends the shared catalogue; keep the previous
 * count for review while retaining every existing command. */
#if 0
#define UMI_DOCUMENT_COMMAND_COUNT 25U
#endif
/* Tab insertion joins the canonical catalogue; preserve the prior count. */
#if 0
#define UMI_DOCUMENT_COMMAND_COUNT 27U
#endif
/* Newline insertion extends the catalogue; retain the preceding size for review. */
#if 0
#define UMI_DOCUMENT_COMMAND_COUNT 28U
#endif
/* Preserve the former count; the contextual comment command is appended. */
#if 0
#define UMI_DOCUMENT_COMMAND_COUNT 30U
#endif
#define UMI_DOCUMENT_COMMAND_COUNT 31U
#define UMI_DOCUMENT_COMMAND_INSERT_NEWLINE "umicom.document.insert-newline"
#define UMI_DOCUMENT_COMMAND_INSERT_PLAIN_NEWLINE "umicom.document.insert-plain-newline"
#define UMI_DOCUMENT_COMMAND_INSERT_INDENTATION "umicom.document.insert-indentation"
#define UMI_DOCUMENT_COMMAND_UPPERCASE_SELECTION "umicom.document.line-edit.uppercase-selection"
#define UMI_DOCUMENT_COMMAND_LOWERCASE_SELECTION "umicom.document.line-edit.lowercase-selection"
#define UMI_DOCUMENT_COMMAND_SORT_LINES_ASCENDING "umicom.document.line-edit.sort-lines-ascending"
#define UMI_DOCUMENT_COMMAND_SORT_LINES_DESCENDING "umicom.document.line-edit.sort-lines-descending"
#define UMI_DOCUMENT_COMMAND_DELETE_LINE "umicom.document.line-edit.delete-line"
#define UMI_DOCUMENT_COMMAND_DUPLICATE_LINE "umicom.document.line-edit.duplicate-line"
#define UMI_DOCUMENT_COMMAND_MOVE_LINE_UP "umicom.document.line-edit.move-line-up"
#define UMI_DOCUMENT_COMMAND_MOVE_LINE_DOWN "umicom.document.line-edit.move-line-down"
#define UMI_DOCUMENT_COMMAND_JOIN_LINE_WITH_NEXT "umicom.document.line-edit.join-line-with-next"
#define UMI_DOCUMENT_COMMAND_TRIM_TRAILING_WHITESPACE "umicom.document.line-edit.trim-trailing-whitespace"
#define UMI_DOCUMENT_COMMAND_INDENT_LINES "umicom.document.line-edit.indent-lines"
#define UMI_DOCUMENT_COMMAND_OUTDENT_LINES "umicom.document.line-edit.outdent-lines"
#define UMI_DOCUMENT_COMMAND_TOGGLE_LINE_COMMENT "umicom.document.line-edit.toggle-line-comment"
#define UMI_DOCUMENT_COMMAND_TOGGLE_LANGUAGE_COMMENT "umicom.document.line-edit.toggle-language-comment"

#define UMI_DOCUMENT_COMMAND_MATCH_DELIMITER "umicom.document.match-delimiter"
#define UMI_DOCUMENT_COMMAND_SELECT_DELIMITER_CONTENT "umicom.document.select-delimiter-content"
#define UMI_DOCUMENT_COMMAND_SELECT_DELIMITER_PAIR "umicom.document.select-delimiter-pair"

/** Resolve one of the exact line-edit identifiers above, including line sorting.
 * The original nine commands remain supported. No execution or
 * allocation occurs. Unknown names return NOT_FOUND without modifying out_kind;
 * NULL input or output returns INVALID_ARGUMENT. */
UmiStatus UmiDocumentLineEditCommandFromId(const char *command_id, UmiEditorEditCommandKind *out_kind);

/**
 * Add document commands only after its inputs and available capacity have been checked.
 */
UmiStatus umi_document_commands_register(UmiCommandRegistry *registry,
                                          UmiDocumentCoordinator *coordinator);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_COMMANDS_H */
