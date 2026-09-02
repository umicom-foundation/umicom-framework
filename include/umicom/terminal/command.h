/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/command.h
 *
 * PURPOSE:
 *   Parse an explicit command line into owned program and argument records without invoking an implicit shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TERMINAL_COMMAND_H
#define UMICOM_TERMINAL_COMMAND_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the terminal command data shared with callers of this public contract.
 */
typedef struct UmiTerminalCommand {
    char source[UMI_TERMINAL_COMMAND_CAPACITY];
    char argument_storage[UMI_TERMINAL_MAX_ARGUMENTS]
                         [UMI_TERMINAL_ARGUMENT_CAPACITY];
    const char *arguments[UMI_TERMINAL_MAX_ARGUMENTS];
    size_t argument_count;
} UmiTerminalCommand;

/**
 * Initialise terminal command from caller-provided values so later operations receive a
 * known state.
 */
void umi_terminal_command_init(UmiTerminalCommand *command);
/**
 * Read terminal command into validated module state and return a status when input cannot
 * be used.
 */
UmiStatus umi_terminal_command_parse(UmiTerminalCommand *command,
                                     const char *text);
/**
 * Provide the terminal command program operation used by this module and its client
 * applications.
 */
const char *umi_terminal_command_program(const UmiTerminalCommand *command);
/**
 * Provide the terminal command format operation used by this module and its client
 * applications.
 */
UmiStatus umi_terminal_command_format(const UmiTerminalCommand *command,
                                      char *out_text,
                                      size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
