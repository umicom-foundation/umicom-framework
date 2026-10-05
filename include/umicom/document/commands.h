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
#define UMI_DOCUMENT_COMMAND_COUNT 23U
#define UMI_DOCUMENT_COMMAND_DELETE_LINE "umicom.document.line-edit.delete-line"
#define UMI_DOCUMENT_COMMAND_DUPLICATE_LINE "umicom.document.line-edit.duplicate-line"
#define UMI_DOCUMENT_COMMAND_MOVE_LINE_UP "umicom.document.line-edit.move-line-up"
#define UMI_DOCUMENT_COMMAND_MOVE_LINE_DOWN "umicom.document.line-edit.move-line-down"
#define UMI_DOCUMENT_COMMAND_JOIN_LINE_WITH_NEXT "umicom.document.line-edit.join-line-with-next"
#define UMI_DOCUMENT_COMMAND_TRIM_TRAILING_WHITESPACE "umicom.document.line-edit.trim-trailing-whitespace"
#define UMI_DOCUMENT_COMMAND_INDENT_LINES "umicom.document.line-edit.indent-lines"
#define UMI_DOCUMENT_COMMAND_OUTDENT_LINES "umicom.document.line-edit.outdent-lines"
#define UMI_DOCUMENT_COMMAND_TOGGLE_LINE_COMMENT "umicom.document.line-edit.toggle-line-comment"

#define UMI_DOCUMENT_COMMAND_MATCH_DELIMITER "umicom.document.match-delimiter"
#define UMI_DOCUMENT_COMMAND_SELECT_DELIMITER_CONTENT "umicom.document.select-delimiter-content"
#define UMI_DOCUMENT_COMMAND_SELECT_DELIMITER_PAIR "umicom.document.select-delimiter-pair"

/**
 * Add document commands only after its inputs and available capacity have been checked.
 */
UmiStatus umi_document_commands_register(UmiCommandRegistry *registry,
                                          UmiDocumentCoordinator *coordinator);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_COMMANDS_H */
