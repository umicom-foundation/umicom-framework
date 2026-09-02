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

#define UMI_DOCUMENT_COMMAND_COUNT 10U

/**
 * Add document commands only after its inputs and available capacity have been checked.
 */
UmiStatus umi_document_commands_register(UmiCommandRegistry *registry,
                                          UmiDocumentCoordinator *coordinator);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DOCUMENT_COMMANDS_H */
